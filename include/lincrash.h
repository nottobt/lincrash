// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * LinCrash (Short for Linux Crash)
 * Ver. 1.3.0b (Major 1, Minor 3, Patch 0, beta)
 * HEADER FILE <lincrash.h>
 */

#ifndef LINCRASH_H
#define LINCRASH_H

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

// General defines
#define EFI_SB_VAR "/sys/firmware/efi/efivars/SecureBoot-8be4df61-93ca-11d2-aa0d-00e098032b8c"
#define VER "Version 1.3.0b"
#define KRNLCH 4
#define LOGFILE "/tmp/lincrash/lincrash.log"
#define TMPDIR "/tmp/lincrash"

// Modes (Will be used with cmake/gcc)
#ifdef DEBUG
	#define DEBUG_MODE 1
#else
	#define DEBUG_MODE 0
#endif

#ifdef PROT
	#define PROT_DISABLE 1
#else
	#define PROT_DISABLE 0
#endif

int logs(long long errcode, const char *format, ...) {
	char formlog[1024];
	char message[512];
	char formtime[64];
	time_t rawtime;
	time(&rawtime);
	struct tm *time1 = localtime(&rawtime);
	strftime(formtime, sizeof(formtime), "%Y-%m-%d %H:%M:%S", time1);
	va_list args;
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);
	if (errcode == 0) {
		snprintf(formlog, sizeof(formlog), "[%s] INFO: %s\n", formtime, message);
	} else {
		snprintf(formlog, sizeof(formlog), "[%s] ERR %lld: %s\n", formtime, errcode, message);
	}
	FILE *LOGFILE1 = fopen(LOGFILE, "a");
	if (LOGFILE1 == NULL) {
		perror("Logfile append failed");
		exit(13);
	}
	fprintf(LOGFILE1, "%s", formlog);
	fclose(LOGFILE1);
	sync();
	return 0;
}

volatile int remdir(const char *path) {
	DIR *dir1 = opendir(path);
	size_t pathlen = strlen(path);
	int r = -1;
	if (dir1) {
		struct dirent *p;
		r = 0;
		while (!r && (p = readdir(dir1))) {
			int len;
			char *buf;
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;
			len = pathlen + strlen(p->d_name) + 2;
			buf = malloc(len);
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode)) {
						r = remdir(buf);
					} else {
						r = unlink(buf);
					}
				}
				free(buf);
			} else {
				r = -1;
			}
		}
		closedir(dir1);
	}
	if (!r) {
		r = rmdir(path);
	}
	return r;
}

int init(void) {
	int result = remdir(TMPDIR);
	if (result == 0) {
		printf("Deleting directory completed successfully.\n");
	}
	else if (result == -1) {
		printf("Directory already deleted or not found.\n");
	}
	else {
		printf("Error: Unable to delete directory. Do you have access to /tmp/lincrash?\n");
		exit(0);
	}
	result = mkdir(TMPDIR, 0777);
	if (result == 0) {
		printf("Making directory completed successfully.\n");
		logs(0, "Making directory: %s complete.", TMPDIR);
	} else {
		printf("Error code %i: Unable to make directory. Do you have access to /tmp or are you running out of space?\n", result);
		exit(0);
	}
	printf("Press any key to continue...\n");
	getchar();
	system("clear");
}

int sbcheck(void) {
	if (access(EFI_SB_VAR, F_OK) != 0) {
		return -1;
	}
	FILE *fp = fopen(EFI_SB_VAR, "rb");
	if (!fp) {
		return -1;
	}
	uint8_t buffer[5];
	size_t read_bytes = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	if (read_bytes < 5) {
		return -1;
	}
	return (int)buffer[4];
}

int getroot() {
	if (getuid() != 0) {
		printf("Permission denied. Are you root?\n");
		logs(1, "Permission denied. User isn't running the program as root.");
		exit(1);
	}
	return 0;
}

int opt1(void) {
	long pagesize = sysconf(_SC_PAGESIZE);
	char *buffer = mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED) {
		perror("mmap failed");
		logs(1, "Nmap Failure: exiting...");
		return 1;
	}
	buffer[0] = 'X';
	madvise(buffer, pagesize, MADV_DONTFORK);
	printf("Simulating active MCE hardware corruption at RAM location: %p\n", buffer);
	logs(0, "Simulating active MCE hardware corruption in RAM");
	if (madvise(buffer, pagesize, MADV_HWPOISON) < 0) {
		perror("madvise HWPOISON failed");
		return 1;
	}
	printf("Reading memory address to provoke active handling path...\n");
	char fault = *buffer;
	return 0;
}
int opt2(void) {
	int *sigsegv = (int *)0xFFFF6FFFFFFFFFFF;
	printf("Triggering General Protection Fault (#GP) @ Non-canonical void address: %p\n", sigsegv);
	logs(0, "Triggering General Protection Fault (#GP) @ Address: %p", sigsegv);
	*sigsegv = 2;
	return 0;
}

int opt3(void) {
	int *sigsegv = NULL;
	printf("Triggering NULL pointer dereference...\n");
	logs(0, "Triggering userspace segmentation fault via NULL pointer dereference");
	*sigsegv = 45;
	return 0;
}
int opt4(void) {
	char errstr[128];
	char cmd[256];
	char cmd1[256];
	int opt = 0;
	int sel_ko = 0;
	FILE *make = fopen("modules/Makefile", "w");
	if (make == NULL) {
		perror("Error creating makefile!");
		logs(1, "Makefile creation failed. Permission denied or disk full.");
		exit(1);
	}
	printf("Kernel-level warning/crash initiated.\n");
	printf("Before we start, we must VERify that secureboot is disabled. (Secureboot must be disabled in order to trigger the kernel-level bug)\n");
	int status = sbcheck();

	switch (status) {
		case 1:
			printf("Secure Boot is ENABLED.\n");
			logs(1, "Secure Boot is ENABLED. Lincrash can't continue Option 4.");
			return 0;
			break;
		case 0:
			printf("Secure Boot is DISABLED.\n");
			break;
		default:
			printf("Could not determine Secure Boot state (Non-UEFI system or insufficient permissions).\n");
			logs(1, "Could not determine Secure Boot state (Non-UEFI system or insufficient permissions).");
			return 1;
			break;
	}
	printf("You have 4 options!\n");
	printf("You have 4 options!\n"
	"Option 1: Trigger a Kernel Warning [Function WARN()]\n"
	"Option 2: Trigger a Kernel Bug [Function BUG()]\n"
	"Option 3: Trigger a Kernel Oops via Divide by zero error\n"
	"Option 4: Trigger a Kernel Panic [Function panic()]\n"
	"Choose one: ");
	scanf("%i", &opt);
	printf("Building makefile...\n");
	switch (opt) {
		case 1:
			fprintf(make, "obj-m += lincrashlkm1.o\n\n");
			sel_ko = 1;
			break;
		case 2:
			fprintf(make, "obj-m += lincrashlkm2.o\n\n");
			sel_ko = 2;
			break;
		case 3:
			fprintf(make, "obj-m += lincrashlkm3.o\n\n");
			sel_ko = 3;
			break;
		case 4:
			fprintf(make, "obj-m += lincrashlkm4.o\n\n");
			sel_ko = 4;
			break;
		default:
			printf("Wrong option: %i!\n", opt);
			snprintf(errstr, sizeof(errstr), "User chose option %i, but it is invalid", opt);
			logs(1, errstr);
			fclose(make);
			exit(1);
	}
	printf("Building makefile...\n");
	logs(0, "User initiated Option 4 and selected kernel sub-option: %i", opt);
	fprintf(make, "KDIR := /lib/modules/$(shell uname -r)/build\n");
	fprintf(make, "all:\n");
	fprintf(make, "\tmake -C $(KDIR) M=$(PWD)/modules modules\n");
	fprintf(make, "clean:\n");
	fprintf(make, "\tmake -C $(KDIR) M=$(PWD)/modules clean\n");
	fclose(make);
	snprintf(cmd1, sizeof(cmd1), "sudo cp modules/ko%i.c /tmp/lincrash/lincrashlkm%i.c && sudo cp -rf Makefile /tmp/lincrash/Makefile && sudo make -C /tmp/lincrash", sel_ko, sel_ko);
	snprintf(cmd, sizeof(cmd), "sudo insmod /tmp/lincrash/lincrashlkm%i.ko", sel_ko);
	system(cmd1);
	system(cmd);
	return 0;
}

volatile int opt5(void) {
	int cho;
	system("clear");
	printf("Memory Leaker!");
	logs(0, "Memory leaker chosen. (opt4)");
	printf("Select the memory size:");
	printf("Option 1: 8kb (Slowest)\n"
	"Option 2: 64mb (Faster)\n"
	"Option 3: 512mb (Fastest)\n");
	printf("Choose one\n");
	scanf("%i", &cho);
	size_t allocsize;
	switch (cho) {
		case 1:
			allocsize = 1024 * 1024 * 8;
			break;
		case 2:
			allocsize = 1024 * 1024 * 64;
			break;
		case 3:
			allocsize = 1024 * 1024 * 512;
			break;
		default:
			printf("Wrong option.\nExiting...");
			exit(0);
	}
	printf("Executing memory leak... Ctrl + C if you choose not to execute, or\nwait for the OOM reaper to reap the executable.\n");
	logs(0, "Executing memory leak now...");
	printf("Memory leak allocations will be printed in: %s.\n", LOGFILE);
	struct timespec salloc, ealloc;
	while (1) {
		clock_gettime(CLOCK_MONOTONIC, &salloc);
		void *ptr = malloc(allocsize);
		memset(ptr, 0xF, allocsize);
		clock_gettime(CLOCK_MONOTONIC, &ealloc);
		double ttaken = (ealloc.tv_sec - salloc.tv_sec) * 1000.0 + (ealloc.tv_nsec - salloc.tv_nsec) / 1000000.0;
		logs(0, "Allocated memory size %zu bytes in duration: %.3f", allocsize, ttaken);

	}
	exit(0);
}

#endif

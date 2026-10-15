// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * LinCrash (Short for Linux Crash)
 * Ver. 1.1.0b (Major 1, Minor 1, Patch 0, beta)
 * HEADER FILE <lincrash.h>
 */

#ifndef LINCRASH_H
#define LINCRASH_H

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

// General defines
#define EFI_SB_VAR "/sys/firmware/efi/efivars/SecureBoot-8be4df61-93ca-11d2-aa0d-00e098032b8c"
#define ver "Version 1.1.0b"
#define krnlch 4

// Modes (Will be used with cmake/gcc)
#ifdef DEBUG
	#define DEBUG_MODE 1
#else
	#define DEBUG_MODE 0
#endif

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
		printf("Permission denied (UID is not 0), are you root?\n");
		return 0; // Exit the function early since it needs root
	}
	return 1;
}

static int opt1(void) {
	long pagesize = sysconf(_SC_PAGESIZE);
	char *buffer = mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (buffer == MAP_FAILED) {
		perror("mmap failed");
		return 1;
	}
	buffer[0] = 'X';
	madvise(buffer, pagesize, MADV_DONTFORK);
	printf("Simulating active MCE hardware corruption at RAM location: %p\n", buffer);
	if (madvise(buffer, pagesize, MADV_HWPOISON) < 0) {
		perror("madvise HWPOISON failed");
		return 1;
	}
	printf("Reading memory address to provoke active handling path...\n");
	char fault = *buffer;
	return 0;
}

static int opt2(void) {
	int *sigsegv = (int *)0xFFFF6FFFFFFFFFFF;
	printf("Triggering General Protection Fault (#GP) @ Non-canonical void address: %p\n", sigsegv);
	*sigsegv = 2;
	return 0;
}

static int opt3(void) {
	int *sigsegv = NULL;
	printf("Triggering NULL pointer dereference...\n");
	*sigsegv = 45;
	return 0;
}

static int opt4(void) {
	char cmd[256];
	int opt = 0;
	int sel_ko = 0;
	FILE *make = fopen("modules/Makefile", "w");
	if (make == NULL) {
			perror("Error creating makefile!");
			return 1;
	}
	printf("Kernel-level warning/crash initiated.\n");
	printf("Before we start, we must verify that secureboot is disabled. (Secureboot must be disabled in order to trigger the kernel-level bug)\n");
	int status = sbcheck();

	switch (status) {
		case 1:
			printf("Secure Boot is ENABLED.\n");
			return 0;
			break;
		case 0:
			printf("Secure Boot is DISABLED.\n");
			break;
		default:
			printf("Could not determine Secure Boot state (Non-UEFI system or insufficient permissions).\n");
			return 13;
			break;
	}
	printf("You have 4 options!\n");
	printf("Option 1: Trigger a Kernel Warning [Function WARN()]\n");
	printf("Option 2: Trigger a Kernel Bug [Function BUG()]\n");
	printf("Option 3: Trigger a Kernel Oops [Function DIE()]\n");
	printf("Option 4: Trigger a Kernel Panic [Function panic()]\n");
	printf("Choose one: ");
	scanf("%i", &opt);
	printf("Building makefile...\n");
	switch (opt) {
		case 1:
			fprintf(make, "obj-m += ko1.o\n\n");
			sel_ko = 1;
			break;
		case 2:
			fprintf(make, "obj-m += ko2.o\n\n");
			sel_ko = 2;
			break;
		case 3:
			fprintf(make, "obj-m += ko3.o\n\n");
			sel_ko = 3;
			break;
		case 4:
			fprintf(make, "obj-m += ko4.o\n\n");
			sel_ko = 4;
			break;
		default:
			printf("Wrong option: %i!\n", opt);
			fclose(make);
			return 1;
	}
	fprintf(make, "KDIR := /lib/modules/$(shell uname -r)/build\n");
	fprintf(make, "all:\n");
	fprintf(make, "\tmake -C $(KDIR) M=$(PWD)/modules modules\n");
	fprintf(make, "clean:\n");
	fprintf(make, "\tmake -C $(KDIR) M=$(PWD)/modules clean\n");
	fclose(make);
	system("make -C modules");
	snprintf(cmd, sizeof(cmd), "sudo insmod modules/ko%i.ko", sel_ko);
	system(cmd);
	return 0;
}

#endif

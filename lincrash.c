// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * LinCrash (Short for Linux Crash)
 * Ver. 1.3.0b (Major 1, Minor 3, Patch 0, beta)
 * MAIN C FILE <lincrash.c>
 */

#include "include/lincrash.h"

int ch(const int usrch) {
    switch (usrch) {
        case 1:
            system("clear");
            opt1();
            return 0;
            break;
        case 2:
            system("clear");
            opt2();
            return 0;
            break;
        case 3:
            system("clear");
            opt3();
            return 0;
            break;
        case 4:
            system("clear");
            opt4();
            return 0;
            break;
        case 5:
            system("clear");
            opt5();
            return 0;
            break;
        default:
            printf("Wrong choice. Exiting...");
            logs(1, "User chose wrong choice. The program will exit.");
            exit(1);
    }
    return 0;
}
int main() {
    char str2[256];
    int choice = 0;
    system("clear");
    init();
    logs(0, "---------- NEW LOG ENTRY ----------\n");
    logs(0, "Lincrash Ver. %s", VER);
    printf("Welcome to LinCrash %s!\n", VER);
    if (DEBUG_MODE == 1) {
        printf("You're in DEBUG mode. (Your version of Lincrash was compiled with debug mode)\n");
        logs(0, "Debug mode is active.");
    }
    if (PROT_DISABLE == 1) {
        printf("This version of LinCrash was compiled with protection mode. As this program includes harmful code, the program will halt.\n");
        logs(1, "Protection mode is active. The program cannot continue.");
        exit(1);
    }
    getroot();
    printf("You have 4 options:\n");
    printf("The options are:\n");
    for (int i = 1; i <= 5; i++) {
        switch (i) {
            case 1:
                printf("1: MCE hardware corruption\n [User-Kernel space]\n");
                break;
            case 2:
                printf("2: General Protection Fault (#GP) [User space]\n");
                break;
            case 3:
                printf("3: Null Pointer Dereference / Segmentation fault [User space]\n");
                break;
            case 4:
                printf("4: Kernel-level errors (%i choices) [Kernel space]\n", KRNLCH);
                break;
            case 5:
                printf("5: Memory leak via memory allocation");
                break;
        }
        printf("Press enter to continue...\n");
        getchar();
	system("clear");
    }
    printf("Choice? ");
    scanf("%i", &choice);
    snprintf(str2, sizeof(str2), "User chose choice: %i", choice);
    logs(0, str2);
    ch(choice);
    return 0;
}

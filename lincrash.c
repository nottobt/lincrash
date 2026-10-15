// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * LinCrash (Short for Linux Crash)
 * Ver. 1.1.0b (Major 1, Minor 1, Patch 0, beta)
 * MAIN C FILE <lincrash.c>
 */

#include <include/lincrash.h>

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
    }
    return 0;
}
int main() {
    int choice = 0;
    system("clear");
    printf("Welcome to LinCrash %s!\n", ver);
    if (DEBUG_MODE == 1) {
        printf("You're in DEBUG mode. (Your version of Lincrash was compiled with debug mode)\n");
    }
    printf("You have 4 options:\n");
    printf("The options are:\n");
    for (int i = 1; i <= 4; i++) {
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
                printf("4: Kernel-level errors (%i choices) [Kernel space]\n", krnlch);
                break;
        }
        printf("Press enter to continue...\n");
        getchar();
	system("clear");
    }
    printf("Choice? ");
    scanf("%i", &choice);
    ch(choice);
    return 0;
}

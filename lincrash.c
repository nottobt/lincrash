// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 NotToBT <nottobf@gmail.com>

/*
 * LinCrash (Short for Linux Crash)
 * Ver. 1.0.1b (Major 0, Minor 0, Patch 1)
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
    printf("You have 4 options:\n");
    printf("The options are:\n");
    for (int i = 1; i <= 4; i++) {
        switch (i) {
            case 1:
                printf("1: MCE hardware corruption\n");
                break;
            case 2:
                printf("2: General Protection Fault (#GP)\n");
                break;
            case 3:
                printf("3: Null Pointer Dereference / Segmentation fault\n");
                break;
            case 4:
                printf("4: Kernel-level errors (%i choices)\n", krnlch);
                break;
        }
        printf("Press enter to continue...\n");
        getchar();
    }
    scanf("%i", &choice);
    ch(choice);
    return 0;
}

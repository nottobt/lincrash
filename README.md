# LinCrash (Linux Crash Simulation Utility)

---

## 📌 Overview

**LinCrash** is an experimental system-testing utility designed for Linux systems. It allows developers and kernel engineers to test and develop several error codes, and several other bugs to test.

---

> ## ⚠️ DISCLAIMER: SYSTEM INSTABILITY AND DATA LOSS

> Running **LinCrash**, particularly Option 4, may destabilize, crash, or panic your operating system's kernel.

> **Unsaved data may be lost.** Some operations may cause system-wide disruption or require a reboot.

> Run LinCrash only in a disposable virtual machine or other isolated test environment. Do not run it on a production system or a machine containing important data.

> The author and contributors provide this software without warranty, subject to the terms of the GNU General Public License, version 2.0 only (GPL-2.0-only). They disclaim liability to the extent permitted by applicable law. See the accompanying `LICENSE` file for the complete license terms.

---

## 🚀 Features

* **MCE Hardware Corruption (`MADV_HWPOISON`):** Flags simulated defective RAM pages in user-space to test kernel Machine Check Exception handling.
* **General Protection Fault (#GP):** Dereferences non-canonical virtual addresses (`0xFFFF6FFFFFFFFFFF`) to provoke memory management unit (MMU) faults.
* **User-Space Null Pointer Dereference:** Triggers clean segmentation faults (`SIGSEGV`).
* **Dynamic Kernel Module Generator:** Automatically builds dynamic Makefiles to compile and load kernel-level bugs
  * `ko1.ko` – Kernel Warning (`WARN()`)
  * `ko2.ko` – Kernel Bug (`BUG()`)
  * `ko3.ko` – Kernel Oops via Null Pointer Dereference
  * `ko4.ko` – Kernel Panic (`panic()`)
* **Secure Boot Check:** Built-in parser for EFI variables to verify Secure Boot status before attempting kernel module insertions.

* Prerequisites
Ensure you have the essential C toolchain and Linux kernel headers installed:
```bash
# On Debian/Ubuntu systems:
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r) git
```

Compile the executable.
```bash
git clone https://github.com/nottobt/lincrash.git
cd lincrash
gcc lincrash.c -O3 -I. -o lincrash
```
There are multiple flags for gcc.
   -DDEBUG - Enters the program in debug mode.
   -DPROT - Protected mode - refuses to execute anything and exits.
Run this with root privileges (required for madvise and several memory-editing/allocating tools)
```bash
sudo ./lincrash
```

# LinCrash (Linux Crash Simulation Utility)

---

## 📌 Overview

**LinCrash** is an experimental system-testing utility designed for Linux systems. It allows developers and kernel engineers to test and develop several error codes, and several other bugs to test.

---

## ⚠️ Disclaimer

> **WARNING:**  
> Running **LinCrash**—especially Option 4—will destabilise, crash, or panic your operating system kernel. Unsaved files WILL be lost (if you haven't saved your files), and the author and developers of this program are not responsible for the instability and panics caused. (Check the GPL license clause for more info)

---

## 🚀 Features

* **MCE Hardware Corruption (`MADV_HWPOISON`):** Flags simulated defective RAM pages in user-space to test kernel Machine Check Exception handling.
* **General Protection Fault (#GP):** Dereferences non-canonical virtual addresses (`0xFFFF6FFFFFFFFFFF`) to provoke memory management unit (MMU) faults.
* **User-Space Null Pointer Dereference:** Triggers clean segmentation faults (`SIGSEGV`).
* **Dynamic Kernel Module Generator:** Automatically builds dynamic Makefiles to compile and load kernel-level bugs
  * `ko1.ko` – Kernel Warning (`WARN()`)
  * `ko2.ko` – Kernel Bug (`BUG()`)
  * `ko3.ko` – Kernel Oops (`oops_enter / die`)
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
gcc lincrash.c -I. -o lincrash
```

Run this with root privileges (required for madvise and several memory-editing/allocating tools)
```bash
sudo ./lincrash
```

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/panic.h>

static int __init lincrash_init(void) {
    pr_alert("Triggering Kernel WARN...\n");
    int lincrashval = 1;
    WARN(lincrashval == 1, "Lincrash Intentional Panic");
    return -EINVAL;
}

static void __exit lincrash_exit(void) {
    pr_info("Unloaded!\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>")


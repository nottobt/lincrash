#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/kallsyms.h>

typedef void (*die_t)(const char *str, struct pt_regs *regs, long err);


static int __init lincrash_init(void) {
    struct pt_regs regs;
    die_t oops;
    memset(&regs, 0, sizeof(struct pt_regs));
    oops = (die_t)kallsyms_lookup_name("die");
    if (!oops) {
        pr_err("lincrash: Could not lookup symbol: die(). Exiting...\n");
        return -EINVAL;
    }
    pr_alert("Triggering Kernel Oops...\n");
    oops("Lincrash Intentional Oops", &regs, 1);
    return -EINVAL;
}
static void __exit lincrash_exit(void) {
    pr_info("Unloaded!\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>")

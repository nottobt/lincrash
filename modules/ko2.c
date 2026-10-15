#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>

static int __init lincrash_init(void) {
    pr_alert("Triggering Kernel BUG...\n");
    BUG();
    return -EINVAL;
}

static void __exit lincrash_exit(void) {
    pr_info("Unloaded!\n");
}

module_init(lincrash_init);
module_exit(lincrash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>");

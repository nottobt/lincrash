#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bug.h>

static int __init lincrash_init(void) {
    int zero = 0;
    int result;
    pr_alert("Triggering Kernel Oops...\n");
    result = 42 / zero;
    pr_alert("Result: %d\n", result);
    return 0;
}
static void __exit lincrash_exit(void) {
    pr_info("Unloaded!\n");
}
module_init(lincrash_init);
module_exit(lincrash_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("NotToBT <nottobf@gmail.com>");
MODULE_DESCRIPTION("LinCrash Module 3 - Divide by Zero Oops");

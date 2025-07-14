// capslock_notifier.c

#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#define AUTHOR "Burak Ozter"
#define DESCRIPTION "Sample kernel module that tracks Caps Lock state"

static int __init capslock_notifier_init(void)
{
    printk("Init capslock_notifier.c\n");
    return 0;
}

static void __exit capslock_notifier_exit(void)
{
    printk("exit capslock_notifier.c\n");
}

module_init(capslock_notifier_init);
module_exit(capslock_notifier_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);

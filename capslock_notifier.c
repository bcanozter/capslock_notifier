// capslock_notifier.c

#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#define AUTHOR "Burak Ozter"
#define DESCRIPTION "Sample kernel module that tracks Caps Lock state"

#define KEY_CAPSLOCK 58
static bool is_capslock_on = false;
static void input_handler_event_fn(struct input_handle *handle, unsigned int type, unsigned int code, int value)
{
    if (type == EV_KEY && code == KEY_CAPSLOCK)
    {
        printk("is_capslock_on %d",value);
        is_capslock_on = value;
    }
}


//Ref. https://github.com/torvalds/linux/blob/155a3c003e555a7300d156a5252c004c392ec6b0/drivers/tty/vt/keyboard.c#L1566
static int input_handler_connect_fn(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id)
{
    struct input_handle *handle;
    int error;

    if (!test_bit(EV_KEY, dev->evbit) || !test_bit(KEY_CAPSLOCK, dev->keybit))
        return -ENODEV;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "capslock_notifier_handle";

    error = input_register_handle(handle);
    if (error)
        goto err_free_handle;

    error = input_open_device(handle);
    if (error)
        goto err_unregister_handle;

    is_capslock_on = test_bit(LED_CAPSL, dev->led);
    printk("Initial Caps Lock state: %s (device: %s)\n", is_capslock_on ? "ON" : "OFF", dev->name);

    return 0;

err_unregister_handle:
    input_unregister_handle(handle);
err_free_handle:
    kfree(handle);
    return error;
}

static void input_handler_disconnect_fn(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id input_ids[] = {
    {.driver_info = 1},
    {},
};

static struct input_handler input_handler = {
    .event = input_handler_event_fn,
    .connect = input_handler_connect_fn,
    .disconnect = input_handler_disconnect_fn,
    .name = "capslock_notifier_input_handler",
    .id_table = input_ids,
};

static int __init capslock_notifier_init(void)
{
    printk("Init capslock_notifier.c\n");
    return input_register_handler(&input_handler);
}

static void __exit capslock_notifier_exit(void)
{
    printk("exit capslock_notifier.c\n");
    input_unregister_handler(&input_handler);
}

module_init(capslock_notifier_init);
module_exit(capslock_notifier_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);

// capslock_notifier.c

#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define AUTHOR "Burak Ozter"
#define DESCRIPTION "Sample kernel module that tracks Caps Lock state"

#define PROC_FILENAME "capslock_state"
static bool is_capslock_on = false;
static struct input_handler input_handler;
static struct proc_dir_entry *proc_file;

static struct delayed_work led_check_work;
static struct input_dev *input_dev_handle;

static void led_check_work_fn(struct work_struct *work)
{
    if (input_dev_handle)
    {
        if (test_bit(LED_CAPSL, input_dev_handle->ledbit))
        {
            bool is_capslock_on = test_bit(LED_CAPSL, input_dev_handle->led);
            printk(KERN_DEBUG "(%s): Caps Lock LED state: %s\n", input_dev_handle->name, is_capslock_on ? "ON" : "OFF");
        }
        else
        {
            printk(KERN_WARNING "This device(%s) does NOT have a Caps Lock LED.\n", input_dev_handle->name);
        }
    }
}

static void input_handler_event_fn(struct input_handle *handle, unsigned int type, unsigned int code, int value)
{
    if (type == EV_KEY && code == KEY_CAPSLOCK && value == 0)
    {
        input_dev_handle = handle->dev;
        schedule_delayed_work(&led_check_work, msecs_to_jiffies(200));
    }
}

// Ref. https://github.com/torvalds/linux/blob/155a3c003e555a7300d156a5252c004c392ec6b0/drivers/tty/vt/keyboard.c#L1566
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
    printk(KERN_DEBUG "Connected input device: %s\n", dev->name);
    error = input_register_handle(handle);
    if (error)
        goto err_free_handle;

    error = input_open_device(handle);
    if (error)
        goto err_unregister_handle;

    is_capslock_on = test_bit(LED_CAPSL, dev->led);
    printk(KERN_DEBUG "(%s): Initial Caps Lock state: %s \n", dev->name, is_capslock_on ? "ON" : "OFF");

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
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = {BIT_MASK(EV_KEY)},
    },
    {},
};

static struct input_handler input_handler = {
    .event = input_handler_event_fn,
    .connect = input_handler_connect_fn,
    .disconnect = input_handler_disconnect_fn,
    .name = "capslock_notifier_input_handler",
    .id_table = input_ids,
};

static ssize_t capslock_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char state[3] = {is_capslock_on ? '1' : '0', '\n', '\0'};
    return simple_read_from_buffer(buf, count, ppos, state, sizeof(state));
}

static const struct proc_ops proc_file_ops = {
    .proc_read = capslock_proc_read,
};

static int __init capslock_notifier_init(void)
{
    printk(KERN_INFO "Init capslock_notifier module\n");
    int err;
    proc_file = proc_create(PROC_FILENAME, 0644, NULL, &proc_file_ops);
    if (NULL == proc_file)
    {
        pr_alert("Error:Could not initialize /proc/%s\n", PROC_FILENAME);
        return -ENOMEM;
    }

    INIT_DELAYED_WORK(&led_check_work, led_check_work_fn);

    err = input_register_handler(&input_handler);
    if (err)
    {
        proc_remove(proc_file);
        return err;
    }

    return 0;
}

static void __exit capslock_notifier_exit(void)
{
    printk(KERN_INFO "exit capslock_notifier module\n");
    input_unregister_handler(&input_handler);
    if (proc_file)
    {
        proc_remove(proc_file);
        proc_file = NULL;
    }
}

module_init(capslock_notifier_init);
module_exit(capslock_notifier_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);

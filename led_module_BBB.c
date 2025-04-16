#include <linux/module.h>
#include <linux/printk.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/string.h>

#define GPIO_0_BASE_ADDRESS 0x44E07000
#define GPIO_0_SIZE         0x1000
#define GPIO_DATA_OUT       0x13c
#define GPIO_OE             0x134

static ssize_t led_module_write(struct file *file, const char *user_buffer, size_t len, loff_t *pos);
static ssize_t led_module_read(struct file *file, char __user *buf, size_t len, loff_t *pos);
static int led_module_open(struct inode *inode, struct file *file);
static int led_module_close(struct inode *inodep, struct file *filp);

static void led_on(void);
static void led_off(void);

static const struct file_operations led_module_fops = 
{
    .owner			= THIS_MODULE,
    .write			= led_module_write,
	  .read			= led_module_read,
    .open			= led_module_open,
    .release		= led_module_close
};

struct miscdevice led_module = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "led_module",
    .fops = &led_module_fops,
};

void __iomem * GPIO_0_Virtual_Ptr = NULL;


static void led_on(void)
{
    u32 data;
    data |= (1 << 31);
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_DATA_OUT));
}

static void led_off(void)
{
    u32 data;
    readl(GPIO_0_Virtual_Ptr + GPIO_DATA_OUT);
    data &= ~(1 << 31); 
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_DATA_OUT));
}

static ssize_t led_module_write(struct file *file, const char *user_buffer, size_t len, loff_t *pos)
{
    char user_data[10];
    memset(user_data, 0x0, 10);

    if (copy_from_user(user_data, user_buffer, len))
        return 0;

    printk("user write: %s \n", user_data);

    if (strncmp(user_data, "on", 2) == 0)
    {
        printk("write on");
        led_on();
    }

    if (strncmp(user_data, "off", 3) == 0)
    {
        printk("write off");
        led_off();
    }

    return len;
}

static ssize_t led_module_read(struct file *file, char __user *buf, size_t len, loff_t *pos)
{
    int ret = 0;
    u32 data = 0;

    pr_info("%d %s\n", __LINE__, __func__);

    data = readl(GPIO_0_Virtual_Ptr + GPIO_DATA_OUT);
    data = (data >> 31) & 0xff;

    ret = copy_to_user(&buf[0], &data, 1);
    if(ret != 0)
    {
        pr_info("Copy to user failed\n");
        return -1;
    }

    return 1;
}

static int led_module_open(struct inode *inode, struct file *file)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 0;
}

static int led_module_close(struct inode *inodep, struct file *filp)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 0;
}

/* Module Init */
static int __init led_module_init(void)
{
    int ret = 0;
    u32 data = 0;

    pr_info("led module BBB init\n");

    ret = misc_register(&led_module);
    if(ret != 0)
    {
        pr_info("misc failed\n");
        return -1;
    }

    GPIO_0_Virtual_Ptr = ioremap(GPIO_0_BASE_ADDRESS, GPIO_0_SIZE);
    if(GPIO_0_Virtual_Ptr == NULL)
    {
        pr_info("ioremap failed\n");
        return -1;
    }
    pr_info("GPIO_0_Virtual_Ptr = %p\n", GPIO_0_Virtual_Ptr);

    /* Configure GPIO as output */
    data = readl(GPIO_0_Virtual_Ptr + GPIO_OE);
    data &= ~(1 << 31);
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_OE));

    return 0;
}

/* Module Exit */
static void __exit led_module_exit(void)
{
    misc_deregister(&led_module);

    pr_info("GPIO_0_Virtual_Ptr = %p\n", GPIO_0_Virtual_Ptr);

    pr_info("led module BBB exit\n");
}

module_init(led_module_init);
module_exit(led_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Huu Kim Quyen - 21161263");
MODULE_DESCRIPTION("led kernel module");

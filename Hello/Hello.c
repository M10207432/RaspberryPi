#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>

//Define GPIO & SETTING
#define GPIO_BASE 0xf2200000
#define BLOCK	4096

#define INP_GPIO(b) *(volatile unsigned int *)(0x00+b) &=(unsigned int)0xFFFF8FFF
#define OUT_GPIO(b) *(volatile unsigned int *)(0x00+b) |=(unsigned int)0x00001000

#define GPIO_SET(b) *(volatile unsigned int *)(0x0000001C+b) |= 0x10
#define GPIO_CLR(b) *(volatile unsigned int *)(0x00000028+b) |= 0x10

//Global value
struct timer_list led_timer;
char kbledstatus = 0;
int status=0;

static void hello_timer(unsigned long ptr){
	printk("Timer\n");
	
	led_timer.expires = jiffies + HZ;
	add_timer(&led_timer);
}

static int hello_init(void){
	printk("Hello\n");

	OUT_GPIO(GPIO_BASE);
	
	init_timer(&led_timer);
	led_timer.function = hello_timer;
	led_timer.data = (unsigned long) &kbledstatus;
	led_timer.expires = jiffies + HZ;
	add_timer(&led_timer);

	return 0;
}

static void hello_exit(void){
	del_timer(&led_timer);
	printk("Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TheShellWave");
MODULE_DESCRIPTION("Raspberry Driver");
MODULE_VERSION("0.1")

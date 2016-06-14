#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define LED 27
#define BUTTON 23
#define GPIO_INT_VECTOR "BTN INT"
#define DEV_NAME "Device"

MODULE_LICENSE("GPL");

static short int button_irq = 0;
static unsigned long flags = 0;
static int led_trigger = 0;


/*====================================
	Interrupt service routine
	
	For interrupt controller 
	choose service to work
=====================================*/
static irqreturn_t BTN_ISR(int irq, void *data){
	local_irq_save(flags);			//save cpu flags ,and then disable cpu interrupt
	printk("BTN ISR !!!\n");	
	led_trigger =led_trigger ? (0):(1);
	local_irq_restore(flags);		//restore cpu flags and enable cpu interrupt

	return IRQ_HANDLED;
}

int init_module(void){
	printk("Module Button Interrupt\n");

	//---------------------------------LED set as output
	if(gpio_is_valid(LED) < 0){
		return -1;	
	}
	if(gpio_request(LED, "LED") < 0){
		return -1;
	}
	gpio_direction_output(LED, 0);


	//-----------------------------BTN set as input and interrupt
	if(gpio_request(BUTTON, "BUTTON") < 0){
		return -1;
	}
	if(gpio_is_valid(BUTTON) < 0){
		return -1;
	}
	if((button_irq=gpio_to_irq(BUTTON)) < 0){
		return -1;
	}
	if(request_irq(button_irq, BTN_ISR, IRQF_TRIGGER_RISING, GPIO_INT_VECTOR, DEV_NAME)){
		return -1;
	}
	
	return 0;
}



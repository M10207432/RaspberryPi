#include <asm/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <mach/platform.h>

#define LED 5
#define BUTTON 24
#define GPIO_INT_VECTOR "BTN INT"
#define DEV_NAME "Device"

MODULE_LICENSE("GPL");

static short int button_irq = 0;
static unsigned long flags = 0;
static bool led_trigger = 0;

/*=================================
		GPIO address order
==================================*/
struct GPIO_REG{
	uint32_t GPFSEL[6];	//Set function=> 32bit {Reserve:Pin9:Pin8...Pin0} (Each Pin with 3 bits) 000->input 001->output
	uint32_t Reserved1;
	uint32_t GPSET[2];	//Pin n as bit id
	uint32_t Reserved2;
	uint32_t GPCLR[2];	//Pin n as bit id
	uint32_t Reserved3;
	uint32_t GPLEV[2];	//Get the Pin status
};

struct GPIO_REG *reg_gpio;

/*=================================
	Reg Setting (Input/Output)
		    (Set Value)
==================================*/
static void GPIO_SET(int gpio, int status){
	reg_gpio->GPFSEL[gpio/10]= (reg_gpio->GPFSEL[gpio/10] & ~(7<< ((gpio % 10)*3))) | ((status << ((gpio % 10)*3)));	
}

static void GPIO_VALUE(int gpio, bool value){
	if(value){
		reg_gpio->GPSET[gpio/32]=(1<<(gpio%32));
	}else{
		reg_gpio->GPCLR[gpio/32]=(1<<(gpio%32));
	}
}

/*==============================
	UART Function
===============================*/


/*====================================
	Interrupt service routine
	
	For interrupt controller 
	choose service to work
=====================================*/
static irqreturn_t BTN_ISR(int irq, void *data){
	local_irq_save(flags);			//save cpu flags ,and then disable cpu interrupt
	printk("BTN ISR !!!\n");	
	//gpio_set_value(LED,led_trigger);
	GPIO_VALUE(LED,led_trigger);
	led_trigger =led_trigger ? (0):(1);
	local_irq_restore(flags);		//restore cpu flags and enable cpu interrupt

	return IRQ_HANDLED;
}

int init_module(void){
	printk("Module Button Interrupt\n");

	//---------------------------------LED set as output
	reg_gpio=(struct GPIO_REG*) __io_address(GPIO_BASE);
	printk("reg_gpio=%x\n",GPIO_BASE);
	GPIO_SET(LED,1); //set as output

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
	if(request_irq(button_irq, BTN_ISR, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, GPIO_INT_VECTOR, DEV_NAME)){
		return -1;
	}
	
	return 0;
}

void cleanup_module(void){
	GPIO_SET(LED,0);

	free_irq(button_irq, DEV_NAME);
	gpio_free(BUTTON);
}

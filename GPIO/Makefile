obj-m += GPIO.o

KERNEL=/lib/modules/$(shell uname -r)/build
KERNEL2=/lib/modules/4.1.19+/build
KERNEL3=/usr/src/linux-headers-4.1.19-v7+

all:
	make -C $(KERNEL3) M=$(shell pwd) modules V=1 ARCH=arm
clean:
	make -C $(KERNEL3) M=$(shell pwd) clean


MODULE_NAME = driver

obj-m += driver.o
all:
	sudo rmmod $(MODULE_NAME)
	rm -rf *.o *.ko *.mod.c *.order *.symvers *.mod.o
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod ./$(MODULE_NAME).ko
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
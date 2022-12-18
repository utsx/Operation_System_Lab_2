obj-m += driver.o
all:
	rm -rf *.o *.ko *.mod.c *.order *.symvers *.mod.o
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
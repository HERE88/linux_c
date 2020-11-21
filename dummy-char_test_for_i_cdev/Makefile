obj-m := dummy-char_test_for_i_cdev.o

KERNELDIR := /lib/modules/$(shell uname -r)/build

all default: modules
install: modules_install

modules modules_install help clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) ARCH=x86 CROSS_COMPILE= $@

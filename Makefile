PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR = /usr/src/linux-headers-$(KVERSION)/

MODULE_NAME = demo
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	$(CC) C_file_109000000.c -o test
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm test
obj-m := led_module_BBB.o

KERNEL_SRC = /home/quyen/work/bbb/kernelbuildscripts/KERNEL

CROSS = /home/quyen/work/bbb/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

all:
        make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL_SRC) M=$(PWD) modules

build_app:
        /home/quyen/work/bbb/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc -o led_app led_app.c

clean:
        make -C $(KERNEL_SRC) M=$(shell pwd) clean
        rm led_app

copy:
        scp led_module_BBB.ko debian@192.168.137.2:/home/debian/es/led_module/led_on_off
        scp led_app debian@192.168.137.2:/home/debian/es/led_module/led_on_off

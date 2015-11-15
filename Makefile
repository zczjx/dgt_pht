CROSS_COMPILE ?= arm-linux-
CC := gcc  
OBJS := main.o  fb_show_font.o
PWD = $(shell pwd)
CFLAGS += -I $(PWD)/inc
CFLAGS += -I /mnt/sdb/SM210_DEV_ENV/toolchain/opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/sys-root/usr/include/freetype2 
CFLAGS += -lfreetype -lm 
CFLAGS += -finput-charset=GBK -fexec-charset=UTF-8

all:$(OBJS)
	$(CROSS_COMPILE)$(CC)  -o app $^ $(CFLAGS)

main.o: main.c
	$(CROSS_COMPILE)$(CC)  -o $@ -c $^ $(CFLAGS)

fb_show_font.o: fb_show_font.c
	$(CROSS_COMPILE)gcc  -o $@ -c $^   $(CFLAGS)

clean:
	-rm app
	-rm *.o 

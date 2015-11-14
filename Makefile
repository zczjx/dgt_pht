CROSS_COMPILE ?= arm-linux-
CC := gcc
OBJS := main.o
PWD = $(shell pwd)
CFLAGS += -I $(PWD)/inc

all:$(OBJS)
	$(CROSS_COMPILE)$(CC)  $(CFLAGS) -o app $^

main.o: fb_show_font.c
	$(CROSS_COMPILE)$(CC)  $(CFLAGS) -o $@ -c $^

clean:
	-rm app
	-rm *.o 

TOPDIR  := $(shell cd ..; pwd)

CC=gcc
CFLAGS=-Wall -Wextra -L. -lftd2xx -ldl -lpthread -Wl,-rpath /usr/local/lib

APP = messages

all: $(APP)

binary.h: ../arp-msg.bin
	bin2c ../arp-msg.bin binary.h binary

$(APP): main.c arp.c binary.h
	$(CC) arp.c main.c -o $(APP) $(CFLAGS)
	
clean:
	rm -f *.o $(APP) binary.h

run: messages
	sudo modprobe -r ftdi_sio usbserial
	sudo ./messages

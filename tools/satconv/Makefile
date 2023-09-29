CC = gcc

TARGET = satconv
CFLAGS = -g -std=gnu99 -Wall -fsanitize=address -DEZXML_NOMMAP

OBJECTS = satconv.o ezxml.o qdbmp.o map.o sprite.o tile.o

all: $(TARGET)

clean:
	rm *.o

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

CC=gcc
TARGET=wakepc

wakepc: wakepc.c
	$(CC) -o $@ $^

install:
	install .pclist ~/
	install $(TARGET) /usr/sbin/

.PHONY: clean

clean:
	rm -rf *.o wakepc

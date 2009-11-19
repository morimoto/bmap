OBJS	= main.o frame.o bitmap.o
CFLAGS	= -Wall -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion $(EXTR)
LDFLAGS	= -static

-include .config

CC	= $(CROSS)gcc
STRIP	= $(CROSS)strip

bmap: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) $@

clean:
	rm -f $(OBJS) bmap

OBJS	= main.o frame.o bitmap.o
CFLAGS	= -Wall -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion $(EXTR)
LDFLAGS	= -static

-include .config

CC	= $(CROSS)gcc
STRIP	= $(CROSS)strip

.c.o:
	@echo "CC $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -c $<

bmap: $(OBJS)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	@$(STRIP) $@
	@echo $@

clean:
	@rm -f $(OBJS) bmap

mrproper: clean
	@rm -f .config

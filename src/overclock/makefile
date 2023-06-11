ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = overclock

CC = $(CROSS_COMPILE)gcc
CFLAGS	= -Os -lrt -ldl -Wl,--gc-sections -s
CFLAGS  += -I. -I../common -DPLATFORM=\"$(UNION_PLATFORM)\"

all:
	$(CC) $(TARGET).c ../common/direct_overclock.c -o $(TARGET).elf $(CFLAGS)
clean:
	rm -rf $(TARGET).elf

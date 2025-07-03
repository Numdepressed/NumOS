CC = gcc
CFLAGS = -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs
LDFLAGS = -m elf_i386 -Ttext 0x10000

OBJS = kernel_entry.o kernel.o drivers/screen.o drivers/keyboard.o

all: kernel.bin

kernel_entry.o: kernel_entry.asm
	nasm -f elf32 kernel_entry.asm -o $@

kernel.o: kernel.c kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

drivers/screen.o: drivers/screen.c drivers/screen.h
	$(CC) $(CFLAGS) -c $< -o $@

drivers/keyboard.o: drivers/keyboard.c drivers/keyboard.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o kernel.elf
	objcopy -O binary kernel.elf kernel.bin

clean:
	rm -f *.o drivers/*.o *.bin *.elf

.PHONY: all clean

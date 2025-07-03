STAGE2_OBJS = stage2.o gdt.o print.o

all: mbr.bin stage2.bin

mbr.bin: mbr.asm
	nasm -f bin mbr.asm -o mbr.bin

stage2.o: stage2.asm
	nasm -f elf32 stage2.asm -o stage2.o

gdt.o: gdt.asm
	nasm -f elf32 gdt.asm -o gdt.o

print.o: print.asm
	nasm -f elf32 print.asm -o print.o

stage2.bin: $(STAGE2_OBJS) stage2.ld
	ld -m elf_i386 -T stage2.ld $(STAGE2_OBJS) -o stage2.elf
	objcopy -O binary stage2.elf stage2.bin

clean:
	rm -f *.o *.bin *.elf

.PHONY: all clean
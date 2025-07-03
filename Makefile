all: hdd.img

hdd.img: ../boot/mbr.bin ../boot/stage2.bin ../kernel/kernel.bin
	# Create 10MB disk image
	dd if=/dev/zero of=hdd.img bs=1024 count=10240
	# Write MBR to first sector
	dd if=../boot/mbr.bin of=hdd.img bs=512 count=1 conv=notrunc
	# Write Stage2 to second sector
	dd if=../boot/stage2.bin of=hdd.img bs=512 seek=1 conv=notrunc
	# Write kernel starting at sector 3
	dd if=../kernel/kernel.bin of=hdd.img bs=512 seek=2 conv=notrunc

clean:
	rm -f hdd.img

.PHONY: all clean
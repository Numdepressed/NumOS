# Root Makefile
BOOT_DIR = boot
KERNEL_DIR = kernel
TOOLS_DIR = tools

all: $(BOOT_DIR)/mbr.bin $(BOOT_DIR)/stage2.bin $(KERNEL_DIR)/kernel.bin $(TOOLS_DIR)/hdd.img

$(BOOT_DIR)/mbr.bin:
	make -C $(BOOT_DIR) mbr.bin

$(BOOT_DIR)/stage2.bin:
	make -C $(BOOT_DIR) stage2.bin

$(KERNEL_DIR)/kernel.bin:
	make -C $(KERNEL_DIR) kernel.bin

$(TOOLS_DIR)/hdd.img: $(BOOT_DIR)/mbr.bin $(BOOT_DIR)/stage2.bin $(KERNEL_DIR)/kernel.bin
	make -C $(TOOLS_DIR) hdd.img

run: $(TOOLS_DIR)/hdd.img
	qemu-system-i386 -drive file=$(TOOLS_DIR)/hdd.img,format=raw,index=0,media=disk

debug: $(TOOLS_DIR)/hdd.img
	qemu-system-i386 -drive file=$(TOOLS_DIR)/hdd.img,format=raw,index=0,media=disk -s -S

clean:
	make -C $(BOOT_DIR) clean
	make -C $(KERNEL_DIR) clean
	make -C $(TOOLS_DIR) clean

.PHONY: all clean run debug
TARGET_ISO := los.iso
TARGET_BIN := los.bin
ISO_DIR := isodir

all: $(TARGET_ISO)

run: kernel
	qemu-system-i386 -kernel $(TARGET_BIN)

clean:
	cd kernel; make clean

	-rm -r build
	-rm -r $(ISO_DIR)
	-rm los.iso

kernel:
	cd kernel; make

.PHONY: all run clean kernel

$(TARGET_ISO): $(ISO_DIR)/boot/los.bin $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR)

$(ISO_DIR)/boot/los.bin: $(TARGET_BIN) | $(ISO_DIR)/boot/grub
	cp $< $(ISO_DIR)/boot

$(ISO_DIR)/boot/grub/grub.cfg: | $(ISO_DIR)/boot/grub
	echo 'menuentry "los" {' >> $@
	echo '    multiboot /boot/los.bin' >> $@
	echo '}' >> $@

$(ISO_DIR)/boot/grub:
	mkdir -p $@

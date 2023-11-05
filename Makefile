TARGET_ISO := los.iso

BUILD_DIR := build
SRC_DIR := kernel
ISO_DIR := isodir
DEP_DIR := $(BUILD_DIR)/.deps

AS := i686-elf-as

CXX := i686-elf-g++
CXXFLAGS := -fno-exceptions -fno-rtti -ffreestanding -nostdlib -O2 -Wall -Wextra
CXX_DEP_FLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.cpp.Td

SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp' -or -name '*.s')
OBJS := $(SRCS:$(SRC_DIR)/%=$(BUILD_DIR)/%.o)

all: $(TARGET_ISO)

run: los.bin
	qemu-system-i386 -kernel $<

clean:
	rm -r $(BUILD_DIR)
	rm -r $(ISO_DIR)
	rm los.bin
	rm los.iso

.PHONY: all run clean

# --- #
# BIN #
# --- #

los.bin: linker.ld $(OBJS)
	$(CXX) $(CXXFLAGS) -T $< -o $@ $(OBJS) -lgcc

$(BUILD_DIR)/%.s.o: $(SRC_DIR)/%.s  | $(BUILD_DIR)
	$(AS) $< -o $@

%.o: %.cpp # Delete the default implicit rule.
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.cpp.d | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXX_DEP_FLAGS) $(CXXFLAGS) -c $< -o $@

# During compilation dependencies are written to a temporary file in order to
# avoid generating broken dependency files or incorrect timestamps.
# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# Move them to a non-temporary file only after the compilation has
# succeeded.
	mv -f $(DEP_DIR)/$*.cpp.Td $(DEP_DIR)/$*.cpp.d
	touch $@

DEP_FILES := $(SRCS:$(SRC_DIR)/%=$(DEP_DIR)/%.d)
$(DEP_FILES):
	mkdir -p $(dir $@)

include $(wildcard $(DEP_FILES))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- #
# ISO #
# --- #

$(TARGET_ISO): $(ISO_DIR)/boot/los.bin $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR)

$(ISO_DIR)/boot/los.bin: los.bin | $(ISO_DIR)/boot/grub
	cp los.bin $(ISO_DIR)/boot

$(ISO_DIR)/boot/grub/grub.cfg: | $(ISO_DIR)/boot/grub
	echo 'menuentry "los" {' >> $@
	echo '    multiboot /boot/los.bin' >> $@
	echo '}' >> $@

$(ISO_DIR)/boot/grub:
	mkdir -p $@

OBJ_DIR = .objs
SRC_DIR = src

ISO_NAME = vertex.iso

RESOURCES = vertex-resources
TOOLCHAIN = vertex-toolchain
BUILD = vertex-build

AS = nasm
AFLAGS = -f elf

LD = $(TOOLCHAIN)/bin/i686-elf-ld
CC = $(TOOLCHAIN)/bin/i686-elf-gcc
GDB = gdb
OBJCOPY = $(TOOLCHAIN)/bin/i686-elf-objcopy

CFLAGS = -g -ffreestanding -std=gnu99 -Wall -Wextra -I./src/include
LDFLAGS = -ffreestanding -nostdlib -lgcc -T $(RESOURCES)/linker.ld

ISO_MAKER_WINDOWS = $(TOOLCHAIN)/bin/grub-mkrescue
ISO_MAKER_MACOS = i386-elf-grub-mkrescue
EMULATOR = qemu-system-i386

findfiles = $(foreach ext, c s, $(wildcard $(1)/*.$(ext)))
getobjs = $(foreach ext, c s, $(filter %.o,$(patsubst %.$(ext),%.o,$(1))))

PATHS = $(shell find $(SRC_DIR) -type d -print)
PATHS := $(foreach f,$(PATHS),$(if $(filter extern,$(subst /, ,$f)),,$f))

VERTEX_FILES = $(foreach path, $(PATHS), $(call findfiles, $(path)))

OBJECTS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(call getobjs, $(VERTEX_FILES)))
OBJECTS := $(foreach f,$(OBJECTS),$(if $(filter extern,$(subst /, ,$f)),,$f))

all: $(BUILD)/boot/vertex.bin $(ISO_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p `dirname $@`
	$(AS) $(AFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD)/boot/vertex.bin: $(OBJECTS)
	@mkdir -p `dirname $@`
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/boot/grub/grub.cfg: $(RESOURCES)/grub.cfg
	@mkdir -p `dirname $@`
	cp $^ $@

macos: $(BUILD)/boot/vertex.bin $(BUILD)/boot/grub/grub.cfg
	$(ISO_MAKER_MACOS) -d /usr/local/lib/grub/i386-pc -o $(ISO_NAME) $(BUILD)
	$(OBJCOPY) --only-keep-debug $(BUILD)/boot/vertex.bin vertex.debug

linux: $(BUILD)/boot/vertex.bin $(BUILD)/boot/grub/grub.cfg
	$(ISO_MAKER_WINDOWS) -d $(TOOLCHAIN)/lib/grub/i386-pc -o $(ISO_NAME) $(BUILD)


run: $(ISO_NAME)
	$(EMULATOR) -serial file:output.txt -m 512 -cdrom $^
	#$(GDB) --command=gdb.txt vertex-build/boot/vertex.bin

clean:
	@rm -rf $(OBJECTS) $(ISO_NAME) $(BUILD)/boot/vertex.bin

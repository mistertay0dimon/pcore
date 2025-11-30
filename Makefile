# Compilers and flags
CC=gcc
AS=gcc
CFLAGS=-m32 -ffreestanding -O2 -Wall -Wextra -Isrc/inc
LDFLAGS=-m32 -ffreestanding -O2 -nostdlib -T src/linker.ld

# Directories
SRC_DIR=src
BIN_DIR=bin
ISO_DIR=iso
GRUB_CFG=$(ISO_DIR)/boot/grub/grub.cfg

# Source files
C_SRCS=$(wildcard $(SRC_DIR)/*.c)
S_SRCS=$(wildcard $(SRC_DIR)/*.s)

# Object files
C_OBJS=$(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(C_SRCS))
S_OBJS=$(patsubst $(SRC_DIR)/%.s,$(BIN_DIR)/%.o,$(S_SRCS))
OBJS=$(C_OBJS) $(S_OBJS)

# ELF and ISO
KERNEL_ELF=$(BIN_DIR)/kernel.elf
ISO=$(BIN_DIR)/pcore.iso

# Default target
all: $(KERNEL_ELF) $(ISO)

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile C files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.s | $(BIN_DIR)
	$(AS) $(CFLAGS) -c $< -o $@

# Link ELF binary
$(KERNEL_ELF): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

# Create GRUB ISO
$(ISO): $(KERNEL_ELF) $(GRUB_CFG) | $(BIN_DIR)
	mkdir -p $(BIN_DIR)/iso/boot/grub
	cp $(KERNEL_ELF) $(BIN_DIR)/iso/boot/kernel.elf
	cp $(GRUB_CFG) $(BIN_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(BIN_DIR)/iso

# Clean all build files
clean:
	rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/*.elf $(BIN_DIR)/.iso $(BIN_DIR)/iso
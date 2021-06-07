ARMCCC = aarch64-linux-gnu
CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only

BUILD_DIR = .build
SRC_DIR = src

C_FILES = $(wildcard $(SRC_DIR)/*.c)
S_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o) $(S_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

all: kernel8.img

kernel8.img: linker.ld $(OBJ_FILES)
	$(ARMCCC)-ld -T linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMCCC)-objdump -d $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/kernel8.dsa
	$(ARMCCC)-nm -n $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/kernel8.map
	$(ARMCCC)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) -MMD -c $< -o $@

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

clean:
	rm -rf $(BUILD_DIR) *.img

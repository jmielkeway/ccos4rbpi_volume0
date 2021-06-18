phony := clean config exec
ifeq (,$(filter $(phony), $(MAKECMDGOALS)))

ifndef ARCH
$(error ARCH is not specified)
endif

ifndef BOARD
$(error BOARD is not specified)
endif

endif

ARMCCC = aarch64-linux-gnu
CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only

BUILD_DIR = .build
LIB_DIR = $(BUILD_DIR)/libs

BOARD_SRC_DIR = arch/$(ARCH)/board/$(BOARD)/
BOARD_OBJ_DIR = $(BUILD_DIR)/board/
BOARD_C_FILES = $(wildcard $(BOARD_SRC_DIR)/*.c)
BOARD_S_FILES = $(wildcard $(BOARD_SRC_DIR)/*.S)
BOARD_OBJ_FILES = $(BOARD_C_FILES:$(BOARD_SRC_DIR)/%.c=$(BOARD_OBJ_DIR)/%_c.o) \
    $(BOARD_S_FILES:$(BOARD_SRC_DIR)/%.S=$(BOARD_OBJ_DIR)/%_s.o)
BOARD_INCLUDE_DIR = $(BOARD_SRC_DIR)/include/

ARCH_SRC_DIR = arch/$(ARCH)/
ARCH_OBJ_DIR = $(BUILD_DIR)/arch/
ARCH_C_FILES = $(wildcard $(ARCH_SRC_DIR)/*.c)
ARCH_S_FILES = $(wildcard $(ARCH_SRC_DIR)/*.S)
ARCH_OBJ_FILES = $(ARCH_C_FILES:$(ARCH_SRC_DIR)/%.c=$(ARCH_OBJ_DIR)/%_c.o) \
    $(ARCH_S_FILES:$(ARCH_SRC_DIR)/%.S=$(ARCH_OBJ_DIR)/%_s.o)
ARCH_INCLUDE_DIR = $(ARCH_SRC_DIR)/include/

EXEC_SRC_DIR = $(ARCH_SRC_DIR)/exec/
EXEC_C_FILES = $(wildcard $(EXEC_SRC_DIR)/*.c)
EXEC_OBJ_DIR = $(BUILD_DIR)/exec/
EXEC_OBJ_FILES = $(EXEC_C_FILES:$(EXEC_SRC_DIR)/%.c=$(EXEC_OBJ_DIR)/%.o)
EXEC_GEN_DIR = $(BUILD_DIR)/exec/generated/

CONFIG_GEN_DIR = $(BUILD_DIR)/config/generated/

KERNEL_SRC_DIR = src/
KERNEL_OBJ_DIR = $(BUILD_DIR)/kernel/
KERNEL_C_FILES = $(wildcard $(KERNEL_SRC_DIR)/*.c)
KERNEL_OBJ_FILES = $(KERNEL_C_FILES:$(KERNEL_SRC_DIR)/%.c=$(KERNEL_OBJ_DIR)/%.o)
KERNEL_INCLUDE_DIR = include/

USER_SRC_DIR = arch/$(ARCH)/user
USER_OBJ_DIR = $(BUILD_DIR)/user
USER_C_FILES = $(wildcard $(USER_SRC_DIR)/*.c)
USER_S_FILES = $(wildcard $(USER_SRC_DIR)/*.S)
USER_OBJ_FILES = $(USER_C_FILES:$(USER_SRC_DIR)/%.c=$(USER_OBJ_DIR)/%_c.o) \
    $(USER_S_FILES:$(USER_SRC_DIR)/%.S=$(USER_OBJ_DIR)/%_s.o)
USER_INCLUDE_DIR = $(USER_SRC_DIR)/include/

OBJ_FILES = $(BOARD_OBJ_FILES) \
    $(ARCH_OBJ_FILES)   \
    $(KERNEL_OBJ_FILES) \
    $(EXEC_OBJ_FILES)   \
    $(USER_OBJ_FILES)

.PHONY: $(phony)

all: kernel8.img

kernel8.img: $(BUILD_DIR)/linker.ld \
    $(ARCH_OBJ_FILES)     \
    $(USER_OBJ_FILES)     \
    $(LIB_DIR)/libboard.a \
    $(LIB_DIR)/libcheesecake.a
	$(ARMCCC)-ld -T $(BUILD_DIR)/linker.ld \
        -o $(BUILD_DIR)/kernel8.elf  \
        $(ARCH_OBJ_FILES)            \
        $(USER_OBJ_FILES)            \
        -L$(LIB_DIR)                 \
        -lboard -lcheesecake -lboard -lcheesecake
	$(ARMCCC)-objdump -D $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/kernel8.dsa
	$(ARMCCC)-nm -n $(BUILD_DIR)/kernel8.elf > $(BUILD_DIR)/kernel8.map
	$(ARMCCC)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

$(BUILD_DIR)/linker.ld: $(ARCH_SRC_DIR)/linker.template
	mkdir -p $(@D)
	gcc -E -x c \
        -I$(ARCH_INCLUDE_DIR)             \
        -I$(CONFIG_GEN_DIR)               \
        -D__LINKER__                      \
        $(ARCH_SRC_DIR)/linker.template | \
        grep -v "^#" |                    \
        sed 's/user\/\.\*/user\/\*/g' > $(BUILD_DIR)/linker.ld

$(ARCH_OBJ_DIR)/%_c.o: $(ARCH_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(KERNEL_INCLUDE_DIR) \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(USER_INCLUDE_DIR)   \
        -I$(EXEC_GEN_DIR)       \
        -I$(CONFIG_GEN_DIR)     \
        -MMD -c $< -o $@

$(ARCH_OBJ_DIR)/%_s.o: $(ARCH_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(KERNEL_INCLUDE_DIR) \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(USER_INCLUDE_DIR)   \
        -I$(EXEC_GEN_DIR)       \
        -I$(CONFIG_GEN_DIR)     \
        -MMD -c $< -o $@

$(LIB_DIR)/libboard.a: $(BOARD_OBJ_FILES)
	mkdir -p $(@D)
	$(ARMCCC)-ar crs $(LIB_DIR)/libboard.a $(BOARD_OBJ_FILES)

$(BOARD_OBJ_DIR)/%_s.o: $(BOARD_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(BOARD_INCLUDE_DIR)  \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(KERNEL_INCLUDE_DIR) \
        -I$(EXEC_GEN_DIR)       \
        -I$(CONFIG_GEN_DIR)     \
        -MMD -c $< -o $@

$(BOARD_OBJ_DIR)/%_c.o: $(BOARD_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(BOARD_INCLUDE_DIR)  \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(KERNEL_INCLUDE_DIR) \
        -I$(EXEC_GEN_DIR)       \
        -I$(CONFIG_GEN_DIR)     \
        -MMD -c $< -o $@

$(LIB_DIR)/libcheesecake.a: $(KERNEL_OBJ_FILES)
	mkdir -p $(@D)
	$(ARMCCC)-ar crs $(LIB_DIR)/libcheesecake.a $(KERNEL_OBJ_FILES)

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(KERNEL_INCLUDE_DIR) \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(USER_INCLUDE_DIR)   \
        -I$(CONFIG_GEN_DIR)     \
        -MMD -c $< -o $@

exec: $(EXEC_GEN_DIR)/exec/asm-offsets.h

$(EXEC_GEN_DIR)/exec/asm-offsets.h: $(EXEC_GEN_DIR)/asm-offsets
	mkdir -p $(@D)
	$< > $@

$(EXEC_GEN_DIR)/asm-offsets: $(EXEC_OBJ_DIR)/asm-offsets.o
	mkdir -p $(@D)
	gcc -MMD $< -o $@

$(EXEC_OBJ_DIR)/asm-offsets.o: $(EXEC_SRC_DIR)/asm-offsets.c
	mkdir -p $(@D)
	gcc -I$(KERNEL_INCLUDE_DIR) \
        -I$(ARCH_INCLUDE_DIR)   \
        -I$(CONFIG_GEN_DIR)     \
        -MMD -c $< -o $@

$(USER_OBJ_DIR)/%_c.o: $(USER_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(USER_INCLUDE_DIR) \
        -MMD -c $< -o $@

$(USER_OBJ_DIR)/%_s.o: $(USER_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMCCC)-gcc $(CFLAGS) \
        -I$(USER_INCLUDE_DIR) \
        -MMD -c $< -o $@

config: $(CONFIG_GEN_DIR)/config/config.h

$(CONFIG_GEN_DIR)/config/config.h: config/config.py cheesecake.conf
	mkdir -p $(@D)
	python config/config.py < cheesecake.conf > $@

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

clean:
	rm -rf $(BUILD_DIR) *.img

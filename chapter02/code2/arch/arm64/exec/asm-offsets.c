#include <stdio.h>
#include "arch/process.h"

int main()
{
    printf("#ifndef _EXEC_ASM_OFFSETS_H\n");
    printf("#define _EXEC_ASM_OFFSETS_H\n");
    printf("\n");
    printf("#define %s \t\t\t%lu\n", "STRUCT_STACK_SAVE_REGISTERS_SIZE", sizeof(struct stack_save_registers));
    printf("\n");
    printf("#endif\n");
    return 0;
}


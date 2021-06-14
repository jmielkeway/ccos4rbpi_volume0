/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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


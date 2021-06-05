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

#include "cake/log.h"
#include "cake/process.h"
#include "cake/vm.h"
#include "arch/abort.h"
#include "arch/page.h"
#include "arch/process.h"
#include "arch/schedule.h"
#include "user/signal.h"

extern int do_kill();
extern int populate_page_tables(unsigned long addr, struct memmap *mm);

int do_bad(unsigned long addr, unsigned long esr, struct stack_save_registers *ssr);
int do_page_fault(unsigned long addr, unsigned long esr, struct stack_save_registers *ssr);

struct fault_handler {
   int (*fn)(unsigned long addr, unsigned long esr, struct stack_save_registers *ssr);
   char *name;
};

static struct fault_handler fault_handlers[] = {
    {do_bad, "ADDRESS_SIZE_FAULT_LEVEL_0_000000"},
    {do_bad, "ADDRESS_SIZE_FAULT_LEVEL_1_000001"},
    {do_bad, "ADDRESS_SIZE_FAULT_LEVEL_2_000010"},
    {do_bad, "ADDRESS_SIZE_FAULT_LEVEL_3_000011"},
    {do_page_fault, "TRANSLATION_FAULT_LEVEL_0_000100"},
    {do_page_fault, "TRANSLATION_FAULT_LEVEL_1_000101"},
    {do_page_fault, "TRANSLATION_FAULT_LEVEL_2_000110"},
    {do_page_fault, "TRANSLATION_FAULT_LEVEL_3_000111"},
    {do_bad, "RESERVED_001000"},
    {do_page_fault, "ACCESS_FLAG_FAULT_LEVEL_1_001001"},
    {do_page_fault, "ACCESS_FLAG_FAULT_LEVEL_2_001010"},
    {do_page_fault, "ACCESS_FLAG_FAULT_LEVEL_3_001011"},
    {do_bad, "RESERVED_001100"},
    {do_bad, "PERMISSION_FAULT_LEVEL_1_001101"},
    {do_bad, "PERMISSION_FAULT_LEVEL_2_001110"},
    {do_bad, "PERMISSION_FAULT_LEVEL_3_001111"},
    {do_bad, "SYNCHRONOUS_EXTERNAL_ABORT_EX_PAGE_TABLE_WALK_010000"},
    {do_bad, "RESERVED_010001"},
    {do_bad, "RESERVED_010010"},
    {do_bad, "RESERVED_010011"},
    {do_bad, "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_0_010100"},
    {do_bad, "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_1_010101"},
    {do_bad, "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_2_010110"},
    {do_bad, "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_3_010111"},
    {do_bad, "SYNCHRONOUS_PARITY_OR_ECC_EX_PAGE_TABLE_WALK_01l000"},
    {do_bad, "RESERVED_011001"},
    {do_bad, "RESERVED_011010"},
    {do_bad, "RESERVED_011011"},
    {do_bad, "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_0_011100"},
    {do_bad, "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_1_011101"},
    {do_bad, "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_2_011110"},
    {do_bad, "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_3_011111"},
    {do_bad, "ALIGHNMENT_FAULT_100001"},
    {do_bad, "RESERVED_100010"},
    {do_bad, "RESERVED_100011"},
    {do_bad, "RESERVED_100100"},
    {do_bad, "RESERVED_100100"},
    {do_bad, "RESERVED_100101"},
    {do_bad, "RESERVED_100110"},
    {do_bad, "RESERVED_100111"},
    {do_bad, "RESERVED_101000"},
    {do_bad, "RESERVED_101001"},
    {do_bad, "RESERVED_101010"},
    {do_bad, "RESERVED_101011"},
    {do_bad, "RESERVED_101100"},
    {do_bad, "RESERVED_101101"},
    {do_bad, "RESERVED_101110"},
    {do_bad, "RESERVED_101111"},
    {do_bad, "TLB_CONFLICT_110000"},
    {do_bad, "RESERVED_110001"},
    {do_bad, "RESERVED_110010"},
    {do_bad, "RESERVED_110011"},
    {do_bad, "RESERVED_110100"},
    {do_bad, "RESERVED_110101"},
    {do_bad, "RESERVED_110110"},
    {do_bad, "RESERVED_110111"},
    {do_bad, "RESERVED_111000"},
    {do_bad, "RESERVED_111001"},
    {do_bad, "RESERVED_111010"},
    {do_bad, "RESERVED_111011"},
    {do_bad, "RESERVED_111100"},
    {do_bad, "RESERVED_111101"},
    {do_bad, "RESERVED_111110"},
    {do_bad, "RESERVED_111111"}
};

int do_bad(unsigned long addr, unsigned long esr, struct stack_save_registers *ssr)
{
    int pid = CURRENT->pid;
    return do_kill(pid, SIGSEGV);
}

int do_page_fault(unsigned long addr, unsigned long esr, struct stack_save_registers *ssr)
{
    struct process *current;
    struct memmap *mm;
    if(addr > STACK_TOP) {
        return do_bad(addr, esr, ssr);
    }
    current = CURRENT;
    mm = current->memmap;
    if(!mm) {
        return do_bad(addr, esr, ssr);
    }
    if(populate_page_tables(addr, mm)) {
        return do_bad(addr, esr, ssr);
    }
    return 0;
}

void mem_abort(unsigned long addr, unsigned long esr, struct stack_save_registers *ssr)
{
    struct fault_handler *handler = &(fault_handlers[esr & ESR_ELx_FSC]);
    if(handler->fn(addr, esr, ssr)) {
        do_bad(addr, esr, ssr);
    }
}

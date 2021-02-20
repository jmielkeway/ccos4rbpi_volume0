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
#include "cake/types.h"

#define ESR_ISS_SHIFT                       (0)
#define ESR_IL_SHIFT                        (25)
#define ESR_EC_SHIFT                        (26)

#define ESR_ISS_MASK                        (0x01FFFFFF)
#define ESR_IL_MASK                         (0x02000000)
#define ESR_EC_MASK                         (0xFC000000)

#define ESR_ISS_VALUE(esr)                  ((esr & ESR_ISS_MASK) >> ESR_ISS_SHIFT)
#define ESR_IL_VALUE(esr)                   ((esr & ESR_IL_MASK) >> ESR_IL_SHIFT)
#define ESR_EC_VALUE(esr)                   ((esr & ESR_EC_MASK) >> ESR_EC_SHIFT)

#define ISS_IFSC_SHIFT                      (0)
#define ISS_DFSC_SHIFT                      (0)

#define ISS_IFSC_MASK                       (0x0000003F)
#define ISS_DFSC_MASK                       (0x0000003F)

#define INSTRUCTION_ABORT_IFSC_VALUE(iss)   ((iss & ISS_IFSC_MASK) >> ISS_IFSC_SHIFT)
#define DATA_ABORT_DFSC_VALUE(iss)          ((iss & ISS_DFSC_MASK) >> ISS_DFSC_SHIFT)

#define SYNC_INSTRUCTION_ABORT_SAME_100001  (0b100001)
#define SYNC_DATA_ABORT_SAME_100101         (0b100101)

extern unsigned long __far();

static char *errorcodes[] = {
    "SYNC_UNKNOWN_REASON_000000",
    "SYNC_TRAPPED_WFI_OR_WFE_000001",
    "SYNC_RESERVED_000010",
    "SYNC_TRAPPED_MCR_OR_MRC_000011",
    "SYNC_TRAPPED_MCRR_OR_MRRC_000100",
    "SYNC_TRAPPED_MCR_OR_MRC_000101",
    "SYNC_TRAPPED_LDC_OR_STC_ACCESS_000110",
    "SYNC_TRAPPED_SVE_SIMD_FLOATING_POINT_000111",
    "SYNC_TRAPPED_VMRS_ACCESS_001000",
    "SYNC_TRAPPED_POINTER_AUTH_001001",
    "SYNC_RESERVED_001010",
    "SYNC_RESERVED_001011",
    "SYNC_TRAPPED_MRRC_001100",
    "SYNC_RESERVED_001101",
    "SYNC_ILLEGAL_EXECUTION_STATE_001110",
    "SYNC_RESERVED_001111",
    "SYNC_RESERVED_010000",
    "SYNC_SVC_AARCH32_010001",
    "SYNC_HVC_AARCH32_010010",
    "SYNC_SMC_AARCH32_010011",
    "SYNC_RESERVED_010100",
    "SYNC_SVC_AARCH64_010101",
    "SYNC_HVC_AARCH64_010110",
    "SYNC_SMC_AARCH64_010111",
    "SYNC_TRAPPED_MSR_OR_MRS_AARCH64_011000",
    "SYNC_SVE_ACCESS_011001",
    "SYNC_TRAPPED_ERET_A_B_011010",
    "SYNC_RESERVED_011011",
    "SYNC_RESERVED_011100",
    "SYNC_RESERVED_011101",
    "SYNC_RESERVED_011110",
    "SYNC_IMPLEMENTATION_DEFINED_EL3_011111",
    "SYNC_INSTRUCTION_ABORT_LOWER_100000",
    "SYNC_INSTRUCTION_ABORT_SAME_100001",
    "SYNC_PC_ALIGNMENT_FAULT_100010",
    "SYNC_RESERVED_100011",
    "SYNC_DATA_ABORT_LOWER_100100",
    "SYNC_DATA_ABORT_SAME_100101",
    "SYNC_SP_ALIGNMENT_FAULT_100110",
    "SYNC_RESERVED_100111",
    "SYNC_TRAPPED_FLOATING_POINT_AARCH32_101000",
    "SYNC_RESERVED_101001",
    "SYNC_RESERVED_101010",
    "SYNC_RESERVED_101011",
    "SYNC_TRAPPED_FLOATING_POINT_AARCH64_101100",
    "SYNC_RESERVED_101101",
    "SYNC_RESERVED_101110",
    "SYNC_SERROR_INTERRUPT_101111",
    "SYNC_BREAKPOINT_LOWER_110000",
    "SYNC_BREAKPOINT_SAME_110001",
    "SYNC_SOFTWARE_STEP_LOWER_110010",
    "SYNC_SOFTWARE_STEP_SAME_110011",
    "SYNC_WATCHPOINT_LOWER_110100",
    "SYNC_WATCHPOINT_SAME_110101",
    "SYNC_RESERVED_110110",
    "SYNC_RESERVED_110111",
    "SYNC_BKPT_AARCH32_111000",
    "SYNC_RESERVED_111001",
    "SYNC_VECTOR_CATCH_AARCH32_111010",
    "SYNC_RESERVED_111011",
    "SYNC_BRK_AARCH64_111100",
    "SYNC_RESERVED_111101",
    "SYNC_RESERVED_111110",
    "SYNC_RESERVED_111111"
};

char *faultstatus[] = {
    "ADDRESS_SIZE_FAULT_LEVEL_0_000000",
    "ADDRESS_SIZE_FAULT_LEVEL_1_000001",
    "ADDRESS_SIZE_FAULT_LEVEL_2_000010",
    "ADDRESS_SIZE_FAULT_LEVEL_3_000011",
    "TRANSLATION_FAULT_LEVEL_0_000100",
    "TRANSLATION_FAULT_LEVEL_1_000101",
    "TRANSLATION_FAULT_LEVEL_2_000110",
    "TRANSLATION_FAULT_LEVEL_3_000111",
    "RESERVED_001000",
    "ACCESS_FLAG_FAULT_LEVEL_1_001001",
    "ACCESS_FLAG_FAULT_LEVEL_2_001010",
    "ACCESS_FLAG_FAULT_LEVEL_3_001011",
    "RESERVED_001100",
    "PERMISSION_FAULT_LEVEL_1_001101",
    "PERMISSION_FAULT_LEVEL_2_001110",
    "PERMISSION_FAULT_LEVEL_3_001111",
    "SYNCHRONOUS_EXTERNAL_ABORT_EX_PAGE_TABLE_WALK_010000",
    "RESERVED_010001",
    "RESERVED_010010",
    "RESERVED_010011",
    "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_0_010100",
    "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_1_010101",
    "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_2_010110",
    "SYNCHRONOUS_EXTERNAL_ABORT_PAGE_TABLE_WALK_LEVEL_3_010111",
    "SYNCHRONOUS_PARITY_OR_ECC_EX_PAGE_TABLE_WALK_01l000",
    "RESERVED_011001",
    "RESERVED_011010",
    "RESERVED_011011",
    "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_0_011100",
    "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_1_011101",
    "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_2_011110",
    "SYNCRHONOUS_PARITY_OR_ECC_PAGE_TABLE_WALK_LEVEL_3_011111",
    "ALIGHNMENT_FAULT_100001",
    "RESERVED_100010",
    "RESERVED_100011",
    "RESERVED_100100",
    "RESERVED_100100",
    "RESERVED_100101",
    "RESERVED_100110",
    "RESERVED_100111",
    "RESERVED_101000",
    "RESERVED_101001",
    "RESERVED_101010",
    "RESERVED_101011",
    "RESERVED_101100",
    "RESERVED_101101",
    "RESERVED_101110",
    "RESERVED_101111",
    "TLB_CONFLICT_110000",
    "RESERVED_110001",
    "RESERVED_110010",
    "RESERVED_110011",
    "RESERVED_110100",
    "RESERVED_110101",
    "RESERVED_110110",
    "RESERVED_110111",
    "RESERVED_111000",
    "RESERVED_111001",
    "RESERVED_111010",
    "RESERVED_111011",
    "RESERVED_111100",
    "RESERVED_111101",
    "RESERVED_111110",
    "RESERVED_111111"
};

void handle_sync_el1(unsigned long esr)
{
    char *abort_fault_status;
    unsigned long errorcode, iss, fsc;
    unsigned long far;
    errorcode = ESR_EC_VALUE(esr);
    iss = ESR_ISS_VALUE(esr);
    char *err_message = errorcodes[errorcode];
    log("%s\r\n", err_message);
    switch(errorcode) {
        case SYNC_INSTRUCTION_ABORT_SAME_100001:
            fsc = INSTRUCTION_ABORT_IFSC_VALUE(iss);
            abort_fault_status = faultstatus[fsc];
            far = __far();
            log("%s\r\n", abort_fault_status);
            log("Fault Address Register: %x\r\n", far);
            break;
        case SYNC_DATA_ABORT_SAME_100101:
            fsc = DATA_ABORT_DFSC_VALUE(iss);
            abort_fault_status = faultstatus[fsc];
            far = __far();
            log("%s\r\n", abort_fault_status);
            log("Fault Address Register: %x\r\n", far);
            break;
        default:
            log("No futher comment\r\n");
            break;
    }
}

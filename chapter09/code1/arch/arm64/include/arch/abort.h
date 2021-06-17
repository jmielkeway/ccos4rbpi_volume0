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

#ifndef _ARCH_ABORT_H
#define _ARCH_ABORT_H

#define ESR_ELx_EC_SVC64        (0x15)
#define ESR_ELx_EC_DABT_LOW     (0x24)
#define ESR_ELx_EC_IABT_LOW     (0x20)
#define ESR_ELx_EC_DABT_CUR     (0x25)
#define ESR_ELx_EC_IABT_CUR     (0x21)

#define ESR_ELx_FSC             (0x3F)
#define ESR_ELx_FSC_PERM        (0x0C)
#define ESR_ELx_FSC_TYPE        (0x3C)

#define ESR_ELx_EC_SHIFT        (26)
#define ESR_ELx_IL_SHIFT        (25)
#define ESR_ELx_ISS_SHIFT       (0)

#define ESR_ELx_ISS_MASK        (0x01FFFFFF)
#define ESR_ELx_IL_MASK         (0x02000000)
#define ESR_ELx_EC_MASK         (0xFC000000)

#define ESR_ELx_ISS_VALUE(esr)  ((esr & ESR_ELx_ISS_MASK) >> ESR_ELx_ISS_SHIFT)
#define ESR_ELx_IL_VALUE(esr)   ((esr & ESR_ELx_IL_MASK) >> ESR_ELx_IL_SHIFT)
#define ESR_ELx_EC_VALUE(esr)   ((esr & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)

#define ESR_ELx_EC(esr)         (((esr) & ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)

#endif

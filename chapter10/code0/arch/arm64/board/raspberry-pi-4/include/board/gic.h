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

#ifndef _BOARD_GIC_H
#define _BOARD_GIC_H

#include "board/peripheral.h"

#define GIC_BASE_REG                (GIC_BASE)
#define GIC_DISTR_OFFSET            ((GIC_BASE_REG) + 0x1000)
#define GICD_CTLR                   ((GIC_DISTR_OFFSET) + 0x000)
#define GICD_TYPER                  ((GIC_DISTR_OFFSET) + 0x004)
#define GICD_IIDR                   ((GIC_DISTR_OFFSET) + 0x008)
#define GICD_IGROUPR_OFFSET         ((GIC_DISTR_OFFSET) + 0x080)
#define GICD_IGROUPR0               ((GIC_DISTR_OFFSET) + 0x080)
#define GICD_IGROUPR1               ((GIC_DISTR_OFFSET) + 0x084)
#define GICD_IGROUPR2               ((GIC_DISTR_OFFSET) + 0x088)
#define GICD_IGROUPR3               ((GIC_DISTR_OFFSET) + 0x08C)
#define GICD_IGROUPR4               ((GIC_DISTR_OFFSET) + 0x090)
#define GICD_IGROUPR5               ((GIC_DISTR_OFFSET) + 0x094)
#define GICD_IGROUPR6               ((GIC_DISTR_OFFSET) + 0x098)
#define GICD_IGROUPR7               ((GIC_DISTR_OFFSET) + 0x09C)
#define GICD_ISENABLER_OFFSET       ((GIC_DISTR_OFFSET) + 0x100)
#define GICD_ISENABLER0             ((GICD_ISENABLER_OFFSET) + 0x00)
#define GICD_ISENABLER1             ((GICD_ISENABLER_OFFSET) + 0x04)
#define GICD_ISENABLER2             ((GICD_ISENABLER_OFFSET) + 0x08)
#define GICD_ISENABLER3             ((GICD_ISENABLER_OFFSET) + 0x0C)
#define GICD_ISENABLER4             ((GICD_ISENABLER_OFFSET) + 0x10)
#define GICD_ISENABLER5             ((GICD_ISENABLER_OFFSET) + 0x14)
#define GICD_ISENABLER6             ((GICD_ISENABLER_OFFSET) + 0x18)
#define GICD_ISENABLER7             ((GICD_ISENABLER_OFFSET) + 0x1C)
#define GICD_ISPENDINGR_OFFSET      ((GIC_DISTR_OFFSET) + 0x200)
#define GICD_ISPENDINGR0            ((GICD_ISPENDINGR_OFFSET) + 0x00)
#define GICD_ISPENDINGR1            ((GICD_ISPENDINGR_OFFSET) + 0x04)
#define GICD_ISPENDINGR2            ((GICD_ISPENDINGR_OFFSET) + 0x08)
#define GICD_ISPENDINGR3            ((GICD_ISPENDINGR_OFFSET) + 0x0C)
#define GICD_ISPENDINGR4            ((GICD_ISPENDINGR_OFFSET) + 0x10)
#define GICD_ISPENDINGR5            ((GICD_ISPENDINGR_OFFSET) + 0x14)
#define GICD_ISPENDINGR6            ((GICD_ISPENDINGR_OFFSET) + 0x18)
#define GICD_ISPENDINGR7            ((GICD_ISPENDINGR_OFFSET) + 0x1C)
#define GICD_ITARGETSR_OFFSET       ((GIC_DISTR_OFFSET) + 0x800)
#define GICD_ITARGETSR0             ((GICD_ITARGETSR_OFFSET) + 0x00)
#define GICD_ITARGETSR1             ((GICD_ITARGETSR_OFFSET) + 0x04)
#define GICD_ITARGETSR2             ((GICD_ITARGETSR_OFFSET) + 0x08)
#define GICD_ITARGETSR3             ((GICD_ITARGETSR_OFFSET) + 0x0C)
#define GICD_ITARGETSR4             ((GICD_ITARGETSR_OFFSET) + 0x10)
#define GICD_ITARGETSR5             ((GICD_ITARGETSR_OFFSET) + 0x14)
#define GICD_ITARGETSR6             ((GICD_ITARGETSR_OFFSET) + 0x18)
#define GICD_ITARGETSR7             ((GICD_ITARGETSR_OFFSET) + 0x1C)
#define GICD_ITARGETSR8             ((GICD_ITARGETSR_OFFSET) + 0x20)
#define GICD_ITARGETSR9             ((GICD_ITARGETSR_OFFSET) + 0x24)
#define GICD_ITARGETSR10            ((GICD_ITARGETSR_OFFSET) + 0x28)
#define GICD_ITARGETSR11            ((GICD_ITARGETSR_OFFSET) + 0x2C)
#define GICD_ITARGETSR12            ((GICD_ITARGETSR_OFFSET) + 0x30)
#define GICD_ITARGETSR13            ((GICD_ITARGETSR_OFFSET) + 0x34)
#define GICD_ITARGETSR14            ((GICD_ITARGETSR_OFFSET) + 0x38)
#define GICD_ITARGETSR15            ((GICD_ITARGETSR_OFFSET) + 0x3C)
#define GICD_ITARGETSR16            ((GICD_ITARGETSR_OFFSET) + 0x40)
#define GICD_ITARGETSR17            ((GICD_ITARGETSR_OFFSET) + 0x44)
#define GICD_ITARGETSR18            ((GICD_ITARGETSR_OFFSET) + 0x48)
#define GICD_ITARGETSR19            ((GICD_ITARGETSR_OFFSET) + 0x4C)
#define GICD_ITARGETSR20            ((GICD_ITARGETSR_OFFSET) + 0x50)
#define GICD_ITARGETSR21            ((GICD_ITARGETSR_OFFSET) + 0x54)
#define GICD_ITARGETSR22            ((GICD_ITARGETSR_OFFSET) + 0x58)
#define GICD_ITARGETSR23            ((GICD_ITARGETSR_OFFSET) + 0x5C)
#define GICD_ITARGETSR24            ((GICD_ITARGETSR_OFFSET) + 0x60)
#define GICD_ITARGETSR25            ((GICD_ITARGETSR_OFFSET) + 0x64)
#define GICD_ITARGETSR26            ((GICD_ITARGETSR_OFFSET) + 0x68)
#define GICD_ITARGETSR27            ((GICD_ITARGETSR_OFFSET) + 0x6C)
#define GICD_SGIR                   ((GIC_DISTR_OFFSET) + 0xF00)
#define GIC_CPUIFACE_OFFSET         ((GIC_BASE_REG) + 0x2000)
#define GICC_CTLR                   ((GIC_CPUIFACE_OFFSET) + 0x00)
#define GICC_PMR                    ((GIC_CPUIFACE_OFFSET) + 0x04)
#define GICC_IAR                    ((GIC_CPUIFACE_OFFSET) + 0x0C)
#define GICC_EOIR                   ((GIC_CPUIFACE_OFFSET) + 0x10)

#endif

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

#ifndef _ARCH_START_H
#define _ARCH_START_H

#define PSR_MODE_EL0t   0b0000

extern void memset(void *dest, int c, unsigned long count);

static inline void start_thread(struct stack_save_registers *ssr, unsigned long pc,
    unsigned long sp)
{
    memset(ssr, 0, sizeof(*ssr));
    ssr->pc = pc;
    ssr->pstate = PSR_MODE_EL0t;
    ssr->sp = sp;
}

#endif

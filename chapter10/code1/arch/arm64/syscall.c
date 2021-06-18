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

#include "arch/page.h"
#include "arch/syscall.h"

extern long sys_read(int fd, char *buffer, unsigned long count);
extern long sys_write(unsigned int fd, char *user, unsigned long count);

void * sys_call_table [NUM_SYSCALLS] __attribute__((__aligned__(PAGE_SIZE))) = {
    [0 ... NUM_SYSCALLS - 1] = 0,
    [SYSCALL_READ] = sys_read,
    [SYSCALL_WRITE] = sys_write
};

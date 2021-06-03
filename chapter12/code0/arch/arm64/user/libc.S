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

#include "user/syscall.h"

#define __SYSCALL(name, number) \
.globl __##name ;\
__##name: ;\
    mov     w8, number ;\
    svc     0x0 ;\
    ret 

__SYSCALL(clone, SYSCALL_CLONE)

__SYSCALL(cpustat, SYSCALL_CPUSTAT)

__SYSCALL(exec, SYSCALL_EXEC)

__SYSCALL(exit, SYSCALL_EXIT)

__SYSCALL(getpid, SYSCALL_GETPID)

__SYSCALL(ioctl, SYSCALL_IOCTL)

__SYSCALL(read, SYSCALL_READ)

__SYSCALL(sigaction, SYSCALL_SIGACTION)

__SYSCALL(sigprocmask, SYSCALL_SIGPROCMASK)

__SYSCALL(sigreturn, SYSCALL_SIGRETURN)

__SYSCALL(waitpid, SYSCALL_WAITPID)

__SYSCALL(write, SYSCALL_WRITE)

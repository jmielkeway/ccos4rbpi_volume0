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
#include "user/cpu.h"
#include "user/signal.h"
#include "user/syscall.h"

extern int sys_clone(unsigned long flags, unsigned long thread_input, unsigned long arg);
extern int sys_cpustat(unsigned long cpu, struct user_cpuinfo *cpuinfo);
extern int sys_exec(void *user_function);
extern void sys_exit(int code);
extern int sys_getpid();
extern long sys_ioctl(int fd, unsigned int request, unsigned long arg);
extern long sys_read(int fd, char *buffer, unsigned long count);
extern int sys_sigaction(int signo, struct sigaction *sigaction, struct sigaction *unused);
extern int sys_sigprocmask(unsigned long how, unsigned long *newset, unsigned long *oldset);
extern void sys_sigreturn();
extern int sys_waitpid(int pid, int *status, int options);
extern long sys_write(unsigned int fd, char *user, unsigned long count);

void * sys_call_table [NUM_SYSCALLS] __attribute__((__aligned__(PAGE_SIZE))) = {
    [0 ... NUM_SYSCALLS - 1] = 0,
    [SYSCALL_READ] = sys_read,
    [SYSCALL_WRITE] = sys_write,
    [SYSCALL_CLONE] = sys_clone,
    [SYSCALL_EXEC] = sys_exec,
    [SYSCALL_IOCTL] = sys_ioctl,
    [SYSCALL_GETPID] = sys_getpid,
    [SYSCALL_SIGRETURN] = sys_sigreturn,
    [SYSCALL_SIGACTION] = sys_sigaction,
    [SYSCALL_SIGPROCMASK] = sys_sigprocmask,
    [SYSCALL_WAITPID] = sys_waitpid,
    [SYSCALL_EXIT] = sys_exit,
    [SYSCALL_CPUSTAT] = sys_cpustat,
};

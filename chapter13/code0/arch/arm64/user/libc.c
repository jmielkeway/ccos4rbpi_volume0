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

#include "user/cpu.h"
#include "user/signal.h"

extern int __clone(unsigned long flags, unsigned long thread_input, unsigned long arg);
extern int __cpustat(unsigned long cpu, struct user_cpuinfo *cpuinfo);
extern int __exec(int (*user_function)(void));
extern void __exit(int code);
extern int __getpid();
extern int __ioctl(int fd, unsigned int request, void *arg);
extern long __read(int fd, char *buffer, unsigned long count);
extern int __sigaction(int signo, struct sigaction *sigaction, struct sigaction *unused);
extern int __sigprocmask(unsigned long how, unsigned long *newset, unsigned long *oldset);
extern void __sigreturn();
extern int __waitpid(int pid, int *status, int options);
extern long __write(int fd, char *buffer, unsigned long count);

int libc_sigaddset(unsigned long *set, int signo)
{
    *set |= (1 << signo);
    return 0;
}

int libc_strcmp(const char *a, const char *b)
{
    while(*a && (*a == *b)) {
        a++;
        b++;
    }
    return *((const unsigned char *) a) - *((const unsigned char *) b);
}

unsigned long libc_strlen(const char *s)
{
    unsigned long n = 0;
    while(*s++) {
        n++;
    }
    return n;
}

int clone(unsigned long flags)
{
    return __clone(flags, 0, 0);
}

int cpustat(unsigned long cpu, struct user_cpuinfo *cpuinfo)
{
    return __cpustat(cpu, cpuinfo);
}

int exec(int (*user_function)(void))
{
    return __exec(user_function);
}

void exit(int code)
{
    __exit(code);
}

int getpid()
{
    return __getpid();
}

int ioctl(int fd, unsigned int request, void *arg)
{
    return __ioctl(fd, request, arg);
}

long read(int fd, char *buffer, unsigned long count)
{
    return __read(fd, buffer, count);
}

int signal(int signo, void (*fn)(int))
{
    struct sigaction sigaction;
    sigaction.restore = __sigreturn;
    sigaction.fn = fn;
    return __sigaction(signo, &sigaction, 0);
}

int sigprocmask(unsigned long how, unsigned long *newset, unsigned long *oldset)
{
    return __sigprocmask(how, newset, oldset);
}

int waitpid(int pid, int *status, int options)
{
    return __waitpid(pid, status, options);
}

long write(int fd, char *buffer, unsigned long count)
{
    return __write(fd, buffer, count);
}

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

#ifndef _USER_SYSCALL_H
#define _USER_SYSCALL_H

#define SYSCALL_ZERO_RESERVED   (0)
#define SYSCALL_READ            (1)
#define SYSCALL_WRITE           (2)
#define SYSCALL_CLONE           (3)
#define SYSCALL_EXEC            (4)
#define SYSCALL_IOCTL           (5)
#define SYSCALL_GETPID          (6)
#define SYSCALL_SIGRETURN       (7)
#define SYSCALL_SIGACTION       (8)
#define SYSCALL_SIGPROCMASK     (9)
#define SYSCALL_WAITPID         (10)
#define SYSCALL_EXIT            (11)
#define SYSCALL_CPUSTAT         (12)
#define NUM_SYSCALLS            (13)

#endif

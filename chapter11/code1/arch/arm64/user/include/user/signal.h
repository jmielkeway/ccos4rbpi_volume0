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

#ifndef _USER_SIGNAL_H
#define _USER_SIGNAL_H

#define SIGINT          1
#define SIGKILL         2
#define SIGSEGV         3
#define SIGCHLD         4
#define SIGCONT         5
#define SIGSTOP         6
#define SIGTSTP         7
#define SIGTTIN         8
#define SIGTTOU         9
#define NUM_SIGNALS     9

#define SIG_BLOCK       (0)
#define SIG_UNBLOCK     (1)
#define SIG_SETMASK     (2)

struct sigaction { 
    unsigned long flags;
    void (*fn)(int); 
    void (*restore)(void);
};

#endif

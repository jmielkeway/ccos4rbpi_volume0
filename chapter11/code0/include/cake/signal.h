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

#ifndef _CAKE_SIGNAL_H
#define _CAKE_SIGNAL_H

#include "cake/list.h"
#include "cake/lock.h"
#include "cake/wait.h"
#include "user/signal.h"

#define SIGNAL_FLAGS_STOPPED    (0b0001)
#define SIGNAL_FLAGS_CONTINUED  (0b0010)
#define SIGNAL_SEND_PRIVATE     (1UL)

struct sighandler {
    struct sigaction sigaction[NUM_SIGNALS];
    struct spinlock lock;    
};

struct siginfo {
    unsigned int signo;
    unsigned long error;
    unsigned int code;
    union {
        struct {
            unsigned int sender_pid;
        } kill;
        struct {
            unsigned int child_pid;
            unsigned int status;
        } child;
    } fields;
};

struct signal {
    unsigned long refcount;
    unsigned long pending[1];
    unsigned long blocked[1];
    unsigned int flags;
    unsigned int exitcode;
    struct sighandler sighandler;
    struct list signallist;
    struct spinlock lock;
    struct waitqueue waitqueue;
};

struct sigqueue {
    struct list list;
    struct siginfo info;
};

#endif

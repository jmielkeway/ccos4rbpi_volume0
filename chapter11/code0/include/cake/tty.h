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

#ifndef _CAKE_TTY_H
#define _CAKE_TTY_H

#include "cake/file.h"
#include "cake/wait.h"

#define TERMIOS_NEWLINE         (0)
#define TERMIOS_ERASE           (1)
#define TERMIOS_INTR            (2)
#define TERMIOS_EOF             (3)
#define TERMIOS_MAX             (8)

#define TTY_NEWLINE_CHAR(tty)   ((tty)->termios[TERMIOS_NEWLINE])
#define TTY_ERASE_CHAR(tty)     ((tty)->termios[TERMIOS_ERASE])
#define TTY_INTR_CHAR(tty)      ((tty)->termios[TERMIOS_INTR])
#define TTY_EOF_CHAR(tty)       ((tty)->termios[TERMIOS_EOF])

struct tty_termios;

struct tty {
    struct tty_ops *ops;
    struct tty_ldisc *ldisc;
    void *driver_data;
    void *disc_data;
    char termios[TERMIOS_MAX];
    unsigned int index;
    unsigned int open_count;
    int pid_leader;
    struct waitqueue waitqueue;
};

struct tty_driver {
    struct tty_ops *ops;
    unsigned int basefile;
    unsigned int num_devices;
};

struct tty_ldisc {
    struct tty_ldisc_ops *ops;
    struct tty *tty;
};

struct tty_ldisc_ops {
    int (*close)(struct tty *tty);
    int (*ioctl)(struct tty *tty, unsigned int command, unsigned long arg);
    int (*open)(struct tty *tty);
    long (*read)(struct tty *tty, char *buffer, unsigned long count);
    int (*receive_buf)(struct tty *tty, char *buffer, unsigned int count);
    long (*write)(struct tty *tty, char *buffer, unsigned long count);
};

struct tty_ops {
    int (*close)(struct tty *tty, struct file *file);
    int (*open)(struct tty *tty, struct file *file);
    int (*write)(struct tty *tty, unsigned char *buffer, unsigned int count);
};


void register_tty_driver(struct tty_driver *driver);

#endif

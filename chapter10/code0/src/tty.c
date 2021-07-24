/*
 *
 * Copyright (C) 1991, 1992, 1993 Linus Torvalds
 * Copyright (C) 1994 Julian Cowley
 * Copyright (C) 1994 Theodore Ts'o
 *
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

#include "cake/allocate.h"
#include "cake/user.h"
#include "cake/tty.h"

#define N_TTY_BUF_SIZE      (4096)

extern struct file *filesystem_file(unsigned int i);

static int n_tty_close(struct tty *tty);
static int n_tty_open(struct tty *tty);
static long n_tty_write(struct tty *tty, char *user, unsigned long n);
static int tty_close(struct file *file);
static int tty_open(struct file *file);
static long tty_write(struct file *file, char *user, unsigned long n);

static struct tty_ldisc_ops n_tty_ldisc_ops = {
    .close = n_tty_close,
    .open = n_tty_open,
    .write = n_tty_write
};
static struct file_ops tty_file_ops = {
    .close = tty_close,
    .open  = tty_open,
    .write = tty_write
};

static int n_tty_close(struct tty *tty)
{
    struct n_tty_data *ldata;
    ldata = tty->disc_data;
    cake_free(ldata);
    return 0;
}

static int n_tty_open(struct tty *tty)
{
    return 0;
}

static long n_tty_write(struct tty *tty, char *buffer, unsigned long count)
{
    char *b = buffer;
    unsigned long written;
    while(count) {
        written = tty->ops->write(tty, (unsigned char *) b, count);
        count -= written;
        b += written;
    }
    return b - buffer;
}

void register_tty_driver(struct tty_driver *driver)
{
    struct tty *tty;
    struct file *file;
    for(unsigned int i = driver->basefile; i < driver->num_devices; i++) {
        tty = cake_alloc(sizeof(*tty));
        tty->ops = driver->ops;
        tty->index = i - driver->basefile;
        tty->waitqueue.waitlist.prev = &(tty->waitqueue.waitlist);
        tty->waitqueue.waitlist.next = &(tty->waitqueue.waitlist);
        file = filesystem_file(i);
        file->ops = &tty_file_ops;
        file->extension = tty;
    }
}

static int tty_close(struct file *file)
{
    struct tty *tty = file->extension;
    --tty->open_count;
    if(!tty->open_count) {
        tty->ldisc->ops->close(tty);
        cake_free(tty->ldisc);
    }
    return tty->ops->close(tty, file);
}

static int tty_open(struct file *file)
{
    int ret;
    struct tty *tty = file->extension;
    ret = tty->ops->open(tty, file);
    if(!tty->ldisc) {
        struct tty_ldisc *ldisc = cake_alloc(sizeof(struct tty_ldisc));
        ldisc->ops = &n_tty_ldisc_ops;
        ldisc->tty = tty;
        tty->ldisc = ldisc;
        ldisc->ops->open(tty);
    }
    tty->open_count++;
    return ret;
}

static long tty_write(struct file *file, char *user, unsigned long n)
{
    unsigned int count;
    unsigned long written = 0;
    struct tty *tty = file->extension;
    struct tty_ldisc *ld = tty->ldisc;
    char *cake_buffer = cake_alloc(N_TTY_BUF_SIZE);
    if(!cake_buffer) {
        return - 1;
    }
    while(written < n) {
        count = n - written;
        if(count > N_TTY_BUF_SIZE) {
            count = N_TTY_BUF_SIZE;
        }
        copy_from_user(cake_buffer, user, count);
        count = ld->ops->write(tty, cake_buffer, count);
        user += count;
        written += count;
    }
    cake_free(cake_buffer);
    return written;
}

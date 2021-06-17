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

#ifndef _CAKE_FILE_H
#define _CAKE_FILE_H

#define MAX_OPEN_FILES      (16)

struct file {
    struct file_ops *ops;
    unsigned long flags;
    long pos;
    void *extension;
};

struct file_ops {
    int (*ioctl)(struct file *self, unsigned int request, unsigned long args);
    long (*read)(struct file *self, char *user, unsigned long n);
    long (*write)(struct file *self, char *user, unsigned long n);
    int (*open)(struct file *self);
    int (*close)(struct file *self);
};

struct folder {
    struct file *files[MAX_OPEN_FILES];
    unsigned int next;
    unsigned int openmap;
};

#endif

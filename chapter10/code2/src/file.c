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

#include "cake/file.h"
#include "arch/schedule.h"

#define MAX_OPEN_FILES      (16)
#define OPEN_FULL_MASK      ((1 << (MAX_OPEN_FILES)) - 1)
#define FD_TOGGLE(map, fd)  (map ^= (1 << fd))
#define FD_CHECK(map, fd)   (map & (1 << fd))

extern struct file *filesystem_file(unsigned int i);

int do_open(int file_reservation) {
    int fd;
    struct process *current;
    struct folder *folder;
    struct file *file;
    current = CURRENT;
    folder = &(current->folder);
    if(folder->openmap == OPEN_FULL_MASK) {
        return -1;
    }
    file = filesystem_file(file_reservation);
    fd = folder->next;
    if(file->ops->open(file)) {
        folder->files[fd] = 0;
        fd = -1;
    }
    else {
        folder->files[fd] = file;
        FD_TOGGLE(folder->openmap, fd);
        for(int i = 0; i < MAX_OPEN_FILES; i++) {
            if(!FD_CHECK(folder->openmap, i)) {
                folder->next = i;
                break;
            }
        }
    }
    return fd;
}

int sys_open(int file_reservation)
{
    return do_open(file_reservation);
}

long sys_read(unsigned int fd, char *user, unsigned long count)
{
    long retval;
    struct process *current = CURRENT;
    struct folder *folder = &(current->folder);
    struct file *file = folder->files[fd];
    if(FD_CHECK(folder->openmap, fd)) {
        retval = file->ops->read(file, user, count);
    }
    else {
       retval = -1;
    }
    return retval;
}

long sys_write(unsigned int fd, char *user, unsigned long count)
{
    struct process *current = CURRENT;
    struct folder *folder = &(current->folder);
    struct file *file = folder->files[fd];
    if(FD_CHECK(folder->openmap, fd)) {
        return file->ops->write(file, user, count);
    }
    else {
        return -1;
    }
}

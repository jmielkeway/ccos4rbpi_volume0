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
  
#include "cake/allocate.h"
#include "cake/file.h"
#include "board/filesystem.h"

extern int rpi3_miniuart_init();

static struct file device_files[DEVICE_RESERVATIONS]; 
static struct file regular_files[NUM_REGULAR_FILES];
static struct file *filesystem[FILESYSTEM_SIZE];

struct file **assign_filesystem()
{
    struct file *file;
    unsigned int inx = 0;
    for(unsigned int i = 0; i < DEVICE_RESERVATIONS; i++) {
        filesystem[inx++] = &(device_files[i]);
    }
    for(unsigned int i = 0; i < NUM_REGULAR_FILES; i++) {
        file = &(regular_files[i]);
        file->extension = alloc_pages(0);
        filesystem[inx++] = file;
    }
    return filesystem;
}

void drivers_init()
{
    rpi3_miniuart_init(RPI3_MINIUART_BASEFILE);
}

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

extern struct file **assign_filesystem();
extern void drivers_init();

static struct file **filesystem;

struct file *filesystem_file(unsigned int i)
{
    return filesystem[i];
}

void filesystem_init()
{
    filesystem = assign_filesystem();
    drivers_init();
}

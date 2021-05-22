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

#include "board/filesystem.h"

extern long do_exec(int (*user_function)(void), int init);
extern long do_open(int reserved_file);

int startup_user(void *user_function)
{
    do_exec(user_function, 1);
    do_open(RPI4_MINIUART_BASEFILE);
    do_open(RPI4_MINIUART_BASEFILE);
    return 0;
}

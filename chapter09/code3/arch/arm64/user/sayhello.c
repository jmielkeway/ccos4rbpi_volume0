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

extern long sayhello();
extern long usloopsleep();

unsigned long user_bss = 0;
unsigned long user_data = 5;
const long user_rodata = 7;

int say_hello()
{
    while(1) {
        sayhello();
        usloopsleep();
    }
    return 0;
}

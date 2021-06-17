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

#define STDIN   (0)
#define STDOUT  (1)

extern long usloopsleep();
extern long write(int fd, char *buffer, unsigned long count);

int say_hello()
{
    while(1) {
        write(STDOUT, "I <3 Old York Cheesecake!\n", 27);
        usloopsleep();
    }
    return 0;
}

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

extern long __read(int fd, char *buffer, unsigned long count);
extern long __usloopsleep();
extern long __write(int fd, char *buffer, unsigned long count);

long read(int fd, char *buffer, unsigned long count)
{
    return __read(fd, buffer, count);
}

long usloopsleep()
{
    return __usloopsleep();
}

long write(int fd, char *buffer, unsigned long count)
{
    return __write(fd, buffer, count);
}

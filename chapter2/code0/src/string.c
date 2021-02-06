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

void xintos(unsigned long x, char *t)
{
    int temp_size = 0;
    char c, temp[16];
    *(t++) = '0';
    *(t++) = 'x';
    do {
        c = x % 16;
        if(c < 10) {
            c = c + 0x30;
        } else {
            c = c + 0x37;
        }
        temp[temp_size++] = c;
        x /= 16;
    } while(x);
    while(temp_size--) {
        *(t++) = temp[temp_size];
    }
    *(t++) = '\0';
    return;
}

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

extern void __delay(unsigned long delay);
extern void uart_puts(char *s);

void cheesecake_main(void)
{
    char *version = "Version: 0.1.0.1\r\n";
    uart_puts("Hello, Cheesecake!\r\n");
    while (1) {
        uart_puts(version);
        uart_puts("\r\n");
        __delay(20000000);
    }
}

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

#include "cake/log.h"

extern int  __uart_can_tx();
extern void __uart_putchar(char c);

static void uart_puts(char *s);

static struct console console = {
    .write = uart_puts
};

static inline int check_ready()
{
    return __uart_can_tx();
}

static inline void uart_putchar(char c)
{
    while(!check_ready()) {
    }
    __uart_putchar(c);
}

void console_init()
{
    register_console(&console);
}

static void uart_puts(char *s)
{
    if(s) {
        for(char *str = s; *str != '\0'; str++) {
            uart_putchar(*str);
        }
    }
}

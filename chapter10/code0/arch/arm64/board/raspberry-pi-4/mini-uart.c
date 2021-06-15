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
#include "cake/tty.h"

extern int  __uart_can_tx();
extern void __uart_putchar(char c);

static int rpi4_miniuart_close(struct tty *tty, struct file *file);
static int rpi4_miniuart_open(struct tty *tty, struct file *file);
static int rpi4_miniuart_write(struct tty *tty, unsigned char *buffer, unsigned int count);
static void uart_puts(char *s);

static struct console console = {
    .write = uart_puts
};
struct rpi4_miniuart {
    struct tty *tty;
} rpi4_miniuart;
static struct tty_ops rpi4_miniuart_ops = {
    .close = rpi4_miniuart_close,
    .open = rpi4_miniuart_open,
    .write = rpi4_miniuart_write
};
static struct tty_driver rpi4_miniuart_tty_driver = {
    .ops = &rpi4_miniuart_ops,
    .num_devices = 1
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

static int rpi4_miniuart_close(struct tty *tty, struct file *file)
{
    tty->driver_data = 0;
    rpi4_miniuart.tty = 0;
    return 0;
}

int rpi4_miniuart_init(int reserved_file)
{
    rpi4_miniuart_tty_driver.basefile = reserved_file;
    register_tty_driver(&rpi4_miniuart_tty_driver);
    return 0;
}

static int rpi4_miniuart_open(struct tty *tty, struct file *file)
{
    rpi4_miniuart.tty = tty;
    tty->driver_data = &rpi4_miniuart;
    return 0;
}

static int rpi4_miniuart_write(struct tty *tty, unsigned char *buffer, unsigned int count)
{
    char c;
    unsigned int i;
    for(i = 0; i < count; i++) {
        c = buffer[i];
        switch(c) {
            case '\n':
                uart_putchar('\r');
                uart_putchar('\n');
                break;
            default:
                uart_putchar(c);
                break;
        }
    }
    return i;
}

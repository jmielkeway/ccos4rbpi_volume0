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

#include "cake/lock.h"
#include "cake/log.h"
#include "cake/tty.h"
#include "cake/work.h"
#include "arch/lock.h"

#define READBUF_SIZE    (128)
#define READBUF_MASK    (READBUF_SIZE - 1)
#define MASK(x)         ((x) & (READBUF_MASK))

#define IRQ_READ_PENDING    (0xC4)

extern int  __uart_can_tx();
extern char __uart_getchar();
extern void __uart_irqenable();
extern char __uart_irqstatus();
extern void __uart_putchar(char c);

static int rpi4_miniuart_close(struct tty *tty, struct file *file);
static int rpi4_miniuart_open(struct tty *tty, struct file *file);
static int rpi4_miniuart_write(struct tty *tty, unsigned char *buffer, unsigned int count);
static void uart_puts(char *s);

struct rpi4_miniuart_receive_buffer {
    char readbuf[READBUF_SIZE];
    unsigned long head;
    unsigned long tail;
    struct spinlock lock;
    struct work work;
};

static struct console console = {
    .write = uart_puts
};
struct rpi4_miniuart {
    struct tty *tty;
    struct rpi4_miniuart_receive_buffer buffer;
} rpi4_miniuart;
static struct tty_ops rpi4_miniuart_ops = {
    .close = rpi4_miniuart_close,
    .open = rpi4_miniuart_open,
    .write = rpi4_miniuart_write
};
static char rpi4_miniuart_termios[TERMIOS_MAX] = {
    [TERMIOS_NEWLINE] = '\r',
    [TERMIOS_ERASE] = 0x7F,
    [TERMIOS_INTR] = 0x3,
    [TERMIOS_EOF] = 0x4
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

static void rpi4_miniuart_collect(struct work *work)
{
    unsigned long count, tail, flags, size;
    char *buf;
    struct tty *tty = rpi4_miniuart.tty;
    struct spinlock *lock = &(rpi4_miniuart.buffer.lock);
    flags = SPIN_LOCK_IRQSAVE(lock);
    count = rpi4_miniuart.buffer.head - rpi4_miniuart.buffer.tail;
    tail = MASK(rpi4_miniuart.buffer.tail);
    size = READBUF_SIZE - tail;
    rpi4_miniuart.buffer.tail += count;
    SPIN_UNLOCK_IRQRESTORE(lock, flags);
    buf = &(rpi4_miniuart.buffer.readbuf[tail]);
    if(count > size) {
        tty->ldisc->ops->receive_buf(tty, buf, size);
        count -= size;
        buf = rpi4_miniuart.buffer.readbuf;
    }
    tty->ldisc->ops->receive_buf(tty, buf, count);
}

static int rpi4_miniuart_close(struct tty *tty, struct file *file)
{
    tty->driver_data = 0;
    rpi4_miniuart.tty = 0;
    return 0;
}

int rpi4_miniuart_init(int reserved_file)
{
    struct work *work;
    rpi4_miniuart_tty_driver.basefile = reserved_file;
    register_tty_driver(&rpi4_miniuart_tty_driver);
    work = &(rpi4_miniuart.buffer.work);
    work->worklist.prev = &(work->worklist);
    work->worklist.next = &(work->worklist);
    work->todo = rpi4_miniuart_collect;
    __uart_irqenable();
    return 0;
}

void rpi4_miniuart_interrupt()
{
    char c;
    unsigned long space; 
    unsigned long head;
    struct spinlock *lock = &(rpi4_miniuart.buffer.lock);
    SPIN_LOCK(lock);
    head = rpi4_miniuart.buffer.head;
    space = READBUF_SIZE - (head - rpi4_miniuart.buffer.tail);
    if(space) {
        while(__uart_irqstatus() == IRQ_READ_PENDING) {
            c = __uart_getchar();
            rpi4_miniuart.buffer.readbuf[MASK(head++)] = c;
        }
    }
    rpi4_miniuart.buffer.head = head;
    SPIN_UNLOCK(lock);
    enqueue_work(&(rpi4_miniuart.buffer.work));
}

static int rpi4_miniuart_open(struct tty *tty, struct file *file)
{
    rpi4_miniuart.tty = tty;
    tty->driver_data = &rpi4_miniuart;
    for(int i = 0; i < TERMIOS_MAX; i++) {
        tty->termios[i] = rpi4_miniuart_termios[i];
    }
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

static void uart_puts(char *s)
{
    if(s) {
        for(char *str = s; *str != '\0'; str++) {
            uart_putchar(*str);
        }
    }
}

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

#include "cake/allocate.h"
#include "cake/bitops.h"
#include "cake/compiler.h"
#include "cake/error.h"
#include "cake/file.h"
#include "cake/schedule.h"
#include "cake/tty.h"
#include "cake/user.h"
#include "arch/atomic.h"
#include "arch/barrier.h"
#include "arch/schedule.h"
#include "user/signal.h"

#define N_TTY_BUF_SIZE      (4096)
#define N_TTY_SIZE_MASK     (N_TTY_BUF_SIZE - 1)
#define MASK(x)             ((x) & (N_TTY_SIZE_MASK))

#define DISABLED_CHAR       ('\0')
#define ECHO_OP_START       (0xFF)

#define _U                  (0b00000001)
#define _L                  (0b00000010)
#define _D                  (0b00000100)
#define _C                  (0b00001000)
#define _P                  (0b00010000)
#define _S                  (0b00100000)
#define _X                  (0b01000000)
#define _SP                 (0b10000000)
#define IS_CHAR_MASK(x)     (ctype[(int)(unsigned char)(x)])
#define ISCTRL(c)           ((IS_CHAR_MASK(c) & (_C)))

extern int do_kill(int pid, int signal);
extern struct file *filesystem_file(unsigned int i);
extern void memset(void *dest, int c, unsigned long count);
extern struct process *pid_process(unsigned int pid);
extern void pid_put(int pid);

static int n_tty_close(struct tty *tty);
static int n_tty_open(struct tty *tty);
static long n_tty_read(struct tty *tty, char *user, unsigned long n);
static int n_tty_receive_buf(struct tty *tty, char *user, unsigned int n);
static void n_tty_receive_marked_char(struct tty *tty, char c);
static void n_tty_receive_normal_char(struct tty *tty, char c);
static long n_tty_write(struct tty *tty, char *user, unsigned long n);
static int tty_close(struct file *file);
static int tty_ioctl(struct file *file, unsigned int request, unsigned long arg);
static int tty_open(struct file *file);
static long tty_read(struct file *file, char *user, unsigned long n);
static long tty_write(struct file *file, char *user, unsigned long n);

struct n_tty_data {
    unsigned long read_head;
    unsigned long commit_head;
    unsigned long canon_head;
    unsigned long echo_head;
    unsigned long echo_commit;
    unsigned long echo_mark;
    unsigned long char_map[BITMAP_SIZE(256)];
    unsigned char read_buf[N_TTY_BUF_SIZE];
    unsigned char echo_buf[N_TTY_BUF_SIZE];
    unsigned long read_flags[BITMAP_SIZE(N_TTY_BUF_SIZE)];
    unsigned long read_tail;
    unsigned long line_start;
    unsigned int column;
    unsigned int canon_column;
    unsigned long echo_tail;
};

static const unsigned char ctype[] = {
    _C,     _C,    _C,    _C,    _C,    _C,    _C,    _C,
    _C,     _C|_S, _C|_S, _C|_S, _C|_S, _C|_S, _C,    _C,
    _C,     _C,    _C,    _C,    _C,    _C,    _C,    _C,
    _C,     _C,    _C,    _C,    _C,    _C,    _C,    _C,
    _S|_SP, _P,    _P,    _P,    _P,    _P,    _P,    _P,
    _P,     _P,    _P,    _P,    _P,    _P,    _P,    _P,
    _D,     _D,    _D,    _D,    _D,    _D,    _D,    _D,
    _D,     _D,    _P,    _P,    _P,    _P,    _P,    _P,
    _P,     _U|_X, _U|_X, _U|_X, _U|_X, _U|_X, _U|_X, _U,
    _U,     _U,    _U,    _U,    _U,    _U,    _U,    _U,
    _U,     _U,    _U,    _U,    _U,    _U,    _U,    _U,
    _U,     _U,    _U,    _P,    _P,    _P,    _P,    _P,
    _P,     _L|_X, _L|_X, _L|_X, _L|_X, _L|_X, _L|_X, _L,
    _L,     _L,    _L,    _L,    _L,    _L,    _L,    _L,
    _L,     _L,    _L,    _L,    _L,    _L,    _L,    _L,
    _L,     _L,    _L,    _P,    _P,    _P,    _P,    _C,
     0,      0,     0,     0,     0,     0,     0,     0,
     0,      0,     0,     0,     0,     0,     0,     0,
     0,      0,     0,     0,     0,     0,     0,     0,
     0,      0,     0,     0,     0,     0,     0,     0,
    _S|_SP, _P,    _P,    _P,    _P,    _P,    _P,    _P,
    _P,     _P,    _P,    _P,    _P,    _P,    _P,    _P,
    _P,     _P,    _P,    _P,    _P,    _P,    _P,    _P,
    _P,     _P,    _P,    _P,    _P,    _P,    _P,    _P,
    _U,     _U,    _U,    _U,    _U,    _U,    _U,    _U,
    _U,     _U,    _U,    _U,    _U,    _U,    _U,    _U,
    _U,     _U,    _U,    _U,    _U,    _U,    _U,    _P,
    _U,     _U,    _U,    _U,    _U,    _U,    _U,    _L,
    _L,     _L,    _L,    _L,    _L,    _L,    _L,    _L,
    _L,     _L,    _L,    _L,    _L,    _L,    _L,    _L,
     0,      0,     0,     0,     0,     0,     0,     0,
     0,      0,     0,     0,     0,     0,     0,     0
};
static struct tty_ldisc_ops n_tty_ldisc_ops = {
    .close = n_tty_close,
    .open = n_tty_open,
    .read = n_tty_read,
    .receive_buf = n_tty_receive_buf,
    .write = n_tty_write
};
static struct file_ops tty_file_ops = {
    .close = tty_close,
    .ioctl = tty_ioctl,
    .open  = tty_open,
    .read  = tty_read,
    .write = tty_write
};

static inline unsigned char *echo_buf_addr(struct n_tty_data *ldata, unsigned long i);
static inline unsigned char *read_buf_addr(struct n_tty_data *ldata, unsigned long i);

static inline void add_echo_byte(struct n_tty_data *ldata, unsigned char c)
{
    *echo_buf_addr(ldata, ldata->echo_head) = c;
    SMP_WMB();
    ldata->echo_head++;
}

static inline void add_read_byte(struct n_tty_data *ldata, unsigned char c)
{
    *read_buf_addr(ldata, ldata->read_head) = c;
    ldata->read_head++;
}

static inline unsigned char echo_buf(struct n_tty_data *ldata, unsigned long i)
{
    SMP_RMB();
    return ldata->echo_buf[MASK(i)];
}

static inline unsigned char *echo_buf_addr(struct n_tty_data *ldata, unsigned long i)
{
    return &ldata->echo_buf[MASK(i)];
}

static inline unsigned char read_buf(struct n_tty_data *ldata, unsigned long i)
{
    return ldata->read_buf[MASK(i)];
}

static inline unsigned char *read_buf_addr(struct n_tty_data *ldata, unsigned long i)
{
    return &ldata->read_buf[MASK(i)];
}

static void erase(struct tty *tty, unsigned char c)
{
    struct n_tty_data *ldata = tty->disc_data;
    unsigned long head;
    if (ldata->read_head == ldata->canon_head) {
        return;
    }
    head = ldata->read_head;
    head--;
    c = read_buf(ldata, head);
    ldata->read_head = head;
    if (ISCTRL(c)) {
        add_echo_byte(ldata, '\b');
        add_echo_byte(ldata, ' ');
        add_echo_byte(ldata, '\b');
    }
    add_echo_byte(ldata, '\b');
    add_echo_byte(ldata, ' ');
    add_echo_byte(ldata, '\b');
}

static void flush_echoes(struct tty *tty)
{
    unsigned char c, op, caret;
    struct n_tty_data *ldata = tty->disc_data;
    unsigned long head, tail;
    caret = '^';
    head = ldata->echo_head;
    ldata->echo_mark = head;
    ldata->echo_commit = head;
    tail = ldata->echo_tail;
    while(MASK(ldata->echo_commit) != MASK(tail)) {
        c = echo_buf(ldata, tail);
        if(c == ECHO_OP_START) {
            op = echo_buf(ldata, tail + 1);
            op ^= 0100;
            tty->ops->write(tty, &caret, 1);
            tty->ops->write(tty, &op, 1);
            tail += 2;
        }
        else {
            tty->ops->write(tty, &c, 1);
            tail += 1;
        }
    }
    ldata->echo_tail = tail;
}

static int n_tty_check_jobctl(struct tty *tty, int signal) {
    int pid;
    struct process *current;
    current = CURRENT;
    pid = current->pid;
    if(pid != READ_ONCE(tty->pid_leader)) {
        do_kill(pid, signal);
        return -ERESTARTSYS;
    }
    return 0;
}

static int n_tty_close(struct tty *tty)
{
    struct n_tty_data *ldata;
    ldata = tty->disc_data;
    cake_free(ldata);
    return 0;
}

static int n_tty_open(struct tty *tty)
{
    unsigned long *marked_chars;
    struct n_tty_data *ldata;
    ldata = cake_alloc(sizeof(*ldata));
    memset(ldata, 0, sizeof(*ldata));
    marked_chars = ldata->char_map;
    tty->disc_data = ldata;
    bitmap_zero(marked_chars, 256);
    set_bit(marked_chars, TTY_NEWLINE_CHAR(tty));
    set_bit(marked_chars, TTY_ERASE_CHAR(tty));
    set_bit(marked_chars, TTY_INTR_CHAR(tty));
    set_bit(marked_chars, TTY_EOF_CHAR(tty));
    return 0;
}

static long n_tty_read(struct tty *tty, char *buffer, unsigned long count) 
{
    void *copy_from_buf;
    unsigned long n, size, more, c, t;
    unsigned long eol, found;
    unsigned long tail;
    struct wait wait;
    struct n_tty_data *ldata = tty->disc_data;
    if((c = n_tty_check_jobctl(tty, SIGTTIN)) != 0) {
        return c;
    }
    wait.sleeping = CURRENT;
    wait.waitlist.prev = &(wait.waitlist);
    wait.waitlist.next = &(wait.waitlist);
    while(1) {
        enqueue_wait(&(tty->waitqueue), &wait, PROCESS_STATE_INTERRUPTIBLE);
        if(READ_ONCE(ldata->canon_head) != READ_ONCE(ldata->read_tail)) {
            break; 
        }
        schedule_self();
    }
    dequeue_wait(&(tty->waitqueue), &wait);
    n = LOAD_ACQUIRE(&(ldata->canon_head)) - ldata->read_tail;
    n = count + 1 > n ? n : count + 1;
    tail = MASK(ldata->read_tail);
    size = tail + n > N_TTY_BUF_SIZE ? N_TTY_BUF_SIZE : tail + n;
    eol = find_next_bit(ldata->read_flags, tail, size);
    more = n - (size - tail);
    if((eol == N_TTY_BUF_SIZE) && more) {
        eol = find_next_bit(ldata->read_flags, 0, more);
        found = (eol != more);
    }
    else {
        found = (eol != size); 
    }
    n = eol - tail;
    if(n > N_TTY_BUF_SIZE) {
        n += N_TTY_BUF_SIZE;
    }
    c = n + found;
    if (!found || read_buf(ldata, eol) != DISABLED_CHAR) {
        c = c > count ? count : c;
        n = c;
    }
    t = n;
    size = N_TTY_BUF_SIZE - tail;
    copy_from_buf = read_buf_addr(ldata, tail);
    if(t > size) {
        copy_to_user(buffer, copy_from_buf, size);
        buffer += size;
        t -= size;
        copy_from_buf = ldata->read_buf;
    }
    copy_to_user(buffer, copy_from_buf, t);
    if (found)
        clear_bit(ldata->read_flags, eol);
    ldata->read_tail += c;
    return n;
}

static int n_tty_receive_buf(struct tty *tty, char *buffer, unsigned int count)
{
    char c;
    struct n_tty_data *ldata = tty->disc_data;
    unsigned long tail;
    int room, n, t, rcvd;
    rcvd = 0;
    while(1) {
        tail = ldata->read_tail;
        room = N_TTY_BUF_SIZE - (ldata->read_head - tail);
        room--;
        if(room <= 0) {
            break;
        }
        n = room > count ? count : room;
        if(!n) {
            break;
        }
        t = n;
        while(t--) {
            c = *buffer++;
            if(test_bit(ldata->char_map, c)) {
                n_tty_receive_marked_char(tty, c);
            }
            else {
                n_tty_receive_normal_char(tty, c);
            }
        }
        flush_echoes(tty);
        count -= n;
        rcvd += n;
    }
    return rcvd;
}

static void n_tty_receive_marked_char(struct tty *tty, char c)
{
    struct n_tty_data *ldata = tty->disc_data;
    if(c == TTY_NEWLINE_CHAR(tty)) {
        add_echo_byte(ldata, '\n');
        flush_echoes(tty);
        set_bit(ldata->read_flags, MASK(ldata->read_head));
        add_read_byte(ldata, '\n');
        STORE_RELEASE(&(ldata->canon_head), ldata->read_head);
        wake_waiter(&(tty->waitqueue));
    }
    else if(c == TTY_ERASE_CHAR(tty)) {
        erase(tty, c);
        flush_echoes(tty);
    }
    else if(c == TTY_INTR_CHAR(tty)) {
        add_echo_byte(ldata, ECHO_OP_START);
        add_echo_byte(ldata, c);
        add_echo_byte(ldata, '\n');
        flush_echoes(tty);
        ldata->read_tail = ldata->read_head;
        set_bit(ldata->read_flags, MASK(ldata->read_head));
        add_read_byte(ldata, '\n');
        STORE_RELEASE(&(ldata->canon_head), ldata->read_head);
        do_kill(tty->pid_leader, SIGINT);
        wake_waiter(&(tty->waitqueue));
    }
    else if(c == TTY_EOF_CHAR(tty)) {
        c = DISABLED_CHAR;
        set_bit(ldata->read_flags, MASK(ldata->read_head));
        add_read_byte(ldata, c);
        STORE_RELEASE(&(ldata->canon_head), ldata->read_head);
        wake_waiter(&(tty->waitqueue));
    }
}

static void n_tty_receive_normal_char(struct tty *tty, char c)
{
    struct n_tty_data *ldata = tty->disc_data;
    if(ISCTRL(c)) {
        add_echo_byte(ldata, ECHO_OP_START);
    }
    add_echo_byte(ldata, c);
    add_read_byte(ldata, c);
}

static long n_tty_write(struct tty *tty, char *buffer, unsigned long count)
{
    char *b = buffer;
    unsigned long c, written;
    if((c = n_tty_check_jobctl(tty, SIGTTOU)) != 0) {
        return c;
    }
    while(count) {
        written = tty->ops->write(tty, (unsigned char *) b, count);
        count -= written;
        b += written;
    }
    return b - buffer;
}

void register_tty_driver(struct tty_driver *driver)
{
    struct tty *tty;
    struct file *file;
    for(unsigned int i = driver->basefile; i < driver->num_devices; i++) {
        tty = cake_alloc(sizeof(*tty));
        tty->ops = driver->ops;
        tty->index = i - driver->basefile;
        tty->waitqueue.waitlist.prev = &(tty->waitqueue.waitlist);
        tty->waitqueue.waitlist.next = &(tty->waitqueue.waitlist);
        file = filesystem_file(i);
        file->ops = &tty_file_ops;
        file->extension = tty;
    }
}

static int tty_close(struct file *file)
{
    struct tty *tty = file->extension;
    --tty->open_count;
    if(!tty->open_count) {
        tty->ldisc->ops->close(tty);
        cake_free(tty->ldisc);
    }
    return tty->ops->close(tty, file);
}

static int tty_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
    unsigned long pid = arg;
    struct process *p = pid_process(pid);
    struct tty *tty = file->extension;
    if(p) {
        XCHG_RELAXED(&(tty->pid_leader), arg);
        pid_put(pid);
        return 0;
    }
    else {
        return -1;
    }
}

static int tty_open(struct file *file)
{
    int ret;
    struct tty *tty = file->extension;
    ret = tty->ops->open(tty, file);
    if(!tty->ldisc) {
        struct tty_ldisc *ldisc = cake_alloc(sizeof(struct tty_ldisc));
        ldisc->ops = &n_tty_ldisc_ops;
        ldisc->tty = tty;
        tty->ldisc = ldisc;
        ldisc->ops->open(tty);
    }
    tty->open_count++;
    return ret;
}

static long tty_read(struct file *file, char *user, unsigned long n)
{
    unsigned long count;
    struct tty *tty = file->extension;
    struct tty_ldisc *ld = tty->ldisc;
    count = ld->ops->read(tty, user, n);
    return count;
}

static long tty_write(struct file *file, char *user, unsigned long n)
{
    unsigned int count;
    unsigned long written = 0;
    struct tty *tty = file->extension;
    struct tty_ldisc *ld = tty->ldisc;
    char *cake_buffer = cake_alloc(N_TTY_BUF_SIZE);
    if(!cake_buffer) {
        return - 1;
    }
    while(written < n) {
        count = n - written;
        if(count > N_TTY_BUF_SIZE) {
            count = N_TTY_BUF_SIZE;
        }
        copy_from_user(cake_buffer, user, count);
        count = ld->ops->write(tty, cake_buffer, count);
        user += count;
        written += count;
    }
    cake_free(cake_buffer);
    return written;
}

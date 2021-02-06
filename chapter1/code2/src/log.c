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

#include <stdarg.h>
#include "cake/types.h"
#include "cake/log.h"
#include "cake/string.h"

#define BUFFER_SIZE                     (255)
#define MAX_BUFFER_DATA_FOR_NULL_TERM   (BUFFER_SIZE - 1)

struct writebuf {
    u8 pos;
    char s[BUFFER_SIZE];
};

static void flush(struct writebuf *w);
static int puthex(unsigned long x, struct writebuf *w);
static int putstr(char *s, struct writebuf *w);

extern void console_init();

static struct console *console;

static int check_flush(struct writebuf *w)
{
    if(w->pos == MAX_BUFFER_DATA_FOR_NULL_TERM) {
        flush(w);
        return 1;
    }
    return 0;
}

static void cleanbuf(struct writebuf *w)
{
    w->pos = 0;
    for(unsigned int i = 0; i < BUFFER_SIZE; i++) {
        w->s[i] = 0;
    }
}

static void flush(struct writebuf *w)
{
    if(w->pos) {
        console->write(w->s);
    }
}

void log(char *fmt, ...)
{
    char c;
    va_list va;
    va_start(va, fmt);
    struct writebuf w;
    cleanbuf(&w);
    while((c = *(fmt++)) != '\0') {
        if(c != '%') {
            w.s[w.pos++] = c;
            if(check_flush(&w)) {
                va_end(va);
                return;
            }
        }
        else {
            int ret;
            c = *(fmt++);
            switch(c)
            {
                case 's':
                    ret = putstr(va_arg(va, char*), &w);
                    if(ret) {
                        va_end(va);
                        return;
                    }
                    break;
                case 'x':
                    ret = puthex(va_arg(va, unsigned long), &w);
                    if(ret) {
                        va_end(va);
                        return;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    flush(&w);
    va_end(va);
}

void log_init()
{
    console_init();
}

static int puthex(unsigned long x, struct writebuf *w)
{
    char c;
    int d = 0;
    char temp[27];
    xintos(x, temp);
    while((c = temp[d++]) != '\0') {
        w->s[w->pos++] = c;
        if(check_flush(w)) {
            return 1;
        }
    }
    return 0;
}

static int putstr(char *s, struct writebuf *w)
{
    char c;
    while((c = *s++)  != '\0') {
        w->s[w->pos++] = c;
        if(check_flush(w)) {
            return 1;
        }
    }
    return 0;
}

void register_console(struct console *c)
{
    console = c;
}

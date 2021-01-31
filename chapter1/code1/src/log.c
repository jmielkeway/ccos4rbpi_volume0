#include "cake/log.h"

extern void console_init();

static struct console *console;

void log(char *s)
{
    console->write(s);
}

void log_init()
{
    console_init();
}

void register_console(struct console *c)
{
    console = c;
}

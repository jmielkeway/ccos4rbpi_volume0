#ifndef _CAKE_LOG_H
#define _CAKE_LOG_H

struct console {
    void (*write)(char *s);
};

void log(char *s);
void register_console(struct console *c);

#endif

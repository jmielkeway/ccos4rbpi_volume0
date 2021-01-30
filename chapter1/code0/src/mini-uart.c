extern int  __uart_can_io();
extern void __uart_putchar(char c);

static inline int check_ready()
{
    return __uart_can_io();
}

static inline void uart_putchar(char c)
{
    while(!check_ready()) {
    }
    return __uart_putchar(c);
}

void uart_puts(char *s)
{
    if(s) {
        for(char *str = s; *str != '\0'; str++) {
            uart_putchar(*str);
        }
    }
}

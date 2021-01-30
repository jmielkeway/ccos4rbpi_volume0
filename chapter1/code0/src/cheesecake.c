extern void __delay(unsigned long);
extern void uart_puts(char *s);

void cheesecake_main(void)
{
    char *version = "Version: 0.1.1\r\n";
    uart_puts("Hello, Cheesecake!\r\n");
    while (1) {
        uart_puts(version);
        uart_puts("\r\n");
        __delay(20000000);
    }
}

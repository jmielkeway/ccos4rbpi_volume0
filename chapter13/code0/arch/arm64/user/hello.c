#define STDIN       (0)
#define STDOUT      (1)
#define BUF_LEN     (0x100)

extern void exit(int code);
extern long write(int fd, char *buffer, unsigned long count);

int hello()
{
    write(STDOUT, "Hello, World!\n", 15);
    exit(0);
    return 0;
}

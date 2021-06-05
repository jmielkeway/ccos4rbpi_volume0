#define STDIN       (0)
#define STDOUT      (1)
#define BUF_LEN     (0x100)

void exit(int code);
long read(int fd, char *buffer, unsigned long count);
long write(int fd, char *buffer, unsigned long count);

int cat()
{
    char buf[BUF_LEN];
    unsigned long len;
    write(STDOUT, "Super Simple Cat:\n", 19); 
    while((len = read(STDIN, buf, BUF_LEN)) > 0) {
        write(STDOUT, buf, len);
    }
    exit(0);
    return 0;
}

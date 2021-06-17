#define STDIN       (0)
#define STDOUT      (1)
#define BUF_LEN     (0x100)

extern long read(int fd, char *buffer, unsigned long count);
extern long write(int fd, char *buffer, unsigned long count);

int cat()
{
    char buf[BUF_LEN];
    unsigned long len;
    write(STDOUT, "Super Simple Cat:\n", 19); 
    while(1) {
        while((len = read(STDIN, buf, BUF_LEN)) > 0) {
            write(STDOUT, buf, len);
        }
        write(STDOUT, "EOF!\n", 6);
    }
    return 0;
}

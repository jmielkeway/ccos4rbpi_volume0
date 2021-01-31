#include "cake/log.h"
#include "arch/timing.h"

extern void log_init();
void init();

void cheesecake_main(void)
{
    char *version = "Version: 0.1.2\r\n";
    init();
    log("Hello, Cheesecake!\r\n");
    while (1) {
        log(version);
        log("\r\n");
        DELAY(20000000);
    }
}

void init()
{
    log_init();
    log("LOG MODULE INITIALIZED\r\n");
}

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

#include "cake/log.h"
#include "arch/timing.h"

extern void log_init();

static void init();

void cheesecake_main(void)
{
    unsigned long count = 1;
    char *version = "0.1.2.3";
    init();
    log("Hello, Cheesecake!\r\n");
    while (1) {
        log("Version: %s\r\n", version);
        log("Message count: %x\r\n", count++);
        log("\r\n");
        DELAY(20000000);
    }
}

static void init()
{
    log_init();
    log("LOG MODULE INITIALIZED\r\n");
}

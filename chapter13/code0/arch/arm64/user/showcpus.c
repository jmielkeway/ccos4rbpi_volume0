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

#include "user/cpu.h"

#define STDOUT  (1)

unsigned long libc_strlen(const char *s);
int cpustat(unsigned long cpu, struct user_cpuinfo *cpuinfo);
void exit(int code);
void write(int fd, char *buffer, unsigned long count);

static void ltoa(unsigned long l, char *a);

int showcpus()
{
    unsigned long cpu, len;
    char statsbuf[64];
    struct user_cpuinfo cpuinfo;
    cpu = 0;
    while(cpustat(cpu, &cpuinfo)) {
        ltoa(cpuinfo.cpu, statsbuf);
        len = libc_strlen(statsbuf);
        statsbuf[len] = '\n';
        statsbuf[len + 1] = '\0';
        write(STDOUT, "\nCPU #: ", 9);
        write(STDOUT, statsbuf, len + 2);
        ltoa(cpuinfo.weight, statsbuf);
        len = libc_strlen(statsbuf);
        statsbuf[len] = '\n';
        statsbuf[len + 1] = '\0';
        write(STDOUT, "CPU WEIGHT: ", 13);
        write(STDOUT, statsbuf, len + 2);
        ltoa(cpuinfo.pid, statsbuf);
        len = libc_strlen(statsbuf);
        statsbuf[len] = '\n';
        statsbuf[len + 1] = '\0';
        write(STDOUT, "CPU RUNNING: ", 14);
        write(STDOUT, statsbuf, len + 2);
        cpu++;
    }
    exit(0);
    return 0;
}

void ltoa(unsigned long l, char *a)
{
    int temp_size = 0;
    char c, temp[20];
    do {
        c = l % 10;
        c = c + 0x30;
        temp[temp_size++] = c;
        l /= 10;
    } while(l);
    while(temp_size--) {
        *(a++) = temp[temp_size];
    }
    *(a++) = '\0';
}

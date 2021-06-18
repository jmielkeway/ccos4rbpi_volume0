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

#include "user/signal.h"

#define STDIN       (0)
#define STDOUT      (1)
#define BUF_LEN     (0x100)

#define SHELL_PROMPT ":~# "

int libc_sigaddset(unsigned long *set, int signo);
int libc_strcmp(const char *a, const char *b);
long libc_strlen(const char *s);

int clone(unsigned long flags);
int exec(int (*user_function)(void));
int getpid();
int ioctl(int fd, unsigned int request, unsigned long arg);
long read(int fd, char *buffer, unsigned long count);
int signal(int signo, void (*fn)(int));
int sigprocmask(unsigned long how, unsigned long *newset, unsigned long *oldset);
long waitpid(int pid, int *status, int options);
long write(int fd, char *buffer, unsigned long count);

int cat();

static void sigchld_handler(int signo);
static int shell_pid;

int shell()
{
    int pid;
    unsigned int shell_prompt_len;
    unsigned long mask, prev_mask;
    char buf[BUF_LEN];
    char *shell_prompt;
    unsigned long len;
    shell_prompt = SHELL_PROMPT;
    shell_prompt_len = libc_strlen(shell_prompt) + 1;
    mask = 0;
    shell_pid = getpid();
    ioctl(STDIN, 0, shell_pid);
    ioctl(STDOUT, 0, shell_pid);
    signal(SIGCHLD, sigchld_handler);
    libc_sigaddset(&mask, SIGCHLD);
    while(1) {
        write(STDOUT, "Welcome to Cheesecake Shell!\n", 30);
        while(1) {
            write(STDOUT, shell_prompt, shell_prompt_len);
            len = read(STDIN, buf, BUF_LEN - 1);
            if(!len) {
                write(STDOUT, "\n", 2);
                break;
            }
            else {
                sigprocmask(SIG_BLOCK, &mask, &prev_mask);
                buf[len - 1] = '\0';
                if(!libc_strcmp(buf, "")) {
                    continue;
                }
                else if(!libc_strcmp(buf, "exit")) {
                    break;
                }
                else if(!libc_strcmp(buf, "cat")) {
                    if((pid = clone(0)) == 0) {
                        exec(cat);
                    }
                    else if(pid < 0) {
                        write(STDOUT, "ERROR DURING FORK!\n", 20);
                    }
                }
                else {
                    buf[len - 1] = '\n';
                    write(STDOUT, "NO PROGRAM: ", 13);
                    write(STDOUT, buf, len);
                }
            }
            sigprocmask(SIG_SETMASK, &prev_mask, 0);
        }
    }
    return 0;
}

void sigchld_handler(int signo)
{
    write(STDOUT, "MISSION ACCOMPLISHED: RECEIVED A SIGCHLD!\n", 43);
}

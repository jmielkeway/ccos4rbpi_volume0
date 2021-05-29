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

#include "user/fork.h"
#include "user/signal.h"
#include "user/wait.h"

#define NUM_BUILTINS    (3)
#define NUM_PROGRAMS    (2)

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
int fault();
int hello();
int infinity();
int showcpus();

struct program {
    char name[32];
    char desc[32];
    int (*program)(void);
};

static int shell_builtin(char *buffer);
static int shell_clone_exec(char *buffer);
static int shell_exit(char *buffer);
static int shell_help();
static int shell_ls();
static void shell_run(int (*fn)(void));
static void shell_sigchld_handler(int signo);

static int fg;
static int shell_pid;

int shell()
{
    int pid, flags, status;
    unsigned int shell_prompt_len;
    unsigned long mask, prev_mask, len;
    char buf[BUF_LEN];
    char *shell_prompt;
    shell_prompt = SHELL_PROMPT;
    shell_prompt_len = libc_strlen(shell_prompt) + 1;
    mask = prev_mask = 0;
    shell_pid = getpid();
    ioctl(STDIN, 0, shell_pid);
    ioctl(STDOUT, 0, shell_pid);
    signal(SIGCHLD, shell_sigchld_handler);
    libc_sigaddset(&mask, SIGCHLD);
    libc_sigaddset(&prev_mask, SIGINT);
    while(1) {
        write(STDOUT, "Welcome to Cheesecake Shell!\n", 30);
        while(1) {
            sigprocmask(SIG_SETMASK, &prev_mask, 0);
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
                else if(shell_exit(buf)) {
                    break;
                }
                else if(shell_builtin(buf)) {
                    continue;
                }
                else if((fg = shell_clone_exec(buf)) == 0) {
                    buf[len - 1] = '\n';
                    write(STDOUT, "NO PROGRAM: ", 13);
                    write(STDOUT, buf, len);
                    continue;
                }
            }
            flags = 0;
            while((pid = waitpid(-1, &status, flags)) != 0) {
                if(pid == fg) {
                    if(WIFEXITED(status)) {
                        flags |= WNOHANG;
                        ioctl(STDIN, 0, shell_pid);
                        ioctl(STDOUT, 0, shell_pid);
                        switch(WEXITDECODE(status)) {
                            case 0:
                            case SIGINT:
                                break;
                            case SIGSEGV:
                                write(STDOUT, "Segmentation Fault\n", 20); 
                                break;
                            default:
                                write(STDOUT, "Undefined Error\n", 17); 
                                break;
                        }
                    }
                }
            }
            sigprocmask(SIG_SETMASK, &prev_mask, 0);
        }
    }
    return 0;
}

static int shell_builtin(char *buffer)
{
    int found = 0;
    if(!libc_strcmp(buffer, "ls")) {
        shell_ls();
        found = 1;
    }
    else if(!libc_strcmp(buffer, "help")) {
        shell_help();
        found = 1;
    }
    return found;
}

static int shell_clone_exec(char *buffer)
{
    int pid = 0;
    unsigned long flags = (CLONE_STANDARD | CLONE_PRIORITY_USER);
    if(!libc_strcmp(buffer, "cat")) {
        if((pid = clone(flags)) == 0) {
            shell_run(cat);
        }
    }
    else if(!libc_strcmp(buffer, "fault")) {
        if((pid = clone(flags)) == 0) {
            shell_run(fault);
        }
    }
    else if(!libc_strcmp(buffer, "hello")) {
        if((pid = clone(flags)) == 0) {
            shell_run(hello);
        }
    }
    else if(!libc_strcmp(buffer, "infinity")) {
        if((pid = clone(flags)) == 0) {
            shell_run(infinity);
        }
    }
    else if(!libc_strcmp(buffer, "showcpus")) {
        if((pid = clone(flags)) == 0) {
            shell_run(showcpus);
        }
    }
    return pid;
}

static int shell_exit(char *buffer)
{
    return !libc_strcmp(buffer, "exit");
}

static int shell_help()
{
    write(STDOUT, "Cheesecake Shell v1.0.0\n", 25);
    write(STDOUT, "Usage: [command]\n\n", 19);
    write(STDOUT, "  help      display this help message\n", 39);
    write(STDOUT, "  ls        list available programs\n", 37);
    return 0;
}

static int shell_ls()
{
    write(STDOUT, "cat\n", 5);
    write(STDOUT, "fault\n", 7);
    write(STDOUT, "hello\n", 7);
    write(STDOUT, "infinity\n", 10);
    write(STDOUT, "showcpus\n", 10);
    return 0;
}

static void shell_run(int (*fn)(void))
{
    int pid = getpid();
    ioctl(STDIN, 0, pid);
    ioctl(STDOUT, 0, pid);
    exec(fn);
}

void shell_sigchld_handler(int signo)
{
}

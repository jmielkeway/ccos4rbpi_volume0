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

extern int __clone(unsigned long flags, unsigned long thread_input, unsigned long arg);
extern int __exec(void *user_function);
extern int __getpid();
extern int __ioctl(int fd, unsigned int request, void *arg);
extern long __read(int fd, char *buffer, unsigned long count);
extern long __write(int fd, char *buffer, unsigned long count);

int libc_strcmp(const char *a, const char *b)
{
    while(*a && (*a == *b)) {
        a++;
        b++;
    }
    return *((const unsigned char *) a) - *((const unsigned char *) b);
}

unsigned long libc_strlen(const char *s)
{
    unsigned long n;
    while(*s++) {
        n++;
    }
    return n;
}

int clone(unsigned long flags)
{
    return __clone(flags, 0, 0);
}

int exec(void *user_function)
{
    return __exec(user_function);
}

int getpid()
{
    return __getpid();
}

int ioctl(int fd, unsigned int request, void *arg)
{
    return __ioctl(fd, request, arg);
}

long read(int fd, char *buffer, unsigned long count)
{
    return __read(fd, buffer, count);
}

long write(int fd, char *buffer, unsigned long count)
{
    return __write(fd, buffer, count);
}

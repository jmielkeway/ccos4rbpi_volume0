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

#ifndef _USER_WAIT_H
#define _USER_WAIT_H

#define WNOHANG             (0b0001)
#define WSTOPPED            (0b0010)
#define WCONTINUED          (0b0100)
#define WEXITED             (0b1000)

#define WIFSTOPPED(x)       (x & WSTOPPED)
#define WIFCONTINUED(x)     (x & WCONTINUED)
#define WIFEXITED(x)        (x & WEXITED)

#define EXITCODE_SHIFT      (8)
#define WEXITCODE(x)        (x << EXITCODE_SHIFT)
#define WEXITDECODE(x)      (((unsigned int) (x)) >> EXITCODE_SHIFT)

#endif

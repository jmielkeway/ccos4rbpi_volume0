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

#ifndef _ARCH_SCHEDULE_H
#define _ARCH_SCHEDULE_H

#include "cake/process.h"

#define ARCH_GET_CURRENT    __current()
#define SCHEDULE_CURRENT    __schedule_current_init

struct process *__current();
void __schedule_current_init();

#endif

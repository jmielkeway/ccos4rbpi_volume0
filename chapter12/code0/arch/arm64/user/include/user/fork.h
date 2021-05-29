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

#ifndef _USER_FORK_H
#define _USER_FORK_H

#define CLONE_STANDARD              0b00000000
#define CLONE_CAKE_THREAD           0b00000001
#define CLONE_VM                    0b00000010 
#define CLONE_SIGNAL                0b00000100 
#define CLONE_FILES                 0b00001000 
#define CLONE_THREAD                (CLONE_VM | CLONE_SIGNAL | CLONE_FILES)

#define CLONE_PRIORITY_MAX          ((3))
#define CLONE_PRIORITY_SHIFT        ((8))
#define CLONE_PRIORITY_CAKE_THREAD  ((0) << (CLONE_PRIORITY_SHIFT))
#define CLONE_PRIORITY_USER         ((1) << (CLONE_PRIORITY_SHIFT))
#define CLONE_PRIORITY(x)           (((x) >> (CLONE_PRIORITY_SHIFT)) & CLONE_PRIORITY_MAX)

#endif

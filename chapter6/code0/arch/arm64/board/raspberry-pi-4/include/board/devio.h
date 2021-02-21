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

#ifndef _H_BOARD_DEVIO_H
#define _H_BOARD_DEVIO_H

    .macro __DEV_READ_32, dst, src
        ldr     \dst, [\src]
        dsb     ld
    .endm

    .macro __DEV_WRITE_32, src, dst
        dsb     st
        str     \src, [\dst]
    .endm

    .macro __DEV_WRITE_8, src, dst
        dsb     st
        strb    \src, [\dst]
    .endm

#endif

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

#ifndef _BOARD_PERIPHERAL_H
#define _BOARD_PERIPHERAL_H

#define MAIN_PERIPH_BASE             (0x47C000000)
#define LOCAL_PERIPH_BASE            (0x4C0000000)
#define GIC_BASE                     ((LOCAL_PERIPH_BASE) + 0x00040000)

#endif

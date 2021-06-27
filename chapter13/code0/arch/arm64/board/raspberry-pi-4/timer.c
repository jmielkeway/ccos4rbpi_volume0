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

#define INTERRUPT_INTERVAL  (100000)

extern unsigned int __timer_clock_low();
extern void __timer_control_reset();
extern void __timer_set_compare(unsigned int compare);

static unsigned int current;

void timer_init()
{
    current = __timer_clock_low();
    current += INTERRUPT_INTERVAL;
    __timer_set_compare(current);
}

void timer_interrupt()
{
    timer_init();
    __timer_control_reset();
}

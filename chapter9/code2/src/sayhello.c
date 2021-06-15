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

#include "cake/log.h"
#include "arch/smp.h"
#include "arch/irq.h"

long sys_sayhello()
{
    unsigned long cpuid = SMP_ID();
    log("CPU #%x SAYS HELLO!\r\n", cpuid);
    return 0;
}

long sys_usloopsleep()
{
    WAIT_FOR_INTERRUPT();
    return 0;
}

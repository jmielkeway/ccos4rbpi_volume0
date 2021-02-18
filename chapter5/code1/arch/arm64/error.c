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

const char *error_messages[] = {
    "INVALID_SYNC_EL1T",
    "INVALID_IRQ_EL1T",
    "INVALID_FIQ_EL1T",
    "INVALID_ERROR_EL1T",
    "INVALID_SYNC_EL1H",
    "INVALID_IRQ_EL1H",
    "INVALID_FIQ_EL1H",
    "INVALID_ERROR_EL1H",
    "INVALID_SYNC_EL0_64",
    "INVALID_IRQ_EL0_64",
    "INVALID_FIQ_EL0_64",
    "INVALID_ERROR_EL0_64",
    "INVALID_SYNC_EL0_32",
    "INVALID_IRQ_EL0_32",
    "INVALID_FIQ_EL0_32",
    "INVALID_ERROR_EL0_32"
};

void log_invalid_exception(int idx)
{
    const char *message = error_messages[idx];
    log("Encountered Invalid Exception Entry: %s\r\n", message);
}

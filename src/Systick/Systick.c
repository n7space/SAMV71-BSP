/**@file
 * This file is part of the ARM BSP for the Test Environment.
 *
 * @copyright 2020-2021 N7 Space Sp. z o.o.
 *
 * Test Environment was developed under a programme of,
 * and funded by, the European Space Agency (the "ESA").
 *
 *
 * Licensed under the ESA Public License (ESA-PL) Permissive,
 * Version 2.3 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://essr.esa.int/license/list
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Systick.h"

#include <string.h>

void
Systick_init(Systick *const systick)
{
	systick->registers = (Systick_Registers *)SYSTICK_ADDRESS_BASE;
}

void
Systick_setConfig(Systick *const systick, const Systick_Config *const config)
{
	const uint32_t mask = SYSTICK_CSR_CLKSOURCE_MASK
			| SYSTICK_CSR_ENABLE_MASK | SYSTICK_CSR_TICKINT_MASK;
	systick->registers->csr = (systick->registers->csr & ~mask)
			| ((((uint32_t)config->clockSource)
					   << SYSTICK_CSR_CLKSOURCE_OFFSET)
					& SYSTICK_CSR_CLKSOURCE_MASK)
			| (((config->isEnabled ? 1U : 0U)
					   << SYSTICK_CSR_ENABLE_OFFSET)
					& SYSTICK_CSR_ENABLE_MASK)
			| (((config->isInterruptEnabled ? 1U : 0U)
					   << SYSTICK_CSR_TICKINT_OFFSET)
					& SYSTICK_CSR_TICKINT_MASK);
	systick->registers->rvr = (uint32_t)(
			(config->reloadValue << SYSTICK_RVR_RELOAD_OFFSET)
			& SYSTICK_RVR_RELOAD_MASK);
}

void
Systick_getConfig(const Systick *const systick, Systick_Config *const config)
{
	const uint32_t rvr = systick->registers->rvr;
	const uint32_t csr = systick->registers->csr;
	config->clockSource =
			(Systick_ClockSource)((csr & SYSTICK_CSR_CLKSOURCE_MASK)
					>> SYSTICK_CSR_CLKSOURCE_OFFSET);
	config->isEnabled = (csr & SYSTICK_CSR_ENABLE_MASK) != 0u;
	config->isInterruptEnabled = (csr & SYSTICK_CSR_TICKINT_MASK) != 0u;
	config->reloadValue = (uint32_t)((rvr & SYSTICK_RVR_RELOAD_MASK)
			>> SYSTICK_RVR_RELOAD_OFFSET);
}

void
Systick_getImplementationInformation(const Systick *const systick,
		Systick_ImplementationInformation *const info)
{
	const uint32_t calib = systick->registers->calib;
	info->calibrationValue = (uint32_t)((calib & SYSTICK_CALIB_TENMS_MASK)
			>> SYSTICK_CALIB_TENMS_OFFSET);
	info->isCalibrationValueExact = (calib & SYSTICK_CALIB_SKEW_MASK) == 0u;
	info->isTheReferenceClockImplemented =
			(calib & SYSTICK_CALIB_NOREF_MASK) == 0u;
}

void
Systick_clearCurrentValue(Systick *const systick)
{
	systick->registers->cvr = 0;
}

uint32_t
Systick_getCurrentValue(const Systick *const systick)
{
	return (uint32_t)((systick->registers->cvr & SYSTICK_CVR_CURRENT_MASK)
			>> SYSTICK_CVR_CURRENT_OFFSET);
}

bool
Systick_hasCountedToZero(const Systick *const systick)
{
	return (systick->registers->csr & SYSTICK_CSR_COUNTFLAG_MASK) != 0u;
}

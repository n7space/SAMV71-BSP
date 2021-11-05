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

#include "Rstc.h"

#include <assert.h>
#include <stddef.h>

static volatile Rstc_Registers *Rstc_registers =
		(volatile Rstc_Registers *)RSTC_BASE_ADDRESS;

void
Rstc_getStatus(Rstc_Status *const status)
{
	assert(status != NULL);
	const uint32_t regValue = Rstc_registers->sr;
	status->wasUserResetTriggered = (regValue & RSTC_SR_URSTS_MASK) != 0u;
	status->resetType = (Rstc_ResetType)((regValue & RSTC_SR_RSTTYP_MASK)
			>> RSTC_SR_RSTTYP_OFFSET);
	status->isNrstHigh = (regValue & RSTC_SR_NRSTL_MASK) != 0u;
	status->isCommandInProgress = (regValue & RSTC_SR_SRCMP_MASK) != 0u;
}

bool
Rstc_isBusy(void)
{
	Rstc_Status status;
	Rstc_getStatus(&status);
	return status.isCommandInProgress;
}

Rstc_ResetType
Rstc_getLastResetType(void)
{
	Rstc_Status status;
	Rstc_getStatus(&status);
	return status.resetType;
}

__attribute__((noreturn)) void
Rstc_resetSystem(void)
{
	Rstc_registers->cr = (RSTC_CR_KEY_PASSWD << RSTC_CR_KEY_OFFSET)
			| ((1u << RSTC_CR_PROCRST_OFFSET)
					& RSTC_CR_PROCRST_MASK);

	while (true)
		asm volatile("nop");
}

void
Rstc_triggerExternalReset(void)
{
	Rstc_registers->cr = (RSTC_CR_KEY_PASSWD << RSTC_CR_KEY_OFFSET)
			| ((1u << RSTC_CR_EXTRST_OFFSET) & RSTC_CR_EXTRST_MASK);
}

bool
Rstc_wasNrstTransitionDetected(void)
{
	Rstc_Status status;
	Rstc_getStatus(&status);
	return status.wasUserResetTriggered;
}

void
Rstc_setUserResetEnabled(bool enabled)
{
	uint32_t mr = Rstc_registers->mr;
	mr &= ~RSTC_MR_URSTEN_MASK;
	mr |= RSTC_MR_KEY_PASSWD << RSTC_MR_KEY_OFFSET;
	mr |= ((enabled ? 1u : 0u) << RSTC_MR_URSTEN_OFFSET)
			& RSTC_MR_URSTEN_MASK;
	Rstc_registers->mr = mr;
}

bool
Rstc_isUserResetEnabled(void)
{
	return (Rstc_registers->mr & RSTC_MR_URSTEN_MASK) != 0u;
}

void
Rstc_setUserResetInterruptEnabled(bool enabled)
{
	uint32_t mr = Rstc_registers->mr;
	mr &= ~RSTC_MR_URSTIEN_MASK;
	mr |= RSTC_MR_KEY_PASSWD << RSTC_MR_KEY_OFFSET;
	mr |= ((enabled ? 1u : 0u) << RSTC_MR_URSTIEN_OFFSET)
			& RSTC_MR_URSTIEN_MASK;
	Rstc_registers->mr = mr;
}

bool
Rstc_isUserResetInterruptEnabled(void)
{
	return (Rstc_registers->mr & RSTC_MR_URSTIEN_MASK) != 0u;
}

void
Rstc_setExternalResetLength(uint8_t length)
{
	uint32_t mr = Rstc_registers->mr;
	mr &= ~RSTC_MR_ERSTL_MASK;
	mr |= RSTC_MR_KEY_PASSWD << RSTC_MR_KEY_OFFSET;
	mr |= (((uint32_t)length << RSTC_MR_ERSTL_OFFSET) & RSTC_MR_ERSTL_MASK);
	Rstc_registers->mr = mr;
}

uint8_t
Rstc_getExternalResetLength(void)
{
	return (uint8_t)((Rstc_registers->mr & RSTC_MR_ERSTL_MASK)
			>> RSTC_MR_ERSTL_OFFSET);
}

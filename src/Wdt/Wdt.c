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

#include "Wdt.h"

#include <string.h>

void
Wdt_init(Wdt *const wdt)
{
	wdt->registers = (Wdt_Registers *)WDT0_ADDRESS_BASE;
}

void
Wdt_setConfig(Wdt *const wdt, const Wdt_Config *const config)
{
	wdt->registers->mr =
			(((uint32_t)config->counterValue << WDT_MR_WDV_OFFSET)
					& WDT_MR_WDV_MASK)
			| (((uint32_t)config->deltaValue << WDT_MR_WDD_OFFSET)
					& WDT_MR_WDD_MASK)
			| (((config->isDisabled ? 1u : 0u)
					   << WDT_MR_WDDIS_OFFSET)
					& WDT_MR_WDDIS_MASK)
			| (((config->isFaultInterruptEnabled ? 1u : 0u)
					   << WDT_MR_WDFIEN_OFFSET)
					& WDT_MR_WDFIEN_MASK)
			| (((config->isResetEnabled ? 1u : 0u)
					   << WDT_MR_WDRSTEN_OFFSET)
					& WDT_MR_WDRSTEN_MASK)
			| (((config->isHaltedOnDebug ? 1u : 0u)
					   << WDT_MR_WDDBGHLT_OFFSET)
					& WDT_MR_WDDBGHLT_MASK)
			| (((config->isHaltedOnIdle ? 1u : 0u)
					   << WDT_MR_WDIDLEHLT_OFFSET)
					& WDT_MR_WDIDLEHLT_MASK);
}

void
Wdt_getConfig(const Wdt *const wdt, Wdt_Config *const config)
{
	config->counterValue = (wdt->registers->mr & WDT_MR_WDV_MASK)
			>> WDT_MR_WDV_OFFSET;
	config->deltaValue = (wdt->registers->mr & WDT_MR_WDD_MASK)
			>> WDT_MR_WDD_OFFSET;
	config->isDisabled = (wdt->registers->mr & WDT_MR_WDDIS_MASK) != 0u;
	config->isFaultInterruptEnabled =
			(wdt->registers->mr & WDT_MR_WDFIEN_MASK) != 0u;
	config->isResetEnabled =
			(wdt->registers->mr & WDT_MR_WDRSTEN_MASK) != 0u;
	config->isHaltedOnDebug =
			(wdt->registers->mr & WDT_MR_WDDBGHLT_MASK) != 0u;
	config->isHaltedOnIdle =
			(wdt->registers->mr & WDT_MR_WDIDLEHLT_MASK) != 0u;
}

void
Wdt_reset(Wdt *const wdt)
{
	wdt->registers->cr =
			((WDT_CR_KEY << WDT_CR_KEY_OFFSET) & WDT_CR_KEY_MASK)
			| ((1u << WDT_CR_WDRSTT_OFFSET) & WDT_CR_WDRSTT_MASK);
}

bool
Wdt_isUnderflow(const Wdt *const wdt)
{
	return (wdt->registers->sr & WDT_SR_WDUNF_MASK) != 0u;
}

bool
Wdt_isError(const Wdt *const wdt)
{
	return (wdt->registers->sr & WDT_SR_WDERR_MASK) != 0u;
}

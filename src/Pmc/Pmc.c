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

#include "Pmc.h"

#include <assert.h>

#include <Utils/Utils.h>

#include "PmcPeripheralId.h"
#include "PmcRegisters.h"

#define PMC_MCKRDY_TIMEOUT 10000
#define MEASUREMENT_ACCURACY 0.1

static Pmc_Registers *const pmcRegisters = ((Pmc_Registers *)PMC_BASE_ADDRESS);

static inline void
waitForRegister(const volatile uint32_t *const address, const uint32_t mask)
{
	while ((*address & mask) == 0u)
		asm volatile("nop");
}

static void
setRegisterAndWaitForMck(volatile uint32_t *const reg, uint32_t value)
{
	volatile uint32_t timeout = PMC_MCKRDY_TIMEOUT;
	*reg = value;
	while (((pmcRegisters->sr & PMC_SR_MCKRDY_MASK) == 0u)
			&& (timeout > 0u))
		--timeout;

	if (timeout == 0u)
		assert(false && "The main clock hasn't initialized within appropriate time.");
}

static void
resetMainAndMasterClockConfiguration(void)
{
	uint32_t mckr = pmcRegisters->mckr;
	mckr &= ~PMC_MCKR_CSS_MASK;
	mckr |= Pmc_MasterckSrc_Mainck;
	setRegisterAndWaitForMck(&pmcRegisters->mckr, mckr);

	setRegisterAndWaitForMck(&pmcRegisters->ckgrPllar, CKGR_PLLAR_ONE_MASK);
	setRegisterAndWaitForMck(&pmcRegisters->ckgrMor,
			pmcRegisters->ckgrMor
					| (CKGR_MOR_KEY_VALUE
							<< CKGR_MOR_KEY_OFFSET)
					| CKGR_MOR_MOSCRCEN_MASK);
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCRCS_MASK);

	if ((pmcRegisters->ckgrMor & CKGR_MOR_MOSCSEL_MASK) != 0u) {
		uint32_t ckgrMor = pmcRegisters->ckgrMor;
		ckgrMor &= ~CKGR_MOR_MOSCSEL_MASK;
		ckgrMor |= (CKGR_MOR_KEY_VALUE << CKGR_MOR_KEY_OFFSET);
		setRegisterAndWaitForMck(&pmcRegisters->ckgrMor, ckgrMor);
	}

	setRegisterAndWaitForMck(&pmcRegisters->ckgrMor,
			((CKGR_MOR_KEY_VALUE << CKGR_MOR_KEY_OFFSET)
					| CKGR_MOR_MOSCRCEN_MASK));

	mckr = pmcRegisters->mckr;
	mckr &= ~PMC_MCKR_MDIV_MASK;
	setRegisterAndWaitForMck(&pmcRegisters->mckr, mckr);

	mckr &= ~PMC_MCKR_PRES_MASK;
	setRegisterAndWaitForMck(&pmcRegisters->mckr, mckr);
}

static void
configureMainckRcOsc(const Pmc_Config *const config)
{
	uint32_t ckgrMor = pmcRegisters->ckgrMor;

	ckgrMor &= ~(uint32_t)CKGR_MOR_KEY_MASK;
	ckgrMor |= CKGR_MOR_KEY_VALUE << CKGR_MOR_KEY_OFFSET;

	ckgrMor |= CKGR_MOR_MOSCRCEN_MASK;
	pmcRegisters->ckgrMor = ckgrMor;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCRCS_MASK);

	ckgrMor &= ~(uint32_t)CKGR_MOR_MOSCRCF_MASK;
	ckgrMor |= ((uint32_t)config->rcOscFreq << CKGR_MOR_MOSCRCF_OFFSET);
	pmcRegisters->ckgrMor = ckgrMor;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCRCS_MASK);

	ckgrMor &= ~(uint32_t)CKGR_MOR_MOSCSEL_MASK;
	pmcRegisters->ckgrMor = ckgrMor;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCSELS_MASK);
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	ckgrMor &= ~(uint32_t)CKGR_MOR_MOSCXTEN_MASK;
	pmcRegisters->ckgrMor = ckgrMor;
}

static bool
configureMainckXOsc(const Pmc_Config *const config, int *const errCode)
{
	uint32_t ckgrMor = pmcRegisters->ckgrMor;

	ckgrMor &= ~(uint32_t)CKGR_MOR_KEY_MASK;
	ckgrMor |= CKGR_MOR_KEY_VALUE << CKGR_MOR_KEY_OFFSET;

	ckgrMor &= ~(uint32_t)CKGR_MOR_MOSCXTST_MASK;
	ckgrMor |= (uint32_t)((config->xoscStartupTime
					      / CKGR_MOR_MOSCXTST_DIVIDER)
				   << CKGR_MOR_MOSCXTST_OFFSET)
			& CKGR_MOR_MOSCXTST_MASK;

	ckgrMor |= CKGR_MOR_MOSCXTEN_MASK;

	pmcRegisters->ckgrMor = ckgrMor;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCRCS_MASK);
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCXTS_MASK);
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	ckgrMor |= CKGR_MOR_MOSCSEL_MASK;

	pmcRegisters->ckgrMor = ckgrMor;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCSELS_MASK);
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	Pmc_MainckMeasurement measurement;

	measurement.measuredClk = Pmc_MeasuredClk_XOsc;
	measurement.refFreq = PMC_SLOW_CLOCK_FREQ;
	measurement.measuredFreq = 0;
	Pmc_measureMainck(&measurement);
	if (!isBetweenUint32(measurement.measuredFreq,
			    (uint32_t)((float)PMC_MAIN_CRYSTAL_FREQ
					    * (1.0 - MEASUREMENT_ACCURACY)),
			    (uint32_t)((float)PMC_MAIN_CRYSTAL_FREQ
					    * (1.0 + MEASUREMENT_ACCURACY))))
		return returnError(errCode,
				Pmc_Error_MeasuredFrequencyIsIncorrect);

	ckgrMor &= ~CKGR_MOR_MOSCRCEN_MASK;
	ckgrMor &= ~CKGR_MOR_MOSCRCF_MASK;
	pmcRegisters->ckgrMor = ckgrMor;
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);
	return true;
}

static void
configurePlla(const Pmc_Config *const config)
{
	pmcRegisters->ckgrPllar = CKGR_PLLAR_ONE_MASK
			| ((uint32_t)(config->pllaMul << CKGR_PLLAR_MULA_OFFSET)
					& CKGR_PLLAR_MULA_MASK)
			| ((uint32_t)(config->pllaStartupTime
					   << CKGR_PLLAR_PLLACOUNT_OFFSET)
					& CKGR_PLLAR_PLLACOUNT_MASK)
			| ((uint32_t)(config->pllaDiv << CKGR_PLLAR_DIVA_OFFSET)
					& CKGR_PLLAR_DIVA_MASK);

	if ((config->pllaDiv > 0u) && (config->pllaMul > 0u))
		waitForRegister(&(pmcRegisters->sr), PMC_SR_LOCKA_MASK);
}

static void
configureMckr(const Pmc_Config *const config)
{
	uint32_t mckr;

	if (config->masterckSrc != Pmc_MasterckSrc_Pllack) {
		mckr = pmcRegisters->mckr;
		mckr &= ~PMC_MCKR_CSS_MASK;
		mckr |= (((uint32_t)config->masterckSrc << PMC_MCKR_CSS_OFFSET)
				& PMC_MCKR_CSS_MASK);

		pmcRegisters->mckr = mckr;

		waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);
	}

	mckr = pmcRegisters->mckr;
	mckr &= ~PMC_MCKR_PRES_MASK;
	mckr |= (((uint32_t)config->masterckPresc << PMC_MCKR_PRES_OFFSET)
			& PMC_MCKR_PRES_MASK);

	pmcRegisters->mckr = mckr;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	mckr = pmcRegisters->mckr;
	mckr &= ~PMC_MCKR_MDIV_MASK;
	mckr |= (((uint32_t)config->masterckDiv << PMC_MCKR_MDIV_OFFSET)
			& PMC_MCKR_MDIV_MASK);

	pmcRegisters->mckr = mckr;

	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	if (config->masterckSrc == Pmc_MasterckSrc_Pllack) {
		mckr = pmcRegisters->mckr;
		mckr &= ~PMC_MCKR_CSS_MASK;
		mckr |= (((uint32_t)config->masterckSrc << PMC_MCKR_CSS_OFFSET)
				& PMC_MCKR_CSS_MASK);

		pmcRegisters->mckr = mckr;

		waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);
	}
}

static void
configurePck(const Pmc_Config *const config)
{
	for (uint32_t i = 0; i < PMC_PCK_CLOCKS; i++) {
		pmcRegisters->scdr = 1u << (PMC_SCDR_PCK0_OFFSET + i);

		pmcRegisters->pck[i] = (((uint32_t)config->pckConfig[i].pckSrc
							<< PMC_PCK_CSS_OFFSET)
				| ((uint32_t)config->pckConfig[i].pckPresc
						<< PMC_PCK_PRES_OFFSET));

		if (config->pckConfig[i].isEnabled) {
			pmcRegisters->scer = 1u << (PMC_SCER_PCK0_OFFSET + i);

			waitForRegister(&(pmcRegisters->sr),
					(1u << (PMC_SR_PCKRDY0_OFFSET + i)));
		}
	}
}

static void
configurePeripheralCk(const Pmc_Config *const config)
{
	uint32_t peripheralClocks =
			config->peripheralClkEnabledFlags & 0xFFFFFFFFu;
	pmcRegisters->pcer0 = peripheralClocks & PMC_PCER0_MASK;
	pmcRegisters->pcdr0 = (~peripheralClocks) & PMC_PCDR0_MASK;

	peripheralClocks = (uint32_t)((config->peripheralClkEnabledFlags >> 32)
			& 0xFFFFFFFFu);
	pmcRegisters->pcer1 = peripheralClocks & PMC_PCER1_MASK;
	pmcRegisters->pcdr1 = (~peripheralClocks) & PMC_PCDR1_MASK;
}

static void
configureMainckBypass(void)
{
	uint32_t ckgrMor = pmcRegisters->ckgrMor;
	// Set the access key.
	ckgrMor &= ~(uint32_t)CKGR_MOR_KEY_MASK;
	ckgrMor |= CKGR_MOR_KEY_VALUE << CKGR_MOR_KEY_OFFSET;

	// Switch to external clock; crystal oscillator must be disabled.
	ckgrMor &= ~(uint32_t)CKGR_MOR_MOSCXTEN_MASK;
	ckgrMor |= CKGR_MOR_MOSCXTBY_MASK;
	pmcRegisters->ckgrMor = ckgrMor;

	// Wait for completion.
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	// Select crystal oscillator.
	ckgrMor |= CKGR_MOR_MOSCSEL_MASK;

	pmcRegisters->ckgrMor = ckgrMor;

	// Wait for switch completion.
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MOSCSELS_MASK);
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);

	// Disable RC Oscillator.
	ckgrMor &= ~CKGR_MOR_MOSCRCEN_MASK;
	pmcRegisters->ckgrMor = ckgrMor;
	waitForRegister(&(pmcRegisters->sr), PMC_SR_MCKRDY_MASK);
}

bool
Pmc_setConfig(const Pmc_Config *const config, int *const errCode)
{
	resetMainAndMasterClockConfiguration();

	switch (config->mainckSrc) {
	case Pmc_MainckSrc_RcOsc: configureMainckRcOsc(config); break;
	case Pmc_MainckSrc_XOsc:
		if (!configureMainckXOsc(config, errCode))
			return false;
		break;
	case Pmc_MainckSrc_XOscBypassed: configureMainckBypass(); break;
	}

	configurePlla(config);
	configureMckr(config);
	configurePck(config);
	configurePeripheralCk(config);
	return true;
}

void
Pmc_getConfig(Pmc_Config *const config)
{
	uint32_t ckgrMor = pmcRegisters->ckgrMor;
	config->rcOscFreq = (ckgrMor & CKGR_MOR_MOSCRCF_MASK)
			>> CKGR_MOR_MOSCRCF_OFFSET;
	config->xoscStartupTime =
			(uint16_t)(((ckgrMor & CKGR_MOR_MOSCXTST_MASK)
						   >> CKGR_MOR_MOSCXTST_OFFSET)
					* CKGR_MOR_MOSCXTST_DIVIDER);

	if ((ckgrMor & CKGR_MOR_MOSCSEL_MASK) != 0u) {
		// Main crystal oscillator.
		if ((ckgrMor & CKGR_MOR_MOSCXTBY_MASK) != 0u)
			config->mainckSrc = Pmc_MainckSrc_XOscBypassed;
		else
			config->mainckSrc = Pmc_MainckSrc_XOsc;
	} else
		// Main RC oscillator.
		config->mainckSrc = Pmc_MainckSrc_RcOsc;

	uint32_t ckgrPllar = pmcRegisters->ckgrPllar;
	config->pllaMul = (uint8_t)((ckgrPllar & CKGR_PLLAR_MULA_MASK)
			>> CKGR_PLLAR_MULA_OFFSET);
	config->pllaDiv = (ckgrPllar & CKGR_PLLAR_DIVA_MASK)
			>> CKGR_PLLAR_DIVA_OFFSET;
	config->pllaStartupTime = (ckgrPllar & CKGR_PLLAR_PLLACOUNT_MASK)
			>> CKGR_PLLAR_PLLACOUNT_OFFSET;

	uint32_t mckr = pmcRegisters->mckr;
	config->masterckSrc = (mckr & PMC_MCKR_CSS_MASK) >> PMC_MCKR_CSS_OFFSET;
	config->masterckPresc =
			(mckr & PMC_MCKR_PRES_MASK) >> PMC_MCKR_PRES_OFFSET;
	config->masterckDiv =
			(mckr & PMC_MCKR_MDIV_MASK) >> PMC_MCKR_MDIV_OFFSET;

	uint32_t pcsr0 = pmcRegisters->pcsr0 & PMC_PCSR0_MASK;
	uint32_t pcsr1 = pmcRegisters->pcsr1 & PMC_PCSR1_MASK;
	config->peripheralClkEnabledFlags = pcsr0 | ((uint64_t)pcsr1 << 32);

	uint32_t scsr = pmcRegisters->scsr;
	for (uint32_t i = 0; i < PMC_PCK_CLOCKS; i++) {
		uint32_t pck = pmcRegisters->pck[i];
		config->pckConfig[i].isEnabled =
				(scsr & (1u << (PMC_SCSR_PCK0_OFFSET + i)))
				!= 0u;
		config->pckConfig[i].pckSrc =
				(pck & PMC_PCK_CSS_MASK) >> PMC_PCK_CSS_OFFSET;
		config->pckConfig[i].pckPresc = (pck & PMC_PCK_PRES_MASK)
				>> PMC_PCK_PRES_OFFSET;
	}
}

void
Pmc_enablePeripheralClk(const Pmc_PeripheralId peripheralId)
{
	if ((uint32_t)peripheralId < 32u)
		pmcRegisters->pcer0 = 1u << (uint32_t)peripheralId;
	else
		pmcRegisters->pcer1 = 1u << ((uint32_t)peripheralId - 32u);
}

void
Pmc_disablePeripheralClk(const Pmc_PeripheralId peripheralId)
{
	if ((uint32_t)peripheralId < 32u)
		pmcRegisters->pcdr0 = 1u << (uint32_t)peripheralId;
	else
		pmcRegisters->pcdr1 = 1u << ((uint32_t)peripheralId - 32u);
}

bool
Pmc_isPeripheralClkEnabled(const Pmc_PeripheralId peripheralId)
{
	if ((uint32_t)peripheralId < 32u)
		return (pmcRegisters->pcsr0 & (1u << (uint32_t)peripheralId))
				!= 0u;
	return (pmcRegisters->pcsr1 & (1u << ((uint32_t)peripheralId - 32u)))
			!= 0u;
}

void
Pmc_measureMainck(Pmc_MainckMeasurement *data)
{
	pmcRegisters->ckgrMcfr =
			((uint32_t)data->measuredClk << CKGR_MCFR_CCSS_OFFSET)
			& CKGR_MCFR_CCSS_MASK;
	pmcRegisters->ckgrMcfr |= 1u << CKGR_MCFR_RCMEAS_OFFSET;

	waitForRegister(&(pmcRegisters->ckgrMcfr), CKGR_MCFR_MAINFRDY_MASK);

	data->measuredFreq = (((pmcRegisters->ckgrMcfr & CKGR_MCFR_MAINF_MASK)
					      >> CKGR_MCFR_MAINF_OFFSET)
					     * data->refFreq)
			/ CKGR_MCFR_MAINF_DIVIDER;
}

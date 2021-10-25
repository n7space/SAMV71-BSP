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

/**
 * @defgroup Pmc Pmc
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_PMC_H
#define BSP_PMC_H

/// \file Pmc.h
/// \brief Main header file for the Power Management Controller (PMC) driver.

#include <stdbool.h>
#include <stdint.h>

#include "PmcPeripheralId.h"

/// \brief Number of peripheral clocks.
#define PMC_PCK_CLOCKS 8u

/// \brief The built-in slow clock frequency in Hz.
#define PMC_SLOW_CLOCK_FREQ 32768u

#ifndef PMC_MAIN_CRYSTAL_FREQ
/// \brief The main crystal ocsillator frequency in Hz (can be overriden by an
/// external define).
#define PMC_MAIN_CRYSTAL_FREQ 12000000u
#endif

/// \brief Enumeration listring possible error codes.
typedef enum {
	/// \brief The measured clock frequency is incorrect.
	Pmc_Error_MeasuredFrequencyIsIncorrect = 1,
} Pmc_Error;

/// \brief Main clock source selection enumeration.
typedef enum {
	Pmc_MainckSrc_RcOsc, ///< Use the main RC oscillator.
	Pmc_MainckSrc_XOsc, ///< Use the main crystal oscillator.
	Pmc_MainckSrc_XOscBypassed, ///< Use an external clock (bypass the main crystal oscillator).
} Pmc_MainckSrc;

/// \brief Main RC oscillator frequency selection enumeration.
typedef enum {
	Pmc_RcOscFreq_4M = 0, ///< Set the frequency at 4MHz.
	Pmc_RcOscFreq_8M = 1, ///< Set the frequency at 8MHz.
	Pmc_RcOscFreq_12M = 2, ///< Set the frequency at 12MHz.
} Pmc_RcOscFreq;

/// \brief Master clock source selection enumeration.
typedef enum {
	Pmc_MasterckSrc_Slck = 0, ///< Use the slow clock.
	Pmc_MasterckSrc_Mainck = 1, ///< Use the main clock.
	Pmc_MasterckSrc_Pllack = 2, ///< Use the PLLA clock.
} Pmc_MasterckSrc;

/// \brief Master clock (processor clock) prescaler enumeration.
typedef enum {
	Pmc_MasterckPresc_1 = 0, ///< Don't divide the selected clock.
	Pmc_MasterckPresc_2 = 1, ///< Divide the selected clock by 2.
	Pmc_MasterckPresc_4 = 2, ///< Divide the selected clock by 4.
	Pmc_MasterckPresc_8 = 3, ///< Divide the selected clock by 8.
	Pmc_MasterckPresc_16 = 4, ///< Divide the selected clock by 16.
	Pmc_MasterckPresc_32 = 5, ///< Divide the selected clock by 32.
	Pmc_MasterckPresc_64 = 6, ///< Divide the selected clock by 64.
	Pmc_MasterckPresc_3 = 7, ///< Divide the selected clock by 3.
} Pmc_MasterckPresc;

/// \brief Master clock division selection enumeration. Only divider values 1
/// and 2 are allowed according to the SAMV71 familly errata DS80000767A, 5.1
typedef enum {
	Pmc_MasterckDiv_1 = 0, ///< Divide the clock further by 1.
	Pmc_MasterckDiv_2 = 1, ///< Divide the clock further by 2.
} Pmc_MasterckDiv;

/// \brief Programmable clock source selection enumeration.
typedef enum {
	Pmc_PckSrc_Slck = 0, ///< Use the slow clock.
	Pmc_PckSrc_Mainck = 1, ///< Use the main clock.
	Pmc_PckSrc_Pllack = 2, ///< Use the PLLA clock.
	Pmc_PckSrc_Masterck = 4, ///< Use the master clock.
} Pmc_PckSrc;

/// \brief A programmable clock configuration descriptor.
typedef struct {
	bool isEnabled; ///< Flag indicating whether to enable the programmable clock.
	Pmc_PckSrc pckSrc; ///< Clock source selector.
	uint8_t pckPresc; ///< Clock prescaler value.
} Pmc_PckConfig;

/// \brief Power Management Controller configuration descriptor.
typedef struct {
	Pmc_MainckSrc mainckSrc; ///< Main clock source selector.
	Pmc_RcOscFreq rcOscFreq; ///< Main RC oscillator frequency selector.
	uint16_t xoscStartupTime; ///< Main crystal oscillator startup time value.

	uint8_t pllaMul; ///< PLLA clock frequency multiplier value.
	uint8_t pllaDiv; ///< PLLA clock frequency divider value.
	uint8_t pllaStartupTime; ///< PLLA clock startup time value.

	Pmc_MasterckSrc masterckSrc; ///< Master clock source selector.
	Pmc_MasterckPresc
			masterckPresc; ///< Processor clock prescaler selector.
	Pmc_MasterckDiv masterckDiv; ///< MCK clock divider selector.

	uint64_t peripheralClkEnabledFlags; ///< Bitmask indicating enabled peripheral clocks.

	/// \brief Programmable clocks' configuration descriptors.
	Pmc_PckConfig pckConfig[PMC_PCK_CLOCKS];
} Pmc_Config;

/// \brief Enumeration used to select the oscillator to measure.
typedef enum {
	Pmc_MeasuredClk_RcOsc = 0, ///< Select the main RC oscillator.
	Pmc_MeasuredClk_XOsc = 1, ///< Select the main crystal oscillator.
} Pmc_MeasuredClk;

/// \brief Main clock frequency measurement descriptor.
typedef struct {
	Pmc_MeasuredClk measuredClk; ///< Oscillator to measure.
	uint32_t refFreq; ///< Reference frequeuncy.
	uint32_t measuredFreq; ///< Measured requency.
} Pmc_MainckMeasurement;

/// \brief Function used to configure the PMC.
/// \param [in] config PMC configuration descriptor.
/// \param [out] errCode Possible error code in case of a failure (may be NULL).
/// \returns Whether the configuration was successfully set.
bool Pmc_setConfig(const Pmc_Config *const config, int *const errCode);

/// \brief Function used to retrieve current configuration of the PMC.
/// \param [out] config Pointer to a configuration descriptor used to store
/// current configuration.
void Pmc_getConfig(Pmc_Config *const config);

/// \brief Function used to enable a peripheral clock.
/// \param [in] peripheralId Identifier of the peripheral clock.
void Pmc_enablePeripheralClk(const Pmc_PeripheralId peripheralId);

/// \brief Function used to disable a peripheral clock.
/// \param [in] peripheralId Identifier of the peripheral clock.
void Pmc_disablePeripheralClk(const Pmc_PeripheralId peripheralId);

/// \brief Function used to query the state of a peripheral clock.
/// \param [in] peripheralId Identifier of the peripheral clock.
/// \returns Whether the clock is enabled.
bool Pmc_isPeripheralClkEnabled(const Pmc_PeripheralId peripheralId);

/// \brief Function used to measure the frequency of a particular oscillator in
/// the system. \param [in,out] data Pointer to a measurement descriptor.
void Pmc_measureMainck(Pmc_MainckMeasurement *data);

#endif // BSP_PMC_H

/** @} */

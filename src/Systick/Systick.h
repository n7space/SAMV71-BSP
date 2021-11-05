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

/// \brief Header for the SysTick driver.

/**
 * @defgroup Systick Systick
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_SYSTICK_H
#define BSP_SYSTICK_H

#include <Utils/Utils.h>

#include "SystickRegisters.h"

/// \brief Enumeration listing possible SysTick clock sources.
typedef enum {
	/// \brief SysTick uses the IMPLEMENTATION DEFINED external reference
	/// clock.
	Systick_ClockSource_ImplementationDefined = 0,
	/// \brief SysTick uses the procesor clock.
	Systick_ClockSource_ProcessorClock = 1,
} Systick_ClockSource;

/// \brief Structure holding SysTick configuration.
typedef struct {
	Systick_ClockSource clockSource; ///< Clock source.
	bool isInterruptEnabled; ///< Is SysTick interrupt enabled.
	bool isEnabled; ///< Is SysTick enabled.
	uint32_t reloadValue; ///< Reload value.
} Systick_Config;

/// \brief Structure holding SysTick Implementation Information.
typedef struct {
	/// \brief Indicates whether the IMPLEMENTATION DEFINED reference clock
	/// is implemented.
	bool isTheReferenceClockImplemented;
	/// \brief Indicates whether the 10ms calibration value is exact.
	bool isCalibrationValueExact;
	/// \brief Holds a reload value to be used for 10ms timing. Value of 0
	/// indicates unknown.
	uint32_t calibrationValue;
} Systick_ImplementationInformation;

/// \brief Structure representing SysTick.
typedef struct {
	Systick_Registers *registers; ///< Pointer to SysTick registers.
} Systick;

/// \brief Initializes the structure representing SysTick.
/// \param [in,out] systick Pointer to a structure representing SysTick.
void Systick_init(Systick *const systick);

/// \brief Sets the SysTick configuration.
/// \param [in,out] systick Pointer to a structure representing SysTick.
/// \param [in] config SysTick instance configuration.
void Systick_setConfig(
		Systick *const systick, const Systick_Config *const config);

/// \brief Gets the SysTick configuration.
/// \param [in] systick Pointer to a structure representing SysTick.
/// \param [out] config SysTick instance configuration.
void Systick_getConfig(
		const Systick *const systick, Systick_Config *const config);

/// \brief Gets the SysTick Implementation Information.
/// \param [in] systick Pointer to a structure representing SysTick.
/// \param [out] info Implementation information.
void Systick_getImplementationInformation(const Systick *const systick,
		Systick_ImplementationInformation *const info);

/// \brief Clears the current SysTick counter value.
/// \param [in] systick Pointer to a structure representing SysTick.
void Systick_clearCurrentValue(Systick *const systick);

/// \brief Returns the current SysTick counter value.
/// \param [in] systick Pointer to a structure representing SysTick.
/// \returns Current counter value.
uint32_t Systick_getCurrentValue(const Systick *const systick);

/// brief Returns whether the counter has counted to 0 since the last read.
/// \param [in] systick Pointer to a structure representing SysTick.
/// \returns Whether the counter has counted to 0.
bool Systick_hasCountedToZero(const Systick *const systick);

#endif // BSP_SYSTICK_H

/** @} */

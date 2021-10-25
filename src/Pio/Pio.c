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

#include "Pio.h"

#include <Utils/Utils.h>

void
Pio_init(const Pio_Port port, Pio *const pio)
{
	switch (port) {
	case Pio_Port_A:
		pio->port = Pio_Port_A;
		pio->reg = (Pio_Registers *)PIOA_ADDRESS_BASE;
		break;
	case Pio_Port_B:
		pio->port = Pio_Port_B;
		pio->reg = (Pio_Registers *)PIOB_ADDRESS_BASE;
		break;
	case Pio_Port_C:
		pio->port = Pio_Port_C;
		pio->reg = (Pio_Registers *)PIOC_ADDRESS_BASE;
		break;
	case Pio_Port_D:
		pio->port = Pio_Port_D;
		pio->reg = (Pio_Registers *)PIOD_ADDRESS_BASE;
		break;
	case Pio_Port_E:
		pio->port = Pio_Port_E;
		pio->reg = (Pio_Registers *)PIOE_ADDRESS_BASE;
		break;
	default: return;
	}
}

static inline void
setControlConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	if (config->control == Pio_Control_Pio) {
		pio->reg->per = pinMask;
	} else {
		volatile uint32_t abcdsr1 = pio->reg->abcdsr1;
		volatile uint32_t abcdsr2 = pio->reg->abcdsr2;
		abcdsr1 &= ~pinMask;
		abcdsr2 &= ~pinMask;
		if (config->control == Pio_Control_PeripheralA) {
			pio->reg->abcdsr1 = abcdsr1;
			pio->reg->abcdsr2 = abcdsr2;
		} else if (config->control == Pio_Control_PeripheralB) {
			pio->reg->abcdsr1 = abcdsr1 | pinMask;
			pio->reg->abcdsr2 = abcdsr2;
		} else if (config->control == Pio_Control_PeripheralC) {
			pio->reg->abcdsr1 = abcdsr1;
			pio->reg->abcdsr2 = abcdsr2 | pinMask;
		} else {
			pio->reg->abcdsr1 = abcdsr1 | pinMask;
			pio->reg->abcdsr2 = abcdsr2 | pinMask;
		}

		pio->reg->pdr = pinMask;
	}
}

static inline void
setDirectionConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	switch (config->direction) {
	case Pio_Direction_Input:
		pio->reg->odr = pinMask;
		pio->reg->owdr = pinMask;
		break;
	case Pio_Direction_Output:
		pio->reg->oer = pinMask;
		pio->reg->owdr = pinMask;
		break;
	case Pio_Direction_SynchronousOutput:
		pio->reg->oer = pinMask;
		pio->reg->ower = pinMask;
		break;
	}
}

static inline void
setPullConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	switch (config->pull) {
	case Pio_Pull_None:
		pio->reg->pudr = pinMask;
		pio->reg->ppddr = pinMask;
		break;
	case Pio_Pull_Up:
		pio->reg->ppddr = pinMask;
		pio->reg->puer = pinMask;
		break;
	case Pio_Pull_Down:
		pio->reg->pudr = pinMask;
		pio->reg->ppder = pinMask;
		break;
	}
}

static inline void
setFilterConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	switch (config->filter) {
	case Pio_Filter_None:
		pio->reg->ifdr = pinMask;
		pio->reg->ifscdr = pinMask;
		break;
	case Pio_Filter_Glitch:
		pio->reg->ifscdr = pinMask;
		pio->reg->ifer = pinMask;
		break;
	case Pio_Filter_Debounce:
		pio->reg->ifscer = pinMask;
		pio->reg->ifer = pinMask;
		break;
	}
}

static inline void
setMultiDriveConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	if (config->isMultiDriveEnabled)
		pio->reg->mder = pinMask;
	else
		pio->reg->mddr = pinMask;
}

static inline void
setSchmittTriggerConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	volatile uint32_t schmitt = pio->reg->schmitt & ~pinMask;

	if (config->isSchmittTriggerDisabled)
		pio->reg->schmitt = schmitt | pinMask;
	else
		pio->reg->schmitt = schmitt;
}

static inline void
setIrqConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	pio->reg->idr = pinMask;
	switch (config->irq) {
	case Pio_Irq_None: pio->reg->aimdr = pinMask; break;
	case Pio_Irq_EdgeBoth:
		pio->reg->aimdr = pinMask;
		pio->reg->ier = pinMask;
		break;
	case Pio_Irq_EdgeRising:
		pio->reg->aimer = pinMask;
		pio->reg->esr = pinMask;
		pio->reg->rehlsr = pinMask;
		pio->reg->ier = pinMask;
		break;
	case Pio_Irq_EdgeFalling:
		pio->reg->aimer = pinMask;
		pio->reg->esr = pinMask;
		pio->reg->fellsr = pinMask;
		pio->reg->ier = pinMask;
		break;
	case Pio_Irq_LevelLow:
		pio->reg->aimer = pinMask;
		pio->reg->lsr = pinMask;
		pio->reg->fellsr = pinMask;
		pio->reg->ier = pinMask;
		break;
	case Pio_Irq_LevelHigh:
		pio->reg->aimer = pinMask;
		pio->reg->lsr = pinMask;
		pio->reg->rehlsr = pinMask;
		pio->reg->ier = pinMask;
		break;
	}
}

static inline void
setPioConfiguration(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	setControlConfig(pio, pinMask, config);
	setDirectionConfig(pio, pinMask, config);
	setPullConfig(pio, pinMask, config);
	setFilterConfig(pio, pinMask, config);
	setMultiDriveConfig(pio, pinMask, config);
	setSchmittTriggerConfig(pio, pinMask, config);
	setIrqConfig(pio, pinMask, config);
}

void
Pio_setPortConfig(Pio *const pio, const Pio_Port_Config *const config)
{
	setPioConfiguration(pio, config->pins, &config->pinsConfig);

	// Set the slow clock divider for the debounce filter
	pio->reg->scdr = ((uint32_t)config->debounceFilterDiv
					 & PIO_SCDR_DIV_MASK)
			<< PIO_SCDR_DIV_OFFSET;
}

bool
Pio_getPortConfig(const Pio *const pio, Pio_Port_Config *const config,
		int *errCode)
{
	config->debounceFilterDiv = (pio->reg->scdr & PIO_SCDR_DIV_MASK)
			>> PIO_SCDR_DIV_OFFSET;

	return Pio_getPinsConfig(
			pio, config->pins, &config->pinsConfig, errCode);
}

void
Pio_setPinsConfig(Pio *const pio, const uint32_t pinMask,
		const Pio_Pin_Config *const config)
{
	setPioConfiguration(pio, pinMask, config);
}

static bool
getControlConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t psr = pio->reg->psr & pinMask;
	const uint32_t abcdsr1 = pio->reg->abcdsr1 & pinMask;
	const uint32_t abcdsr2 = pio->reg->abcdsr2 & pinMask;

	if (psr == pinMask)
		config->control = Pio_Control_Pio;
	else if (psr == 0u) {
		if ((abcdsr1 == 0u) && (abcdsr2 == 0u))
			config->control = Pio_Control_PeripheralA;
		else if ((abcdsr1 == pinMask) && (abcdsr2 == 0u))
			config->control = Pio_Control_PeripheralB;
		else if ((abcdsr1 == 0u) && (abcdsr2 == pinMask))
			config->control = Pio_Control_PeripheralC;
		else if ((abcdsr1 == pinMask) && (abcdsr2 == pinMask))
			config->control = Pio_Control_PeripheralD;
		else
			return false;
	} else
		return false;
	return true;
}

static bool
getPullConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t pusr = pio->reg->pusr & pinMask;
	const uint32_t ppdsr = pio->reg->ppdsr & pinMask;

	if ((pusr == pinMask) && (ppdsr == pinMask))
		config->pull = Pio_Pull_None;
	else if ((pusr == 0u) && (ppdsr == pinMask))
		config->pull = Pio_Pull_Up;
	else if ((pusr == pinMask) && (ppdsr == 0u))
		config->pull = Pio_Pull_Down;
	else
		return false;
	return true;
}

static bool
getDirectionConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t osr = pio->reg->osr & pinMask;
	const uint32_t owsr = pio->reg->owsr & pinMask;

	if ((osr == 0u) && (owsr == 0u))
		config->direction = Pio_Direction_Input;
	else if ((osr == pinMask) && (owsr == 0u))
		config->direction = Pio_Direction_Output;
	else if ((osr == pinMask) && (owsr == pinMask))
		config->direction = Pio_Direction_SynchronousOutput;
	else
		return false;
	return true;
}

static bool
getFilterConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t ifsr = pio->reg->ifsr & pinMask;
	const uint32_t ifscsr = pio->reg->ifscsr & pinMask;

	if ((ifsr == 0u) && (ifscsr == 0u))
		config->filter = Pio_Filter_None;
	else if ((ifsr == pinMask) && (ifscsr == 0u))
		config->filter = Pio_Filter_Glitch;
	else if ((ifsr == pinMask) && (ifscsr == pinMask))
		config->filter = Pio_Filter_Debounce;
	else
		return false;
	return true;
}

static bool
getMultiDriveConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t mdsr = pio->reg->mdsr & pinMask;

	if (mdsr == pinMask)
		config->isMultiDriveEnabled = true;
	else if (mdsr == 0u)
		config->isMultiDriveEnabled = false;
	else
		return false;
	return true;
}

static bool
getSchmittTriggerConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t schmitt = pio->reg->schmitt & pinMask;

	if (schmitt == pinMask)
		config->isSchmittTriggerDisabled = true;
	else if (schmitt == 0u)
		config->isSchmittTriggerDisabled = false;
	else
		return false;
	return true;
}

static bool
getIrqConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config)
{
	const uint32_t imr = pio->reg->imr & pinMask;
	const uint32_t aimmr = pio->reg->aimmr & pinMask;
	const uint32_t elsr = pio->reg->elsr & pinMask;
	const uint32_t frlhsr = pio->reg->frlhsr & pinMask;

	if ((imr == 0u) && (aimmr == 0u))
		config->irq = Pio_Irq_None;
	else if ((imr == pinMask) && (aimmr == 0u))
		config->irq = Pio_Irq_EdgeBoth;
	else if ((imr == pinMask) && (aimmr == pinMask) && (elsr == 0u)
			&& (frlhsr == pinMask))
		config->irq = Pio_Irq_EdgeRising;
	else if ((imr == pinMask) && (aimmr == pinMask) && (elsr == 0u)
			&& (frlhsr == 0u))
		config->irq = Pio_Irq_EdgeFalling;
	else if ((imr == pinMask) && (aimmr == pinMask) && (elsr == pinMask)
			&& (frlhsr == pinMask))
		config->irq = Pio_Irq_LevelHigh;
	else if ((imr == pinMask) && (aimmr == pinMask) && (elsr == pinMask)
			&& (frlhsr == 0u))
		config->irq = Pio_Irq_LevelLow;
	else
		return false;
	return true;
}

bool
Pio_getPinsConfig(const Pio *const pio, const uint32_t pinMask,
		Pio_Pin_Config *const config, int *errCode)
{
	if (!getControlConfig(pio, pinMask, config))
		return returnError(
				errCode, Pio_ErrorCodes_ControlConfigMismatch);
	if (!getPullConfig(pio, pinMask, config))
		return returnError(errCode, Pio_ErrorCodes_PullConfigMismatch);
	if (!getDirectionConfig(pio, pinMask, config))
		return returnError(errCode,
				Pio_ErrorCodes_DirectionConfigMismatch);
	if (!getFilterConfig(pio, pinMask, config))
		return returnError(
				errCode, Pio_ErrorCodes_FilterConfigMismatch);
	if (!getMultiDriveConfig(pio, pinMask, config))
		return returnError(errCode,
				Pio_ErrorCodes_MultiDriveConfigMismatch);
	if (!getSchmittTriggerConfig(pio, pinMask, config))
		return returnError(errCode,
				Pio_ErrorCodes_SchmittTriggerConfigMismatch);
	if (!getIrqConfig(pio, pinMask, config))
		return returnError(errCode, Pio_ErrorCodes_IrqConfigMismatch);
	return true;
}

void
Pio_setPins(Pio *const pio, const uint32_t pinMask)
{
	pio->reg->sodr = pinMask;
}

void
Pio_resetPins(Pio *const pio, const uint32_t pinMask)
{
	pio->reg->codr = pinMask;
}

uint32_t
Pio_getPins(const Pio *const pio)
{
	return pio->reg->pdsr;
}

void
Pio_setPortValue(Pio *const pio, const uint32_t value)
{
	pio->reg->odsr = value;
}

uint32_t
Pio_getIrqStatus(const Pio *const pio)
{
	return pio->reg->isr;
}

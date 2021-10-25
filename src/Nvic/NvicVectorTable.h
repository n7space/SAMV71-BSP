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

/// \brief Header describing the vector table for the Cortex-M7 processor.

#ifndef BSP_NVIC_VECTORTABLE_H
#define BSP_NVIC_VECTORTABLE_H

#include <stdint.h>

#include "Nvic.h"

/// \brief Structure describing the layout of the vector table.
typedef struct {
	void *initialStackPointer; ///< Initial value of the stack pointer.
	void *initialProgramCounter; ///< Initial value of the program counter.
	void *nonMaskableInterruptHandler; ///< Handler of the non-maskable interrupt exception.
	void *hardFaultHandler; ///< Handler of the hard fault exception.
	void *memoryManagementFaultHandler; ///< Handler of the memory management fault exception.
	void *busFaultHandler; ///< Handler of the bus fault exception.
	void *usageFaultHandler; ///< Handler of the usage fault exception.
	void *reserved0[4]; ///< Reserved.
	void *svCallHandler; ///< Handler of the supervisor call exception.
	void *resesrved1[2]; // Reserved.
	void *pendSvHandler; ///< Handler of the PendSV request.
	void *sysTickHandler; ///< Handler of the system timer exception.
	/// \brief Table of interrupt handlers.
	Nvic_InterruptHandler irqHandler[Nvic_InterruptCount];
} Nvic_VectorTable;

#endif // BSP_NVIC_VECTORTABLE_H

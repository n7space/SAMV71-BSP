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

#ifndef BSP_SYSTICK_REGISTERS_H
#define BSP_SYSTICK_REGISTERS_H

#include <stdint.h>

/// \brief Structure representing SysTick registers.
typedef struct {
	volatile uint32_t csr; ///< Control and Status register.
	volatile uint32_t rvr; ///< Reload Value register.
	volatile uint32_t cvr; ///< Current Value register.
	volatile uint32_t calib; ///< Calibration Value register.
	volatile uint32_t reserved[56]; ///< Reserved.
} Systick_Registers;

/// \brief Address base of the SysTick registers.
#define SYSTICK_ADDRESS_BASE 0xE000E010

/// \brief Count Flag register offset.
#define SYSTICK_CSR_COUNTFLAG_OFFSET 16U
/// \brief Count Flag register mask.
#define SYSTICK_CSR_COUNTFLAG_MASK 0x00010000U

/// \brief SysTick Clock Source register offset.
#define SYSTICK_CSR_CLKSOURCE_OFFSET 2U
/// \brief SysTick Clock Source register mask.
#define SYSTICK_CSR_CLKSOURCE_MASK 0x00000004U

/// \brief SysTick Interrupt Status register offset.
#define SYSTICK_CSR_TICKINT_OFFSET 1U
/// \brief SysTick Interrupt Status register mask.
#define SYSTICK_CSR_TICKINT_MASK 0x00000002U

/// \brief SysTick Enable register offset.
#define SYSTICK_CSR_ENABLE_OFFSET 0U
/// \brief SysTick Enable register mask.
#define SYSTICK_CSR_ENABLE_MASK 0x00000001U

/// \brief SysTick Reload Value register offset.
#define SYSTICK_RVR_RELOAD_OFFSET 0U
/// \brief SysTick Reload Value register mask.
#define SYSTICK_RVR_RELOAD_MASK 0x00FFFFFFU

/// \brief SysTick Current Value register offset.
#define SYSTICK_CVR_CURRENT_OFFSET 0U
/// \brief SysTick Current Value register mask.
#define SYSTICK_CVR_CURRENT_MASK 0xFFFFFFFFU

/// \brief 10ms Reload Value register offset.
#define SYSTICK_CALIB_TENMS_OFFSET 0U
/// \brief 10ms Reload Value register mask.
#define SYSTICK_CALIB_TENMS_MASK 0x00FFFFFFU

/// \brief Is the 10ms Reload Value Exact register offset.
#define SYSTICK_CALIB_SKEW_OFFSET 30U
/// \brief Is the 10ms Reload Value Exact register mask.
#define SYSTICK_CALIB_SKEW_MASK 0x40000000U

/// \brief Is the Implementation Defined Reference Clock Implemented register
/// offset.
#define SYSTICK_CALIB_NOREF_OFFSET 31U
/// \brief Is the Implementation Defined Reference Clock Implemented register
/// mask.
#define SYSTICK_CALIB_NOREF_MASK 0x80000000U

#endif // BSP_SYSTICK_REGISTERS_H

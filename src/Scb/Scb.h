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

/// \brief Header containing prototypes of System Control Block specific
/// functions.

/**
 * @defgroup Scb Scb
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_SCB_H
#define BSP_SCB_H

#include "ScbRegisters.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#ifdef CLANG_TIDY
// Clang-tidy assumes x86 architecture.
#define ASM_R0 "eax"
#define ASM_R1 "ebx"
#define ASM_R2 "ecx"
#else
#define ASM_R0 "r0"
#define ASM_R1 "r1"
#define ASM_R2 "r2"
#endif

/// \brief Returns whether the instruction cache is enabled.
/// \returns Whether the instruction cache is enabled.
static inline bool
Scb_isICacheEnabled(void)
{
	const volatile Scb_Registers *const scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	return (scb->ccr & SCB_CCR_IC_MASK) != 0u;
}

/// \brief Returns whether the data cache is enabled.
/// \returns Whether the data cache is enabled.
static inline bool
Scb_isDCacheEnabled(void)
{
	const volatile Scb_Registers *const scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	return (scb->ccr & SCB_CCR_DC_MASK) != 0u;
}

/// \brief Returns whether the memory management exception is enabled.
/// \returns Whether the memory management exception is enabled.
static inline bool
Scb_isMemoryManagementExceptionEnabled(void)
{
	const volatile Scb_Registers *const scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	return (scb->shcsr & SCB_SHCSR_MEMFAULTENA_MASK) != 0u;
}

/// \brief Disables the instruction cache.
/// \returns Whether the cache invalidation procedure was executed.
static inline bool
Scb_DisableICache(void)
{
	volatile Scb_Registers *const scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	// If the cache is already disabled, don't disable it, as cache clean
	// and invalidation may lead to artefacts.
	if ((scb->ccr & SCB_CCR_IC_MASK) == 0u)
		return false;
	/*
	Below should be the equivalent of this code. This is rewritten to assembly to avoid any
	instrumentation (e.g. gcov) interference
	asm volatile("dsb");
	asm volatile("isb");
	scb->ccr &= ~SCB_CCR_IC_MASK; // Disable I-Cache.
	scb->iciallu = 0; // Invalidate I-cache.
	asm volatile("dsb");
	asm volatile("isb");
	*/
	const uint32_t newCcr = scb->ccr & ~(uint32_t)SCB_CCR_IC_MASK;
	asm volatile("mov r1, #0\n" // r1 = 0;
		     "dsb\n"
		     "isb\n"
		     "str %2, [%1]\n" // *CCR = newCcr;
		     "str r1, [%0]\n" // *ICIALLU = r1 (0);
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "dsb\n"
		     "isb\n"
			:
			: "r"(&(scb->iciallu)), "r"(&(scb->ccr)), "r"(newCcr)
			: ASM_R1);
	return true;
}

/// \brief Enables the instruction cache.
static inline void
Scb_EnableICache(void)
{
	const volatile Scb_Registers *const scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	/*
          Below should be the equivalent of this code. This is rewritten to assembly to avoid any
          instrumentation (e.g. gcov) interference.
          asm volatile ("dsb");
          asm volatile ("isb");
          scb->iciallu = 0; // Invalidate I-cache.
          scb->ccr |= SCB_CCR_IC_MASK; // Enable I-Cache.
          asm volatile ("dsb");
          asm volatile ("isb");
        */
	const uint32_t newCcr = scb->ccr | SCB_CCR_IC_MASK;
	asm volatile("mov r1, #0\n" // r1 = 0;
		     "dsb\n"
		     "isb\n"
		     "str r1, [%0]\n" // *ICIALLU = r1 (0);
		     "str %2, [%1]\n" // *CCR = newCcr;
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "nop\n"
		     "dsb\n"
		     "isb\n"
			:
			: "r"(&(scb->iciallu)), "r"(&(scb->ccr)), "r"(newCcr)
			: ASM_R1);
}

/// \brief Enables the data cache.
static inline void
Scb_EnableDCache(void)
{
	const volatile Scb_Registers *scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	const uint32_t ccsidr = scb->ccsidr;
	const uint32_t sets = (uint32_t)((ccsidr & SCB_CCSIDR_NUMSETS_MASK)
			>> SCB_CCSIDR_NUMSETS_OFFSET);
	const uint32_t ways =
			(uint32_t)((ccsidr & SCB_CCSIDR_ASSOCIATIVITY_MASK)
					>> SCB_CCSIDR_ASSOCIATIVITY_OFFSET);
	/*
	Below should be the equivalent of this code. This is rewritten to assembly to avoid any
	instrumentation (e.g. gcov) interference
	asm volatile("dsb");
	for (uint32_t set = sets; set >= 0; set--)
		for (uint32_t way = ways; way >= 0; way--)
			scb->dcisw = (((set << SCB_DCCISW_SET_OFFSET)
						      & SCB_DCCISW_SET_MASK)
					| ((way << SCB_DCCISW_WAY_OFFSET)
							& SCB_DCCISW_WAY_MASK));
	asm volatile("dsb");
	scb->ccr |= SCB_CCR_DC_MASK; // Enable D-Cache.
	asm volatile("dsb\n"
		     "isb\n");
	*/
	const uint32_t newCcr = scb->ccr | SCB_CCR_DC_MASK;
	asm volatile("dsb\n"
		     "ESETLOOP%=:\n"
		     "mov r0, %4\n" // r0(way) = ways;
		     "EWAYLOOP%=:\n"
		     "mov r1, %3\n" // r1 = sets;
		     "lsl r1, %5\n" // r1 = r1 << SCB_DCCISW_SET_OFFSET;
		     "mov r2, r0\n" // r2 = way;
		     "lsl r2, %6\n" // r2 = r2 << SCB_DCCISW_WAY_OFFSET;
		     "orr r1, r2\n" // r1 |= r2;
		     "str r1, [%0]\n" // DCISW = r1;
		     "sub r0, r0, 1\n" // r1(ways)--;
		     "cmp r0, 0\n"
		     "bge EWAYLOOP%=\n" // while(way >= 0);
		     "sub %3, %3, 1\n" // r0(sets)--;
		     "cmp %3, 0\n"
		     "bge ESETLOOP%=\n" // while(set >= 0);
		     "dsb\n"
		     "str %2, [%1]\n" // CCR = newCcr;
		     "dsb\n"
		     "isb\n"
			:
			: "r"(&(scb->dcisw)), "r"(&(scb->ccr)), "r"(newCcr),
			"r"(sets), "r"(ways), "n"(SCB_DCCISW_SET_OFFSET),
			"n"(SCB_DCCISW_WAY_OFFSET)
			: ASM_R0, ASM_R1, ASM_R2);
}

/// \brief Disables the data cache.
/// \returns Whether the cache invalidation procedure was executed.
static inline bool
Scb_DisableDCache(void)
{
	const volatile Scb_Registers *scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;
	// If the cache is already disabled, don't disable it, as cache clean
	// and invalidation may lead to artefacts.
	if ((scb->ccr & SCB_CCR_DC_MASK) == 0u)
		return false;
	const uint32_t ccsidr = scb->ccsidr;
	const uint32_t sets = (uint32_t)((ccsidr & SCB_CCSIDR_NUMSETS_MASK)
			>> SCB_CCSIDR_NUMSETS_OFFSET);
	const uint32_t ways =
			(uint32_t)((ccsidr & SCB_CCSIDR_ASSOCIATIVITY_MASK)
					>> SCB_CCSIDR_ASSOCIATIVITY_OFFSET);
	/*
	Below should be the equivalent of this code. This is rewritten to assembly to avoid any
	instrumentation (e.g. gcov) interference.
	asm volatile("dsb");
	scb->ccr &= ~SCB_CCR_DC_MASK; // Disable D-Cache.
	for (uint32_t set = sets; set >= 0; set--)
		for (uint32_t way = ways; way >= 0; way--)
			scb->dccisw = (((set << SCB_DCCISW_SET_OFFSET)
						       & SCB_DCCISW_SET_MASK)
					| ((way << SCB_DCCISW_WAY_OFFSET)
							& SCB_DCCISW_WAY_MASK));
	asm volatile("dsb");
	asm volatile("dsb\n"
		     "isb\n");
	*/
	const uint32_t newCcr = scb->ccr & ~(uint32_t)SCB_CCR_DC_MASK;
	asm volatile("dsb\n"
		     "str %2, [%1]\n" // CCR = newCcr;
		     "DSETLOOP%=:\n"
		     "mov r0, %4\n" // r0(way) = ways;
		     "DWAYLOOP%=:\n"
		     "mov r1, %3\n" // r1 = sets;
		     "lsl r1, %5\n" // r1 = r1 << SCB_DCCISW_SET_OFFSET;
		     "mov r2, r0\n" // r2 = way;
		     "lsl r2, %6\n" // r2 = r2 << SCB_DCCISW_WAY_OFFSET;
		     "orr r1, r2\n" // r1 |= r2;
		     "str r1, [%0]\n" // DCCSW = r1;
		     "sub r0, r0, 1\n" // r1(ways)--;
		     "cmp r0, 0\n"
		     "bge DWAYLOOP%=\n" // while(way >= 0);
		     "sub %3, %3, 1\n" // r0(sets)--;
		     "cmp %3, 0\n"
		     "bge DSETLOOP%=\n" // while(set >= 0);
		     "dsb\n"
		     "isb\n"
			:
			: "r"(&(scb->dccisw)), "r"(&(scb->ccr)), "r"(newCcr),
			"r"(sets), "r"(ways), "n"(SCB_DCCISW_SET_OFFSET),
			"n"(SCB_DCCISW_WAY_OFFSET)
			: ASM_R0, ASM_R1, ASM_R2);
	return true;
}

/// \brief Enables or disables the MemoryManagement exception.
/// \param [in] enabled Enable/disable flag.
static inline void
Scb_setMemoryManagementExceptionEnabled(const bool enabled)
{
	volatile Scb_Registers *const scb =
			(volatile Scb_Registers *)SCB_BASE_ADDRESS;

	if (enabled)
		scb->shcsr |= SCB_SHCSR_MEMFAULTENA_MASK;
	else
		scb->shcsr &= ~SCB_SHCSR_MEMFAULTENA_MASK;
}

#endif // BSP_SCB_H

/** @} */

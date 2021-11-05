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

#include "Nvic.h"

#include <assert.h>

#include <Scb/ScbRegisters.h>

#include "NvicRegisters.h"
#include "NvicVectorTable.h"

static volatile Nvic_Registers *nvic =
		(volatile Nvic_Registers *)NVIC_BASE_ADDRESS;
static volatile Scb_Registers *scb = (volatile Scb_Registers *)SCB_BASE_ADDRESS;

static inline uint8_t
getRegisterOffset(Nvic_Irq const irqn)
{
	return (uint8_t)irqn >> 5;
}

void
Nvic_enableInterrupt(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	nvic->iser[getRegisterOffset(irqn)] = 1u << ((uint32_t)irqn % 32u);
}

void
Nvic_disableInterrupt(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	nvic->icer[getRegisterOffset(irqn)] = 1u << ((uint32_t)irqn % 32u);
}

bool
Nvic_isInterruptEnabled(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	return (nvic->iser[getRegisterOffset(irqn)]
			       & (1u << ((uint32_t)irqn % 32u)))
			!= 0u;
}

void
Nvic_setInterruptPending(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	nvic->ispr[getRegisterOffset(irqn)] = 1u << ((uint32_t)irqn % 32u);
}

void
Nvic_clearInterruptPending(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	nvic->icpr[getRegisterOffset(irqn)] = 1u << ((uint32_t)irqn % 32u);
}

bool
Nvic_isInterruptPending(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	return (nvic->ispr[getRegisterOffset(irqn)]
			       & (1u << ((uint32_t)irqn % 32u)))
			!= 0u;
}

bool
Nvic_isInterruptActive(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	return (nvic->iabr[getRegisterOffset(irqn)]
			       & (1u << ((uint32_t)irqn % 32u)))
			!= 0u;
}

void
Nvic_setInterruptPriority(Nvic_Irq const irqn, uint8_t priority)
{
	assert((int32_t)irqn >= 0);
	nvic->ipr[irqn] = (uint8_t)(
			((uint32_t)priority << NVIC_IRQ_PRIORITY_OFFSET)
			& 0xFFu);
}

uint8_t
Nvic_getInterruptPriority(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	return (uint8_t)(nvic->ipr[irqn] >> NVIC_IRQ_PRIORITY_OFFSET);
}

void
Nvic_triggerInterrupt(Nvic_Irq const irqn)
{
	assert((int32_t)irqn >= 0);
	nvic->stir = (uint32_t)irqn;
}

void
Nvic_setPriorityGrouping(uint8_t const priorityGroup)
{
	assert(priorityGroup <= 7u);

	uint32_t aircr = scb->aircr;
	aircr &= (uint32_t)(~SCB_AIRCR_VECTKEY_MASK);
	aircr |= (SCB_AIRCR_VECTKEY_WRITE_KEY << SCB_AIRCR_VECTKEY_OFFSET);
	aircr &= (uint32_t)(~SCB_AIRCR_PRIGROUP_MASK);
	aircr |= (((uint32_t)priorityGroup & 0x07u)
			<< SCB_AIRCR_PRIGROUP_OFFSET);
	scb->aircr = aircr;
}

uint8_t
Nvic_getPriorityGrouping(void)
{
	return (uint8_t)((scb->aircr & SCB_AIRCR_PRIGROUP_MASK)
			>> SCB_AIRCR_PRIGROUP_OFFSET);
}

void
Nvic_relocateVectorTable(void *const address)
{
	const uint32_t addressInt = (uint32_t)address;
	assert((addressInt & SCB_VTOR_TBLOFF_MASK) == addressInt);
	scb->vtor = addressInt;
}

void *
Nvic_getVectorTableAddress(void)
{
	return (void *)scb->vtor;
}

void
Nvic_setInterruptHandlerAddress(
		Nvic_Irq const irqn, const Nvic_InterruptHandler address)
{
	volatile uint32_t *dccmvau = (volatile uint32_t *)SCB_DCCMVAU_ADDRESS;
	volatile Nvic_VectorTable *vtable =
			(volatile Nvic_VectorTable *)scb->vtor;
	vtable->irqHandler[irqn] = address;
	asm volatile("dsb"); // Data Synchronization Barrier
	*dccmvau = (uint32_t)(&vtable->irqHandler[irqn]);
}

Nvic_InterruptHandler
Nvic_getInterruptHandlerAddress(Nvic_Irq const irqn)
{
	const volatile Nvic_VectorTable *vtable =
			(volatile Nvic_VectorTable *)scb->vtor;
	return vtable->irqHandler[irqn];
}

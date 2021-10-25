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

/// \brief Header for the Nested Vectored Interrupt Controller (NVIC) driver for
/// ATSAMV71Q21.

/**
 * @defgroup Nvic Nvic
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_NVIC_H
#define BSP_NVIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// \brief A typedef for a function that can be used to register an interrupt
/// handler.
typedef void (*Nvic_InterruptHandler)(void);

/// \brief An enumeration of hardware interrupts handled by the microcontroller.
typedef enum {
	Nvic_Irq_NonMaskable = -14, ///< Exception - nonmaskable IRQ
	Nvic_Irq_HardFault = -13, ///< Exception - hardware fault
	Nvic_Irq_MemoryManagement =
			-12, ///< Exception - memory management fault
	Nvic_Irq_BusFault = -11, ///< Exception - bus fault
	Nvic_Irq_UsageFault = -10, ///< Exception - usage fault
	Nvic_Irq_SVCall = -5, ///< Exception - supervisor call
	Nvic_Irq_DebugMonitor = -4, ///< Reserved for debug.
	Nvic_Irq_PendSV = -2, ///< IRQ-driven request for system-level service
	Nvic_Irq_SysTick = -1, ///< Exception - zero reached by system timer
	Nvic_Irq_Supply = 0, ///< Supply Controller
	Nvic_Irq_Reset = 1, ///< Reset Controller
	Nvic_Irq_RealTimeClock = 2, ///< Real-time Clock
	Nvic_Irq_RealTimeTimer = 3, ///< Real-time Timer
	Nvic_Irq_WatchdogTimer = 4, ///< Watchdog Timer
	Nvic_Irq_PowerManagement = 5, ///< Power Management Controller
	Nvic_Irq_EmbeddedFlash = 6, ///< Enhanced Embedded Flash Controller
	Nvic_Irq_Uart0 = 7, ///< UART 0
	Nvic_Irq_Uart1 = 8, ///< UART 1
	Nvic_Irq_PioA = 10, ///< PIO Controller A
	Nvic_Irq_PioB = 11, ///< PIO Controller B
	Nvic_Irq_PioC = 12, ///< PIO Controller C
	Nvic_Irq_Usart0 = 13, ///< USART 0
	Nvic_Irq_Usart1 = 14, ///< USART 1
	Nvic_Irq_Usart2 = 15, ///< USART 2
	Nvic_Irq_PioD = 16, ///< PIO Controller D
	Nvic_Irq_PioE = 17, ///< PIO Controller E
	Nvic_Irq_Hsmci = 18, ///< High-speed Multimedia Card Interface
	Nvic_Irq_Twihs0 = 19, ///< Two-wire Interface 0
	Nvic_Irq_Twihs1 = 20, ///< Two-wire Interface 1
	Nvic_Irq_Spi0 = 21, ///< SPI 0
	Nvic_Irq_SynchronousSerial = 22, ///< Synchronous Serial Controller
	Nvic_Irq_Timer0_Channel0 = 23, ///< Timer Counter 0, Channel 0
	Nvic_Irq_Timer0_Channel1 = 24, ///< Timer Counter 0, Channel 1
	Nvic_Irq_Timer0_Channel2 = 25, ///< Timer Counter 0, Channel 2
	Nvic_Irq_Timer1_Channel0 = 26, ///< Timer Counter 1, Channel 0
	Nvic_Irq_Timer1_Channel1 = 27, ///< Timer Counter 1, Channel 1
	Nvic_Irq_Timer1_Channel2 = 28, ///< Timer Counter 1, Channel 2
	Nvic_Irq_Afec0 = 29, ///< Analog Front-End Controller 0
	Nvic_Irq_Dac = 30, ///< Digital-to-Analog Converter
	Nvic_Irq_Pwm0 = 31, ///< Pulse Width Modulation Controller 0
	Nvic_Irq_IntegrityCheckMonitor = 32, ///< Integrity Check Monitor
	Nvic_Irq_AnalogComparator = 33, ///< Analog Comparator Controller
	Nvic_Irq_UsbHost = 34, ///< USB Host / Device Controller
	Nvic_Irq_Mcan0_Irq0 = 35, ///< CAN 0 Line 0
	Nvic_Irq_Mcan0_Irq1 = 36, ///< CAN 0 Line 1
	Nvic_Irq_Mcan1_Irq0 = 37, ///< CAN 1 Line 0
	Nvic_Irq_Mcan1_Irq1 = 38, ///< CAN 1 Line 1
	Nvic_Irq_Gmac = 39, ///< Ethernet MAC
	Nvic_Irq_Afec1 = 40, ///< Analog Front-End Controller 1
	Nvic_Irq_Twihs2 = 41, ///< Two-wire Interface 2
	Nvic_Irq_Spi1 = 42, ///< SPI 1
	Nvic_Irq_Qspi = 43, ///< Quad I/O SPI
	Nvic_Irq_Uart2 = 44, ///< UART 2
	Nvic_Irq_Uart3 = 45, ///< UART 3
	Nvic_Irq_Uart4 = 46, ///< UART 4
	Nvic_Irq_Timer2_Channel0 = 47, ///< Timer Counter 2, Channel 0
	Nvic_Irq_Timer2_Channel1 = 48, ///< Timer Counter 2, Channel 1
	Nvic_Irq_Timer2_Channel2 = 49, ///< Timer Counter 2, Channel 2
	Nvic_Irq_Timer3_Channel0 = 50, ///< Timer Counter 3, Channel 0
	Nvic_Irq_Timer3_Channel1 = 51, ///< Timer Counter 3, Channel 1
	Nvic_Irq_Timer3_Channel2 = 52, ///< Timer Counter 3, Channel 2
	Nvic_Irq_MediaLB_Irq0 = 53, ///< MediaLB IRQ 0
	Nvic_Irq_MediaLB_Irq1 = 54, ///< MediaLB IRQ 1
	Nvic_Irq_Aes = 56, ///< Advanced Encryption Standard IRQ
	Nvic_Irq_TrueRng = 57, ///< True Random Number Generator
	Nvic_Irq_Xdmac = 58, ///< DMA Controller
	Nvic_Irq_Isi = 59, ///< Image Sensor Interface
	Nvic_Irq_Pwm1 = 60, ///< Pulse Width Modulation Controller 1
	Nvic_Irq_Fpu = 61, ///< Floating-Point Unit (OFC, IFC, IOC, DZC, IDC bits)
	Nvic_Irq_Sdramc = 62, ///< SDRAM Controller
	Nvic_Irq_ReinforcedSafetyWatchdog =
			63, ///< Reinforced Safety Watchdog Timer
	Nvic_Irq_CacheWarning = 64, ///< Cache ECC Warning
	Nvic_Irq_CacheFault = 65, ///< Cache ECC Fault
	Nvic_Irq_Gmac_Queue1 = 66, ///< Ethernet MAC Queue 1 DMA IRQ
	Nvic_Irq_Gmac_Queue2 = 67, ///< Ethernet MAC Queue 2 DMA IRQ
	Nvic_Irq_Fpu_Ixc =
			68, ///< Floaoting-Point Unit IRQ (cumulative exception bit)
	Nvic_Irq_I2Sound0 = 69, ///< Inter-IC Sound Controller 0
	Nvic_Irq_I2Sound1 = 70, ///< Inter-IC Sound Controller 1
	Nvic_Irq_Gmac_Queue3 = 71, ///< Ethernet MAC Queue 3 DMA IRQ
	Nvic_Irq_Gmac_Queue4 = 72, ///< Ethernet MAC Queue 4 DMA IRQ
	Nvic_Irq_Gmac_Queue5 = 73, ///< Ethernet MAC Queue 5 DMA IRQ
	Nvic_InterruptCount, ///< Interrupt count
} Nvic_Irq;

/// \brief General numeration of driver-specific constants.
enum { Nvic_SystemExceptionCount = 16, /// \brief Number of system exceptions.
};

/// \brief Function used to enable an interrupt in the NVIC.
/// \param [in] irqn Numeric identifier of the interrupt to enable.
void Nvic_enableInterrupt(Nvic_Irq const irqn);

/// \brief Function used to disable an interrupt in the NVIC.
/// \param [in] irqn Numeric identifier of the interrupt to disable.
void Nvic_disableInterrupt(Nvic_Irq const irqn);

/// \brief Function used to check whether an interrupt is enabled in the NVIC.
/// \param [in] irqn Numeric identifier of the interrupt to check.
/// \retval true Interrupt is enabled.
/// \retval false Interrupt is disabled.
bool Nvic_isInterruptEnabled(Nvic_Irq const irqn);

/// \brief Function used to force an interrupt pending signal on an interrupt
/// line. \param [in] irqn Numeric identifier of the interrupt to trigger.
void Nvic_setInterruptPending(Nvic_Irq const irqn);

/// \brief Function used to clear an interrupt pending signal on an interrupt
/// line. \param [in] irqn Numeric identifier of the interrupt to clear.
void Nvic_clearInterruptPending(Nvic_Irq const irqn);

/// \brief Function used to check if an interrupt pending signal is asserted on
/// an interrupt line. \param [in] irqn Numeric identifier of the interrupt to
/// check.
/// \retval true Interrupt is pending.
/// \retval false Interrupt is not pending.
bool Nvic_isInterruptPending(Nvic_Irq const irqn);

/// \brief Function used to check if an interrupt is currently being processed.
/// \param [in] irqn Numeric identifier of the interrupt to check.
/// \retval true Interrupt is active.
/// \retval false Interrupt is not active.
bool Nvic_isInterruptActive(Nvic_Irq const irqn);

/// \brief Function used to set priority of an interrupt.
/// \param [in] irqn Numeric identifier of the interrupt to modify.
/// \param [in] priority Priority to set for the interrupt.
void Nvic_setInterruptPriority(Nvic_Irq const irqn, uint8_t priority);

/// \brief Function used to check the priority of an interrupt.
/// \param [in] irqn Numeric identifier of the interrupt to check.
/// \returns Priority of the specified interrupt.
uint8_t Nvic_getInterruptPriority(Nvic_Irq const irqn);

/// \brief Function used to trigger a software-generated interrupt.
/// \param [in] irqn Numeric identifier of the interrupt to trigger.
void Nvic_triggerInterrupt(Nvic_Irq const irqn);

/// \brief Function used to configure priority grouping.
/// \param [in] priorityGroup Index of the bit splitting between group priority
///                           and subpriority.
void Nvic_setPriorityGrouping(uint8_t const priorityGroup);

/// \brief Function used to retrieve the priority grouping configuration.
/// \returns Index of the bit splitting between group priority and subpriority.
uint8_t Nvic_getPriorityGrouping(void);

/// \brief Function used to change the address of the vector table in NVIC.
/// \param [in] address Address of the new vector table.
void Nvic_relocateVectorTable(void *const address);

/// \brief Function used to retrieve the address of the vector table in NVIC.
/// \returns Currently used base address of the vector table.
void *Nvic_getVectorTableAddress(void);

/// \brief Function used to set the address of an interrupt handler from the
/// currently used
///        vector table.
/// \param [in] irqn Numeric identifier of the interrupt to set the handler of.
/// \param [in] address Pointer to the function to be used as a handler of the
/// interrupt.
void Nvic_setInterruptHandlerAddress(
		Nvic_Irq const irqn, const Nvic_InterruptHandler address);

/// \brief Function used to retrieve the address of an interrupt handler from
/// the currently used
///        vector table.
/// \param [in] irqn Numeric identifier of the interrupt to retrieve the handler
/// of. \returns Pointer to the handler.
Nvic_InterruptHandler Nvic_getInterruptHandlerAddress(Nvic_Irq const irqn);

#endif // BSP_NVIC_H

/** @} */

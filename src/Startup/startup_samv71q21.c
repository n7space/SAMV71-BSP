/**
 * \file
 *
 * \brief GCC startup file for ATSAMV71Q21
 *
 * Copyright (c) 2017 Atmel Corporation, a wholly owned subsidiary of Microchip
 * Technology Inc.
 *
 * \license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \license_stop
 *
 */

#include "startup_samv71q21.h"

#include <Nvic/Nvic.h>

// Gathering coverage from startup procedure is not possible with GCOV
// (some of its procedures starts later)
// LCOV_EXCL_START

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Reset handler */
void Reset_Handler(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M7 core handlers */
// clang-format off
void NMI_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MemManage_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DebugMon_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
// clang-format on

/* Peripherals handlers */
// clang-format off
void SUPC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RSTC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTT_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PMC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART0_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART1_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOA_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOB_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART2_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOD_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOE_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HSMCI_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIHS0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIHS1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI0_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SSC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0CH0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0CH1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0CH2_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1CH0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1CH1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1CH2_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AFEC0_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DACC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PWM0_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ICM_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ACC_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USBHS_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MCAN0_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MCAN1_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GMAC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AFEC1_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIHS2_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI1_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void QSPI_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART2_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART3_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UART4_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2CH0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2CH1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2CH2_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3CH0_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3CH1_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3CH2_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MLB_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AES_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TRNG_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void XDMAC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ISI_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PWM1_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SDRAMC_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RSWDT_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
// clang-format on

/* Exception Table */
__attribute__((section(".vectors"))) const DeviceVectors exception_table = {
	// clang-format off

  /* Configure Initial Stack Pointer, using linker-generated symbols */
  .pvStack = (void*) (&_estack),

  .pfnReset_Handler              = Reset_Handler,
  .pfnNMI_Handler                = NMI_Handler,
  .pfnHardFault_Handler          = HardFault_Handler,
  .pfnMemManage_Handler          = MemManage_Handler,
  .pfnBusFault_Handler           = BusFault_Handler,
  .pfnUsageFault_Handler         = UsageFault_Handler,
  .pfnReserved1_Handler          = 0uL, /* Reserved */
  .pfnReserved2_Handler          = 0uL, /* Reserved */
  .pfnReserved3_Handler          = 0uL, /* Reserved */
  .pfnReserved4_Handler          = 0uL, /* Reserved */
  .pfnSVC_Handler                = SVC_Handler,
  .pfnDebugMon_Handler           = DebugMon_Handler,
  .pfnReserved5_Handler          = 0uL, /* Reserved */
  .pfnPendSV_Handler             = PendSV_Handler,
  .pfnSysTick_Handler            = SysTick_Handler,

  /* Configurable interrupts */
  .pfnSUPC_Handler               = SUPC_Handler,   /* 0  Supply Controller */
  .pfnRSTC_Handler               = RSTC_Handler,   /* 1  Reset Controller */
  .pfnRTC_Handler                = RTC_Handler,    /* 2  Real-time Clock */
  .pfnRTT_Handler                = RTT_Handler,    /* 3  Real-time Timer */
  .pfnWDT_Handler                = WDT_Handler,    /* 4  Watchdog Timer */
  .pfnPMC_Handler                = PMC_Handler,    /* 5  Power Management Controller */
  .pfnEFC_Handler                = EFC_Handler,    /* 6  Embedded Flash Controller */
  .pfnUART0_Handler              = UART0_Handler,  /* 7  Universal Asynchronous Receiver Transmitter */
  .pfnUART1_Handler              = UART1_Handler,  /* 8  Universal Asynchronous Receiver Transmitter */
  .pvReserved9                   = 0uL,            /* 9  Reserved */
  .pfnPIOA_Handler               = PIOA_Handler,   /* 10 Parallel Input/Output Controller */
  .pfnPIOB_Handler               = PIOB_Handler,   /* 11 Parallel Input/Output Controller */
  .pfnPIOC_Handler               = PIOC_Handler,   /* 12 Parallel Input/Output Controller */
  .pfnUSART0_Handler             = USART0_Handler, /* 13 Universal Synchronous Asynchronous Receiver Transmitter */
  .pfnUSART1_Handler             = USART1_Handler, /* 14 Universal Synchronous Asynchronous Receiver Transmitter */
  .pfnUSART2_Handler             = USART2_Handler, /* 15 Universal Synchronous Asynchronous Receiver Transmitter */
  .pfnPIOD_Handler               = PIOD_Handler,   /* 16 Parallel Input/Output Controller */
  .pfnPIOE_Handler               = PIOE_Handler,   /* 17 Parallel Input/Output Controller */
  .pfnHSMCI_Handler              = HSMCI_Handler,  /* 18 High Speed MultiMedia Card Interface */
  .pfnTWIHS0_Handler             = TWIHS0_Handler, /* 19 Two-wire Interface High Speed */
  .pfnTWIHS1_Handler             = TWIHS1_Handler, /* 20 Two-wire Interface High Speed */
  .pfnSPI0_Handler               = SPI0_Handler,   /* 21 Serial Peripheral Interface */
  .pfnSSC_Handler                = SSC_Handler,    /* 22 Synchronous Serial Controller */
  .pfnTC0CH0_Handler             = TC0CH0_Handler, /* 23 Timer Counter 0 Channel 0 */
  .pfnTC0CH1_Handler             = TC0CH1_Handler, /* 24 Timer Counter 0 Channel 1 */
  .pfnTC0CH2_Handler             = TC0CH2_Handler, /* 25 Timer Counter 0 Channel 2 */
  .pfnTC1CH0_Handler             = TC1CH0_Handler, /* 26 Timer Counter 1 Channel 0 */
  .pfnTC1CH1_Handler             = TC1CH1_Handler, /* 27 Timer Counter 1 Channel 1 */
  .pfnTC1CH2_Handler             = TC1CH2_Handler, /* 28 Timer Counter 1 Channel 2 */
  .pfnAFEC0_Handler              = AFEC0_Handler,  /* 29 Analog Front-End Controller */
  .pfnDACC_Handler               = DACC_Handler,   /* 30 Digital-to-Analog Converter Controller */
  .pfnPWM0_Handler               = PWM0_Handler,   /* 31 Pulse Width Modulation Controller */
  .pfnICM_Handler                = ICM_Handler,    /* 32 Integrity Check Monitor */
  .pfnACC_Handler                = ACC_Handler,    /* 33 Analog Comparator Controller */
  .pfnUSBHS_Handler              = USBHS_Handler,  /* 34 USB High-Speed Interface */
  .pfnMCAN0_Handler              = MCAN0_Handler,  /* 35 Controller Area Network */
  .pvReserved36                  = 0uL,            /* 36 Reserved */
  .pfnMCAN1_Handler              = MCAN1_Handler,  /* 37 Controller Area Network */
  .pvReserved38                  = 0uL,            /* 38 Reserved */
  .pfnGMAC_Handler               = GMAC_Handler,   /* 39 Gigabit Ethernet MAC */
  .pfnAFEC1_Handler              = AFEC1_Handler,  /* 40 Analog Front-End Controller */
  .pfnTWIHS2_Handler             = TWIHS2_Handler, /* 41 Two-wire Interface High Speed */
  .pfnSPI1_Handler               = SPI1_Handler,   /* 42 Serial Peripheral Interface */
  .pfnQSPI_Handler               = QSPI_Handler,   /* 43 Quad Serial Peripheral Interface */
  .pfnUART2_Handler              = UART2_Handler,  /* 44 Universal Asynchronous Receiver Transmitter */
  .pfnUART3_Handler              = UART3_Handler,  /* 45 Universal Asynchronous Receiver Transmitter */
  .pfnUART4_Handler              = UART4_Handler,  /* 46 Universal Asynchronous Receiver Transmitter */
  .pfnTC2CH0_Handler             = TC2CH0_Handler, /* 47 Timer Counter 2 Channel 0 */
  .pfnTC2CH1_Handler             = TC2CH1_Handler, /* 48 Timer Counter 2 Channel 1 */
  .pfnTC2CH2_Handler             = TC2CH2_Handler, /* 49 Timer Counter 2 Channel 2 */
  .pfnTC3CH0_Handler             = TC3CH0_Handler, /* 50 Timer Counter 3 Channel 0 */
  .pfnTC3CH1_Handler             = TC3CH1_Handler, /* 51 Timer Counter 3 Channel 1 */
  .pfnTC3CH2_Handler             = TC3CH2_Handler, /* 52 Timer Counter 3 Channel 2 */
  .pfnMLB_Handler                = MLB_Handler,    /* 53 MediaLB */
  .pvReserved54                  = 0uL,            /* 54 Reserved */
  .pvReserved55                  = 0uL,            /* 55 Reserved */
  .pfnAES_Handler                = AES_Handler,    /* 56 Advanced Encryption Standard */
  .pfnTRNG_Handler               = TRNG_Handler,   /* 57 True Random Number Generator */
  .pfnXDMAC_Handler              = XDMAC_Handler,  /* 58 Extensible DMA Controller */
  .pfnISI_Handler                = ISI_Handler,    /* 59 Image Sensor Interface */
  .pfnPWM1_Handler               = PWM1_Handler,   /* 60 Pulse Width Modulation Controller */
  .pvReserved61                  = 0uL,            /* 61 Reserved */
  .pfnSDRAMC_Handler             = SDRAMC_Handler, /* 62 SDRAM Controller */
  .pfnRSWDT_Handler              = RSWDT_Handler   /* 63 Reinforced Safety Watchdog Timer */
	// clang-format on
};

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));

void _init(void); // provided by libgcc

static void
execute_init_array(void)
{
	const ptrdiff_t preinit_count =
			__preinit_array_end - __preinit_array_start;
	for (ptrdiff_t i = 0; i < preinit_count; i++)
		__preinit_array_start[i]();

	_init();

	const ptrdiff_t init_count = __init_array_end - __init_array_start;
	for (ptrdiff_t i = 0; i < init_count; i++)
		__init_array_start[i]();
}

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void
Reset_Handler(void)
{
	/* Initialize the relocate segment */
	uint32_t *pSrc = &_etext;
	uint32_t *pDest = &_srelocate;

	if (pSrc != pDest) {
		for (; pDest < &_erelocate; pDest++, pSrc++) {
			*pDest = *pSrc;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero; pDest++) {
		*pDest = 0;
	}

	/* Set the vector table base address */
	pSrc = (uint32_t *)&_sfixed;
	Nvic_relocateVectorTable(pSrc);

	/* Initialize ctors */
	execute_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1)
		;
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void
Dummy_Handler(void)
{
	/* Infinite loop */
	while (1)
		;
}

// LCOV_EXCL_STOP

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

#ifndef BSP_PMC_PERIPHERALID_H
#define BSP_PMC_PERIPHERALID_H

/// \brief Enumeration used to select the peripheral during Pmc operations.
typedef enum {
	Pmc_PeripheralId_Uart0 = 7,
	Pmc_PeripheralId_Uart1 = 8,
	Pmc_PeripheralId_Smc = 9,
	Pmc_PeripheralId_PioA = 10,
	Pmc_PeripheralId_PioB = 11,
	Pmc_PeripheralId_PioC = 12,
	Pmc_PeripheralId_Usart0 = 13,
	Pmc_PeripheralId_Usart1 = 14,
	Pmc_PeripheralId_Usart2 = 15,
	Pmc_PeripheralId_PioD = 16,
	Pmc_PeripheralId_PioE = 17,
	Pmc_PeripheralId_Hsmci = 18,
	Pmc_PeripheralId_Twihs0 = 19,
	Pmc_PeripheralId_Twihs1 = 20,
	Pmc_PeripheralId_Spi0 = 21,
	Pmc_PeripheralId_Ssc = 22,
	Pmc_PeripheralId_Tc0Ch0 = 23,
	Pmc_PeripheralId_Tc0Ch1 = 24,
	Pmc_PeripheralId_Tc0Ch2 = 25,
	Pmc_PeripheralId_Tc1Ch0 = 26,
	Pmc_PeripheralId_Tc1Ch1 = 27,
	Pmc_PeripheralId_Tc1Ch2 = 28,
	Pmc_PeripheralId_Afec0 = 29,
	Pmc_PeripheralId_Dacc = 30,
	Pmc_PeripheralId_Pwm0 = 31,
	Pmc_PeripheralId_Icm = 32,
	Pmc_PeripheralId_Acc = 33,
	Pmc_PeripheralId_Usbhs = 34,
	Pmc_PeripheralId_Mcan0 = 35,
	Pmc_PeripheralId_Mcan1 = 37,
	Pmc_PeripheralId_Gmac = 39,
	Pmc_PeripheralId_Afec1 = 40,
	Pmc_PeripheralId_Twihs2 = 41,
	Pmc_PeripheralId_Spi1 = 42,
	Pmc_PeripheralId_Qspi = 43,
	Pmc_PeripheralId_Uart2 = 44,
	Pmc_PeripheralId_Uart3 = 45,
	Pmc_PeripheralId_Uart4 = 46,
	Pmc_PeripheralId_Tc2Ch0 = 47,
	Pmc_PeripheralId_Tc2Ch1 = 48,
	Pmc_PeripheralId_Tc2Ch2 = 49,
	Pmc_PeripheralId_Tc3Ch0 = 50,
	Pmc_PeripheralId_Tc3Ch1 = 51,
	Pmc_PeripheralId_Tc3Ch2 = 52,
	Pmc_PeripheralId_Mlb = 53,
	Pmc_PeripheralId_Aes = 56,
	Pmc_PeripheralId_Trng = 57,
	Pmc_PeripheralId_Xdmac = 58,
	Pmc_PeripheralId_Isi = 59,
	Pmc_PeripheralId_Pwm1 = 60,
	Pmc_PeripheralId_Sdramc = 62,
} Pmc_PeripheralId;

#endif // BSP_PMC_PERIPHERALID_H

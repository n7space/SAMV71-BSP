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

#ifndef BSP_PMC_REGISTERS_H
#define BSP_PMC_REGISTERS_H

#include <stdint.h>

/// \brief Structure mapping PMC registers.
typedef struct {
	volatile uint32_t scer; ///< 0x000 System Clock Enable Register
	volatile uint32_t scdr; ///< 0x004 System Clock Disable Register
	volatile uint32_t scsr; ///< 0x008 System Clock Status Register
	volatile uint32_t reserved1; ///< 0x00C Reserved
	volatile uint32_t pcer0; ///< 0x010 Peripheral Clock Enable Register 0
	volatile uint32_t pcdr0; ///< 0x014 Peripheral Clock Disable Register 0
	volatile uint32_t pcsr0; ///< 0x018 Peripheral Clock Status Register 0
	volatile uint32_t ckgrUckr; ///< 0x01C UTMI Clock Register
	volatile uint32_t ckgrMor; ///< 0x020 Main Oscillator Register
	volatile uint32_t ckgrMcfr; ///< 0x024 Main Clock Frequency Register
	volatile uint32_t ckgrPllar; ///< 0x028 PLLA Register
	volatile uint32_t reserved2; ///< 0x02C Reserved
	volatile uint32_t mckr; ///< 0x030 Master Clock Register
	volatile uint32_t reserved3; ///< 0x034 Reserved
	volatile uint32_t usb; ///< 0x038 USB Clock Register
	volatile uint32_t reserved4; ///< 0x03C Reserved
	volatile uint32_t
			pck[8]; ///< 0x040 - 0x05C Programmable Clock Registers
	volatile uint32_t ier; ///< 0x060 Interrupt Enable Register
	volatile uint32_t idr; ///< 0x064 Interrupt Disable Register
	volatile uint32_t sr; ///< 0x068 Status Register
	volatile uint32_t imr; ///< 0x06C Interrupt Mask Register
	volatile uint32_t fsmr; ///< 0x070 Fast Startup Mode Register
	volatile uint32_t fspr; ///< 0x074 Fast Startup Polarity Register
	volatile uint32_t focr; ///< 0x078 Fault Output Clear Register
	volatile uint32_t reserved5[26]; ///< 0x07C - 0x0E0 Reserved
	volatile uint32_t wpmr; ///< 0x0E4 Write Protection Mode Register
	volatile uint32_t wpsr; ///< 0x0E8 Write Protection Status Register
	volatile uint32_t reserved6[5]; ///< 0x0EC - 0x0FC Reserved
	volatile uint32_t pcer1; ///< 0x100 Peripheral Clock Enable Register 1
	volatile uint32_t pcdr1; ///< 0x104 Peripheral Clock Disable Register 1
	volatile uint32_t pcsr1; ///< 0x108 Peripheral Clock Status Register 1
	volatile uint32_t pcr; ///< 0x10C Peripheral Control Register
	volatile uint32_t ocr; ///< 0x110 Oscillator Calibration Register
	volatile uint32_t slpwkEr0; ///< 0x114 SleepWalking Enable Register 0
	volatile uint32_t slpwkDr0; ///< 0x118 SleepWalking Disable Register 0
	volatile uint32_t slpwkSr0; ///< 0x11C SleepWalking Status Register 0
	volatile uint32_t
			slpwkAsr0; ///< 0x120 SleepWalking Activity Status Register 0
	volatile uint32_t reserved7[3]; ///< 0x124 - 0x12C Reserved
	volatile uint32_t pmmr; ///< 0x130 PLL Maximum Multiplier Value Register
	volatile uint32_t slpwkEr1; ///< 0x134 SleepWalking Enable Register 1
	volatile uint32_t slpwkDr1; ///< 0x138 SleepWalking Disable Register 1
	volatile uint32_t slpwkSr1; ///< 0x13C SleepWalking Status Register 1
	volatile uint32_t
			slpwkAsr1; ///< 0x140 SleepWalking Activity Status Register 1
	volatile uint32_t
			slpwkAipr; ///< 0x144 SleepWalking Activity In Progress Register 1
} Pmc_Registers;

#define PMC_BASE_ADDRESS 0x400E0600

#define PMC_PCER0_MASK 0xFFFFFF80u
#define PMC_PCDR0_MASK 0xFFFFFF80u
#define PMC_PCSR0_MASK 0xFFFFFF80u

#define PMC_PCER1_MASK 0x1F3FFFAFu
#define PMC_PCDR1_MASK 0x1F3FFFAFu
#define PMC_PCSR1_MASK 0x1F3FFFAFu

#define PMC_SCER_PCK0_MASK 0x00000100u
#define PMC_SCER_PCK0_OFFSET 8u
#define PMC_SCER_PCK1_MASK 0x00000200u
#define PMC_SCER_PCK1_OFFSET 9u
#define PMC_SCER_PCK2_MASK 0x00000400u
#define PMC_SCER_PCK2_OFFSET 10u
#define PMC_SCER_PCK3_MASK 0x00000800u
#define PMC_SCER_PCK3_OFFSET 11u
#define PMC_SCER_PCK4_MASK 0x00001000u
#define PMC_SCER_PCK4_OFFSET 12u
#define PMC_SCER_PCK5_MASK 0x00002000u
#define PMC_SCER_PCK5_OFFSET 13u
#define PMC_SCER_PCK6_MASK 0x00004000u
#define PMC_SCER_PCK6_OFFSET 14u
#define PMC_SCER_PCK7_MASK 0x00008000u
#define PMC_SCER_PCK7_OFFSET 15u

#define PMC_SCDR_PCK0_MASK 0x00000100u
#define PMC_SCDR_PCK0_OFFSET 8u
#define PMC_SCDR_PCK1_MASK 0x00000200u
#define PMC_SCDR_PCK1_OFFSET 9u
#define PMC_SCDR_PCK2_MASK 0x00000400u
#define PMC_SCDR_PCK2_OFFSET 10u
#define PMC_SCDR_PCK3_MASK 0x00000800u
#define PMC_SCDR_PCK3_OFFSET 11u
#define PMC_SCDR_PCK4_MASK 0x00001000u
#define PMC_SCDR_PCK4_OFFSET 12u
#define PMC_SCDR_PCK5_MASK 0x00002000u
#define PMC_SCDR_PCK5_OFFSET 13u
#define PMC_SCDR_PCK6_MASK 0x00004000u
#define PMC_SCDR_PCK6_OFFSET 14u
#define PMC_SCDR_PCK7_MASK 0x00008000u
#define PMC_SCDR_PCK7_OFFSET 15u

#define PMC_SCSR_HCLKS_MASK 0x00000001u
#define PMC_SCSR_HCLKS_OFFSET 0u
#define PMC_SCSR_PCK0_MASK 0x00000100u
#define PMC_SCSR_PCK0_OFFSET 8u
#define PMC_SCSR_PCK1_MASK 0x00000200u
#define PMC_SCSR_PCK1_OFFSET 9u
#define PMC_SCSR_PCK2_MASK 0x00000400u
#define PMC_SCSR_PCK2_OFFSET 10u
#define PMC_SCSR_PCK3_MASK 0x00000800u
#define PMC_SCSR_PCK3_OFFSET 11u
#define PMC_SCSR_PCK4_MASK 0x00001000u
#define PMC_SCSR_PCK4_OFFSET 12u
#define PMC_SCSR_PCK5_MASK 0x00002000u
#define PMC_SCSR_PCK5_OFFSET 13u
#define PMC_SCSR_PCK6_MASK 0x00004000u
#define PMC_SCSR_PCK6_OFFSET 14u
#define PMC_SCSR_PCK7_MASK 0x00008000u
#define PMC_SCSR_PCK7_OFFSET 15u

#define CKGR_MOR_MOSCXTEN_MASK 0x00000001u
#define CKGR_MOR_MOSCXTEN_OFFSET 0u
#define CKGR_MOR_MOSCXTBY_MASK 0x00000002u
#define CKGR_MOR_MOSCXTBY_OFFSET 1u
#define CKGR_MOR_WAITMODE_MASK 0x00000004u
#define CKGR_MOR_WAITMODE_OFFSET 2u
#define CKGR_MOR_MOSCRCEN_MASK 0x00000008u
#define CKGR_MOR_MOSCRCEN_OFFSET 3u
#define CKGR_MOR_MOSCRCF_MASK 0x00000070u
#define CKGR_MOR_MOSCRCF_OFFSET 4u
#define CKGR_MOR_MOSCXTST_MASK 0x0000FF00u
#define CKGR_MOR_MOSCXTST_OFFSET 8u
#define CKGR_MOR_MOSCXTST_DIVIDER 8u
#define CKGR_MOR_KEY_MASK 0x00FF0000u
#define CKGR_MOR_KEY_OFFSET 16u
#define CKGR_MOR_MOSCSEL_MASK 0x01000000u
#define CKGR_MOR_MOSCSEL_OFFSET 24u
#define CKGR_MOR_KEY_VALUE 0x37u

#define CKGR_MCFR_MAINF_MASK 0x0000FFFFu
#define CKGR_MCFR_MAINF_OFFSET 0u
#define CKGR_MCFR_MAINF_DIVIDER 16u
#define CKGR_MCFR_MAINFRDY_MASK 0x00010000u
#define CKGR_MCFR_MAINFRDY_OFFSET 16u
#define CKGR_MCFR_RCMEAS_MASK 0x00100000u
#define CKGR_MCFR_RCMEAS_OFFSET 20u
#define CKGR_MCFR_CCSS_MASK 0x01000000u
#define CKGR_MCFR_CCSS_OFFSET 24u

#define CKGR_PLLAR_DIVA_MASK 0x000000FFu
#define CKGR_PLLAR_DIVA_OFFSET 0u
#define CKGR_PLLAR_PLLACOUNT_MASK 0x00003F00u
#define CKGR_PLLAR_PLLACOUNT_OFFSET 8u
#define CKGR_PLLAR_MULA_MASK 0x07FF0000u
#define CKGR_PLLAR_MULA_OFFSET 16u
#define CKGR_PLLAR_ONE_MASK 0x20000000u
#define CKGR_PLLAR_ONE_OFFSET 29u

#define PMC_MCKR_CSS_MASK 0x00000003u
#define PMC_MCKR_CSS_OFFSET 0u
#define PMC_MCKR_PRES_MASK 0x00000070u
#define PMC_MCKR_PRES_OFFSET 4u
#define PMC_MCKR_MDIV_MASK 0x00000300u
#define PMC_MCKR_MDIV_OFFSET 8u

#define PMC_PCK_CSS_MASK 0x00000007u
#define PMC_PCK_CSS_OFFSET 0u
#define PMC_PCK_PRES_MASK 0x00000FF0u
#define PMC_PCK_PRES_OFFSET 4u

#define PMC_SR_MOSCXTS_MASK 0x00000001u
#define PMC_SR_MOSCXTS_OFFSET 0u
#define PMC_SR_LOCKA_MASK 0x00000002u
#define PMC_SR_LOCKA_OFFSET 1u
#define PMC_SR_MCKRDY_MASK 0x00000008u
#define PMC_SR_MCKRDY_OFFSET 3u
#define PMC_SR_PCKRDY0_MASK 0x00000100u
#define PMC_SR_PCKRDY0_OFFSET 8u
#define PMC_SR_PCKRDY1_MASK 0x00000200u
#define PMC_SR_PCKRDY1_OFFSET 9u
#define PMC_SR_PCKRDY2_MASK 0x00000400u
#define PMC_SR_PCKRDY2_OFFSET 10u
#define PMC_SR_PCKRDY3_MASK 0x00000800u
#define PMC_SR_PCKRDY3_OFFSET 11u
#define PMC_SR_PCKRDY4_MASK 0x00001000u
#define PMC_SR_PCKRDY4_OFFSET 12u
#define PMC_SR_PCKRDY5_MASK 0x00002000u
#define PMC_SR_PCKRDY5_OFFSET 13u
#define PMC_SR_PCKRDY6_MASK 0x00004000u
#define PMC_SR_PCKRDY6_OFFSET 14u
#define PMC_SR_MOSCSELS_MASK 0x00010000u
#define PMC_SR_MOSCSELS_OFFSET 16u
#define PMC_SR_MOSCRCS_MASK 0x00020000u
#define PMC_SR_MOSCRCS_OFFSET 17u

// clang-format off

#define PMC_FSMR_FSTT0_MASK     0x00000001
#define PMC_FSMR_FSTT0_OFFSET   0u
#define PMC_FSMR_FSTT1_MASK     0x00000002
#define PMC_FSMR_FSTT1_OFFSET   1u
#define PMC_FSMR_FSTT2_MASK     0x00000004
#define PMC_FSMR_FSTT2_OFFSET   2u
#define PMC_FSMR_FSTT3_MASK     0x00000008
#define PMC_FSMR_FSTT3_OFFSET   3u
#define PMC_FSMR_FSTT4_MASK     0x00000010
#define PMC_FSMR_FSTT4_OFFSET   4u
#define PMC_FSMR_FSTT5_MASK     0x00000020
#define PMC_FSMR_FSTT5_OFFSET   5u
#define PMC_FSMR_FSTT6_MASK     0x00000040
#define PMC_FSMR_FSTT6_OFFSET   6u
#define PMC_FSMR_FSTT7_MASK     0x00000080
#define PMC_FSMR_FSTT7_OFFSET   7u
#define PMC_FSMR_FSTT8_MASK     0x00000100
#define PMC_FSMR_FSTT8_OFFSET   8u
#define PMC_FSMR_FSTT9_MASK     0x00000200
#define PMC_FSMR_FSTT9_OFFSET   9u
#define PMC_FSMR_FSTT10_MASK    0x00000400
#define PMC_FSMR_FSTT10_OFFSET  10u
#define PMC_FSMR_FSTT11_MASK    0x00000800
#define PMC_FSMR_FSTT11_OFFSET  11u
#define PMC_FSMR_FSTT12_MASK    0x00001000
#define PMC_FSMR_FSTT12_OFFSET  12u
#define PMC_FSMR_FSTT13_MASK    0x00002000
#define PMC_FSMR_FSTT13_OFFSET  13u
#define PMC_FSMR_FSTT14_MASK    0x00004000
#define PMC_FSMR_FSTT14_OFFSET  14u
#define PMC_FSMR_FSTT15_MASK    0x00008000
#define PMC_FSMR_FSTT15_OFFSET  15u
#define PMC_FSMR_RTTAL_MASK     0x00010000
#define PMC_FSMR_RTTAL_OFFSET   16u
#define PMC_FSMR_RTCAL_MASK     0x00020000
#define PMC_FSMR_RTCAL_OFFSET   17u
#define PMC_FSMR_USBAL_MASK     0x00040000
#define PMC_FSMR_USBAL_OFFSET   18u
#define PMC_FSMR_LPM_MASK       0x00100000
#define PMC_FSMR_LPM_OFFSET     20u
#define PMC_FSMR_FLPM_MASK      0x00600000
#define PMC_FSMR_FLPM_OFFSET    21u
#define PMC_FSMR_FFLPM_MASK     0x00800000
#define PMC_FSMR_FFLPM_OFFSET   23u

#define PMC_OCR_CAL12_MASK      0x007F0000
#define PMC_OCR_CAL12_OFFSET    16
#define PMC_OCR_SEL12_MASK      0x00800000
#define PMC_OCR_SEL12_OFFSET    23
#define PMC_OCR_CAL8_MASK       0x00007F00
#define PMC_OCR_CAL8_OFFSET     8
#define PMC_OCR_SEL8_MASK       0x00008000
#define PMC_OCR_SEL8_OFFSET     15

// clang-format on

#endif // BSP_PMC_REGISTERS_H

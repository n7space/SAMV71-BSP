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

/// \brief Header containing definitions of registers of Reset Controller
/// (RSTC).

#ifndef BSP_RSTC_REGISTERS_H
#define BSP_RSTC_REGISTERS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint32_t cr;
	uint32_t sr;
	uint32_t mr;
} Rstc_Registers;

// clang-format off

#define RSTC_BASE_ADDRESS       0x400E1800

#define RSTC_CR_PROCRST_MASK    0x00000001u
#define RSTC_CR_PROCRST_OFFSET  0u
#define RSTC_CR_EXTRST_MASK     0x00000008u
#define RSTC_CR_EXTRST_OFFSET   3u
#define RSTC_CR_KEY_MASK        0xFF000000u
#define RSTC_CR_KEY_OFFSET      24u
#define RSTC_CR_KEY_PASSWD      0xA5u

#define RSTC_SR_URSTS_MASK      0x00000001u
#define RSTC_SR_URSTS_OFFSET    0u
#define RSTC_SR_RSTTYP_MASK     0x00000700u
#define RSTC_SR_RSTTYP_OFFSET   8u
#define RSTC_SR_NRSTL_MASK      0x00010000u
#define RSTC_SR_NRSTL_OFFSET    16u
#define RSTC_SR_SRCMP_MASK      0x00020000u
#define RSTC_SR_SRCMP_OFFSET    17u

#define RSTC_MR_URSTEN_MASK     0x00000001u
#define RSTC_MR_URSTEN_OFFSET   0u
#define RSTC_MR_URSTIEN_MASK    0x00000010u
#define RSTC_MR_URSTIEN_OFFSET  4u
#define RSTC_MR_ERSTL_MASK      0x00000F00u
#define RSTC_MR_ERSTL_OFFSET    8u
#define RSTC_MR_KEY_MASK        0xFF000000u
#define RSTC_MR_KEY_OFFSET      24u
#define RSTC_MR_KEY_PASSWD      0xA5u

// clang-format on

#endif // BSP_RSTC_REGISTERS_H

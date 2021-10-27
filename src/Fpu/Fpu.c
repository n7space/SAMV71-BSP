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

#include "Fpu.h"

#include <assert.h>
#include <string.h>

// clang-format off
#ifdef CLANG_TIDY
   // Clang-tidy assumes x86 architecture.
#  define ASM_R0 "eax"
#else
#  define ASM_R0 "r0"
#endif
// clang-format on

void
Fpu_startup(Fpu *const fpu)
{
	const uint32_t oldCpacr = fpu->coprocessorRegisters->cpacr;
	const uint32_t fpuCpMask = FPU_CPACR_CP10_MASK | FPU_CPACR_CP11_MASK;
	const uint32_t invertedFpuCpMask = ~fpuCpMask;
	const uint32_t newCpacr = (oldCpacr & invertedFpuCpMask)
			| ((((uint32_t)Fpu_CoprocessorAccessMode_Full
					    << FPU_CPACR_CP10_OFFSET)
					   | ((uint32_t)Fpu_CoprocessorAccessMode_Full
							   << FPU_CPACR_CP11_OFFSET))
					& fpuCpMask);
	fpu->coprocessorRegisters->cpacr = newCpacr;

	/// Reset pipeline.
	asm volatile("dsb \n\t" /// Data Synchronization Barrier.
		     "isb \n\t" /// Instruction Synchronization Barrier.
		     "mov r0, 0 \n\t"
		     "vmov s0, r0 \n\t"
		     "vmov s1, r0 \n\t"
		     "vmov s2, r0 \n\t"
		     "vmov s3, r0 \n\t"
		     "vmov s4, r0 \n\t"
		     "vmov s5, r0 \n\t"
		     "vmov s6, r0 \n\t"
		     "vmov s7, r0 \n\t"
		     "vmov s8, r0 \n\t"
		     "vmov s9, r0 \n\t"
		     "vmov s10, r0 \n\t"
		     "vmov s11, r0 \n\t"
		     "vmov s12, r0 \n\t"
		     "vmov s13, r0 \n\t"
		     "vmov s14, r0 \n\t"
		     "vmov s15, r0 \n\t"
		     "vmov s16, r0 \n\t"
		     "vmov s17, r0 \n\t"
		     "vmov s18, r0 \n\t"
		     "vmov s19, r0 \n\t"
		     "vmov s20, r0 \n\t"
		     "vmov s21, r0 \n\t"
		     "vmov s22, r0 \n\t"
		     "vmov s23, r0 \n\t"
		     "vmov s24, r0 \n\t"
		     "vmov s25, r0 \n\t"
		     "vmov s26, r0 \n\t"
		     "vmov s27, r0 \n\t"
		     "vmov s28, r0 \n\t"
		     "vmov s29, r0 \n\t"
		     "vmov s30, r0 \n\t"
		     "vmov s31, r0 \n\t" ::
					: ASM_R0);
}

void
Fpu_shutdown(Fpu *const fpu)
{
	const uint32_t oldCpacr = fpu->coprocessorRegisters->cpacr;
	const uint32_t fpuCpMask = FPU_CPACR_CP10_MASK | FPU_CPACR_CP11_MASK;
	const uint32_t invertedFpuCpMask = ~fpuCpMask;
	const uint32_t newCpacr = (oldCpacr & invertedFpuCpMask)
			| ((((uint32_t)Fpu_CoprocessorAccessMode_Denied
					    << FPU_CPACR_CP10_OFFSET)
					   | ((uint32_t)Fpu_CoprocessorAccessMode_Denied
							   << FPU_CPACR_CP11_OFFSET))
					& fpuCpMask);
	fpu->coprocessorRegisters->cpacr = newCpacr;

	/// Reset pipeline.
	asm volatile("dsb \n\t" /// Data Synchronization Barrier.
		     "isb \n\t" /// Instruction Synchronization Barrier.
	);
}

void
Fpu_init(Fpu *const fpu)
{
	fpu->registers = (Fpu_Registers *)FPU_REGISTERS_ADDRESS_BASE;
	fpu->coprocessorRegisters = (Fpu_CoprocessorRegisters *)
			FPU_COPROCESSOR_REGISTER_ADDRESS_BASE;
}

void
Fpu_setConfig(Fpu *const fpu, const Fpu_Config *const config)
{
	const uint32_t configMask = FPU_FPCCR_ASPEN_MASK | FPU_FPCCR_LSPEN_MASK;
	const uint32_t invertedConfigMask = ~configMask;
	fpu->registers->fpccr = (fpu->registers->fpccr & invertedConfigMask)
			| (((config->isFpContextPreservedOnException ? 1u : 0u)
					   << FPU_FPCCR_ASPEN_OFFSET)
					& FPU_FPCCR_ASPEN_MASK)
			| FPU_FPCCR_LSPEN_MASK;
	// Lazy stacking shall be enabled -^

	fpu->registers->fpcar = ((config->exceptionFpRegisterSpaceAddress
						 << FPU_FPCAR_ADDRESS_OFFSET)
						& FPU_FPCAR_ADDRESS_MASK)
			| (fpu->registers->fpcar & ~FPU_FPCAR_ADDRESS_MASK);

	const uint32_t defaultsMask = FPU_FPDSCR_AHP_MASK | FPU_FPDSCR_DN_MASK
			| FPU_FPDSCR_FZ_MASK | FPU_FPDSCR_RMODE_MASK;
	const uint32_t invertedDefaultsMask = ~defaultsMask;
	fpu->registers->fpdscr = (fpu->registers->fpdscr & invertedDefaultsMask)
			| (((uint32_t)config->defaultHalfPrecisionMode
					   << FPU_FPDSCR_AHP_OFFSET)
					& FPU_FPDSCR_AHP_MASK)
			| (((uint32_t)config->defaultNanMode
					   << FPU_FPDSCR_DN_OFFSET)
					& FPU_FPDSCR_DN_MASK)
			| (((uint32_t)config->defaultFlushToZeroMode
					   << FPU_FPDSCR_FZ_OFFSET)
					& FPU_FPDSCR_FZ_MASK)
			| (((uint32_t)config->defaultRoundingMode
					   << FPU_FPDSCR_RMODE_OFFSET)
					& FPU_FPDSCR_RMODE_MASK);

	/// Reset pipeline.
	asm volatile("dsb \n\t" /// Data Synchronization Barrier.
		     "isb \n\t" /// Instruction Synchronization Barrier.
	);
}

void
Fpu_getConfig(const Fpu *const fpu, Fpu_Config *const config)
{
	config->isFpContextPreservedOnException =
			(fpu->registers->fpccr & FPU_FPCCR_ASPEN_MASK) != 0u;
	config->exceptionFpRegisterSpaceAddress =
			(fpu->registers->fpcar & FPU_FPCAR_ADDRESS_MASK)
			>> FPU_FPCAR_ADDRESS_OFFSET;
	config->defaultFlushToZeroMode = (Fpu_FlushToZeroMode)(
			(fpu->registers->fpdscr & FPU_FPDSCR_FZ_MASK)
			>> FPU_FPDSCR_FZ_OFFSET);
	config->defaultHalfPrecisionMode = (Fpu_AlternativeHalfPrecisionMode)(
			(fpu->registers->fpdscr & FPU_FPDSCR_AHP_MASK)
			>> FPU_FPDSCR_AHP_OFFSET);
	config->defaultNanMode = (Fpu_NanMode)(
			(fpu->registers->fpdscr & FPU_FPDSCR_DN_MASK)
			>> FPU_FPDSCR_DN_OFFSET);
	config->defaultRoundingMode = (Fpu_RoundingMode)(
			(fpu->registers->fpdscr & FPU_FPDSCR_RMODE_MASK)
			>> FPU_FPDSCR_RMODE_OFFSET);
}

static inline bool
isFeatureSupported(
		const uint32_t reg, const uint32_t mask, const uint8_t offset)
{
	return ((uint32_t)((reg & mask) >> offset) & FPU_FEATURE_SUPPORTED)
			!= 0u;
}

void
Fpu_getFeatures(const Fpu *const fpu, Fpu_Features *const features)
{
	assert(features);

	features->areRoundingModesSupported = isFeatureSupported(
			fpu->registers->mvfr0, FPU_MVFR0_FP_ROUNDING_MODES_MASK,
			FPU_MVFR0_FP_ROUNDING_MODES_OFFSET);
	features->areDivideOperationsSupported = isFeatureSupported(
			fpu->registers->mvfr0, FPU_MVFR0_DIVIDE_MASK,
			FPU_MVFR0_DIVIDE_OFFSET);
	features->areDoublePrecisionOperationsSupported = isFeatureSupported(
			fpu->registers->mvfr0, FPU_MVFR0_DOUBLE_PRECISION_MASK,
			FPU_MVFR0_DOUBLE_PRECISION_OFFSET);
	features->areShortVectorsSupported = isFeatureSupported(
			fpu->registers->mvfr0, FPU_MVFR0_SHORT_VECTORS_MASK,
			FPU_MVFR0_SHORT_VECTORS_OFFSET);
	features->areSinglePrecisionOperationsSupported = isFeatureSupported(
			fpu->registers->mvfr0, FPU_MVFR0_SINGLE_PRECISION_MASK,
			FPU_MVFR0_SINGLE_PRECISION_OFFSET);
	features->areSquareRootOperationsSupported = isFeatureSupported(
			fpu->registers->mvfr0, FPU_MVFR0_SQUARE_ROOT_MASK,
			FPU_MVFR0_SQUARE_ROOT_OFFSET);
	features->isExceptionTrappingSupported =
			isFeatureSupported(fpu->registers->mvfr0,
					FPU_MVFR0_FP_EXCEPTION_TRAPPING_MASK,
					FPU_MVFR0_FP_EXCEPTION_TRAPPING_OFFSET);
	features->areFpFusedMacOperationsSupported = isFeatureSupported(
			fpu->registers->mvfr1, FPU_MVFR1_FP_FUSED_MAC_MASK,
			FPU_MVFR1_FP_FUSED_MAC_OFFSET);
	features->isNanValuePropagationSupported = isFeatureSupported(
			fpu->registers->mvfr1, FPU_MVFR1_D_NAN_MODE_MASK,
			FPU_MVFR1_D_NAN_MODE_OFFSET);
	features->isFullDenormalizedNumberArithmeticSupported =
			isFeatureSupported(fpu->registers->mvfr1,
					FPU_MVFR1_FTZ_MODE_MASK,
					FPU_MVFR1_FTZ_MODE_OFFSET);
	features->areMisceleneousFeaturesSupported = isFeatureSupported(
			fpu->registers->mvfr2, FPU_MVFR2_VFP_MISC_MASK,
			FPU_MVFR2_VFP_MISC_OFFSET);

	features->fpuRegisterBankSize = (Fpu_RegisterBankSize)(
			(fpu->registers->mvfr0 & FPU_MVFR0_A_SIMD_MASK)
			>> FPU_MVFR0_A_SIMD_OFFSET);
}

void
Fpu_getStatus(const Fpu *const fpu, Fpu_Status *const status)
{
	status->couldBusFaultExceptionPendingbeBeSet =
			(fpu->registers->fpccr & FPU_FPCCR_BFRDY_MASK) != 0u;
	status->couldDebugMonitorExceptionPendingbeBeSet =
			(fpu->registers->fpccr & FPU_FPCCR_MONRDY_MASK) != 0u;
	status->couldHardFaultExceptionPendingbeBeSet =
			(fpu->registers->fpccr & FPU_FPCCR_HFRDY_MASK) != 0u;
	status->couldMemManageExceptionPendingbeBeSet =
			(fpu->registers->fpccr & FPU_FPCCR_MMRDY_MASK) != 0u;
	status->isLazyFpStatePreservationActive =
			(fpu->registers->fpccr & FPU_FPCCR_LSPACT_MASK) != 0u;
	status->wasProcessorInThreadMode =
			(fpu->registers->fpccr & FPU_FPCCR_THREAD_MASK) != 0u;
	status->wasProcessorInUserMode =
			(fpu->registers->fpccr & FPU_FPCCR_USER_MASK) != 0u;
}

static inline uint32_t
getFpscr(void)
{
	uint32_t fpscr;
	// Read FPSCR register value to fpscr variable.
	asm volatile("vmrs %0, fpscr" : "=r"(fpscr));
	return fpscr;
}

static inline void
setFpscr(uint32_t fpscr)
{
	// Save FPSCR register value from fpscr variable.
	asm volatile("vmsr fpscr, %0" ::"r"(fpscr));
}

void
Fpu_setContextConfig(const Fpu_ContextConfig *const config)
{
	const uint32_t configValue = (((uint32_t)config->halfPrecisionMode
						      << FPU_FPSCR_AHP_OFFSET)
						     & FPU_FPSCR_AHP_MASK)
			| (((uint32_t)config->nanMode << FPU_FPSCR_DN_OFFSET)
					& FPU_FPSCR_DN_MASK)
			| (((uint32_t)config->flushToZeroMode
					   << FPU_FPSCR_FZ_OFFSET)
					& FPU_FPSCR_FZ_MASK)
			| (((uint32_t)config->roundingMode
					   << FPU_FPSCR_RMODE_OFFSET)
					& FPU_FPSCR_RMODE_MASK);
	const uint32_t configMask = FPU_FPSCR_AHP_MASK | FPU_FPSCR_DN_MASK
			| FPU_FPSCR_FZ_MASK | FPU_FPSCR_RMODE_MASK;
	const uint32_t invertedConfigMask = ~configMask;

	setFpscr((getFpscr() & invertedConfigMask)
			| (configValue & configMask));
}

void
Fpu_getContextConfig(Fpu_ContextConfig *const config)
{
	const uint32_t fpscr = getFpscr();
	config->flushToZeroMode = (Fpu_FlushToZeroMode)(
			(fpscr & FPU_FPSCR_FZ_MASK) >> FPU_FPSCR_FZ_OFFSET);
	config->halfPrecisionMode = (Fpu_AlternativeHalfPrecisionMode)(
			(fpscr & FPU_FPSCR_AHP_MASK) >> FPU_FPSCR_AHP_OFFSET);
	config->nanMode = (Fpu_NanMode)(
			(fpscr & FPU_FPSCR_DN_MASK) >> FPU_FPSCR_DN_OFFSET);
	config->roundingMode = (Fpu_RoundingMode)((fpscr & FPU_FPSCR_RMODE_MASK)
			>> FPU_FPSCR_RMODE_OFFSET);
}

void
Fpu_getContextState(Fpu_ContextState *const state)
{
	const uint32_t fpscr = getFpscr();
	state->isNegativeConditionCodeFlagSet =
			(fpscr & FPU_FPSCR_N_MASK) != 0u;
	state->isZeroConditionCodeFlagSet = (fpscr & FPU_FPSCR_Z_MASK) != 0u;
	state->isCarryConditionCodeFlagSet = (fpscr & FPU_FPSCR_C_MASK) != 0u;
	state->isOverflowConditionCodeFlagSet =
			(fpscr & FPU_FPSCR_V_MASK) != 0u;

	state->isInputDenormalCumulativeExceptionBitSet =
			(fpscr & FPU_FPSCR_IDC_MASK) != 0u;
	state->isInexactCumulativeExceptionBitSet =
			(fpscr & FPU_FPSCR_IXC_MASK) != 0u;
	state->isUnderflowCumulativeExceptionBitSet =
			(fpscr & FPU_FPSCR_UFC_MASK) != 0u;
	state->isOverflowCumulativeExceptionBitSet =
			(fpscr & FPU_FPSCR_OFC_MASK) != 0u;
	state->isDivisionByZeroCumulativeExceptionBitSet =
			(fpscr & FPU_FPSCR_DZC_MASK) != 0u;
	state->isInvalidOperationCumulativeExceptionBitSet =
			(fpscr & FPU_FPSCR_IOC_MASK) != 0u;
}

void
Fpu_clearExceptions(void)
{
	const uint32_t cleraingMask = FPU_FPSCR_IDC_MASK | FPU_FPSCR_IXC_MASK
			| FPU_FPSCR_UFC_MASK | FPU_FPSCR_OFC_MASK
			| FPU_FPSCR_DZC_MASK | FPU_FPSCR_IOC_MASK;
	setFpscr(getFpscr() & ~cleraingMask);
}

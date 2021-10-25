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

#include "Tic.h"

#include <assert.h>
#include <string.h>

#include <Utils/Utils.h>

#include "TicRegisters.h"

void
Tic_init(Tic *const tic, Tic_Id const id)
{
	memset(tic, 0, sizeof(Tic));

	tic->ticId = id;
	switch (id) {
	case Tic_Id_0:
		tic->regs = (volatile Tic_Registers *)
				TIC_SAMV71_TIC0_BASE_ADDRESS;
		break;
	case Tic_Id_1:
		tic->regs = (volatile Tic_Registers *)
				TIC_SAMV71_TIC1_BASE_ADDRESS;
		break;
	case Tic_Id_2:
		tic->regs = (volatile Tic_Registers *)
				TIC_SAMV71_TIC2_BASE_ADDRESS;
		break;
	case Tic_Id_3:
		tic->regs = (volatile Tic_Registers *)
				TIC_SAMV71_TIC3_BASE_ADDRESS;
		break;
	default: assert(false);
	}
}

void
Tic_enableChannel(Tic *const tic, Tic_Channel const channel)
{
	tic->regs->channelRegs[channel].ccr = TIC_CCR_CLKEN_MASK;
}

void
Tic_disableChannel(Tic *const tic, Tic_Channel const channel)
{
	tic->regs->channelRegs[channel].ccr = TIC_CCR_CLKDIS_MASK;
}

bool
Tic_isChannelEnabled(const Tic *const tic, Tic_Channel const channel)
{
	return ((tic->regs->channelRegs[channel].sr & TIC_SR_CLKSTA_MASK)
			!= 0u);
}

void
Tic_triggerChannel(Tic *const tic, Tic_Channel const channel)
{
	tic->regs->channelRegs[channel].ccr = TIC_CCR_SWTRG_MASK;
}

void
Tic_syncAllChannels(Tic *const tic)
{
	tic->regs->bcr = TIC_BCR_SYNC_MASK;
}

static uint32_t
prepareCaptureModeConfig(Tic_CaptureModeConfig const config)
{
	const uint32_t cmr = ((uint32_t)((config.isStoppedOnRbLoading ? 1u : 0u)
					      << TIC_CMR_CAP_LDBSTOP_OFFSET)
					     & TIC_CMR_CAP_LDBSTOP_MASK)
			| ((uint32_t)((config.isDisabledOnRbLoading ? 1u : 0u)
					   << TIC_CMR_CAP_LDBDIS_OFFSET)
					& TIC_CMR_CAP_LDBDIS_MASK)
			| (((uint32_t)config.externalTriggerEdge
					   << TIC_CMR_CAP_ETRGEDG_OFFSET)
					& TIC_CMR_CAP_ETRGEDG_MASK)
			| (((uint32_t)config.triggerSource
					   << TIC_CMR_CAP_ABETRG_OFFSET)
					& TIC_CMR_CAP_ABETRG_MASK)
			| ((uint32_t)((config.isTriggeredByRcCompare ? 1u : 0u)
					   << TIC_CMR_CAP_CPCTRG_OFFSET)
					& TIC_CMR_CAP_CPCTRG_MASK)
			| ((0u << TIC_CMR_CAP_WAVE_OFFSET)
					& TIC_CMR_CAP_WAVE_MASK)
			| (((uint32_t)config.raLoadingEdgeSelection
					   << TIC_CMR_CAP_LDRA_OFFSET)
					& TIC_CMR_CAP_LDRA_MASK)
			| (((uint32_t)config.rbLoadingEdgeSelection
					   << TIC_CMR_CAP_LDRB_OFFSET)
					& TIC_CMR_CAP_LDRB_MASK)
			| (((uint32_t)config.loadingEdgeSubsampling
					   << TIC_CMR_CAP_SBSMPLR_OFFSET)
					& TIC_CMR_CAP_SBSMPLR_MASK);
	return cmr;
}

static uint32_t
prepareWaveformModeConfig(Tic_WaveformModeConfig const config)
{
	const uint32_t cmr = ((uint32_t)((config.isStoppedOnRcCompare ? 1u : 0u)
					      << TIC_CMR_WVF_CPCSTOP_OFFSET)
					     & TIC_CMR_WVF_CPCSTOP_MASK)
			| ((uint32_t)((config.isDisabledOnRcCompare ? 1u : 0u)
					   << TIC_CMR_WVF_CPCDIS_OFFSET)
					& TIC_CMR_WVF_CPCDIS_MASK)
			| (((uint32_t)config.externalEvent
					   << TIC_CMR_WVF_EEVTEDG_OFFSET)
					& TIC_CMR_WVF_EEVTEDG_MASK)
			| (((uint32_t)config.externalEventSource
					   << TIC_CMR_WVF_EEVT_OFFSET)
					& TIC_CMR_WVF_EEVT_MASK)
			| (((config.isTriggeredByExternalEvent ? 1u : 0u)
					   << TIC_CMR_WVF_ENETRG_OFFSET)
					& TIC_CMR_WVF_ENETRG_MASK)
			| (((uint32_t)config.waveformMode
					   << TIC_CMR_WVF_WAVSEL_OFFSET)
					& TIC_CMR_WVF_WAVSEL_MASK)
			| ((1u << TIC_CMR_WVF_WAVE_OFFSET)
					& TIC_CMR_WVF_WAVE_MASK)
			| (((uint32_t)config.raCompareEffectOnTioa
					   << TIC_CMR_WVF_ACPA_OFFSET)
					& TIC_CMR_WVF_ACPA_MASK)
			| (((uint32_t)config.rcCompareEffectOnTioa
					   << TIC_CMR_WVF_ACPC_OFFSET)
					& TIC_CMR_WVF_ACPC_MASK)
			| (((uint32_t)config.externalEventEffectOnTioa
					   << TIC_CMR_WVF_AEEVT_OFFSET)
					& TIC_CMR_WVF_AEEVT_MASK)
			| (((uint32_t)config.triggerEffectOnTioa
					   << TIC_CMR_WVF_ASWTRG_OFFSET)
					& TIC_CMR_WVF_ASWTRG_MASK)
			| (((uint32_t)config.rbCompareEffectOnTiob
					   << TIC_CMR_WVF_BCPB_OFFSET)
					& TIC_CMR_WVF_BCPB_MASK)
			| (((uint32_t)config.rcCompareEffectOnTiob
					   << TIC_CMR_WVF_BCPC_OFFSET)
					& TIC_CMR_WVF_BCPC_MASK)
			| (((uint32_t)config.externalEventEffectOnTiob
					   << TIC_CMR_WVF_BEEVT_OFFSET)
					& TIC_CMR_WVF_BEEVT_MASK)
			| (((uint32_t)config.triggerEffectOnTiob
					   << TIC_CMR_WVF_BSWTRG_OFFSET)
					& TIC_CMR_WVF_BSWTRG_MASK);
	return cmr;
}

static uint32_t
prepareCmr(const Tic_ChannelConfig *const config)
{
	uint32_t cmr = (((uint32_t)config->clockSource
					<< TIC_CMR_CAP_TCCLKS_OFFSET)
				       & TIC_CMR_CAP_TCCLKS_MASK)
			| (((config->isClockInverted ? 1u : 0u)
					   << TIC_CMR_CAP_CLKI_OFFSET)
					& TIC_CMR_CAP_CLKI_MASK)
			| (((uint32_t)config->burst << TIC_CMR_CAP_BURST_OFFSET)
					& TIC_CMR_CAP_BURST_MASK);

	switch (config->channelMode) {
	case Tic_Mode_Capture:
		cmr |= prepareCaptureModeConfig(
				config->modeConfig.captureModeConfig);
		break;
	case Tic_Mode_Waveform:
		cmr |= prepareWaveformModeConfig(
				config->modeConfig.waveformModeConfig);
		break;
	}

	return cmr;
}

static uint32_t
prepareEmr(const Tic_ChannelConfig *const config)
{
	return (((uint32_t)config->triggerSourceForInputA
				<< TIC_EMR_TRIGSRCA_OFFSET)
			       & TIC_EMR_TRIGSRCA_MASK)
			| (((uint32_t)config->triggerSourceForInputB
					   << TIC_EMR_TRIGSRCB_OFFSET)
					& TIC_EMR_TRIGSRCB_MASK)
			| (((uint32_t)config->useUndividedPck
					   << TIC_EMR_NODIVCLK_OFFSET)
					& TIC_EMR_NODIVCLK_MASK);
}

static uint32_t
prepareSmmr(const Tic_ChannelConfig *const config)
{
	return ((uint32_t)((config->isGrayCounterEnabled ? 1u : 0u)
				<< TIC_SMMR_GCEN_OFFSET)
			       & TIC_SMMR_GCEN_MASK)
			| ((uint32_t)((config->doesGrayCounterCountDown ? 1u
									: 0u)
					   << TIC_SMMR_DOWN_OFFSET)
					& TIC_SMMR_DOWN_MASK);
}

void
Tic_setChannelConfig(Tic *const tic, Tic_Channel const channel,
		const Tic_ChannelConfig *const config)
{
	if (!config->isEnabled)
		Tic_disableChannel(tic, channel);

	tic->regs->channelRegs[channel].rc =
			(config->rc << TIC_RC_RC_OFFSET) & TIC_RC_RC_MASK;

	tic->regs->channelRegs[channel].cmr = prepareCmr(config);

	if (config->channelMode == Tic_Mode_Waveform) {
		// Only in this mode these registers are read/write.
		// Consequently, they have to be set after mode is set in CMR.
		tic->regs->channelRegs[channel].ra =
				(config->modeConfig.waveformModeConfig.ra
						<< TIC_RA_RA_OFFSET)
				& TIC_RA_RA_MASK;
		tic->regs->channelRegs[channel].rb =
				(config->modeConfig.waveformModeConfig.rb
						<< TIC_RB_RB_OFFSET)
				& TIC_RB_RB_MASK;
	}

	tic->regs->channelRegs[channel].smmr = prepareSmmr(config);

	Tic_setChannelIrqConfig(tic, channel, config->irqConfig);

	tic->regs->channelRegs[channel].emr = prepareEmr(config);

	if (config->isEnabled)
		Tic_enableChannel(tic, channel);
}

static void
getCaptureModeConfig(const Tic *const tic, Tic_Channel const channel,
		Tic_ChannelConfig *const config)
{
	uint32_t cmr = tic->regs->channelRegs[channel].cmr;
	Tic_CaptureModeConfig *captureConfig =
			&config->modeConfig.captureModeConfig;

	config->channelMode = Tic_Mode_Capture;

	captureConfig->isStoppedOnRbLoading =
			((cmr & TIC_CMR_CAP_LDBSTOP_MASK) != 0u);
	captureConfig->isDisabledOnRbLoading =
			((cmr & TIC_CMR_CAP_LDBDIS_MASK) != 0u);
	captureConfig->externalTriggerEdge = ((cmr & TIC_CMR_CAP_ETRGEDG_MASK)
			>> TIC_CMR_CAP_ETRGEDG_OFFSET);
	captureConfig->triggerSource = ((cmr & TIC_CMR_CAP_ABETRG_MASK)
			>> TIC_CMR_CAP_ABETRG_OFFSET);
	captureConfig->isTriggeredByRcCompare =
			((cmr & TIC_CMR_CAP_CPCTRG_MASK) != 0u);
	captureConfig->raLoadingEdgeSelection = ((cmr & TIC_CMR_CAP_LDRA_MASK)
			>> TIC_CMR_CAP_LDRA_OFFSET);
	captureConfig->rbLoadingEdgeSelection = ((cmr & TIC_CMR_CAP_LDRB_MASK)
			>> TIC_CMR_CAP_LDRB_OFFSET);
	captureConfig->loadingEdgeSubsampling =
			((cmr & TIC_CMR_CAP_SBSMPLR_MASK)
					>> TIC_CMR_CAP_SBSMPLR_OFFSET);
}

static void
getWaveformModeConfig(const Tic *const tic, Tic_Channel const channel,
		Tic_ChannelConfig *config)
{
	uint32_t cmr = tic->regs->channelRegs[channel].cmr;
	Tic_WaveformModeConfig *waveConfig =
			&config->modeConfig.waveformModeConfig;

	config->channelMode = Tic_Mode_Waveform;

	waveConfig->isStoppedOnRcCompare = ((cmr & TIC_CMR_WVF_CPCSTOP_MASK)
			>> TIC_CMR_WVF_CPCSTOP_OFFSET);
	waveConfig->isDisabledOnRcCompare = ((cmr & TIC_CMR_WVF_CPCDIS_MASK)
			>> TIC_CMR_WVF_CPCDIS_OFFSET);
	waveConfig->externalEvent = ((cmr & TIC_CMR_WVF_EEVTEDG_MASK)
			>> TIC_CMR_WVF_EEVTEDG_OFFSET);
	waveConfig->externalEventSource = ((cmr & TIC_CMR_WVF_EEVT_MASK)
			>> TIC_CMR_WVF_EEVT_OFFSET);
	waveConfig->isTriggeredByExternalEvent =
			((cmr & TIC_CMR_WVF_ENETRG_MASK)
					>> TIC_CMR_WVF_ENETRG_OFFSET);
	waveConfig->waveformMode = ((cmr & TIC_CMR_WVF_WAVSEL_MASK)
			>> TIC_CMR_WVF_WAVSEL_OFFSET);
	waveConfig->raCompareEffectOnTioa = ((cmr & TIC_CMR_WVF_ACPA_MASK)
			>> TIC_CMR_WVF_ACPA_OFFSET);
	waveConfig->rcCompareEffectOnTioa = ((cmr & TIC_CMR_WVF_ACPC_MASK)
			>> TIC_CMR_WVF_ACPC_OFFSET);
	waveConfig->externalEventEffectOnTioa = ((cmr & TIC_CMR_WVF_AEEVT_MASK)
			>> TIC_CMR_WVF_AEEVT_OFFSET);
	waveConfig->triggerEffectOnTioa = ((cmr & TIC_CMR_WVF_ASWTRG_MASK)
			>> TIC_CMR_WVF_ASWTRG_OFFSET);
	waveConfig->rbCompareEffectOnTiob = ((cmr & TIC_CMR_WVF_BCPB_MASK)
			>> TIC_CMR_WVF_BCPB_OFFSET);
	waveConfig->rcCompareEffectOnTiob = ((cmr & TIC_CMR_WVF_BCPC_MASK)
			>> TIC_CMR_WVF_BCPC_OFFSET);
	waveConfig->externalEventEffectOnTiob = ((cmr & TIC_CMR_WVF_BEEVT_MASK)
			>> TIC_CMR_WVF_BEEVT_OFFSET);
	waveConfig->triggerEffectOnTiob = ((cmr & TIC_CMR_WVF_BSWTRG_MASK)
			>> TIC_CMR_WVF_BSWTRG_OFFSET);

	waveConfig->ra = (tic->regs->channelRegs[channel].ra & TIC_RA_RA_MASK)
			>> TIC_RA_RA_OFFSET;
	waveConfig->rb = (tic->regs->channelRegs[channel].rb & TIC_RB_RB_MASK)
			>> TIC_RB_RB_OFFSET;
}

void
Tic_getChannelConfig(const Tic *const tic, Tic_Channel const channel,
		Tic_ChannelConfig *const config)
{
	uint32_t cmr = tic->regs->channelRegs[channel].cmr;

	config->isEnabled = Tic_isChannelEnabled(tic, channel);
	config->clockSource = ((cmr & TIC_CMR_CAP_TCCLKS_MASK)
			>> TIC_CMR_CAP_TCCLKS_OFFSET);
	config->isClockInverted = ((cmr & TIC_CMR_CAP_CLKI_MASK) != 0u);
	config->burst = ((cmr & TIC_CMR_CAP_BURST_MASK)
			>> TIC_CMR_CAP_BURST_OFFSET);

	if ((cmr & TIC_CMR_CAP_WAVE_MASK) == 0u)
		getCaptureModeConfig(tic, channel, config);
	else
		getWaveformModeConfig(tic, channel, config);

	config->isGrayCounterEnabled = ((tic->regs->channelRegs[channel].smmr
							& TIC_SMMR_GCEN_MASK)
			!= 0u);
	config->doesGrayCounterCountDown =
			((tic->regs->channelRegs[channel].smmr
					 & TIC_SMMR_DOWN_MASK)
					!= 0u);

	Tic_getChannelIrqConfig(tic, channel, &config->irqConfig);

	config->triggerSourceForInputA =
			((tic->regs->channelRegs[channel].emr
					 & TIC_EMR_TRIGSRCA_MASK)
					>> TIC_EMR_TRIGSRCA_OFFSET);
	config->triggerSourceForInputB =
			((tic->regs->channelRegs[channel].emr
					 & TIC_EMR_TRIGSRCB_MASK)
					>> TIC_EMR_TRIGSRCB_OFFSET);
	config->useUndividedPck = ((tic->regs->channelRegs[channel].emr
						   & TIC_EMR_NODIVCLK_MASK)
			!= 0u);
	config->rc = (tic->regs->channelRegs[channel].rc & TIC_RC_RC_MASK)
			>> TIC_RC_RC_OFFSET;
}

void
Tic_setChannelIrqConfig(Tic *const tic, Tic_Channel const channel,
		Tic_ChannelIrqConfig const config)
{
	config.isCounterOverflowIrqEnabled
			? Tic_enableChannelIrq(
					tic, channel, Tic_Irq_CounterOverflow)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_CounterOverflow);

	config.isLoadOverrunIrqEnabled ? Tic_enableChannelIrq(
			tic, channel, Tic_Irq_LoadOverrun)
				       : Tic_disableChannelIrq(tic, channel,
						       Tic_Irq_LoadOverrun);

	config.isRaCompareIrqEnabled
			? Tic_enableChannelIrq(tic, channel, Tic_Irq_RaCompare)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_RaCompare);

	config.isRbCompareIrqEnabled
			? Tic_enableChannelIrq(tic, channel, Tic_Irq_RbCompare)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_RbCompare);

	config.isRcCompareIrqEnabled
			? Tic_enableChannelIrq(tic, channel, Tic_Irq_RcCompare)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_RcCompare);

	config.isRaLoadingIrqEnabled
			? Tic_enableChannelIrq(tic, channel, Tic_Irq_RaLoading)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_RaLoading);

	config.isRbLoadingIrqEnabled
			? Tic_enableChannelIrq(tic, channel, Tic_Irq_RbLoading)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_RbLoading);

	config.isExternalTriggerIrqEnabled
			? Tic_enableChannelIrq(
					tic, channel, Tic_Irq_ExternalTrigger)
			: Tic_disableChannelIrq(
					tic, channel, Tic_Irq_ExternalTrigger);
}

void
Tic_getChannelIrqConfig(const Tic *const tic, Tic_Channel const channel,
		Tic_ChannelIrqConfig *const config)
{
	config->isCounterOverflowIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_CounterOverflow);
	config->isLoadOverrunIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_LoadOverrun);
	config->isRaCompareIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_RaCompare);
	config->isRbCompareIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_RbCompare);
	config->isRcCompareIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_RcCompare);
	config->isRaLoadingIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_RaLoading);
	config->isRbLoadingIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_RbLoading);
	config->isExternalTriggerIrqEnabled = Tic_isChannelIrqEnabled(
			tic, channel, Tic_Irq_ExternalTrigger);
}

void
Tic_enableChannelIrq(
		Tic *const tic, Tic_Channel const channel, Tic_Irq const irq)
{
	tic->regs->channelRegs[channel].ier |= (1u << (uint32_t)irq);
}

void
Tic_disableChannelIrq(
		Tic *const tic, Tic_Channel const channel, Tic_Irq const irq)
{
	tic->regs->channelRegs[channel].idr |= (1u << (uint32_t)irq);
}

bool
Tic_isChannelIrqEnabled(const Tic *const tic, Tic_Channel const channel,
		Tic_Irq const irq)
{
	return ((tic->regs->channelRegs[channel].imr & (1u << (uint32_t)irq))
			!= 0u);
}

void
Tic_getChannelStatus(const Tic *const tic, Tic_Channel const channel,
		Tic_ChannelStatus *const status)
{
	const uint32_t regValue = tic->regs->channelRegs[channel].sr;

	status->hasCounterOverflowed = (regValue & TIC_SR_COVFS_MASK) != 0u;
	status->hasLoadOverrunOccurred = (regValue & TIC_SR_LOVRS_MASK) != 0u;
	status->hasRaCompareOccurred = (regValue & TIC_SR_CPAS_MASK) != 0u;
	status->hasRbCompareOccurred = (regValue & TIC_SR_CPBS_MASK) != 0u;
	status->hasRcCompareOccurred = (regValue & TIC_SR_CPCS_MASK) != 0u;
	status->hasRaLoadOccurred = (regValue & TIC_SR_LDRAS_MASK) != 0u;
	status->hasRbLoadOccurred = (regValue & TIC_SR_LDRBS_MASK) != 0u;
	status->hasExternalTriggerOccurred =
			(regValue & TIC_SR_ETRGS_MASK) != 0u;
	status->isChannelEnabled = (regValue & TIC_SR_CLKSTA_MASK) != 0u;
	status->isTioaAsserted = (regValue & TIC_SR_MTIOA_MASK) != 0u;
	status->isTiobAsserted = (regValue & TIC_SR_MTIOB_MASK) != 0u;
}

uint32_t
Tic_getCounterValue(const Tic *const tic, Tic_Channel const channel)
{
	return tic->regs->channelRegs[channel].cv;
}

void
Tic_configureExternalClockSignals(Tic *const tic,
		Tic_ExternalClockSignalSelection const externalClockSignals)
{
	uint32_t bmr = tic->regs->bmr;
	bmr &= (uint32_t) ~(TIC_BMR_TC0XC0S_MASK | TIC_BMR_TC1XC1S_MASK
			| TIC_BMR_TC2XC2S_MASK);
	bmr |= (((uint32_t)externalClockSignals.xc0 << TIC_BMR_TC0XC0S_OFFSET)
			       & TIC_BMR_TC0XC0S_MASK)
			| (((uint32_t)externalClockSignals.xc1
					   << TIC_BMR_TC1XC1S_OFFSET)
					& TIC_BMR_TC1XC1S_MASK)
			| (((uint32_t)externalClockSignals.xc2
					   << TIC_BMR_TC2XC2S_OFFSET)
					& TIC_BMR_TC2XC2S_MASK);
	tic->regs->bmr = bmr;
}

void
Tic_writeProtect(Tic *const tic, bool protect)
{
	const bool clksta = ((tic->regs->channelRegs[Tic_Channel_0].sr
					     & TIC_SR_CLKSTA_MASK)
			!= 0u);

	Tic_enableChannel(tic, Tic_Channel_0);
	tic->regs->wpmr =
			((TIC_WPMR_WPKEY_PASSWD_VALUE << TIC_WPMR_WPKEY_OFFSET)
					& TIC_WPMR_WPKEY_MASK)
			| (((protect ? 1u : 0u) << TIC_WPMR_WPEN_OFFSET)
					& TIC_WPMR_WPEN_MASK);

	if (!clksta)
		Tic_disableChannel(tic, Tic_Channel_0);
}

uint32_t
Tic_getRaValue(const Tic *const tic, const Tic_Channel channel)
{
	return (tic->regs->channelRegs[channel].ra & TIC_RA_RA_MASK)
			>> TIC_RA_RA_OFFSET;
}

uint32_t
Tic_getRbValue(const Tic *const tic, const Tic_Channel channel)
{
	return (tic->regs->channelRegs[channel].rb & TIC_RB_RB_MASK)
			>> TIC_RB_RB_OFFSET;
}

uint32_t
Tic_getRcValue(const Tic *const tic, const Tic_Channel channel)
{
	return (tic->regs->channelRegs[channel].rc & TIC_RC_RC_MASK)
			>> TIC_RC_RC_OFFSET;
}

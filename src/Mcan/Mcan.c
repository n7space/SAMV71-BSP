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

#include "Mcan.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <Utils/Utils.h>

static bool
verifyMcanId(const Mcan_Id id)
{
	return (id < Mcan_Id_Count);
}

void
Mcan_init(const Mcan_Id id, Mcan *const mcan)
{
	if (!verifyMcanId(id))
		assert("Incorrect Mcan ID" && false);
	assert(mcan != NULL);
	memset(mcan, 0, sizeof(Mcan));

	mcan->id = id;

	if (id == Mcan_Id_0)
		mcan->reg = (Mcan_Registers *)MCAN0_ADDRESS_BASE;
	else if (id == Mcan_Id_1)
		mcan->reg = (Mcan_Registers *)MCAN1_ADDRESS_BASE;
}

static uint8_t
decodeElementSizeInBytes(const Mcan_ElementSize size)
{
	switch (size) {
	case Mcan_ElementSize_8: return 8;
	case Mcan_ElementSize_12: return 12;
	case Mcan_ElementSize_16: return 16;
	case Mcan_ElementSize_20: return 20;
	case Mcan_ElementSize_24: return 24;
	case Mcan_ElementSize_32: return 32;
	case Mcan_ElementSize_48: return 48;
	case Mcan_ElementSize_64: return 64;
	}

	assert(0);
	return 0;
}

static uint8_t
decodeTxElementSizeInBytes(const Mcan_ElementSize size)
{
	return (uint8_t)(decodeElementSizeInBytes(size)
			+ (MCAN_TXELEMENT_DATA_WORD * sizeof(uint32_t)));
}

static uint8_t
decodeRxElementSizeInBytes(const Mcan_ElementSize size)
{
	return (uint8_t)(decodeElementSizeInBytes(size)
			+ (MCAN_RXELEMENT_DATA_WORD * sizeof(uint32_t)));
}

static void
setMsgRamBaseAddress(Mcan *const mcan, const Mcan_Config *const config)
{
	mcan->msgRamBaseAddress = config->msgRamBaseAddress;
	uint32_t *baseAddrRegister;

	switch (mcan->id) {
	case Mcan_Id_0:
		baseAddrRegister = (uint32_t *)MATRIX_CCFG_CAN0_ADDR;
		*baseAddrRegister &= ~MATRIX_CCFG_CAN0_CAN0DMABA_MASK;
		*baseAddrRegister |= (uint32_t)mcan->msgRamBaseAddress
				& MATRIX_CCFG_CAN0_CAN0DMABA_MASK;
		return;
	case Mcan_Id_1:
		baseAddrRegister = (uint32_t *)MATRIX_CCFG_SYSIO_ADDR;
		*baseAddrRegister &= ~MATRIX_CCFG_SYSIO_CAN1DMABA_MASK;
		*baseAddrRegister |= (uint32_t)mcan->msgRamBaseAddress
				& MATRIX_CCFG_SYSIO_CAN1DMABA_MASK;
		return;
	default: return;
	}
}

static bool
setPowerDownMode(Mcan *const mcan, const uint32_t timeoutLimit,
		int *const errCode)
{
	uint32_t timeout = timeoutLimit;

	mcan->reg->cccr |= MCAN_CCCR_CSR_MASK;

	while (((mcan->reg->cccr & MCAN_CCCR_CSA_MASK) == 0u) && (timeout > 0u))
		timeout--;

	if (timeout == 0u)
		return returnError(errCode,
				Mcan_ErrorCodes_ClockStopRequestTimeout);

	return true;
}

static bool
setMode(Mcan *const mcan, const Mcan_Config *const config,
		const uint32_t timeoutLimit, int *const errCode)
{
	switch (config->mode) {
	case Mcan_Mode_Normal: break;
	case Mcan_Mode_AutomaticRetransmissionDisabled:
		mcan->reg->cccr |= MCAN_CCCR_DAR_MASK;
		break;
	case Mcan_Mode_Restricted: mcan->reg->cccr |= MCAN_CCCR_ASM_MASK; break;
	case Mcan_Mode_BusMonitoring:
		mcan->reg->cccr |= MCAN_CCCR_MON_MASK;
		break;
	case Mcan_Mode_PowerDown:
		if (!setPowerDownMode(mcan, timeoutLimit, errCode))
			return false;
		break;
	case Mcan_Mode_InternalLoopBackTest:
		mcan->reg->cccr |= MCAN_CCCR_TEST_MASK;
		mcan->reg->cccr |= MCAN_CCCR_MON_MASK;
		mcan->reg->test = MCAN_TEST_LBCK_MASK;
		break;
	}

	if (config->isFdEnabled)
		mcan->reg->cccr |= MCAN_CCCR_FDOE_MASK;

	return true;
}

static void
setNominalTiming(Mcan *const mcan, const Mcan_Config *const config)
{
	const Mcan_BitTiming *timing = &config->nominalBitTiming;
	assert(timing->timeSegmentBeforeSamplePoint > 0u);

	mcan->reg->nbtp = ((uint32_t)(timing->bitRatePrescaler
					   << MCAN_NBTP_NBRP_OFFSET)
					  & MCAN_NBTP_NBRP_MASK)
			| ((uint32_t)(timing->timeSegmentAfterSamplePoint
					   << MCAN_NBTP_NTSEG2_OFFSET)
					& MCAN_NBTP_NTSEG2_MASK)
			| ((uint32_t)(timing->timeSegmentBeforeSamplePoint
					   << MCAN_NBTP_NTSEG1_OFFSET)
					& MCAN_NBTP_NTSEG1_MASK)
			| ((uint32_t)(timing->synchronizationJump
					   << MCAN_NBTP_NSJW_OFFSET)
					& MCAN_NBTP_NSJW_MASK);
}

static void
setDataTiming(Mcan *const mcan, const Mcan_Config *const config)
{
	const Mcan_BitTiming *timing = &config->dataBitTiming;
	assert(timing->timeSegmentBeforeSamplePoint > 0u);

	mcan->reg->dbtp = ((uint32_t)(timing->bitRatePrescaler
					   << MCAN_DBTP_DBRP_OFFSET)
					  & MCAN_DBTP_DBRP_MASK)
			| ((uint32_t)(timing->timeSegmentAfterSamplePoint
					   << MCAN_DBTP_DTSEG2_OFFSET)
					& MCAN_DBTP_DTSEG2_MASK)
			| ((uint32_t)(timing->timeSegmentBeforeSamplePoint
					   << MCAN_DBTP_DTSEG1_OFFSET)
					& MCAN_DBTP_DTSEG1_MASK)
			| ((uint32_t)(timing->synchronizationJump
					   << MCAN_DBTP_DSJW_OFFSET)
					& MCAN_DBTP_DSJW_MASK);
}

static void
setTransmitterDelayCompensation(
		Mcan *const mcan, const Mcan_Config *const config)
{
	if (config->transmitterDelayCompensation.isEnabled)
		mcan->reg->dbtp |= MCAN_DBTP_TDC_MASK;
	else
		mcan->reg->dbtp &= ~MCAN_DBTP_TDC_MASK;

	mcan->reg->tdcr =
			((uint32_t)(config->transmitterDelayCompensation.filter
					 << MCAN_TDCR_TDCF_OFFSET)
					& MCAN_TDCR_TDCF_MASK)
			| ((uint32_t)(config->transmitterDelayCompensation
							   .offset
					   << MCAN_TDCR_TDCO_OFFSET)
					& MCAN_TDCR_TDCO_MASK);
}

static void
setTimestamp(Mcan *const mcan, const Mcan_Config *const config)
{
	mcan->reg->tscc = (((uint32_t)config->timestampClk
					   << MCAN_TSCC_TSS_OFFSET)
					  & MCAN_TSCC_TSS_MASK)
			| (((uint32_t)config->timestampTimeoutPrescaler
					   << MCAN_TSCC_TCP_OFFSET)
					& MCAN_TSCC_TCP_MASK);

	// Clear the timestamp counter.
	mcan->reg->tscv = 0;
}

static void
setTimeout(Mcan *const mcan, const Mcan_Config *const config)
{
	if (config->isTimeoutEnabled) {
		mcan->reg->tocc = MCAN_TOCC_ETOC_MASK
				| (((uint32_t)config->timeoutType
						   << MCAN_TOCC_TOS_OFFSET)
						& MCAN_TOCC_TOS_MASK)
				| (((uint32_t)config->timeoutPeriod
						   << MCAN_TOCC_TOP_OFFSET)
						& MCAN_TOCC_TOP_MASK);
	} else {
		mcan->reg->tocc = 0;
	}
}

static void
setStandardIdFiltering(Mcan *const mcan, const Mcan_Config *const config)
{
	if (config->standardIdFilter.isIdRejected) {
		mcan->reg->gfc |= MCAN_GFC_RRFS_MASK;
		mcan->reg->sidfc = 0;
		mcan->rxStdFilterAddress = NULL;
		mcan->rxStdFilterSize = 0;
	} else {
		mcan->reg->gfc |=
				((uint32_t)config->standardIdFilter
								.nonMatchingPolicy
						<< MCAN_GFC_ANFS_OFFSET)
				& MCAN_GFC_ANFS_MASK;
		const uint32_t filterListAddress =
				(uint32_t)config->standardIdFilter
						.filterListAddress;
		mcan->reg->sidfc = (filterListAddress & MCAN_SIDFC_FLSSA_MASK)
				| ((uint32_t)(config->standardIdFilter
								   .filterListSize
						   << MCAN_SIDFC_LSS_OFFSET)
						& MCAN_SIDFC_LSS_MASK);
		mcan->rxStdFilterAddress =
				config->standardIdFilter.filterListAddress;
		mcan->rxStdFilterSize = config->standardIdFilter.filterListSize;
	}
}

static void
setExtendedIdFiltering(Mcan *const mcan, const Mcan_Config *const config)
{
	if (config->extendedIdFilter.isIdRejected) {
		mcan->reg->gfc |= MCAN_GFC_RRFE_MASK;
		mcan->reg->xidfc = 0;
		mcan->rxExtFilterAddress = NULL;
		mcan->rxExtFilterSize = 0;
	} else {
		mcan->reg->gfc |=
				((uint32_t)config->extendedIdFilter
								.nonMatchingPolicy
						<< MCAN_GFC_ANFE_OFFSET)
				& MCAN_GFC_ANFE_MASK;
		const uint32_t filterListAddress =
				(uint32_t)config->extendedIdFilter
						.filterListAddress;
		mcan->reg->xidfc = (filterListAddress & MCAN_XIDFC_FLESA_MASK)
				| ((uint32_t)(config->extendedIdFilter
								   .filterListSize
						   << MCAN_XIDFC_LSE_OFFSET)
						& MCAN_XIDFC_LSE_MASK);
		mcan->rxExtFilterAddress =
				config->extendedIdFilter.filterListAddress;
		mcan->rxExtFilterSize = config->extendedIdFilter.filterListSize;
	}
}

static void
setRxFifo0(Mcan *const mcan, const Mcan_Config *const config)
{
	mcan->reg->rxesc &= ~MCAN_RXESC_F0DS_MASK;

	if (config->rxFifo0.isEnabled) {
		const uint32_t startAddress =
				(uint32_t)config->rxFifo0.startAddress;
		mcan->reg->rxf0c = (startAddress & MCAN_RXF0C_F0SA_MASK)
				| (((uint32_t)config->rxFifo0.size
						   << MCAN_RXF0C_F0S_OFFSET)
						& MCAN_RXF0C_F0S_MASK)
				| (((uint32_t)config->rxFifo0.watermark
						   << MCAN_RXF0C_F0WM_OFFSET)
						& MCAN_RXF0C_F0WM_MASK)
				| (((uint32_t)config->rxFifo0.mode
						   << MCAN_RXF0C_F0OM_OFFSET)
						& MCAN_RXF0C_F0OM_MASK);
		mcan->reg->rxesc |= (((uint32_t)config->rxFifo0.elementSize
						     << MCAN_RXESC_F0DS_OFFSET)
				& MCAN_RXESC_F0DS_MASK);
		mcan->rxFifo0Address = config->rxFifo0.startAddress;
		mcan->rxFifo0Size = config->rxFifo0.size;
		mcan->rxFifo0ElementSize = decodeRxElementSizeInBytes(
				config->rxFifo0.elementSize);
	} else {
		mcan->reg->rxf0c = 0;
		mcan->rxFifo0Address = NULL;
		mcan->rxFifo0Size = 0;
		mcan->rxFifo0ElementSize = 0;
	}
}

static void
setRxFifo1(Mcan *const mcan, const Mcan_Config *const config)
{
	mcan->reg->rxesc &= ~MCAN_RXESC_F1DS_MASK;

	if (config->rxFifo1.isEnabled) {
		const uint32_t startAddress =
				(uint32_t)config->rxFifo1.startAddress;
		mcan->reg->rxf1c = (startAddress & MCAN_RXF1C_F1SA_MASK)
				| (((uint32_t)config->rxFifo1.size
						   << MCAN_RXF1C_F1S_OFFSET)
						& MCAN_RXF1C_F1S_MASK)
				| (((uint32_t)config->rxFifo1.watermark
						   << MCAN_RXF1C_F1WM_OFFSET)
						& MCAN_RXF1C_F1WM_MASK)
				| (((uint32_t)config->rxFifo1.mode
						   << MCAN_RXF1C_F1OM_OFFSET)
						& MCAN_RXF1C_F1OM_MASK);
		mcan->reg->rxesc |= (((uint32_t)config->rxFifo1.elementSize
						     << MCAN_RXESC_F1DS_OFFSET)
				& MCAN_RXESC_F1DS_MASK);
		mcan->rxFifo1Address = config->rxFifo1.startAddress;
		mcan->rxFifo1Size = config->rxFifo1.size;
		mcan->rxFifo1ElementSize = decodeRxElementSizeInBytes(
				config->rxFifo1.elementSize);
	} else {
		mcan->reg->rxf1c = 0;
		mcan->rxFifo1Address = NULL;
		mcan->rxFifo1Size = 0;
		mcan->rxFifo1ElementSize = 0;
	}
}

static void
setRxBuffer(Mcan *const mcan, const Mcan_Config *const config)
{
	mcan->reg->rxbc = (uint32_t)config->rxBuffer.startAddress
			& MCAN_RXBC_RBSA_MASK;
	mcan->reg->rxesc &= ~MCAN_RXESC_RBDS_MASK;
	mcan->reg->rxesc |= (((uint32_t)config->rxBuffer.elementSize
					     << MCAN_RXESC_RBDS_OFFSET)
			& MCAN_RXESC_RBDS_MASK);
	mcan->rxBufferAddress = config->rxBuffer.startAddress;
	mcan->rxBufferElementSize = decodeRxElementSizeInBytes(
			config->rxBuffer.elementSize);
}

static void
setTxBuffer(Mcan *const mcan, const Mcan_Config *const config)
{
	if (config->txBuffer.isEnabled) {
		assert((config->txBuffer.bufferSize
				       + config->txBuffer.queueSize)
				<= 32u);

		const uint32_t startAddress =
				(uint32_t)config->txBuffer.startAddress;

		mcan->reg->txbc = (startAddress & MCAN_TXBC_TBSA_MASK)
				| (((uint32_t)config->txBuffer.bufferSize
						   << MCAN_TXBC_NDTB_OFFSET)
						& MCAN_TXBC_NDTB_MASK)
				| (((uint32_t)config->txBuffer.queueSize
						   << MCAN_TXBC_TFQS_OFFSET)
						& MCAN_TXBC_TFQS_MASK)
				| (((uint32_t)config->txBuffer.queueType
						   << MCAN_TXBC_TFQM_OFFSET)
						& MCAN_TXBC_TFQM_MASK);
		mcan->reg->txesc = (((uint32_t)config->txBuffer.elementSize
						    << MCAN_TXESC_TBDS_OFFSET)
				& MCAN_TXESC_TBDS_MASK);
		mcan->txBufferAddress = config->txBuffer.startAddress;
		mcan->txBufferSize = config->txBuffer.bufferSize;
		mcan->txQueueAddress = config->txBuffer.startAddress
				+ (((uint32_t)mcan->txElementSize
						   * config->txBuffer.bufferSize)
						/ sizeof(uint32_t));
		mcan->txQueueSize = config->txBuffer.queueSize;
		mcan->txElementSize = decodeTxElementSizeInBytes(
				config->txBuffer.elementSize);
	} else {
		mcan->reg->txbc = 0;
		mcan->reg->txesc = 0;
		mcan->txBufferAddress = NULL;
		mcan->txBufferSize = 0;
		mcan->txQueueAddress = NULL;
		mcan->txQueueSize = 0;
		mcan->txElementSize = 0;
	}
}

static void
setTxEventFifo(Mcan *const mcan, const Mcan_Config *const config)
{
	if (config->txEventFifo.isEnabled) {
		const uint32_t startAddress =
				(uint32_t)config->txEventFifo.startAddress;
		mcan->reg->txefc = (startAddress & MCAN_TXEFC_EFSA_MASK)
				| (((uint32_t)config->txEventFifo.size
						   << MCAN_TXEFC_EFS_OFFSET)
						& MCAN_TXEFC_EFS_MASK)
				| (((uint32_t)config->txEventFifo.watermark
						   << MCAN_TXEFC_EFWM_OFFSET)
						& MCAN_TXEFC_EFWM_MASK);
		mcan->txEventFifoAddress = config->txEventFifo.startAddress;
		mcan->txEventFifoSize = config->txEventFifo.size;
	} else {
		mcan->reg->txefc = 0;
		mcan->txEventFifoAddress = NULL;
		mcan->txEventFifoSize = 0;
	}
}

static void
setInterrupts(Mcan *const mcan, const Mcan_Config *const config)
{
	for (uint32_t i = 0; i < (uint32_t)Mcan_Interrupt_Count; i++) {
		if ((i == (uint32_t)Mcan_Interrupt_Reserved1)
				|| (i == (uint32_t)Mcan_Interrupt_Reserved2))
			continue;

		uint32_t mask = 1u << i;
		mcan->reg->ir = mask;

		if (config->interrupts[i].isEnabled)
			mcan->reg->ie |= mask;
		else
			mcan->reg->ie &= ~mask;

		if (config->interrupts[i].line == Mcan_InterruptLine_0)
			mcan->reg->ils &= ~mask;
		else
			mcan->reg->ils |= mask;
	}
	mcan->reg->ile = 0;
	if (config->isLine0InterruptEnabled)
		mcan->reg->ile |= MCAN_ILE_EINT0_MASK;
	if (config->isLine1InterruptEnabled)
		mcan->reg->ile |= MCAN_ILE_EINT1_MASK;

	mcan->reg->txbtie = 0;
	mcan->reg->txbcie = 0;
}

bool
Mcan_setConfig(Mcan *const mcan, const Mcan_Config *const config,
		const uint32_t timeoutLimit, int *const errCode)
{
	setMsgRamBaseAddress(mcan, config);

	mcan->reg->cccr = MCAN_CCCR_INIT_MASK;

	uint32_t timeout = timeoutLimit;
	while (((mcan->reg->cccr & MCAN_CCCR_INIT_MASK) == 0u)
			&& (timeout > 0u))
		timeout--;

	if (timeout == 0u)
		return returnError(errCode,
				Mcan_ErrorCodes_InitializationStartTimeout);

	while (((mcan->reg->cccr & MCAN_CCCR_CSA_MASK) == MCAN_CCCR_CSA_MASK)
			&& (timeout > 0u))
		timeout--;

	if (timeout == 0u)
		return returnError(errCode,
				Mcan_ErrorCodes_ClockStopRequestTimeout);

	// Enable write-protected registers.
	mcan->reg->cccr |= MCAN_CCCR_CCE_MASK;
	// Clear the CCCR register.
	mcan->reg->cccr = MCAN_CCCR_CCE_MASK | MCAN_CCCR_INIT_MASK;
	mcan->reg->gfc = 0;

	if (!setMode(mcan, config, timeout, errCode))
		return false;
	setNominalTiming(mcan, config);
	if (config->isFdEnabled) {
		setDataTiming(mcan, config);
		setTransmitterDelayCompensation(mcan, config);
	}
	setTimestamp(mcan, config);
	setTimeout(mcan, config);
	setStandardIdFiltering(mcan, config);
	setExtendedIdFiltering(mcan, config);
	setRxFifo0(mcan, config);
	setRxFifo1(mcan, config);
	setRxBuffer(mcan, config);
	setTxBuffer(mcan, config);
	setTxEventFifo(mcan, config);
	setInterrupts(mcan, config);

	mcan->reg->rwd = (uint32_t)(config->wdtCounter << MCAN_RWD_WDC_OFFSET)
			& MCAN_RWD_WDC_MASK;

	// SAE J1939 masking is not supported
	mcan->reg->xidam = 0x1FFFFFFF;

	mcan->reg->cccr &= ~MCAN_CCCR_INIT_MASK;

	return true;
}

static void
getMsgRamBaseAddress(const Mcan *const mcan, Mcan_Config *const config)
{
	config->msgRamBaseAddress = mcan->msgRamBaseAddress;
}

static void
getMode(const Mcan *const mcan, Mcan_Config *const config)
{
	if (((mcan->reg->cccr & MCAN_CCCR_TEST_MASK) != 0u)
			&& ((mcan->reg->cccr & MCAN_CCCR_MON_MASK) != 0u)
			&& ((mcan->reg->test & MCAN_TEST_LBCK_MASK) != 0u))
		config->mode = Mcan_Mode_InternalLoopBackTest;
	else if (((mcan->reg->cccr & MCAN_CCCR_CSR_MASK) != 0u)
			&& ((mcan->reg->cccr & MCAN_CCCR_CSA_MASK) != 0u))
		config->mode = Mcan_Mode_PowerDown;
	else if ((mcan->reg->cccr & MCAN_CCCR_DAR_MASK) != 0u)
		config->mode = Mcan_Mode_AutomaticRetransmissionDisabled;
	else if ((mcan->reg->cccr & MCAN_CCCR_ASM_MASK) != 0u)
		config->mode = Mcan_Mode_Restricted;
	else if ((mcan->reg->cccr & MCAN_CCCR_MON_MASK) != 0u)
		config->mode = Mcan_Mode_BusMonitoring;
	else
		config->mode = Mcan_Mode_Normal;

	config->isFdEnabled = ((mcan->reg->cccr & MCAN_CCCR_FDOE_MASK) != 0u);
}

static void
getNominalTiming(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t nbtp = mcan->reg->nbtp;
	config->nominalBitTiming.bitRatePrescaler =
			(nbtp & MCAN_NBTP_NBRP_MASK) >> MCAN_NBTP_NBRP_OFFSET;
	config->nominalBitTiming.synchronizationJump = (uint8_t)(
			(nbtp & MCAN_NBTP_NSJW_MASK) >> MCAN_NBTP_NSJW_OFFSET);
	config->nominalBitTiming.timeSegmentAfterSamplePoint =
			(nbtp & MCAN_NBTP_NTSEG2_MASK)
			>> MCAN_NBTP_NTSEG2_OFFSET;
	config->nominalBitTiming.timeSegmentBeforeSamplePoint =
			(nbtp & MCAN_NBTP_NTSEG1_MASK)
			>> MCAN_NBTP_NTSEG1_OFFSET;
}

static void
getDataTiming(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t dbtp = mcan->reg->dbtp;
	config->dataBitTiming.bitRatePrescaler =
			(dbtp & MCAN_DBTP_DBRP_MASK) >> MCAN_DBTP_DBRP_OFFSET;
	config->dataBitTiming.synchronizationJump = (uint8_t)(
			(dbtp & MCAN_DBTP_DSJW_MASK) >> MCAN_DBTP_DSJW_OFFSET);
	config->dataBitTiming.timeSegmentAfterSamplePoint =
			(dbtp & MCAN_DBTP_DTSEG2_MASK)
			>> MCAN_DBTP_DTSEG2_OFFSET;
	config->dataBitTiming.timeSegmentBeforeSamplePoint =
			(dbtp & MCAN_DBTP_DTSEG1_MASK)
			>> MCAN_DBTP_DTSEG1_OFFSET;
}

static void
getTransmitterDelayCompensation(
		const Mcan *const mcan, Mcan_Config *const config)
{
	config->transmitterDelayCompensation.isEnabled =
			((mcan->reg->dbtp & MCAN_DBTP_TDC_MASK) != 0u);

	const uint32_t tdcr = mcan->reg->tdcr;
	config->transmitterDelayCompensation.filter =
			(tdcr & MCAN_TDCR_TDCF_MASK) >> MCAN_TDCR_TDCF_OFFSET;
	config->transmitterDelayCompensation.offset =
			(tdcr & MCAN_TDCR_TDCO_MASK) >> MCAN_TDCR_TDCO_OFFSET;
}

static void
getTimestamp(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t tscc = mcan->reg->tscc;
	config->timestampClk =
			(tscc & MCAN_TSCC_TSS_MASK) >> MCAN_TSCC_TSS_OFFSET;
	config->timestampTimeoutPrescaler =
			(tscc & MCAN_TSCC_TCP_MASK) >> MCAN_TSCC_TCP_OFFSET;
}

static void
getTimeout(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t tscc = mcan->reg->tocc;
	config->timeoutType =
			(tscc & MCAN_TOCC_TOS_MASK) >> MCAN_TOCC_TOS_OFFSET;
	config->timeoutPeriod = (uint16_t)(
			(tscc & MCAN_TOCC_TOP_MASK) >> MCAN_TOCC_TOP_OFFSET);
	config->isTimeoutEnabled = ((tscc & MCAN_TOCC_ETOC_MASK) != 0u);
}

static void
getStandardIdFiltering(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t gfc = mcan->reg->gfc;

	config->standardIdFilter.isIdRejected =
			((gfc & MCAN_GFC_RRFS_MASK) != 0u);
	config->standardIdFilter.nonMatchingPolicy =
			(gfc & MCAN_GFC_ANFS_MASK) >> MCAN_GFC_ANFS_OFFSET;
	config->standardIdFilter.filterListAddress = mcan->rxStdFilterAddress;
	config->standardIdFilter.filterListSize = mcan->rxStdFilterSize;
}

static void
getExtendedIdFiltering(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t gfc = mcan->reg->gfc;

	config->extendedIdFilter.isIdRejected =
			((gfc & MCAN_GFC_RRFE_MASK) != 0u);
	config->extendedIdFilter.nonMatchingPolicy =
			(gfc & MCAN_GFC_ANFE_MASK) >> MCAN_GFC_ANFE_OFFSET;
	config->extendedIdFilter.filterListAddress = mcan->rxExtFilterAddress;
	config->extendedIdFilter.filterListSize = mcan->rxExtFilterSize;
}

static Mcan_ElementSize
encodeElementSize(const uint8_t size)
{
	switch (size) {
	case 8: return Mcan_ElementSize_8;
	case 12: return Mcan_ElementSize_12;
	case 16: return Mcan_ElementSize_16;
	case 20: return Mcan_ElementSize_20;
	case 24: return Mcan_ElementSize_24;
	case 32: return Mcan_ElementSize_32;
	case 48: return Mcan_ElementSize_48;
	case 64: return Mcan_ElementSize_64;
	}

	assert(0);
	return 0;
}

static Mcan_ElementSize
encodeTxElementSizeInBytes(const uint8_t size)
{
	return encodeElementSize((uint8_t)(
			size - (MCAN_TXELEMENT_DATA_WORD * sizeof(uint32_t))));
}

static Mcan_ElementSize
encodeRxElementSizeInBytes(const uint8_t size)
{
	return encodeElementSize((uint8_t)(
			size - (MCAN_RXELEMENT_DATA_WORD * sizeof(uint32_t))));
}

static void
getRxFifo0(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t rxf0c = mcan->reg->rxf0c;

	config->rxFifo0.isEnabled = (rxf0c != 0u);

	if (!config->rxFifo0.isEnabled)
		return;

	config->rxFifo0.mode = (rxf0c & MCAN_RXF0C_F0OM_MASK)
			>> MCAN_RXF0C_F0OM_OFFSET;
	config->rxFifo0.watermark = (rxf0c & MCAN_RXF0C_F0WM_MASK)
			>> MCAN_RXF0C_F0WM_OFFSET;
	config->rxFifo0.startAddress = mcan->rxFifo0Address;
	config->rxFifo0.size = mcan->rxFifo0Size;
	config->rxFifo0.elementSize =
			encodeRxElementSizeInBytes(mcan->rxFifo0ElementSize);
}

static void
getRxFifo1(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t rxf1c = mcan->reg->rxf1c;

	config->rxFifo1.isEnabled = (rxf1c != 0u);

	if (!config->rxFifo1.isEnabled)
		return;

	config->rxFifo1.mode = (rxf1c & MCAN_RXF1C_F1OM_MASK)
			>> MCAN_RXF1C_F1OM_OFFSET;
	config->rxFifo1.watermark = (rxf1c & MCAN_RXF1C_F1WM_MASK)
			>> MCAN_RXF1C_F1WM_OFFSET;
	config->rxFifo1.startAddress = mcan->rxFifo1Address;
	config->rxFifo1.size = mcan->rxFifo1Size;
	config->rxFifo1.elementSize =
			encodeRxElementSizeInBytes(mcan->rxFifo1ElementSize);
}

static void
getRxBuffer(const Mcan *const mcan, Mcan_Config *const config)
{
	config->rxBuffer.startAddress = mcan->rxBufferAddress;
	config->rxBuffer.elementSize =
			encodeRxElementSizeInBytes(mcan->rxBufferElementSize);
}

static void
getTxBuffer(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t txbc = mcan->reg->txbc;

	config->txBuffer.isEnabled = (txbc != 0u);

	if (!config->txBuffer.isEnabled)
		return;

	config->txBuffer.startAddress = mcan->txBufferAddress;
	config->txBuffer.bufferSize = mcan->txBufferSize;
	config->txBuffer.queueSize = mcan->txQueueSize;
	config->txBuffer.queueType =
			(txbc & MCAN_TXBC_TFQM_MASK) >> MCAN_TXBC_TFQM_OFFSET;
	config->txBuffer.elementSize =
			encodeTxElementSizeInBytes(mcan->txElementSize);
}

static void
getTxEventFifo(const Mcan *const mcan, Mcan_Config *const config)
{
	const uint32_t txefc = mcan->reg->txefc;

	config->txEventFifo.isEnabled = (txefc != 0u);

	if (!config->txEventFifo.isEnabled)
		return;

	config->txEventFifo.startAddress = mcan->txEventFifoAddress;
	config->txEventFifo.size = mcan->txEventFifoSize;
	config->txEventFifo.watermark = (txefc & MCAN_TXEFC_EFWM_MASK)
			>> MCAN_TXEFC_EFWM_OFFSET;
}

static void
getInterrupts(const Mcan *const mcan, Mcan_Config *const config)
{
	for (uint32_t i = 0; i < (uint32_t)Mcan_Interrupt_Count; i++) {
		if ((i == (uint32_t)Mcan_Interrupt_Reserved1)
				|| (i == (uint32_t)Mcan_Interrupt_Reserved2)) {
			config->interrupts[i].isEnabled = false;
			continue;
		}

		const uint32_t mask = 1u << i;
		config->interrupts[i].isEnabled =
				((mcan->reg->ie & mask) != 0u);

		if ((mcan->reg->ils & mask) == 0u)
			config->interrupts[i].line = Mcan_InterruptLine_0;
		else
			config->interrupts[i].line = Mcan_InterruptLine_1;
	}

	config->isLine0InterruptEnabled =
			((mcan->reg->ile & MCAN_ILE_EINT0_MASK) != 0u);
	config->isLine1InterruptEnabled =
			((mcan->reg->ile & MCAN_ILE_EINT1_MASK) != 0u);
}

void
Mcan_getConfig(const Mcan *const mcan, Mcan_Config *const config)
{
	getMsgRamBaseAddress(mcan, config);
	getMode(mcan, config);
	getNominalTiming(mcan, config);
	getDataTiming(mcan, config);
	getTransmitterDelayCompensation(mcan, config);
	getTimestamp(mcan, config);
	getTimeout(mcan, config);
	getStandardIdFiltering(mcan, config);
	getExtendedIdFiltering(mcan, config);
	getRxFifo0(mcan, config);
	getRxFifo1(mcan, config);
	getRxBuffer(mcan, config);
	getTxBuffer(mcan, config);
	getTxEventFifo(mcan, config);
	getInterrupts(mcan, config);

	config->wdtCounter = (mcan->reg->rwd & MCAN_RWD_WDC_MASK)
			>> MCAN_RWD_WDC_OFFSET;
}

static uint8_t
encodeDataLengthCode(const uint8_t size)
{
	if (size <= 8u)
		return size;
	switch (size) {
	case 12: return 9;
	case 16: return 10;
	case 20: return 11;
	case 24: return 12;
	case 32: return 13;
	case 48: return 14;
	case 64: return 15;
	}
	assert(0);
	return 0;
}

static uint8_t
decodeDataLengthCode(const uint8_t dlc, const bool isCanFdFrame)
{
	if (dlc <= 8u)
		return dlc;
	if (!isCanFdFrame)
		return 8u;

	switch (dlc) {
	case 9: return 12;
	case 10: return 16;
	case 11: return 20;
	case 12: return 24;
	case 13: return 32;
	case 14: return 48;
	case 15: return 64;
	}
	assert(0);
	return 0;
}

static void
txAddElement(Mcan *const mcan, const Mcan_TxElement element,
		uint32_t *const baseAddress, const uint8_t index)
{
	memset(baseAddress, 0, mcan->txElementSize);

	baseAddress[MCAN_TXELEMENT_ESI_WORD] |=
			((uint32_t)element.esiFlag << MCAN_TXELEMENT_ESI_OFFSET)
			& MCAN_TXELEMENT_ESI_MASK;
	baseAddress[MCAN_TXELEMENT_XTD_WORD] |=
			((uint32_t)element.idType << MCAN_TXELEMENT_XTD_OFFSET)
			& MCAN_TXELEMENT_XTD_MASK;
	baseAddress[MCAN_TXELEMENT_RTR_WORD] |=
			((uint32_t)element.frameType
					<< MCAN_TXELEMENT_RTR_OFFSET)
			& MCAN_TXELEMENT_RTR_MASK;
	baseAddress[MCAN_TXELEMENT_MM_WORD] |=
			(uint32_t)(element.marker << MCAN_TXELEMENT_MM_OFFSET)
			& MCAN_TXELEMENT_MM_MASK;

	if (element.idType == Mcan_IdType_Standard)
		baseAddress[MCAN_TXELEMENT_STDID_WORD] |=
				(element.id << MCAN_TXELEMENT_STDID_OFFSET)
				& MCAN_TXELEMENT_STDID_MASK;
	else
		baseAddress[MCAN_TXELEMENT_EXTID_WORD] |=
				(element.id << MCAN_TXELEMENT_EXTID_OFFSET)
				& MCAN_TXELEMENT_EXTID_MASK;

	if (element.isTxEventStored)
		baseAddress[MCAN_TXELEMENT_EFC_WORD] |= MCAN_TXELEMENT_EFC_MASK;
	if (element.isCanFdFormatEnabled)
		baseAddress[MCAN_TXELEMENT_FDF_WORD] |= MCAN_TXELEMENT_FDF_MASK;
	if (element.isBitRateSwitchingEnabled)
		baseAddress[MCAN_TXELEMENT_BRS_WORD] |= MCAN_TXELEMENT_BRS_MASK;
	baseAddress[MCAN_TXELEMENT_DLC_WORD] |=
			(uint32_t)(encodeDataLengthCode(element.dataSize)
					<< MCAN_TXELEMENT_DLC_OFFSET)
			& MCAN_TXELEMENT_DLC_MASK;

	uint8_t *dataPointer =
			(uint8_t *)&baseAddress[MCAN_TXELEMENT_DATA_WORD];
	memcpy(dataPointer, element.data, element.dataSize);

	if (element.isInterruptEnabled)
		mcan->reg->txbtie |= 1u << index;
	else
		mcan->reg->txbtie &= ~(1u << index);
}

bool
Mcan_txBufferAdd(Mcan *const mcan, const Mcan_TxElement element,
		const uint8_t index, int *const errCode)
{
	if (index >= mcan->txBufferSize)
		return returnError(errCode, Mcan_ErrorCodes_IndexOutOfRange);

	uint32_t *bufferPointer = mcan->txBufferAddress
			+ (((uint32_t)mcan->txElementSize * index)
					/ sizeof(uint32_t));

	txAddElement(mcan, element, bufferPointer, index);
	mcan->reg->txbar = 1u << index;

	return true;
}

bool
Mcan_txQueuePush(Mcan *const mcan, const Mcan_TxElement element,
		uint8_t *const index, int *const errCode)
{
	const bool full = (mcan->reg->txfqs & MCAN_TXFQS_TFQF_MASK) != 0u;
	if (full)
		return returnError(errCode, Mcan_ErrorCodes_TxFifoFull);
	*index = (mcan->reg->txfqs & MCAN_TXFQS_TFQPI_MASK)
			>> MCAN_TXFQS_TFQPI_OFFSET;
	uint32_t *baseAddr = mcan->txBufferAddress
			+ ((uint32_t)(mcan->txElementSize * (*index))
					/ sizeof(uint32_t));
	txAddElement(mcan, element, baseAddr, *index);
	mcan->reg->txbar = 1u << *index;

	return true;
}

bool
Mcan_txBufferIsTransmissionFinished(const Mcan *const mcan, const uint8_t index)
{
	return (mcan->reg->txbto & (1u << index)) != 0u;
}

static bool
isTxEventFifoEmpty(const Mcan *const mcan)
{
	const uint8_t count = (mcan->reg->txefs & MCAN_TXEFS_EFFL_MASK)
			>> MCAN_TXEFS_EFFL_OFFSET;
	return count == 0u;
}

static const uint32_t *
getTxEventFifoBaseAddress(const Mcan *const mcan)
{
	const uint8_t getIndex = (mcan->reg->txefs & MCAN_TXEFS_EFGI_MASK)
			>> MCAN_TXEFS_EFGI_OFFSET;
	return mcan->txEventFifoAddress
			+ ((uint32_t)(MCAN_TXEVENTELEMENT_SIZE * getIndex)
					/ sizeof(uint32_t));
}

bool
Mcan_txEventFifoPull(Mcan *const mcan, Mcan_TxEventElement *const element,
		int *const errCode)
{
	if (isTxEventFifoEmpty(mcan))
		return returnError(errCode, Mcan_ErrorCodes_TxEventFifoEmpty);

	const uint32_t *baseAddr = getTxEventFifoBaseAddress(mcan);

	element->esiFlag = (baseAddr[MCAN_TXEVENTELEMENT_ESI_WORD]
					   & MCAN_TXEVENTELEMENT_ESI_MASK)
			>> MCAN_TXEVENTELEMENT_ESI_OFFSET;
	element->idType = (baseAddr[MCAN_TXEVENTELEMENT_XTD_WORD]
					  & MCAN_TXEVENTELEMENT_XTD_MASK)
			>> MCAN_TXEVENTELEMENT_XTD_OFFSET;
	element->frameType = (baseAddr[MCAN_TXEVENTELEMENT_RTR_WORD]
					     & MCAN_TXEVENTELEMENT_RTR_MASK)
			>> MCAN_TXEVENTELEMENT_RTR_OFFSET;
	if (element->idType == Mcan_IdType_Standard)
		element->id = (baseAddr[MCAN_TXEVENTELEMENT_STDID_WORD]
					      & MCAN_TXEVENTELEMENT_STDID_MASK)
				>> MCAN_TXEVENTELEMENT_STDID_OFFSET;
	else
		element->id = (baseAddr[MCAN_TXEVENTELEMENT_EXTID_WORD]
					      & MCAN_TXEVENTELEMENT_EXTID_MASK)
				>> MCAN_TXEVENTELEMENT_EXTID_OFFSET;
	element->marker =
			(uint8_t)((baseAddr[MCAN_TXEVENTELEMENT_MM_WORD]
						  & MCAN_TXEVENTELEMENT_MM_MASK)
					>> MCAN_TXEVENTELEMENT_MM_OFFSET);
	element->eventType = (baseAddr[MCAN_TXEVENTELEMENT_ET_WORD]
					     & MCAN_TXEVENTELEMENT_ET_MASK)
			>> MCAN_TXEVENTELEMENT_ET_OFFSET;
	element->isCanFdFormatEnabled =
			(baseAddr[MCAN_TXEVENTELEMENT_FDF_WORD]
					& MCAN_TXEVENTELEMENT_FDF_MASK)
			!= 0u;
	element->isBitRateSwitchingEnabled =
			(baseAddr[MCAN_TXEVENTELEMENT_BRS_WORD]
					& MCAN_TXEVENTELEMENT_BRS_MASK)
			!= 0u;
	element->timestamp = (baseAddr[MCAN_TXEVENTELEMENT_TXTS_WORD]
					     & MCAN_TXEVENTELEMENT_TXTS_MASK)
			>> MCAN_TXEVENTELEMENT_TXTS_OFFSET;
	element->dataSize = decodeDataLengthCode(
			(baseAddr[MCAN_TXEVENTELEMENT_DLC_WORD]
					& MCAN_TXEVENTELEMENT_DLC_MASK)
					>> MCAN_TXEVENTELEMENT_DLC_OFFSET,
			element->isCanFdFormatEnabled);
	return true;
}

static void
getRxElement(const uint32_t *const baseAddr, Mcan_RxElement *const element)
{
	element->esiFlag = (baseAddr[MCAN_RXELEMENT_ESI_WORD]
					   & MCAN_RXELEMENT_ESI_MASK)
			>> MCAN_RXELEMENT_ESI_OFFSET;
	element->idType = (baseAddr[MCAN_RXELEMENT_XTD_WORD]
					  & MCAN_RXELEMENT_XTD_MASK)
			>> MCAN_RXELEMENT_XTD_OFFSET;
	element->frameType = (baseAddr[MCAN_RXELEMENT_RTR_WORD]
					     & MCAN_RXELEMENT_RTR_MASK)
			>> MCAN_RXELEMENT_RTR_OFFSET;
	if (element->idType == Mcan_IdType_Standard)
		element->id = (baseAddr[MCAN_RXELEMENT_STDID_WORD]
					      & MCAN_RXELEMENT_STDID_MASK)
				>> MCAN_RXELEMENT_STDID_OFFSET;
	else
		element->id = (baseAddr[MCAN_RXELEMENT_EXTID_WORD]
					      & MCAN_RXELEMENT_EXTID_MASK)
				>> MCAN_RXELEMENT_EXTID_OFFSET;
	element->isNonMatchingFrame =
			(baseAddr[MCAN_RXELEMENT_ANMF_WORD]
					& MCAN_RXELEMENT_ANMF_MASK)
			>> MCAN_RXELEMENT_ANMF_OFFSET;
	element->filterIndex = (baseAddr[MCAN_RXELEMENT_FIDX_WORD]
					       & MCAN_RXELEMENT_FIDX_MASK)
			>> MCAN_RXELEMENT_FIDX_OFFSET;
	element->isCanFdFormatEnabled =
			(baseAddr[MCAN_RXELEMENT_FDF_WORD]
					& MCAN_RXELEMENT_FDF_MASK)
			>> MCAN_RXELEMENT_FDF_OFFSET;
	element->isBitRateSwitchingEnabled =
			(baseAddr[MCAN_RXELEMENT_BRS_WORD]
					& MCAN_RXELEMENT_BRS_MASK)
			>> MCAN_RXELEMENT_BRS_OFFSET;
	element->timestamp = (baseAddr[MCAN_RXELEMENT_RXTS_WORD]
					     & MCAN_RXELEMENT_RXTS_MASK)
			>> MCAN_RXELEMENT_RXTS_OFFSET;
	element->dataSize = decodeDataLengthCode(
			(baseAddr[MCAN_RXELEMENT_DLC_WORD]
					& MCAN_RXELEMENT_DLC_MASK)
					>> MCAN_RXELEMENT_DLC_OFFSET,
			element->isCanFdFormatEnabled);
	const uint8_t *dataPointer =
			(const uint8_t *)&baseAddr[MCAN_RXELEMENT_DATA_WORD];
	memcpy(element->data, dataPointer, element->dataSize);
}

void
Mcan_rxBufferGet(Mcan *const mcan, const uint8_t index,
		Mcan_RxElement *const element)
{
	const uint32_t *bufferPointer = mcan->rxBufferAddress
			+ ((uint32_t)(mcan->rxBufferElementSize * index)
					/ sizeof(uint32_t));

	getRxElement(bufferPointer, element);
}

static bool
rx0FifoPull(Mcan *const mcan, Mcan_RxElement *const element, int *const errCode)
{
	const uint8_t count = (mcan->reg->rxf0s & MCAN_RXF0S_F0FL_MASK)
			>> MCAN_RXF0S_F0FL_OFFSET;
	if (count == 0u)
		return returnError(errCode, Mcan_ErrorCodes_RxFifoEmpty);
	const uint8_t getIndex = (mcan->reg->rxf0s & MCAN_RXF0S_F0GI_MASK)
			>> MCAN_RXF0S_F0GI_OFFSET;
	const uint32_t *const baseAddr = mcan->rxFifo0Address
			+ ((uint32_t)(mcan->rxFifo0ElementSize * getIndex)
					/ sizeof(uint32_t));
	getRxElement(baseAddr, element);
	mcan->reg->rxf0a = (uint32_t)(getIndex << MCAN_RXF0A_F0AI_OFFSET)
			& MCAN_RXF0A_F0AI_MASK;
	return true;
}

static bool
rx1FifoPull(Mcan *const mcan, Mcan_RxElement *const element, int *const errCode)
{
	const uint8_t count = (mcan->reg->rxf1s & MCAN_RXF1S_F1FL_MASK)
			>> MCAN_RXF1S_F1FL_OFFSET;
	if (count == 0u)
		return returnError(errCode, Mcan_ErrorCodes_RxFifoEmpty);
	const uint8_t getIndex = (mcan->reg->rxf1s & MCAN_RXF1S_F1GI_MASK)
			>> MCAN_RXF1S_F1GI_OFFSET;
	const uint32_t *const baseAddr = mcan->rxFifo1Address
			+ ((uint32_t)(mcan->rxFifo1ElementSize * getIndex)
					/ sizeof(uint32_t));
	getRxElement(baseAddr, element);
	mcan->reg->rxf1a = (uint32_t)(getIndex << MCAN_RXF1A_F1AI_OFFSET)
			& MCAN_RXF1A_F1AI_MASK;
	return true;
}

bool
Mcan_rxFifoPull(Mcan *const mcan, const Mcan_RxFifoId id,
		Mcan_RxElement *const element, int *const errCode)
{
	assert(mcan != NULL);
	switch (id) {
	case Mcan_RxFifoId_0: return rx0FifoPull(mcan, element, errCode);
	case Mcan_RxFifoId_1: return rx1FifoPull(mcan, element, errCode);
	}
	return returnError(errCode, Mcan_ErrorCodes_InvalidRxFifoId);
}

bool
Mcan_getRxFifoStatus(const Mcan *const mcan, const Mcan_RxFifoId id,
		Mcan_RxFifoStatus *const status, int *const errCode)
{
	switch (id) {
	case Mcan_RxFifoId_0:
		status->count = (mcan->reg->rxf0s & MCAN_RXF0S_F0FL_MASK)
				>> MCAN_RXF0S_F0FL_OFFSET;
		status->isFull = (mcan->reg->rxf0s & MCAN_RXF0S_F0F_MASK)
				>> MCAN_RXF0S_F0F_OFFSET;
		status->isMessageLost =
				(mcan->reg->rxf0s & MCAN_RXF0S_RF0L_MASK)
				>> MCAN_RXF0S_RF0L_OFFSET;
		return true;
	case Mcan_RxFifoId_1:
		status->count = (mcan->reg->rxf1s & MCAN_RXF1S_F1FL_MASK)
				>> MCAN_RXF1S_F1FL_OFFSET;
		status->isFull = (mcan->reg->rxf1s & MCAN_RXF1S_F1F_MASK)
				>> MCAN_RXF1S_F1F_OFFSET;
		status->isMessageLost =
				(mcan->reg->rxf1s & MCAN_RXF1S_RF1L_MASK)
				>> MCAN_RXF1S_RF1L_OFFSET;
		return true;
	}

	return returnError(errCode, Mcan_ErrorCodes_InvalidRxFifoId);
}

void
Mcan_getTxQueueStatus(const Mcan *const mcan, Mcan_TxQueueStatus *const status)
{
	status->isFull = (mcan->reg->txfqs & MCAN_TXFQS_TFQF_MASK)
			>> MCAN_TXFQS_TFQF_OFFSET;
}

void
Mcan_getTxEventFifoStatus(
		const Mcan *const mcan, Mcan_TxEventFifoStatus *const status)
{
	status->count = (mcan->reg->txefs & MCAN_TXEFS_EFFL_MASK)
			>> MCAN_TXEFS_EFFL_OFFSET;
	status->isFull = (mcan->reg->txefs & MCAN_TXEFS_EFF_MASK)
			>> MCAN_TXEFS_EFF_OFFSET;
	status->isMessageLost = (mcan->reg->txefs & MCAN_TXEFS_TEFL_MASK)
			>> MCAN_TXEFS_TEFL_OFFSET;
}

bool
Mcan_setStandardIdFilter(Mcan *const mcan, const Mcan_RxFilterElement element,
		const uint8_t index, int *const errCode)
{
	if (index >= mcan->rxStdFilterSize)
		return returnError(errCode, Mcan_ErrorCodes_IndexOutOfRange);

	uint32_t *bufferPointer = mcan->rxStdFilterAddress
			+ (((uint32_t)MCAN_STDRXFILTERELEMENT_SIZE * index)
					/ sizeof(uint32_t));
	*bufferPointer = (((uint32_t)element.type
					  << MCAN_STDRXFILTERELEMENT_SFT_OFFSET)
					 & MCAN_STDRXFILTERELEMENT_SFT_MASK)
			| (((uint32_t)element.config
					   << MCAN_STDRXFILTERELEMENT_SFEC_OFFSET)
					& MCAN_STDRXFILTERELEMENT_SFEC_MASK)
			| ((element.id1 << MCAN_STDRXFILTERELEMENT_SFID1_OFFSET)
					& MCAN_STDRXFILTERELEMENT_SFID1_MASK)
			| ((element.id2 << MCAN_STDRXFILTERELEMENT_SFID2_OFFSET)
					& MCAN_STDRXFILTERELEMENT_SFID2_MASK);

	return true;
}

bool
Mcan_setExtendedIdFilter(Mcan *const mcan, const Mcan_RxFilterElement element,
		const uint8_t index, int *const errCode)
{
	if (index >= mcan->rxExtFilterSize)
		return returnError(errCode, Mcan_ErrorCodes_IndexOutOfRange);

	uint32_t *bufferPointer = mcan->rxExtFilterAddress
			+ (((uint32_t)MCAN_EXTRXFILTERELEMENT_SIZE * index)
					/ sizeof(uint32_t));

	memset(bufferPointer, 0, MCAN_EXTRXFILTERELEMENT_SIZE);

	bufferPointer[MCAN_EXTRXFILTERELEMENT_EFT_WORD] |=
			((uint32_t)element.type
					<< MCAN_EXTRXFILTERELEMENT_EFT_OFFSET)
			& MCAN_EXTRXFILTERELEMENT_EFT_MASK;
	bufferPointer[MCAN_EXTRXFILTERELEMENT_EFEC_WORD] |=
			((uint32_t)element.config
					<< MCAN_EXTRXFILTERELEMENT_EFEC_OFFSET)
			& MCAN_EXTRXFILTERELEMENT_EFEC_MASK;
	bufferPointer[MCAN_EXTRXFILTERELEMENT_EFID1_WORD] |=
			(element.id1 << MCAN_EXTRXFILTERELEMENT_EFID1_OFFSET)
			& MCAN_EXTRXFILTERELEMENT_EFID1_MASK;
	bufferPointer[MCAN_EXTRXFILTERELEMENT_EFID2_WORD] |=
			(element.id2 << MCAN_EXTRXFILTERELEMENT_EFID2_OFFSET)
			& MCAN_EXTRXFILTERELEMENT_EFID2_MASK;

	return true;
}

void
Mcan_getInterruptStatus(
		const Mcan *const mcan, Mcan_InterruptStatus *const status)
{
	uint32_t flags = mcan->reg->ir;
	mcan->reg->ir = flags;

	status->hasRf0nOccured = (flags & MCAN_IR_RF0N_MASK) != 0u;
	status->hasRf0wOccured = (flags & MCAN_IR_RF0W_MASK) != 0u;
	status->hasRf0fOccured = (flags & MCAN_IR_RF0F_MASK) != 0u;
	status->hasRf0lOccured = (flags & MCAN_IR_RF0L_MASK) != 0u;
	status->hasRf1nOccured = (flags & MCAN_IR_RF1N_MASK) != 0u;
	status->hasRf1wOccured = (flags & MCAN_IR_RF1W_MASK) != 0u;
	status->hasRf1fOccured = (flags & MCAN_IR_RF1F_MASK) != 0u;
	status->hasRf1lOccured = (flags & MCAN_IR_RF1L_MASK) != 0u;
	status->hasHpmOccured = (flags & MCAN_IR_HPM_MASK) != 0u;
	status->hasTcOccured = (flags & MCAN_IR_TC_MASK) != 0u;
	status->hasTcfOccured = (flags & MCAN_IR_TCF_MASK) != 0u;
	status->hasTfeOccured = (flags & MCAN_IR_TFE_MASK) != 0u;
	status->hasTefnOccured = (flags & MCAN_IR_TEFN_MASK) != 0u;
	status->hasTefwOccured = (flags & MCAN_IR_TEFW_MASK) != 0u;
	status->hasTeffOccured = (flags & MCAN_IR_TEFF_MASK) != 0u;
	status->hasTeflOccured = (flags & MCAN_IR_TEFL_MASK) != 0u;
	status->hasTswOccured = (flags & MCAN_IR_TSW_MASK) != 0u;
	status->hasMrafOccured = (flags & MCAN_IR_MRAF_MASK) != 0u;
	status->hasTooOccured = (flags & MCAN_IR_TOO_MASK) != 0u;
	status->hasDrxOccured = (flags & MCAN_IR_DRX_MASK) != 0u;
	status->hasEloOccured = (flags & MCAN_IR_ELO_MASK) != 0u;
	status->hasEpOccured = (flags & MCAN_IR_EP_MASK) != 0u;
	status->hasEwOccured = (flags & MCAN_IR_EW_MASK) != 0u;
	status->hasBoOccured = (flags & MCAN_IR_BO_MASK) != 0u;
	status->hasWdiOccured = (flags & MCAN_IR_WDI_MASK) != 0u;
	status->hasPeaOccured = (flags & MCAN_IR_PEA_MASK) != 0u;
	status->hasPedOccured = (flags & MCAN_IR_PED_MASK) != 0u;
	status->hasAraOccured = (flags & MCAN_IR_ARA_MASK) != 0u;
}

bool
Mcan_isTxFifoEmpty(const Mcan *const mcan)
{
	const uint32_t freeLevel = (mcan->reg->txfqs & MCAN_TXFQS_TFFL_MASK)
			>> MCAN_TXFQS_TFFL_OFFSET;
	const uint32_t queueSize = (mcan->reg->txbc & MCAN_TXBC_TFQS_MASK)
			>> MCAN_TXBC_TFQS_OFFSET;
	return freeLevel == queueSize;
}

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

/**
 * @defgroup Mcan Mcan
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_MCAN_H
#define BSP_MCAN_H

#include <stdbool.h>
#include <stdint.h>

#include "McanRegisters.h"

/// \brief Mcan error codes.
typedef enum {
	Mcan_ErrorCodes_InvalidRxFifoId = 1, ///< Invalid RX FIFO id was given.
	Mcan_ErrorCodes_RxFifoEmpty = 2, ///< RX FIFO is empty.
	Mcan_ErrorCodes_TxFifoFull = 3, ///< RX FIFO is empty.
	Mcan_ErrorCodes_TxEventFifoEmpty = 4, ///< RX FIFO is empty.
	/// \brief Clock domains synchronization timeout.
	Mcan_ErrorCodes_InitializationStartTimeout = 5,
	/// \brief Clock stop request timeout.
	Mcan_ErrorCodes_ClockStopRequestTimeout = 6,
	Mcan_ErrorCodes_IndexOutOfRange = 7, ///< Requested index out of range.
} Mcan_ErrorCodes;

/// \brief Mcan device identifiers.
typedef enum {
	Mcan_Id_0 = 0, ///< MCAN0 Id.
	Mcan_Id_1 = 1, ///< MCAN1 Id.
	Mcan_Id_Count = 2, ///< Number of available MCAN Ids.
} Mcan_Id;

/// \brief Mcan Rx FIFO identifier.
typedef enum {
	Mcan_RxFifoId_0 = 0, ///< Rx FIFO0 Id.
	Mcan_RxFifoId_1 = 1, ///< Rx FIFO1 Id.
} Mcan_RxFifoId;

/// \brief The status of Mcan Rx FIFO.
typedef struct {
	uint8_t count; ///< Number of elements in Rx FIFO.
	bool isFull; ///< FIFO full flag.
	bool isMessageLost; ///< Message lost flag.
} Mcan_RxFifoStatus;

/// \brief The status of Mcan Tx Queue.
typedef struct {
	bool isFull; ///< Queue full flag.
} Mcan_TxQueueStatus;

/// \brief The status of Mcan Tx Event FIFO.
typedef struct {
	uint8_t count; ///< Number of elements in Tx Event FIFO.
	bool isFull; ///< FIFO full flag.
	bool isMessageLost; ///< Message lost flag.
} Mcan_TxEventFifoStatus;

/// \brief Mcan device operation mode.
typedef enum {
	/// \brief Normal operation mode.
	Mcan_Mode_Normal,
	/// \brief Normal operation mode with automatic retransmission disabled.
	Mcan_Mode_AutomaticRetransmissionDisabled,
	/// \brief Restricted operation mode.
	Mcan_Mode_Restricted,
	/// \brief Bus monitoring operation mode.
	Mcan_Mode_BusMonitoring,
	/// \brief Power down operation mode.
	Mcan_Mode_PowerDown,
	/// \brief Internal loopback test operation mode.
	Mcan_Mode_InternalLoopBackTest,
} Mcan_Mode;

/// \brief Nominal and data bit timing configuration.
typedef struct {
	/// \brief Bit Rate Prescaler tq (time quantum) = t_periphclk *
	/// (bitRatePrescaler + 1)
	uint16_t bitRatePrescaler;
	/// \brief (Re)Synchronization Jump Width = tq * (synchronizationJump +
	/// 1)
	uint8_t synchronizationJump;
	/// \brief Time Segment After Sample Point = tq *
	/// (timeSegmentAfterSamplePoint + 1)
	uint8_t timeSegmentAfterSamplePoint;
	/// \brief Time Segment Before Sample Point = tq *
	/// (timeSegmentBeforeSamplePoint + 1)
	uint8_t timeSegmentBeforeSamplePoint;
} Mcan_BitTiming;

/// \brief Transmitter Delay Compensation settings.
typedef struct {
	bool isEnabled; ///< Is Transmitter Delay Compensation enabled flag.
	uint8_t filter; ///< Transmitter Delay Compensation Filter.
	uint8_t offset; ///< Transmitter Delay Compensation Offset.
} Mcan_TransmitterDelayCompensation;

/// \brief Mcan timestamp clock source.
typedef enum {
	Mcan_TimestampClk_None = 0, ///< No clock, timestamp is always 0.
	Mcan_TimestampClk_Internal =
			1, ///< Internal Mcan clock with prescaler is used.
	Mcan_TimestampClk_External = 2, ///< Selected PCK is used.
} Mcan_TimestampClk;

/// \brief Mcan timeout type.
typedef enum {
	Mcan_TimeoutType_Continuous = 0,
	Mcan_TimeoutType_TxEventFifo = 1,
	Mcan_TimeoutType_RxFifo0 = 2,
	Mcan_TimeoutType_RxFifo1 = 3,
} Mcan_TimeoutType;

/// \brief The policy of handling non-matching frames.
typedef enum {
	Mcan_NonMatchingPolicy_RxFifo0 =
			0, ///< Put non-matching frames in RX FIFO 0.
	Mcan_NonMatchingPolicy_RxFifo1 =
			1, ///< Put non-matching frames in RX FIFO 1.
	Mcan_NonMatchingPolicy_Rejected = 2, ///< Reject non-matching frames.
} Mcan_NonMatchingPolicy;

/// \brief Id filtering policy used for standard and extended IDs filtering.
typedef struct {
	bool isIdRejected; ///< Reject all frames flag.
	/// \brief Non-matching frames handling policy.
	Mcan_NonMatchingPolicy nonMatchingPolicy;
	/// \brief Filter list address within message RAM.
	uint32_t *filterListAddress;
	uint8_t filterListSize; ///< Filter list elements count.
} Mcan_IdFilter;

/// \brief Rx FIFO or Rx Buffer element size.
typedef enum {
	Mcan_ElementSize_8 = 0, ///< 8-byte data field.
	Mcan_ElementSize_12 = 1, ///< 12-byte data field.
	Mcan_ElementSize_16 = 2, ///< 16-byte data field.
	Mcan_ElementSize_20 = 3, ///< 20-byte data field.
	Mcan_ElementSize_24 = 4, ///< 24-byte data field.
	Mcan_ElementSize_32 = 5, ///< 32-byte data field.
	Mcan_ElementSize_48 = 6, ///< 48-byte data field.
	Mcan_ElementSize_64 = 7, ///< 64-byte data field.
} Mcan_ElementSize;

/// \brief Rx FIFO operation mode.
typedef enum {
	Mcan_RxFifoOperationMode_Blocking =
			0, ///< No new message is added, when FIFO is full.
	Mcan_RxFifoOperationMode_Overwrite =
			1, ///< Oldest message is overwritten when FIFO is full.
} Mcan_RxFifoOperationMode;

/// \brief Rx FIFO settings.
typedef struct {
	bool isEnabled; ///< Rx FIFO enable flag.
	uint32_t *startAddress; ///< Rx FIFO start address within message RAM.
	uint8_t size; ///< Rx FIFO max elements count.
	/// \brief Rx FIFO watermark level for interrupt; 0 - disabled.
	uint8_t watermark;
	Mcan_RxFifoOperationMode mode; ///< Operation mode.
	Mcan_ElementSize elementSize; ///< The size of Rx FIFO element.
} Mcan_RxFifo;

/// \brief Rx Buffer settings.
typedef struct {
	uint32_t *startAddress; ///< Rx Buffer start address within message RAM.
	Mcan_ElementSize elementSize; ///< The size of Rx Buffer element.
} Mcan_RxBuffer;

/// \brief Tx Queue type.
typedef enum {
	Mcan_TxQueueType_Fifo = 0, ///< Tx FIFO.
	/// \brief Tx Id queue (lower Id means higher priority).
	Mcan_TxQueueType_Id = 1,
} Mcan_TxQueueType;

/// \brief Tx Buffer/FIFO/Queue settings.
typedef struct {
	bool isEnabled; ///< Tx Buffer/FIFO/Queue enable flag.
	uint32_t *startAddress; ///< Tx Buffer start address within message RAM.
	uint8_t bufferSize; ///< Number of Tx Buffer elements.
	uint8_t queueSize; ///< Number of Tx FIFO/Queue elements.
	Mcan_TxQueueType queueType; ///< Type of Tx Queue (FIFO or Id queue).
	/// \brief The size of Tx Buffer/FIFO/Queue element.
	Mcan_ElementSize elementSize;
} Mcan_TxBuffer;

/// \brief Tx Event FIFO settings.
typedef struct {
	bool isEnabled; ///< Tx Event FIFO enable flag.
	uint32_t *startAddress; ///< Tx Event FIFO start address within message RAM.
	uint8_t size; ///< Tx Event FIFO max elements count.
	uint8_t watermark; ///< Tx Event FIFO watermark level for interrupt; 0 - disabled.
} Mcan_TxEventFifo;

/// \brief MCAN interrupt sources.
typedef enum {
	Mcan_Interrupt_Rf0n = 0, ///< Receive FIFO 0 New Message.
	Mcan_Interrupt_Rf0w = 1, ///< Receive FIFO 0 Watermark Reached.
	Mcan_Interrupt_Rf0f = 2, ///< Receive FIFO 0 Full.
	Mcan_Interrupt_Rf0l = 3, ///< Receive FIFO 0 Message Lost.
	Mcan_Interrupt_Rf1n = 4, ///< Receive FIFO 1 New Message.
	Mcan_Interrupt_Rf1w = 5, ///< Receive FIFO 1 Watermark Reached.
	Mcan_Interrupt_Rf1f = 6, ///< Receive FIFO 1 Full.
	Mcan_Interrupt_Rf1l = 7, ///< Receive FIFO 1 Message Lost.
	Mcan_Interrupt_Hpm = 8, ///< High Priority Message.
	Mcan_Interrupt_Tc = 9, ///< Transmission Completed.
	Mcan_Interrupt_Tcf = 10, ///< Transmission Cancellation Finished.
	Mcan_Interrupt_Tfe = 11, ///< Tx FIFO Empty.
	Mcan_Interrupt_Tefn = 12, ///< Tx Event FIFO New Entry.
	Mcan_Interrupt_Tefw = 13, ///< Tx Event FIFO Watermark Reached.
	Mcan_Interrupt_Teff = 14, ///< Tx Event FIFO Full.
	Mcan_Interrupt_Tefl = 15, ///< Tx Event FIFO Element Lost.
	Mcan_Interrupt_Tsw = 16, ///< Timestamp Wraparound.
	Mcan_Interrupt_Mraf = 17, ///< Message RAM Access Failure.
	Mcan_Interrupt_Too = 18, ///< Timeout Occurred.
	Mcan_Interrupt_Drx =
			19, ///< Message stored to Dedicated Receive Buffer.
	Mcan_Interrupt_Reserved1 = 20, ///< Reserved, shall not be used.
	Mcan_Interrupt_Reserved2 = 21, ///< Reserved, shall not be used.
	Mcan_Interrupt_Elo = 22, ///< Error Logging Overflow.
	Mcan_Interrupt_Ep = 23, ///< Error Passive.
	Mcan_Interrupt_Ew = 24, ///< Warning Status.
	Mcan_Interrupt_Bo = 25, ///< Bus_Off Status.
	Mcan_Interrupt_Wdi = 26, ///< Watchdog Interrupt.
	Mcan_Interrupt_Pea = 27, ///< Protocol Error in Arbitration Phase.
	Mcan_Interrupt_Ped = 28, ///< Protocol Error in Data Phase.
	Mcan_Interrupt_Ara = 29, ///< Access to Reserved Address.
	Mcan_Interrupt_Count =
			30, ///< Number of interrupts including reserved indices.
} Mcan_Interrupt;

/// \brief Line connected to given MCAN interrupt.
typedef enum {
	Mcan_InterruptLine_0 = 0, ///< Interrupt Line 0.
	Mcan_InterruptLine_1 = 1, ///< Interrupt Line 1.
} Mcan_InterruptLine;

/// \brief MCAN interrupt configuration.
typedef struct {
	bool isEnabled; ///< Interrupt enable flag.
	Mcan_InterruptLine line; ///< Line connected to the interrupt.
} Mcan_InterruptConfig;

/// \brief MCAN interrupt status structure.
typedef struct {
	bool hasRf0nOccured; ///< Receive FIFO 0 New Message interrupt occured.
	bool hasRf0wOccured; ///< Receive FIFO 0 Watermark Reached interrupt occured.
	bool hasRf0fOccured; ///< Receive FIFO 0 Full interrupt occured.
	bool hasRf0lOccured; ///< Receive FIFO 0 Message Lost interrupt occured.
	bool hasRf1nOccured; ///< Receive FIFO 1 New Message interrupt occured.
	bool hasRf1wOccured; ///< Receive FIFO 1 Watermark Reached interrupt occured.
	bool hasRf1fOccured; ///< Receive FIFO 1 Full interrupt occured.
	bool hasRf1lOccured; ///< Receive FIFO 1 Message Lost interrupt occured.
	bool hasHpmOccured; ///< High Priority Message interrupt occured.
	bool hasTcOccured; ///< Transmission Completed interrupt occured.
	bool hasTcfOccured; ///< Transmission Cancellation Finished interrupt occured.
	bool hasTfeOccured; ///< Tx FIFO Empty interrupt occured.
	bool hasTefnOccured; ///< Tx Event FIFO New Entry interrupt occured.
	bool hasTefwOccured; ///< Tx Event FIFO Watermark Reached interrupt occured.
	bool hasTeffOccured; ///< Tx Event FIFO Full interrupt occured.
	bool hasTeflOccured; ///< Tx Event FIFO Element Lost interrupt occured.
	bool hasTswOccured; ///< Timestamp Wraparound interrupt occured.
	bool hasMrafOccured; ///< Message RAM Access Failure interrupt occured.
	bool hasTooOccured; ///< Timeout Occurred interrupt occured.
	bool hasDrxOccured; ///< Message stored to Dedicated Receive Buffer interrupt occured.
	bool hasEloOccured; ///< Error Logging Overflow interrupt occured.
	bool hasEpOccured; ///< Error Passive interrupt occured.
	bool hasEwOccured; ///< Warning Status interrupt occured.
	bool hasBoOccured; ///< Bus_Off Status interrupt occured.
	bool hasWdiOccured; ///< Watchdog Interrupt interrupt occured.
	bool hasPeaOccured; ///< Protocol Error in Arbitration Phase interrupt occured.
	bool hasPedOccured; ///< Protocol Error in Data Phase interrupt occured.
	bool hasAraOccured; ///< Access to Reserved Address interrupt occured.
} Mcan_InterruptStatus;

/// \brief Mcan configuration structure.
typedef struct {
	/// \brief Base address of the message ram; only the upper 16 bits are
	/// used
	///        which shall be constant for the whole message RAM area.
	uint32_t *msgRamBaseAddress;
	/// \brief Operation mode.
	Mcan_Mode mode;
	/// \brief CAN FD (without bit rate switching) enable flag.
	bool isFdEnabled;
	/// \brief Nominal bit timing settings.
	Mcan_BitTiming nominalBitTiming;
	/// \brief Data bit timing for CAN FD settings.
	Mcan_BitTiming dataBitTiming;
	/// \brief Transmitter Delay Compensation settings.
	Mcan_TransmitterDelayCompensation transmitterDelayCompensation;
	/// \brief The clock source used for timestamp generation.
	Mcan_TimestampClk timestampClk;
	/// \brief Timestamp and timeout clock prescaler; clock = selected clock
	/// / (timestampPrescaler + 1).
	uint8_t timestampTimeoutPrescaler;
	/// \brief Timeout counter enable flag.
	bool isTimeoutEnabled;
	/// \brief The type of timeout to be used (valid only if
	/// isTimeoutEnabled == true).
	Mcan_TimeoutType timeoutType;
	/// \brief Timeout period.
	uint16_t timeoutPeriod;
	/// \brief ID filtering settings for standard CAN IDs.
	Mcan_IdFilter standardIdFilter;
	/// \brief ID filtering settings for extended CAN IDs.
	Mcan_IdFilter extendedIdFilter;
	/// \brief Rx FIFO 0 settings.
	Mcan_RxFifo rxFifo0;
	/// \brief Rx FIFO 1 settings.
	Mcan_RxFifo rxFifo1;
	/// \brief Rx Buffer settings.
	Mcan_RxBuffer rxBuffer;
	/// \brief Tx Buffer settings.
	Mcan_TxBuffer txBuffer;
	/// \brief Tx Event FIFO settings.
	Mcan_TxEventFifo txEventFifo;
	/// \brief MCAN interrupts configuration.
	Mcan_InterruptConfig interrupts[Mcan_Interrupt_Count];
	/// \brief MCAN Line 0 interrupt enabled flag.
	bool isLine0InterruptEnabled;
	/// \brief MCAN Line 1 interrupt enabled flag.
	bool isLine1InterruptEnabled;
	/// \brief RAM watchdog counter value.
	uint8_t wdtCounter;
} Mcan_Config;

/// \brief CAN FD ESI flag value.
typedef enum {
	Mcan_ElementEsi_Dominant =
			0, ///< ESI depends only on error passive flag.
	Mcan_ElementEsi_Recessive = 1, ///< ESI is transmitted recessive.
} Mcan_ElementEsi;

/// \brief The type of CAN Id used in the element.
typedef enum {
	Mcan_IdType_Standard = 0, ///< Element contains CAN Standard Id.
	Mcan_IdType_Extended = 1, ///< Element contains CAN Extended Id.
} Mcan_IdType;

/// \brief The type of CAN Id used in the element.
typedef enum {
	Mcan_FrameType_Data = 0, ///< Frame contains data.
	Mcan_FrameType_Remote = 1, ///< Frame is a transmission request (RTR).
} Mcan_FrameType;

/// \brief The type of detected event.
typedef enum {
	Mcan_TxEventType_Tx = 1, ///< Element was sent.
	Mcan_TxEventType_Cancellation = 2, ///< Transmission was cancelled.
} Mcan_TxEventType;

/// \brief Mcan Tx element for Tx Buffer/FIFO/Queue.
typedef struct {
	Mcan_ElementEsi esiFlag; ///< CAN FD ESI flag value.
	Mcan_IdType idType; ///< The type of CAN Id.
	Mcan_FrameType frameType; ///< The type of frame.
	uint32_t id; ///< CAN Id - 11 or 29 bit (for Extended Frame type).
	uint8_t marker; ///< Message marker to be placed in the Tx Event FIFO.
	bool isTxEventStored; ///< Tx Event storage enable flag.
	bool isCanFdFormatEnabled; ///< CAN FD format enable flag.
	bool isBitRateSwitchingEnabled; ///< Bit rate switching enable flag.
	/// \brief Number of data bytes set in DLC field.
	///        Allowed values:
	///        Standard Id CAN: 0-8,
	///        Extended Id CAN: 0-8, 12, 16, 20, 24, 32, 48, 64.
	uint8_t dataSize;
	uint8_t *data; ///< Data pointer.
	bool isInterruptEnabled; ///< Enable interrupt after transmission complete.
} Mcan_TxElement;

/// \brief Mcan Tx element for Tx Event FIFO.
typedef struct {
	Mcan_ElementEsi esiFlag; ///< CAN FD ESI flag value.
	Mcan_IdType idType; ///< The type of CAN Id.
	Mcan_FrameType frameType; ///< The type of frame.
	uint32_t id; ///< CAN Id - 11 or 29 bit (for Extended Frame type).
	uint8_t marker; ///< Message marker to be placed in the Tx Event FIFO.
	Mcan_TxEventType eventType; ///< Tx Event type.
	bool isCanFdFormatEnabled; ///< CAN FD format enable flag.
	bool isBitRateSwitchingEnabled; ///< Bit rate switching enable flag.
	/// \brief Number of data bytes set in DLC field.
	///        Allowed values:
	///        Standard Id CAN: 0-8,
	///        Extended Id CAN: 0-8, 12, 16, 20, 24, 32, 48, 64.
	uint8_t dataSize;
	uint16_t timestamp; ///< Frame timestamp.
} Mcan_TxEventElement;

/// \brief Mcan Rx element for Rx Buffer/FIFO.
typedef struct {
	Mcan_ElementEsi esiFlag; ///< CAN FD ESI flag value.
	Mcan_IdType idType; ///< The type of CAN Id.
	Mcan_FrameType frameType; ///< The type of frame.
	uint32_t id; ///< CAN Id - 11 or 29 bit (for Extended Frame type).
	bool isNonMatchingFrame; ///< Rx frame did not match any filter.
	uint8_t filterIndex; ///< Matching filter index (valid if !nonMatchingFrame).
	bool isCanFdFormatEnabled; ///< CAN FD format enable flag.
	bool isBitRateSwitchingEnabled; ///< Bit rate switching enable flag.
	uint16_t timestamp; ///< Frame timestamp.
	/// \brief Number of data bytes set in DLC field.
	///        Allowed values:
	///        Standard Id CAN: 0-8,
	///        Extended Id CAN: 0-8, 12, 16, 20, 24, 32, 48, 64.
	uint8_t dataSize;
	uint8_t *data; ///< Data pointer.
} Mcan_RxElement;

/// \brief The type of Rx filter.
typedef enum {
	Mcan_RxFilterType_Range = 0, ///< Range filter; id1 <= id <= id2.
	Mcan_RxFilterType_Dual = 1, ///< Dual filter; id == id1 || id == id2.
	Mcan_RxFilterType_Mask = 2, ///< Mask filter; id & id2 == id1.
} Mcan_RxFilterType;

/// \brief The type of Rx filter.
typedef enum {
	Mcan_RxFilterConfig_Disabled = 0, ///< Disable filter element.
	Mcan_RxFilterConfig_RxFifo0 =
			1, ///< Store in Rx FIFO 0 if filter matches.
	Mcan_RxFilterConfig_RxFifo1 =
			2, ///< Store in Rx FIFO 1 if filter matches.
	Mcan_RxFilterConfig_Reject = 3, ///< Reject ID if filter matches.
	Mcan_RxFilterConfig_Priority = 4, ///< Set priority if filter matches.
	Mcan_RxFilterConfig_PriorityRxFifo0 =
			5, ///< Set priority and store in FIFO 0 if filter matches.
	Mcan_RxFilterConfig_PriorityRxFifo1 =
			6, ///< Set priority and store in FIFO 1 if filter matches,
	Mcan_RxFilterConfig_RxBuffer =
			7, ///< Store into Rx Buffer or as debug message, filter type (SFT[1:0]) ignored, id == id1.
} Mcan_RxFilterConfig;

/// \brief Rx filter element.
typedef struct {
	Mcan_RxFilterType type; ///< The type of configured filter.
	Mcan_RxFilterConfig config; ///< Filter behaviour configuration.
	uint32_t id1; ///< Filter Id1; bits [10:0] for standard Id, [28:0] for extended Id.
	uint32_t id2; ///< Filter Id2; bits [10:0] for standard Id, [28:0] for extended Id.
} Mcan_RxFilterElement;

/// \brief Mcan device descriptor.
typedef struct {
	Mcan_Id id; ///< MCAN core identifier.
	uint32_t *msgRamBaseAddress; ///< Base address of the message ram.
	volatile Mcan_Registers *reg; ///< MCAN core registers.
	uint32_t *txBufferAddress; ///< Address (32-bit) of the Tx Buffer within message RAM.
	uint8_t txBufferSize; ///< Size (number of 32-bit words) of the Tx Buffer.
	uint32_t *txQueueAddress; ///< Address (32-bit) of the Tx Queue within message RAM.
	uint8_t txQueueSize; ///< Size (number of 32-bit words) of the Tx Queue.
	uint8_t txElementSize; ///< Size of the data field (in bytes) of the Tx Element.
	uint32_t *rxBufferAddress; ///< Address (32-bit) of the Rx Buffer within message RAM.
	uint8_t rxBufferElementSize; ///< Size of the data field (in bytes) of the Tx Element.
	uint32_t *rxFifo0Address; ///< Address (32-bit) of the Rx FIFO0 within message RAM.
	uint8_t rxFifo0Size; ///< Size (number of 32-bit words) of the Rx FIFO0.
	uint8_t rxFifo0ElementSize; ///< Size of the data field (in bytes) of the Rx Element in FIFO0.
	uint32_t *rxFifo1Address; ///< Address (32-bit) of the Rx FIFO1 within message RAM.
	uint8_t rxFifo1Size; ///< Size (number of 32-bit words) of the Rx FIFO1.
	uint8_t rxFifo1ElementSize; ///< Size of the data field (in bytes) of the Rx Element in FIFO1.
	uint32_t *txEventFifoAddress; ///< Address (32-bit) of the Tx Event Fifo within message RAM.
	uint8_t txEventFifoSize; ///< Size (number of 32-bit words) of the Tx Event Fifo.
	uint32_t *rxStdFilterAddress; ///< Address (32-bit) of the Standard Id filter within message RAM.
	uint8_t rxStdFilterSize; ///< Size (number of 32-bit words) of the Standard Id filter.
	uint32_t *rxExtFilterAddress; ///< Address (32-bit) of the Extended Id filter within message RAM.
	uint8_t rxExtFilterSize; ///< Size (number of 32-bit words) of the Extended Id filter.
} Mcan;

/// \brief Initializes a device descriptor for Mcan.
/// \param [in] id Mcan device identifier.
/// \param [out] mcan Mcan device descriptor.
void Mcan_init(const Mcan_Id id, Mcan *const mcan);

/// \brief Configures an Mcan device based on a configuration descriptor.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] config A configuration descriptor.
/// \param [in] timeoutLimit Timeout counter value for the configuration process.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Configuration was successful.
/// \retval false Configuration failed.
bool Mcan_setConfig(Mcan *const mcan, const Mcan_Config *const config,
		uint32_t const timeoutLimit, int *const errCode);

/// \brief Reads the current configuration of the Mcan device.
/// \param [in] mcan Mcan device descriptor.
/// \param [out] config A configuration descriptor.
void Mcan_getConfig(const Mcan *const mcan, Mcan_Config *const config);

/// \brief Adds a new element to the Tx Buffer and initializes its transmission.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] element Tx element to send.
/// \param [in] index Tx Buffer index.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Adding element was successful.
/// \retval false Adding element failed.
bool Mcan_txBufferAdd(Mcan *const mcan, const Mcan_TxElement element,
		const uint8_t index, int *const errCode);

/// \brief Adds a new element to the Tx Queue and initializes its transmission.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] element Tx element to send.
/// \param [out] index Memory buffer index at which the element was added.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Adding element was successful.
/// \retval false Adding element failed.
bool Mcan_txQueuePush(Mcan *const mcan, const Mcan_TxElement element,
		uint8_t *const index, int *const errCode);

/// \brief Checks whether the specified Tx Buffer or Queue element was sent.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] index Queried element index.
/// \retval true Element was sent.
/// \retval false Element was not sent.
bool Mcan_txBufferIsTransmissionFinished(
		const Mcan *const mcan, const uint8_t index);

/// \brief Pulls element the Tx Event Queue.
/// \param [in] mcan Mcan device descriptor.
/// \param [out] element Tx Event element pointer.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Pulling element was successful.
/// \retval false Pulling element failed.
bool Mcan_txEventFifoPull(Mcan *const mcan, Mcan_TxEventElement *const element,
		int *const errCode);

/// \brief Receives element from the Rx Buffer.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] index Index of the Rx element to obtain.
/// \param [out] element Rx element pointer.
void Mcan_rxBufferGet(Mcan *const mcan, const uint8_t index,
		Mcan_RxElement *const element);

/// \brief Pulls element the Rx Fifo.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] id The id of the Rx Fifo.
/// \param [out] element Rx element pointer.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Pulling element was successful.
/// \retval false Pulling element failed.
bool Mcan_rxFifoPull(Mcan *const mcan, const Mcan_RxFifoId id,
		Mcan_RxElement *const element, int *const errCode);

/// \brief Reads the status of the Rx Fifo.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] id The id of the Rx Fifo.
/// \param [out] status Rx Fifo status pointer.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Obtaining RxFifo status was successful.
/// \retval false Obtaining RxFifo status failed.
bool Mcan_getRxFifoStatus(const Mcan *const mcan, const Mcan_RxFifoId id,
		Mcan_RxFifoStatus *const status, int *const errCode);

/// \brief Reads the status of the Tx Queue.
/// \param [in] mcan Mcan device descriptor.
/// \param [out] status Tx Queue status pointer.
void Mcan_getTxQueueStatus(
		const Mcan *const mcan, Mcan_TxQueueStatus *const status);

/// \brief Reads the status of the Tx Event Fifo.
/// \param [in] mcan Mcan device descriptor.
/// \param [out] status Tx Event Fifo status pointer.
void Mcan_getTxEventFifoStatus(
		const Mcan *const mcan, Mcan_TxEventFifoStatus *const status);

/// \brief Set the Rx filter for standard CAN Id.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] element Rx filter element.
/// \param [in] index Rx filter list index.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Setting standard ID filter was successful.
/// \retval false Setting standard ID filter failed.
bool Mcan_setStandardIdFilter(Mcan *const mcan,
		const Mcan_RxFilterElement element, const uint8_t index,
		int *const errCode);

/// \brief Set the Rx filter for extended CAN Id.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] element Rx filter element.
/// \param [in] index Rx filter list index.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Setting extended ID filter was successful.
/// \retval false Setting extended ID filter failed.
bool Mcan_setExtendedIdFilter(Mcan *const mcan,
		const Mcan_RxFilterElement element, const uint8_t index,
		int *const errCode);

/// \brief Reads the information about active interrupts.
/// \param [in] mcan Mcan device descriptor.
/// \param [in] status Interrupt status pointer.
void Mcan_getInterruptStatus(
		const Mcan *const mcan, Mcan_InterruptStatus *const status);

/// \brief Resets the timeout counter value when in Continuous mode.
/// \param [in] mcan Mcan device descriptor.
static inline void
Mcan_resetTimeoutCounter(Mcan *const mcan)
{
	mcan->reg->tocv = 0;
}

/// \brief Returns true if the Tx Queue is empty
/// \param [in] mcan Mcan device descriptor.
/// \retval true Hardware transmission FIFO is empty.
/// \retval false Hardware transmission FIFO is not emppty.
bool Mcan_isTxFifoEmpty(const Mcan *const mcan);

#endif // BSP_MCAN_H

/** @} */

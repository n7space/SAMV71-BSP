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

#include "ByteFifo.h"

#include <assert.h>

void
ByteFifo_init(ByteFifo *const fifo, uint8_t *const memoryBlock,
		const size_t memoryBlockSize)
{
	assert(memoryBlockSize > 0u);

	fifo->begin = memoryBlock;
	fifo->end = memoryBlock + memoryBlockSize;
	fifo->first = NULL;
	fifo->last = memoryBlock;
}

void
ByteFifo_initFromBytes(ByteFifo *const fifo, uint8_t *const memoryBlock,
		const size_t memoryBlockSize)
{
	assert(memoryBlockSize > 0u);

	fifo->begin = memoryBlock;
	fifo->end = memoryBlock + memoryBlockSize;
	fifo->first = memoryBlock;
	fifo->last = memoryBlock;
}

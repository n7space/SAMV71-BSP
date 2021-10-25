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

/// \file  Utils.h

/**
 * @defgroup Utils Utils
 * @{
 */

#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// \file  Utils.h
/// \brief Module containing general utility library functions.

/// \brief Boolean lambda prototype.
/// \returns Evaluated value.
typedef bool (*BooleanLambda)(void);

/// \brief Boolean lambda prototype with arg.
/// \returns Evaluated value.
typedef bool (*BooleanArgLambda)(void *);

/// \brief Simplifies writing functions returning boolean and providing optional
/// error code. \details Example: \code bool fun(int* const errCode)
/// {
///   ...
///   if (errorCondition)
///     return returnError(errCode, ERROR_CONDITION_CODE);
///   ...
/// }
/// \endcode
/// \param [out] errCode pointer to store error code to, can be \c NULL.
/// \param [in] returnedError error code to be set.
/// \returns always \b false
static inline bool
returnError(int *const errCode, const int returnedError)
{
	if (errCode != NULL)
		*errCode = returnedError;
	return false;
}

/// \brief Continuously evaluates a boolean lambda until either the evaluation
/// yields true or
///        a timeout occurrs.
/// \param [in] lambda Lambda to be evaluated.
/// \param [in] timeout Timeout value.
/// \returns Whether the lambda evaluated to true before timeout occurred.
static inline bool
evaluateLambdaWithTimeout(const BooleanLambda lambda, const uint32_t timeout)
{
	for (volatile uint32_t timeoutCounter = timeout; timeoutCounter > 0u;
			--timeoutCounter) {
		if (lambda())
			return true;
	}
	return false;
}

/// \brief Continuously evaluates a boolean lambda until either the evaluation
/// yields true or
///        a timeout occurrs.
/// \param [in] lambda Lambda to be evaluated.
/// \param [in] arg Argument for lambda.
/// \param [in] timeout Timeout value.
/// \returns Whether the lambda evaluated to true before timeout occurred.
static inline bool
evaluateArgLambdaWithTimeout(const BooleanArgLambda lambda, void *arg,
		const uint32_t timeout)
{
	for (volatile uint32_t timeoutCounter = timeout; timeoutCounter > 0u;
			--timeoutCounter) {
		if (lambda(arg))
			return true;
	}
	return false;
}

/// \brief Checks whether a number is between two other numbers, excluding the
/// other numbers. \param [in] x Number to be compared. \param [in] lowerBound
/// Lower bound of the range. \param [in] upperBound Upper bound of the range.
/// \returns Whether the compared number is within the defined range, excluding
/// the range extremes.
static inline bool
isBetweenUint32(const uint32_t x, const uint32_t lowerBound,
		const uint32_t upperBound)
{
	return (x > lowerBound) && (x < upperBound);
}

#endif // UTILS_UTILS_H

/** @} */

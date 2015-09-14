/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Author: Johannes Hein <support@os-cillation.de>
 *
 *  easyFPGA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  easyFPGA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with easyFPGA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SDK_COMMUNICATION_TYPES_H_
#define SDK_COMMUNICATION_TYPES_H_

/**
 * \file src/communication/types.h
 *
 * \brief Type definitions for the communication machinery
 */

#include <cstdint> /* special ints */

/**
 * \brief Type for holding the timeout value of a task.
 */
typedef int32_t timeoutval;

/**
 * \brief Type for holding the id of an currently processed task.
 */
typedef uint8_t idval;

/**
 * \brief A variable type for identifying each task executed.
 */
typedef uint64_t tasknumberval;

/**
 * \brief Number of retries type.
 */
typedef int32_t retryval;

#endif  // SDK_COMMUNICATION_TYPES_H_

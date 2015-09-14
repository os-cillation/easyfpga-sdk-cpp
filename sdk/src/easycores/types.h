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

#ifndef SDK_EASYCORES_TYPES_H_
#define SDK_EASYCORES_TYPES_H_

/**
 * \file src/easycores/types.h
 *
 * \brief Type definitions the easycore data model
 */

#include <cstdint> /* int16_t, int32_t */
#include <cstddef> /* NULL */

static const uint16_t MAX_GLOBAL_PIN_COUNT = 100;
static const uint16_t MAX_GLOBAL_REGISTER_COUNT = 100;

static const uint16_t MAX_GLOBAL_OFFSET = MAX_GLOBAL_PIN_COUNT + MAX_GLOBAL_REGISTER_COUNT;

/**
 * \brief Core identification type (Used for different purposes.)
 *
 * Valid values / instantiation with: Integers in [-1, 255]<br>
 * - [-1, 0]: special core indices @see SPECIAL_CORE_INDICES<br>
 * - [1, 255]: normal core indices
 */
typedef int16_t CoreIndex;

/**
 * \brief Special core indices in the range of type CoreIndex
 */
enum SPECIAL_CORE_INDICES : CoreIndex {
    NO_FPGA_ASSOCIATION = -1,
    GPIOPIN_CORE_ASSOCIATION = 0
};

/**
 * \brief Pin identification type
 *
 * Valid values / instantiation with: pin constants from the respective core
 */
typedef int32_t PinConst;

/**
 * \brief Predefined pin direction types.
 */
enum PIN_DIRECTION_TYPE : uint32_t {
    UNDEFINED,
    IN,
    OUT,
    INOUT
};

/**
 * \brief Register identification type
 *
 * Valid values / instantiation with: register constants from the respective core
 */
typedef int32_t RegisterConst;

/**
 * \brief Predefined register access types
 */
enum REGISTER_ACCESS_TYPE : uint32_t {
    READONLY,
    WRITEONLY,
    READWRITE
};

/**
 * \brief Possible values for the communication's operation mode
 */
enum OPERATION_MODE : uint8_t {
    SYNC,
    ASYNC
};

/**
 * \brief Interrupt identification type
 *
 * Valid values / instantiation with: interrupt constants from the
 * respective core
 */
typedef int32_t InterruptConst;

/**
 * \brief The function prototype for interrupt service routines
 *
 * If any core triggered an interrupt, this type a function pointer will
 * be invoked by the framework.
 */
typedef void (*interrupt_callback_function) (void);

#include <list>
#include <map>
#include <string>

/**
 * \brief Type for the hdl generic map
 */
typedef std::map<PinConst, std::list<std::string>> generic_hdl_map;

#endif  // SDK_EASYCORES_TYPES_H_

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

#ifndef SDK_EASYCORES_CAN_UTILS_TYPES_H_
#define SDK_EASYCORES_CAN_UTILS_TYPES_H_

/**
 * \file src/easycores/can/utils/types.h
 *
 * \brief Defines the supported CAN frame format message types
 */

#include <cstdint> /* int8_t */

/**
 * \brief THe CAN frame format message type
 */
enum CAN_MESSAGE_FORMAT_TYPE : int8_t {
    STANDARD_FRAME,
    EXTENDED_FRAME
};

#endif  // SDK_EASYCORES_CAN_UTILS_TYPES_H_

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

#ifndef SDK_GENERATOR_TYPES_H_
#define SDK_GENERATOR_TYPES_H_

/**
 * \file src/generator/types.h
 *
 * \brief Type definitions for the generation process
 */

#include <cstdint> /* int16_t */

/**
 * \brief Type for a vhdl custom signal id / index number.
 */
typedef int16_t CustomSignalIndex;

/**
 * \brief Possible return values of the hdl generation for runtime optimization.
 */
enum HDL_GENERATION_STATUS : uint32_t {
    NEW_BUILD_NECCESSARY,
    NO_NEW_BUILD_NECCESSARY,
    ERRORS_AT_STRUCTURE_DESCRIPTION
};

#endif  // SDK_GENERATOR_TYPES_H_

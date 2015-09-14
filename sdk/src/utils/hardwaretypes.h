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

#ifndef SDK_UTILS_HARDWARETYPES_H_
#define SDK_UTILS_HARDWARETYPES_H_

/**
 * \file src/utils/hardwaretypes.h
 *
 * \brief Provide important type definitions for hardware programming
 */

/**
 * \brief An 8-bit wide variable type with the required semantics of byte operations.
 */
typedef unsigned char byte;

/**
 * \brief An own abstraction of signal states.
 */
typedef bool LogicLevel;

/**
 * \brief Means "high level" on line or "1"
 */
static const LogicLevel HIGH = true;

/**
 * \brief Means "low level" on line or "0"
 */
static const LogicLevel LOW = false;

#include <stdint.h>

/**
 * \brief A bit manipulator: sets the bit bitNumber of a byte pointed by bytePointer.
 */
static inline void setBit(byte* const bytePointer, uint8_t bitNumber) {
    *(bytePointer) |= (1 << bitNumber);
}

/**
 * \brief A bit manipulator: sets the bit bitNumber of a referenced byte.
 */
static inline void setBit(byte &byteReference, uint8_t bitNumber) {
    byteReference |= (1 << bitNumber);
}

/**
 * \brief Tests if the nth bit of byte bytePointer is set.
 */
static inline bool setBitTest(byte* const bytePointer, uint8_t bitNumber) {
    return ((*bytePointer) & (1 << (bitNumber)));
}

/**
 * \brief Tests if the nth bit of a referenced byte is set.
 */
static inline bool setBitTest(byte byteReference, uint8_t bitNumber) {
    return (byteReference & (1 << (bitNumber)));
}

/**
 * \brief A bit manipulator: clears the bit bitNumber of a byte pointed by bytePointer.
 */
static inline void clrBit(byte* const bytePointer, uint8_t bitNumber) {
    *(bytePointer) &= ~(1 << (bitNumber));
}

/**
 * \brief A bit manipulator: clears the bit bitNumber of a referenced byte.
 */
static inline void clrBit(byte &byteReference, uint8_t bitNumber) {
    byteReference &= ~(1 << (bitNumber));
}

#endif  // SDK_UTILS_HARDWARETYPES_H_

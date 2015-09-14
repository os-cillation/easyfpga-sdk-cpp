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

#include "communication/protocol/calculator.h"

byte Calculator::calculateXorParity(byte* byteArray, uint32_t byteArrayLength)
{
    byte checksum = (byte)0x00;

    uint32_t i;
    for (i=0; i<byteArrayLength; i++) {
        checksum ^= *(byteArray+i);
    }

    return checksum;
}

uint32_t Calculator::calculateAdler32Hash(byte* byteArray, uint32_t byteArrayLength)
{
    uint32_t a = 1, b = 0;
    uint32_t index;

    /* Process each byte of the data in order */
    for (index = 0; index < byteArrayLength; ++index)
    {
        a = (a + byteArray[index]) % _MOD_ADLER;
        b = (b + a) % _MOD_ADLER;
    }

    return (b << 16) | a;
}

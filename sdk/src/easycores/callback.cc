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

#include "easycores/callback.h"

Callback::Callback(uint32_t bufferSize) :
    _bufferSize(bufferSize)
{
    _byteRead = new byte[bufferSize];

    for (uint32_t j=0; j<bufferSize; j++) {
        _byteRead[j] = (byte)0x00;
    }
}

Callback::~Callback()
{
    delete[] _byteRead;
    _byteRead = NULL;
}

uint32_t Callback::getBufferSize(void)
{
    return _bufferSize;
}

byte* Callback::getBuffer(void)
{
    return _byteRead;
}

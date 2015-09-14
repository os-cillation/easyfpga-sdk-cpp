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

#include "configuration.h" /* assert(1) */
#include "communication/protocol/frame.h"

#include <cstring> /* memcpy() */

Frame::Frame(byte* frameData, uint16_t totalFrameLength)
{
    assert(totalFrameLength >= 1);

    _data = new byte[totalFrameLength];
    _frameLength = totalFrameLength;

    memcpy(_data, frameData, _frameLength);
}

Frame::~Frame()
{
    delete[] _data;
    _data = NULL;
}

byte Frame::getOperationCode(void)
{
    return *(_data);
}

idval Frame::getId(void)
{
    if (_frameLength > 2) {
        return (idval)*(_data+1);
    }
    else {
        return 0;
    }
}

uint16_t Frame::getTotalFrameLength(void)
{
    return _frameLength;
}

void Frame::getFrameRawData(byte* destByteArray)
{
    /**
     * \todo Safety? (Copies the given data to the pointers memory)
     */
    memcpy(destByteArray, _data, _frameLength);
}

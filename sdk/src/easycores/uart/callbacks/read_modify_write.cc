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

#include "easycores/register.h"
#include "easycores/uart/callbacks/read_modify_write.h"
#include "easycores/uart/uart.h"

UartModifiedWriteCallback::UartModifiedWriteCallback(Uart* core, RegisterConst reg, std::list<std::pair<uint8_t, LogicLevel>> pinList) :
    Callback(1),
    _core(core),
    _reg(reg),
    _pinList(pinList)
{
}

UartModifiedWriteCallback::~UartModifiedWriteCallback()
{
}

bool UartModifiedWriteCallback::call(void)
{
    byte modifiedByte = *(_byteRead);

    for (auto it=_pinList.begin(); it!=_pinList.end(); ++it) {
        if (it->second) {
            setBit(modifiedByte, it->first);
        }
        else {
            clrBit(modifiedByte, it->first);
        }
    }

    return _core->getRegister(_reg)->writeAsync(modifiedByte);
}

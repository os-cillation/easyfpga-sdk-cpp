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
#include "easycores/uart/callbacks/set_baudrate_divisor.h"
#include "easycores/uart/uart.h"

UartSetBaudrateDivisorCallback::UartSetBaudrateDivisorCallback(Uart* core, byte lowByte, byte highByte) :
    Callback(1),
    _core(core),
    _lowByte(lowByte),
    _highByte(highByte)
{
}

UartSetBaudrateDivisorCallback::~UartSetBaudrateDivisorCallback()
{
}

bool UartSetBaudrateDivisorCallback::call(void)
{
    /* Set divisor latch access bit in order to set the baudrate */
    setBit(_byteRead, 7);
    bool success = _core->getRegister(Uart::REGISTER::LCR)->writeAsync(*_byteRead);

    /* Write out low and high byte of the baudrate divisor */
    success &= _core->getRegister(Uart::REGISTER::DLL)->writeAsync(_lowByte);
    success &= _core->getRegister(Uart::REGISTER::DLM)->writeAsync(_highByte);

    /* Reset divisor latch access bit in order to access the buffer registers */
    clrBit(_byteRead, 7);
    success &= _core->getRegister(Uart::REGISTER::LCR)->writeAsync(*_byteRead);

    return success;
}

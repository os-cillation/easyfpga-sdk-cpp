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

#include "easycores/uart/callbacks/interrupt_identification.h"
#include "easycores/uart/uart.h"

UartInterruptIdentificationCallback::UartInterruptIdentificationCallback(Uart::INTERRUPT* target) :
    Callback(1),
    _target(target)
{
}

UartInterruptIdentificationCallback::~UartInterruptIdentificationCallback()
{
}

bool UartInterruptIdentificationCallback::call(void)
{
    if ((*(_byteRead) & 0x0F) == (byte)0x06) {
        *(_target) = Uart::INTERRUPT::RX_LINE_STATUS;
    }
    else if ((*(_byteRead) & 0x0F) == (byte)0x04) {
        *(_target) = Uart::INTERRUPT::RX_AVAILABLE;
    }
    else if ((*(_byteRead) & 0x0F) == (byte)0x0C) {
        *(_target) = Uart::INTERRUPT::CHARACTER_TIMEOUT;
    }
    else if ((*(_byteRead) & 0x0F) == (byte)0x02) {
        *(_target) = Uart::INTERRUPT::TX_EMPTY;
    }
    else if ((*(_byteRead) & 0x0F) == (byte)0x00) {
        *(_target) = Uart::INTERRUPT::MODEM_STATUS;
    }
    else {
        return false;
    }

    return true;
}

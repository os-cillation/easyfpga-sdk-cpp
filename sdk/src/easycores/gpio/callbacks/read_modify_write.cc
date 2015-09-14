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

#include "easycores/gpio/callbacks/read_modify_write.h"
#include "easycores/gpio/gpio8.h"

Gpio8ModifiedWriteCallback::Gpio8ModifiedWriteCallback(Gpio8* core, PinConst pin, LogicLevel level) :
    Callback(1),
    _core(core),
    _pin(pin),
    _level(level)
{
}

Gpio8ModifiedWriteCallback::~Gpio8ModifiedWriteCallback()
{
}

bool Gpio8ModifiedWriteCallback::call(void)
{
    uint8_t digit;
    switch (_pin) {
        case Gpio8::PIN::GPIO1:
            digit = 1;
            break;

        case Gpio8::PIN::GPIO2:
            digit = 2;
            break;

        case Gpio8::PIN::GPIO3:
            digit = 3;
            break;

        case Gpio8::PIN::GPIO4:
            digit = 4;
            break;

        case Gpio8::PIN::GPIO5:
            digit = 5;
            break;

        case Gpio8::PIN::GPIO6:
            digit = 6;
            break;

        case Gpio8::PIN::GPIO7:
            digit = 7;
            break;

        default:
            digit = 0;
    }

    byte modifiedByte;
    if (_level) {
        /* Set Pin in the read byte. */
        modifiedByte = *(_byteRead) | (1 << digit);
    }
    else {
        /* Clear Pin in the read byte. */
        modifiedByte = *(_byteRead) & ~(1 << digit);
    }

    return _core->setAllPins(modifiedByte);
}

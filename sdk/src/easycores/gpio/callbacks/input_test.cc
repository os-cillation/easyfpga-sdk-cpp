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

#include "easycores/gpio/callbacks/input_test.h"
#include "easycores/gpio/gpio8.h"

Gpio8InputTestCallback::Gpio8InputTestCallback(PinConst pin, bool all, bool* resultTarget) :
    Callback(1),
    _pin(pin),
    _all(all),
    _resultTarget(resultTarget)
{
}

Gpio8InputTestCallback::~Gpio8InputTestCallback()
{
}

bool Gpio8InputTestCallback::call(void)
{
    if (_all) {
        *(_resultTarget) = (*(_byteRead) == (byte)0x00);
    }
    else {
        switch (_pin) {
            case Gpio8::PIN::GPIO0:
                *(_resultTarget) = setBitTest(_byteRead, 1);
                break;

            case Gpio8::PIN::GPIO1:
                *(_resultTarget) = setBitTest(_byteRead, 2);
                break;

            case Gpio8::PIN::GPIO2:
                *(_resultTarget) = setBitTest(_byteRead, 3);
                break;

            case Gpio8::PIN::GPIO3:
                *(_resultTarget) = setBitTest(_byteRead, 4);
                break;

            case Gpio8::PIN::GPIO4:
                *(_resultTarget) = setBitTest(_byteRead, 5);
                break;

            case Gpio8::PIN::GPIO5:
                *(_resultTarget) = setBitTest(_byteRead, 6);
                break;

            case Gpio8::PIN::GPIO6:
                *(_resultTarget) = setBitTest(_byteRead, 7);
                break;

            case Gpio8::PIN::GPIO7:
                *(_resultTarget) = setBitTest(_byteRead, 8);
                break;

            default:
                return false;
        }
    }

    return true;
}

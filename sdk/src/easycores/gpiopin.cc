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

#include "easycores/gpiopin.h"

GpioPin::GpioPin(
    std::string name,
    CoreIndex* core,
    PinConst pinNumber,
    PIN_DIRECTION_TYPE type,
    uint8_t bank,
    uint8_t pin) :
    Pin(name, core, pinNumber, type),
    _bank(bank),
    _pin(pin)
{
}

GpioPin::GpioPin(const GpioPin& gpioPin) :
    Pin(gpioPin._name, gpioPin._coreIndex, gpioPin._pinNumber, gpioPin._type),
    _bank(gpioPin._bank),
    _pin(gpioPin._pin)
{
    this->_isBackwardConnected = gpioPin._isBackwardConnected;
    this->_backwardConnection = gpioPin._backwardConnection;
}

GpioPin::~GpioPin()
{
}

uint8_t GpioPin::getBankNumber(void)
{
    return _bank;
}

uint8_t GpioPin::getPinNumber(void)
{
    return _pin;
}

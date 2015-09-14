/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Authors: Johannes Hein <support@os-cillation.de>
 *           Simon Gansen
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

#include "Gpio8AsyncTestFpga.h"

#include "easyfpga/easycores/gpio/gpio8.h"

Gpio8AsyncTestFpga::Gpio8AsyncTestFpga() :
    _gpioCore(std::make_shared<Gpio8>())
{
}

Gpio8AsyncTestFpga::~Gpio8AsyncTestFpga()
{
}

void Gpio8AsyncTestFpga::defineStructure(void)
{
    this->addEasyCore(_gpioCore);

    this->connect(_gpioCore, Gpio8::PIN::GPIO0, GPIOPIN::BANK0_PIN0);
    this->connect(_gpioCore, Gpio8::PIN::GPIO1, GPIOPIN::BANK0_PIN1);
    this->connect(_gpioCore, Gpio8::PIN::GPIO2, GPIOPIN::BANK0_PIN2);
    this->connect(_gpioCore, Gpio8::PIN::GPIO3, GPIOPIN::BANK0_PIN3);
    this->connect(_gpioCore, Gpio8::PIN::GPIO4, GPIOPIN::BANK0_PIN4);
    this->connect(_gpioCore, Gpio8::PIN::GPIO5, GPIOPIN::BANK0_PIN5);
    this->connect(_gpioCore, Gpio8::PIN::GPIO6, GPIOPIN::BANK0_PIN6);
    this->connect(_gpioCore, Gpio8::PIN::GPIO7, GPIOPIN::BANK0_PIN8);
}

gpio8_ptr Gpio8AsyncTestFpga::getGpioCore(void)
{
    return _gpioCore;
}

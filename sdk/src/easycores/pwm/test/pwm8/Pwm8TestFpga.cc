/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Authors: Johannes Hein <support@os-cillation.de>
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

#include "easyfpga/easycores/pwm/pwm8.h"
#include "easyfpga/easycores/pwm/test/pwm8/Pwm8TestFpga.h"

Pwm8TestFpga::Pwm8TestFpga() :
    _pwm1(std::make_shared<Pwm8>())
{
}

Pwm8TestFpga::~Pwm8TestFpga()
{
}

void Pwm8TestFpga::defineStructure(void)
{
    this->addEasyCore(_pwm1);
    this->connect(_pwm1, Pwm8::PIN::PWM_OUT, EasyFpga::GPIOPIN::BANK1_PIN1);
}

pwm8_ptr Pwm8TestFpga::getPwm(void)
{
    return _pwm1;
}

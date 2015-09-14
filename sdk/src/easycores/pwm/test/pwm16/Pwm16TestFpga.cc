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

#include "easyfpga/easycores/pwm/pwm16.h"
#include "easyfpga/easycores/pwm/test/pwm16/Pwm16TestFpga.h"

Pwm16TestFpga::Pwm16TestFpga()
{
    for (uint8_t i=0; i<_PWM_COUNT; i++) {
        _pwmArray[i] = std::make_shared<Pwm16>();
    }
}

Pwm16TestFpga::~Pwm16TestFpga()
{
}

void Pwm16TestFpga::defineStructure(void)
{
    PinConst gpioPin = EasyFpga::GPIOPIN::BANK0_PIN0;

    for (uint8_t i=0; i<_PWM_COUNT; i++) {
        this->addEasyCore(_pwmArray[i]);
        this->connect(_pwmArray[i], Pwm16::PIN::PWM_OUT, gpioPin++);
    }
}

pwm16_ptr Pwm16TestFpga::getPwm(uint8_t index)
{
    if ((0 <= index) && (index < _PWM_COUNT)) {
        return _pwmArray[index];
    }
    else {
        return nullptr;
    }
}

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

#include "FrequencyDividerTestFpga.h"

#include "easyfpga/easycores/pwm/pwm8.h"
#include "easyfpga/easycores/frequencydivider/frequencydivider.h"

FrequencyDividerTestFpga::FrequencyDividerTestFpga() :
    _pwm8_1(std::make_shared<Pwm8>()),
    _pwm8_2(std::make_shared<Pwm8>()),
    _f1(std::make_shared<FrequencyDivider>()),
    _f2(std::make_shared<FrequencyDivider>()),
    _f3(std::make_shared<FrequencyDivider>()),
    _f4(std::make_shared<FrequencyDivider>())
{
}

FrequencyDividerTestFpga::~FrequencyDividerTestFpga()
{
}

void FrequencyDividerTestFpga::defineStructure(void)
{
    this->addEasyCore(_pwm8_1);
    this->addEasyCore(_f1);
    this->connect(_pwm8_1, Pwm8::PIN::PWM_OUT, GPIOPIN::BANK1_PIN0);
    this->connect(_f1, FrequencyDivider::PIN::OUT, _pwm8_1, Pwm8::PIN::CLK_IN);

    this->addEasyCore(_pwm8_2);
    this->addEasyCore(_f2);
    this->addEasyCore(_f3);
    this->addEasyCore(_f4);
    this->connect(_pwm8_2, Pwm8::PIN::PWM_OUT, GPIOPIN::BANK1_PIN1);
    this->connect(_f2, FrequencyDivider::PIN::OUT, _pwm8_2, Pwm8::PIN::CLK_IN);
    this->connect(_f3, FrequencyDivider::PIN::OUT, _f2, FrequencyDivider::PIN::IN);
    this->connect(_f4, FrequencyDivider::PIN::OUT, _f3, FrequencyDivider::PIN::IN);
}

pwm8_ptr FrequencyDividerTestFpga::getPwm1Core(void)
{
    return _pwm8_1;
}

pwm8_ptr FrequencyDividerTestFpga::getPwm2Core(void)
{
    return _pwm8_2;
}

frequencydivider_ptr FrequencyDividerTestFpga::getFreqDiv1Core(void)
{
    return _f1;
}

frequencydivider_ptr FrequencyDividerTestFpga::getFreqDiv2Core(void)
{
    return _f2;
}

frequencydivider_ptr FrequencyDividerTestFpga::getFreqDiv3Core(void)
{
    return _f3;
}

frequencydivider_ptr FrequencyDividerTestFpga::getFreqDiv4Core(void)
{
    return _f4;
}

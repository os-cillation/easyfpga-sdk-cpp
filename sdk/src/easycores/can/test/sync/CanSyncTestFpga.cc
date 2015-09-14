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

#include "CanSyncTestFpga.h"
#include "easycores/can/can.h"

CanSyncTestFpga::CanSyncTestFpga() :
    _can1(std::make_shared<Can>()),
    _can2(std::make_shared<Can>()),
    _can3(std::make_shared<Can>())
{
}

CanSyncTestFpga::~CanSyncTestFpga()
{
}

void CanSyncTestFpga::defineStructure(void)
{
    this->addEasyCore(_can1);
    this->connect(GPIOPIN::BANK1_PIN1, _can1, Can::PIN::RX);
    this->connect(_can1, Can::PIN::TX, GPIOPIN::BANK1_PIN0);

    this->addEasyCore(_can2);
    this->connect(GPIOPIN::BANK1_PIN3, _can2, Can::PIN::RX);
    this->connect(_can2, Can::PIN::TX, GPIOPIN::BANK1_PIN2);

    this->addEasyCore(_can3);
    this->connect(GPIOPIN::BANK1_PIN5, _can3, Can::PIN::RX);
    this->connect(_can3, Can::PIN::TX, GPIOPIN::BANK1_PIN4);
}

can_ptr CanSyncTestFpga::getCanCore1(void)
{
    return _can1;
}

can_ptr CanSyncTestFpga::getCanCore2(void)
{
    return _can2;
}

can_ptr CanSyncTestFpga::getCanCore3(void)
{
    return _can3;
}

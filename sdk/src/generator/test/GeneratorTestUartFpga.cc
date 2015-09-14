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

#include "easyfpga/easycores/uart/uart.h"
#include "easyfpga/generator/test/GeneratorTestUartFpga.h"

GeneratorTestUartFpga::GeneratorTestUartFpga() :
    _uart1(std::make_shared<Uart>()),
    _uart2(std::make_shared<Uart>()),
    _uart3(std::make_shared<Uart>())
{
}

GeneratorTestUartFpga::~GeneratorTestUartFpga()
{
}

void GeneratorTestUartFpga::defineStructure(void)
{
    this->addEasyCore(_uart1);
    this->addEasyCore(_uart2);
    this->connect(_uart1, Uart::PIN::TXD, _uart2, Uart::PIN::RXD);
    this->connect(_uart2, Uart::PIN::TXD, _uart1, Uart::PIN::RXD);
    this->connect(_uart1, Uart::PIN::RTSn, _uart2, Uart::PIN::CTSn);
    this->connect(_uart2, Uart::PIN::RTSn, _uart1, Uart::PIN::CTSn);

    this->addEasyCore(_uart3);
    this->connect(EasyFpga::GPIOPIN::BANK0_PIN0, _uart3, Uart::PIN::RXD);
    this->connect(_uart3, Uart::PIN::TXD, EasyFpga::GPIOPIN::BANK0_PIN1);
    this->connect(EasyFpga::GPIOPIN::BANK0_PIN2, _uart3, Uart::PIN::CTSn);
    this->connect(_uart3, Uart::PIN::RTSn, EasyFpga::GPIOPIN::BANK0_PIN3);
    this->connect(_uart3, Uart::PIN::AUX1, EasyFpga::GPIOPIN::BANK0_PIN4);
    this->connect(_uart3, Uart::PIN::AUX2, EasyFpga::GPIOPIN::BANK0_PIN5);
}

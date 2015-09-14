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

#include "easyfpga/easycores/uart/uart.h"
#include "easyfpga/easycores/uart/test/sync/UartTestFpga.h"

UartTestFpga::UartTestFpga() :
    _uart1(std::make_shared<Uart>()),
    _uart2(std::make_shared<Uart>()),
    _uart3(std::make_shared<Uart>())
{
}

UartTestFpga::~UartTestFpga()
{
}

void UartTestFpga::defineStructure(void)
{
    this->addEasyCore(_uart1);
    this->addEasyCore(_uart2);
    this->connect(_uart1, Uart::PIN::TXD, _uart2, Uart::PIN::RXD);
    this->connect(_uart2, Uart::PIN::TXD, _uart1, Uart::PIN::RXD);
    this->connect(_uart1, Uart::PIN::RTSn, _uart2, Uart::PIN::CTSn);
    this->connect(_uart2, Uart::PIN::RTSn, _uart1, Uart::PIN::CTSn);

    this->connect(_uart1, Uart::PIN::RXD, EasyFpga::GPIOPIN::BANK0_PIN0);
    this->connect(_uart1, Uart::PIN::TXD, EasyFpga::GPIOPIN::BANK0_PIN2);
    this->connect(_uart1, Uart::PIN::TXD, EasyFpga::GPIOPIN::BANK0_PIN3);
}

uart_ptr UartTestFpga::getUart1(void)
{
    return _uart1;
}

uart_ptr UartTestFpga::getUart2(void)
{
    return _uart2;
}

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

#include "easyfpga/easycores/gpio/gpio8.h"
#include "easyfpga/easycores/spi/spi.h"
#include "easyfpga/easycores/spi/test/eeprom_read_write/SpiEepromTestFpga.h"

SpiEepromTestFpga::SpiEepromTestFpga() :
    _gpio8(std::make_shared<Gpio8>()),
    _spi(std::make_shared<Spi>())
{
}

SpiEepromTestFpga::~SpiEepromTestFpga()
{
}

void SpiEepromTestFpga::defineStructure(void)
{
    this->addEasyCore(_gpio8);
    this->connect(_gpio8, Gpio8::PIN::GPIO0, EasyFpga::GPIOPIN::BANK2_PIN1);

    this->addEasyCore(_spi);
    this->connect(_spi, Spi::PIN::SCK, EasyFpga::GPIOPIN::BANK2_PIN0);
    this->connect(_spi, Spi::PIN::MOSI, EasyFpga::GPIOPIN::BANK2_PIN2);
    this->connect(EasyFpga::GPIOPIN::BANK2_PIN3, _spi, Spi::PIN::MISO);
}

gpio8_ptr SpiEepromTestFpga::getGpio8Core(void)
{
    return _gpio8;
}

spi_ptr SpiEepromTestFpga::getSpiCore(void)
{
    return _spi;
}

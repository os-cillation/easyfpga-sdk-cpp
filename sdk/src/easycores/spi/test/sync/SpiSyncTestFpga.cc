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

#include "easyfpga/easycores/spi/spi.h"
#include "easyfpga/easycores/spi/test/sync/SpiSyncTestFpga.h"

SpiSyncTestFpga::SpiSyncTestFpga() :
    _spi(std::make_shared<Spi>())
{
}

SpiSyncTestFpga::~SpiSyncTestFpga()
{
}

void SpiSyncTestFpga::defineStructure(void)
{
    this->addEasyCore(_spi);

    this->connect(_spi, Spi::PIN::SCK, EasyFpga::GPIOPIN::BANK0_PIN1);
    this->connect(_spi, Spi::PIN::MOSI, EasyFpga::GPIOPIN::BANK0_PIN3);
    this->connect(EasyFpga::GPIOPIN::BANK0_PIN5, _spi, Spi::PIN::MISO);
}

spi_ptr SpiSyncTestFpga::getSpiCore(void)
{
    return _spi;
}

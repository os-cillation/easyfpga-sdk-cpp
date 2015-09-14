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

/**
 * \brief Forward declaration of the SpiEepromTestFpga
 */
class SpiEepromTestFpga;

#include <memory>

/**
 * \brief A shared pointer of the SpiEepromTestFpga
 */
typedef std::shared_ptr<SpiEepromTestFpga> spi_eeprom_test_fpga_ptr;


#include "easyfpga/easyfpga.h"
#include "easyfpga/easycores/gpio/gpio8_ptr.h"
#include "easyfpga/easycores/spi/spi_ptr.h"

/**
 * \brief Fpga description for the SpiEepromTest
 */
class SpiEepromTestFpga : public EasyFpga
{
    public:
        SpiEepromTestFpga();
        ~SpiEepromTestFpga();

        void defineStructure(void);

        gpio8_ptr getGpio8Core(void); // for realizing the chip select feature
        spi_ptr getSpiCore(void);

    private:
        gpio8_ptr _gpio8;
        spi_ptr _spi;
};

EASY_FPGA_DESCRIPTION_CLASS(SpiEepromTestFpga)

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

#include "easyfpga/easyfpga.h"
#include "easyfpga/easycores/uart/uart_ptr.h"

/**
 * \brief Fpga description for the GeneratorTest
 */
class GeneratorTestUartFpga : public EasyFpga
{
    public:
        GeneratorTestUartFpga();
        ~GeneratorTestUartFpga();

        void defineStructure(void);

    private:
        uart_ptr _uart1;
        uart_ptr _uart2;
        uart_ptr _uart3;
};

EASY_FPGA_DESCRIPTION_CLASS(GeneratorTestUartFpga)

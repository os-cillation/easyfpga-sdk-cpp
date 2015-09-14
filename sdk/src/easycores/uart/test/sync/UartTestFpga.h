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
 * \brief Forward declaration of the UartTestFpga
 */
class UartTestFpga;

#include <memory>

/**
 * \brief A shared pointer of the UartTestFpga
 */
typedef std::shared_ptr<UartTestFpga> uart_test_fpga_ptr;


#include "easyfpga/easyfpga.h"
#include "easyfpga/easycores/uart/uart_ptr.h"

/**
 * \brief Fpga description for the UartTest
 */
class UartTestFpga : public EasyFpga
{
    public:
        UartTestFpga();
        ~UartTestFpga();

        void defineStructure(void);

        uart_ptr getUart1(void);
        uart_ptr getUart2(void);

    private:
        uart_ptr _uart1;
        uart_ptr _uart2;
        uart_ptr _uart3;
};

EASY_FPGA_DESCRIPTION_CLASS(UartTestFpga)

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

#include "easyfpga.h"
#include "easycores/can/can_ptr.h"

/**
 * \brief Forward declaration of the CanSyncTestFpga
 */
class CanSyncTestFpga;

#include <memory>

/**
 * \brief A shared pointer of the CanSyncTestFpga
 */
typedef std::shared_ptr<CanSyncTestFpga> can_sync_test_fpga_ptr;

/**
 * \brief Fpga description for the CanSyncTest
 */
class CanSyncTestFpga : public EasyFpga
{
    public:
        CanSyncTestFpga();
        ~CanSyncTestFpga();

        void defineStructure(void);

        can_ptr getCanCore1(void);
        can_ptr getCanCore2(void);
        can_ptr getCanCore3(void);

    private:
        can_ptr _can1;
        can_ptr _can2;
        can_ptr _can3;
};

EASY_FPGA_DESCRIPTION_CLASS(CanSyncTestFpga)

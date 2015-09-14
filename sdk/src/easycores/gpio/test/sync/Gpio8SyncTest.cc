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

#include "Gpio8SyncTestFpga.h"

#include "easyfpga/easyfpga.h"
#include "easyfpga/communication/communicator.h"
#include "easyfpga/easycores/gpio/gpio8.h"
#include "easyfpga/easycores/gpio/gpio8_ptr.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

#include <bitset>

/**
 * \brief Tests functionality of the Gpio8 easyCore
 */
class Gpio8SyncTest : public Tester
{
    std::string testName(void) {
        return "Sync test functionality of gpio8";
    }

    bool testMethod(void) {
        gpio8_sync_test_fpga_ptr fpga = std::make_shared<Gpio8SyncTestFpga>();

        if (!fpga->init(0, "Gpio8SyncTestFpga.bin")) {
            return false;
        }

        gpio8_ptr gpio = fpga->getGpioCore();

        /* Case 1: Test functionality of handleRequestReplies() */
        gpio->makeAllPinsOutput();

        byte byte1 = (byte)0x00;
        gpio->getAllPins(&byte1);
        Log().Get(INFO) << "Pins: " << std::bitset<8>(byte1);

        gpio->setAllPins((byte)0xFF);
        gpio->getAllPins(&byte1);
        Log().Get(INFO) << "Pins: " << std::bitset<8>(byte1);

        /*
         * Case 2: Start read modify write operation and see, whether
         * getAllPins() returns an outdated value or the right one
         */
        gpio->setPin(Gpio8::PIN::GPIO0, LOW);
        byte byte2 = (byte)0x00;
        gpio->getAllPins(&byte2);
        Log().Get(INFO) << "Pins: " << std::bitset<8>(byte2);

        if (byte2 != (byte)0xFE)  {
            Log().Get(ERROR) << "The 2nd byte has not the expected value.";
            return false;
        }

        return true;
    }
};

int main(int argc, char** argv)
{
    Gpio8SyncTest test;
    return (uint32_t)test.runTest();
}

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

#include "easyfpga/communication/communicator.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

/**
 * \brief Tests switching functionalities of the Communicator
 */
class CommunicatorSwitchContextTest : public Tester
{
    std::string testName(void) {
        return "swichting context test";
    }

    bool testMethod(void) {
        Log().Get(INFO) << "First, lets try to connect to any connected easyFPGA...";

        Communicator com(NULL);
        if (!com.init(0)) {
            Log().Get(ERROR) << "Cannot establish connection!";
            return false;
        }

        uint32_t serialRead = 0;
        Log().Get(INFO) << "Now, lets execute a sync mcu operation...";
        com.readSerial(&serialRead);

        Log().Get(INFO) << "Now, lets execute a sync soc operation...";
        byte dataByte = (byte)0x00;
        com.writeRegister(dataByte, 0x08, 0x00);

        return true;
    }
};

int main(int argc, char** argv)
{
    CommunicatorSwitchContextTest test;
    return (uint32_t)test.runTest();
}

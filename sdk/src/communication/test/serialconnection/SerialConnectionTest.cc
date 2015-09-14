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

#include "easyfpga/communication/serialconnection.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"
#include "easyfpga/utils/hardwaretypes.h"

#include <sstream>

/**
 * \brief Tests all low level serial port functionalities of SerialConnection
 *
 * This test don't use the high level functions of the Communicator.
 * Thats why it will search through all usb devices and try to requests
 * a serial number from a connected easyFPGA. In the end, the test will
 * only be successful if an easyFPGA was connected.
 */
class SerialConnectionTest : public Tester
{
    std::string testName(void) {
        return "serial connection test";
    }

    bool testMethod(void) {
        SerialConnection connection;
        std::string devicePath("/dev/ttyUSB");
        int32_t i = 0;

        Log().Get(INFO) << "Test all connected usb devices until one delivers a serial number...";
        bool couldConnected = true;
        while (couldConnected) {
            std::stringstream ss;
            ss << devicePath << i++;

            Log().Get(INFO) << "Try to connect to " << ss.str();
            couldConnected = connection.openDevice(ss.str());
            if (couldConnected) {
                Log().Get(INFO) << "Connection successfully established.";

                byte request[1];
                request[0] = (byte)0xD3;
                Log().Get(INFO) << "Send to MCU: " << std::hex << (uint32_t)*(request);
                if (!connection.send(request, 1)) {
                    Log().Get(INFO) << "Error while sending request!";
                    return false;
                }

                byte reply[6];
                if (connection.receive(reply, 6, 1000000)) {
                    uint32_t i;

                    std::stringstream completeAnswer;
                    for (i=0; i<6; i++) {
                        completeAnswer << std::hex << (uint32_t)*(reply+i) << " ";
                    }
                    Log().Get(INFO) << "Get from MCU: " << completeAnswer.str();

                    std::stringstream serial;
                    for (i=1; i<5; i++) {
                        serial << std::hex << (uint32_t)*(reply+i);
                    }
                    Log().Get(INFO) << "This corresponds to a serial: 0x" << serial.str();

                    Log().Get(INFO) << "Disconnect...";
                    if (connection.closeDevice()) {
                        return true;
                    }
                    else {
                        Log().Get(ERROR) << "Cannont disconnect! Abort...";
                        return false;
                    }
                }
                else {
                    Log().Get(INFO) << "Couldn't receive a reply. Try the next device...";
                }

                Log().Get(INFO) << "Disconnect...";
                if (!connection.closeDevice()) {
                    Log().Get(ERROR) << "Cannont disconnect! Abort...";
                }
            }
            else {
                Log().Get(INFO) << "Couldn't connect to '" << ss.str() << "'. Abort...";
            }
        }

        return false;
    }
};

int main(int argc, char** argv)
{
    SerialConnectionTest test;
    return (uint32_t)test.runTest();
}

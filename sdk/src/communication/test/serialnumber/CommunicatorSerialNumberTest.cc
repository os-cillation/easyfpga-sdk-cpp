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
 * \brief Tests writing / reading serial number functionalities of the Communicator
 */
class CommunicatorSerialNumberTest : public Tester
{
    std::string testName(void) {
        return "read / write serial number test";
    }

    bool testMethod(void) {
        uint32_t serialRead = 0;
        const uint32_t serialToWrite = 0xFFFFFFAA;

        Log().Get(INFO) << "First, lets try to connect to any connected easyFPGA...";

        Communicator com(NULL);
        if (!com.init(0)) {
            Log().Get(ERROR) << "Cannot establish connection!";
            return false;
        }

        Log().Get(INFO) << "Now, lets try to read the current serial...";
        if (com.readSerial(&serialRead)) {
            Log().Get(INFO) << "Received serial: 0x" << std::hex << serialRead;
        }
        else {
            Log().Get(ERROR) << "No serial received! Leave test now :-(";
            return false;
        }

        Log().Get(INFO) << "Ok, and now lets try to write the serial 0x" << std::hex << serialToWrite << "...";
        if (com.writeSerial(serialToWrite)) {
            Log().Get(INFO) << "Serial 0x" << std::hex << serialToWrite << " successfully written.";
        }
        else {
            Log().Get(ERROR) << "Could not write serial! Leave test now :-(";
            return false;
        }

        serialRead = 0;
        Log().Get(INFO) << "At the end try to read the written serial...";
        if (com.readSerial(&serialRead)) {
            if (serialRead == serialToWrite) {
                Log().Get(INFO) << "Wuhu! Received expected serial!";
                return true;
            }
            else {
                Log().Get(ERROR) << "The received serial is not the expected one!";
                return false;
            }
        }
        else {
            Log().Get(ERROR) << "No serial received! Leave test now :-(";
            return false;
        }
    }
};

int main(int argc, char** argv)
{
    CommunicatorSerialNumberTest test;
    return (uint32_t)test.runTest();
}

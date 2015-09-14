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
#include "easyfpga/communication/protocol/calculator.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/os/file.h"
#include "easyfpga/utils/unittest/tester.h"

/**
 * \brief Tests the binary upload functionality of the Communicator
 */
class CommunicatorBinaryUploadTest : public Tester
{
    std::string testName(void) {
        return "fpga binary upload test";
    }

    bool testMethod(void) {
        std::string binaryFileName("productionTest.bin");

        Log().Get(INFO) << "Try to connect to any easyFPGA and upload binary " << binaryFileName << "...";

        Communicator com(NULL);

        if (com.init(0)) {
            uint32_t serialRead = 0;
            if (com.readSerial(&serialRead)) {
                Log().Get(DEBUG) << "Connection to easyFPGA with serial number 0x" << std::hex << serialRead << " established.";
            }
        }
        else {
            Log().Get(WARNING) << "No connection to an easyFPGA established. :-(";
            return false;
        }

        File binaryFile(binaryFileName);

        uint64_t size = 0;
        if (binaryFile.getSize(&size)) {
            Log().Get(DEBUG) << "Binary file size: " << size;

            Log().Get(DEBUG) << "Write binary file into internal buffer...";
            byte buffer[size];
            if (binaryFile.writeIntoByteBuffer(buffer)) {
                Log().Get(DEBUG) << "Get status from connected easyFPGA...";
                bool isFpgaConfigured = false;
                uint32_t remoteHash = 0;
                if (com.readStatus(&isFpgaConfigured, &remoteHash)) {
                    uint32_t localHash = Calculator::calculateAdler32Hash(buffer, size); // calculate hash from local binary

                    Log().Get(DEBUG) << "Is fpga configured: " << std::boolalpha << isFpgaConfigured;
                    Log().Get(DEBUG) << "Checksum from host binary: 0x" << std::hex << localHash;
                    Log().Get(DEBUG) << "Checksum from easyFPGA binary: 0x" << std::hex << remoteHash;

                    if (localHash == remoteHash) {
                        Log().Get(DEBUG) << "No new binary upload neccessary.";

                        if (!isFpgaConfigured) {
                            Log().Get(DEBUG) << "Configure Fpga with existing binary...";
                            if (com.configureFpga()) {
                                Log().Get(DEBUG) << "Configuration done. Success!";
                                return true;
                            }
                            else {
                                Log().Get(ERROR) << "Configuration faulty!";
                            }
                        }
                        else {
                            Log().Get(DEBUG) << "No new configuration neccessary. Success!";
                            return true;
                        }
                    }
                    else {
                        Log().Get(DEBUG) << "Try to upload a new binary...";
                        if (com.writeBinary(buffer, size)) {
                            Log().Get(DEBUG) << "Binary upload ok. Now write new status to the easyFPGA...";
                            if (com.writeStatus(true, size, localHash)) {
                                Log().Get(DEBUG) << "Configure Fpga with uploaded VHDL binary...";
                                if (com.configureFpga()) {
                                    Log().Get(DEBUG) << "Configuration done. Success!";
                                    return true;
                                }
                                else {
                                    Log().Get(ERROR) << "Configuration faulty!";
                                }
                            }
                            else {
                                Log().Get(ERROR) << "Error while writing status!";
                            }
                        }
                        else {
                            Log().Get(ERROR) << "Error while binary upload!";
                        }
                    }
                }
                else {
                    Log().Get(ERROR) << "Reading easyFPGA status was faulty!";
                }
            }
            else {
                Log().Get(ERROR) << "Error while reading binary!";
            }
        }
        else {
            Log().Get(ERROR) << "Error while reading binary!";
        }

        Log().Get(WARNING) << "Binary could not be uploaded!";
        return false;
    }
};

int main(int argc, char** argv)
{
    CommunicatorBinaryUploadTest test;
    return (uint32_t)test.runTest();
}

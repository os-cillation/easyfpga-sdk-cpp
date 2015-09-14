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

#include "easyfpga/communication/types.h"
#include "easyfpga/utils/config/configurationfile.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

/**
 * \brief Test the functionality of the class ConfigurationFile
 */
class ConfigurationFileTest : public Tester
{
    std::string testName(void) {
        return "configuration file test";
    }

    bool testMethod(void) {
        ConfigurationFile& file = ConfigurationFile::getInstance();

        if (file.getSocDirectory() == "/usr/local/share/easyfpga/soc") {
            Log().Get(DEBUG) << "Directory containing the soc hdl sources: " << file.getSocDirectory();
        }
        else {
            return false;
        }

        if (file.getLibraryDirectory() == "/usr/local/lib") {
            Log().Get(DEBUG) << "Location of the shared library: " << file.getLibraryDirectory();
        }
        else {
            return false;
        }

        if (file.getUsbDevicesPath() == "/dev/") {
            Log().Get(DEBUG) << "Directory of connected devices: " << file.getUsbDevicesPath();
        }
        else {
            return false;
        }

        if (file.getUsbDeviceIdentifier() == "ttyUSB") {
            Log().Get(DEBUG) << "Regex for identifing usb devices: " << file.getUsbDeviceIdentifier();
        }
        else {
            return false;
        }

        if (file.getMaximumRetriesAllowed() == 3) {
            Log().Get(DEBUG) << "Maximum retries allowed: " << file.getMaximumRetriesAllowed();
        }
        else {
            return false;
        }

        if (file.getLogOutputTarget() != NULL) {
            Log().Get(DEBUG) << "Log output target (pointer): " << file.getLogOutputTarget();
        }
        else {
            return false;
        }

        if (file.getMinimumLogOutputLevel() == 0) {
            Log().Get(DEBUG) << "Mimimum log level of the output messages: " << file.getMinimumLogOutputLevel();
        }
        else {
            return false;
        }

        switch (file.getOperationMode()) {
            case OPERATION_MODE::SYNC:
                Log().Get(DEBUG) << "Setted operation mode: synchronous mode";
                break;

            case OPERATION_MODE::ASYNC:
                Log().Get(DEBUG) << "Setted operation mode: asynchronous mode";
                break;

            default:
                return false;
        }

        return true;
    }
};

int main(int argc, char** argv)
{
    ConfigurationFileTest test;
    return (uint32_t)test.runTest();
}

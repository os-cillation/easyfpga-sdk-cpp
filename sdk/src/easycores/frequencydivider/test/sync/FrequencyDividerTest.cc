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

#include "easyfpga/easycores/frequencydivider/frequencydivider.h"
#include "easyfpga/easycores/frequencydivider/test/sync/FrequencyDividerTestFpga.h"
#include "easyfpga/easycores/pwm/pwm8.h"
#include "easyfpga/easycores/register.h" /* readSync(1) */
#include "easyfpga/utils/hardwaretypes.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

/**
 * \brief Tests functionality of the Gpio8 easyCore
 */
class FrequencyDividerTest : public Tester
{
    std::string testName(void) {
        return "Test functionality of the frequency divider";
    }

    bool testMethod(void) {
        frequency_divider_test_fpga_ptr fpga = std::make_shared<FrequencyDividerTestFpga>();

        if (fpga->init(0, "FrequencyDividerTestFpga.bin")) {
            byte data = (byte)0x00;
            fpga->getFreqDiv2Core()->getRegister(FrequencyDivider::REGISTER::DIV)->readSync(&data);
            Log().Get(INFO) << "Value of DIV2: " << std::hex << (uint32_t)data;
            fpga->getFreqDiv3Core()->getRegister(FrequencyDivider::REGISTER::DIV)->readSync(&data);
            Log().Get(INFO) << "Value of DIV3: " << std::hex << (uint32_t)data;
            fpga->getFreqDiv4Core()->getRegister(FrequencyDivider::REGISTER::DIV)->readSync(&data);
            Log().Get(INFO) << "Value of DIV4: " << std::hex << (uint32_t)data;

            fpga->getFreqDiv2Core()->setDivisor(255);
            fpga->getFreqDiv3Core()->setDivisor(20);
            fpga->getFreqDiv4Core()->bypass();
            fpga->getPwm2Core()->setDutyCycle(0x01);

            return true;
        }
        else {
            return false;
        }
    }
};

int main(int argc, char** argv)
{
    FrequencyDividerTest test;
    return (uint32_t)test.runTest();
}

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

#include "Pwm8TestFpga.h"

#include "easyfpga/easyfpga.h"
#include "easyfpga/easycores/pwm/pwm8.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

/**
 * \brief Tests functionality of easyCore Pwm8
 */
class Pwm8Test : public Tester
{
    std::string testName(void) {
        return "Test sync functionality of Pwm8";
    }

    bool testMethod(void) {
        pwm8_test_fpga_ptr fpga = std::make_shared<Pwm8TestFpga>();

        if (!fpga->init(0 , "Pwm8TestFpga.bin")) {
            return false;
        }

        byte cycle = (byte)0x00;
        fpga->getPwm()->getDutyCycle(&cycle);
        Log().Get(INFO) << "Current duty cycle: 0x" << std::hex << (uint32_t)cycle;
        cycle = (byte)0x10;
        fpga->getPwm()->setDutyCycle(cycle);

        return true;
    }
};

int main(int argc, char** argv)
{
    Pwm8Test test;
    return (uint32_t)test.runTest();
}

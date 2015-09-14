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

#include "Pwm16TestFpga.h"

#include "easyfpga/communication/communicator.h"
#include "easyfpga/easyfpga.h"
#include "easyfpga/easycores/pwm/pwm16.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

#include <unistd.h> /* usleep(1) */

/**
 * \brief Test the asynchronous aai reading / writing functionality of Pwm16 core
 */
class Pwm16Test : public Tester
{
    std::string testName(void) {
        return "pwm16 async aai register read test";
    }

    bool testMethod(void) {
        pwm16_test_fpga_ptr fpga = std::make_shared<Pwm16TestFpga>();

        if (!fpga->init(0, "Pwm16TestFpga.bin")) {
            return false;
        }

        Log().Get(INFO) << "Now dim the leds on bank2 3 times...";
        byte high;
        byte low;

        for (uint32_t i=0; i<3; i++) {
            high = (byte)0x00;
            for (uint32_t j=0; j<63; j++) {
                low = (byte)0x00;
                for (uint32_t l=0; l<63; l++) {
                    for (uint32_t k=48; k<72; k++) {
                        fpga->getPwm(k)->setDutyCycle(low, high);
                    }
                    //usleep(1000);
                    if (!fpga->handleReplies()) return false;
                    low += 8;
                }
                high += 8;
            }
        }

        sleep(1);

        if (!fpga->handleReplies()) return false;

        return true;
    }
};

int main(int argc, char** argv)
{
    Pwm16Test test;
    return (uint32_t)test.runTest();
}

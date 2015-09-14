/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Author: Johannes Hein <support@os-cillation.de>
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

#include "configuration.h"
#include "easyfpga.h"
#include "communication/communicator.h"
#include "easycores/can/can.h"
#include "easycores/can/can_ptr.h"
#include "easycores/can/utils/canframe_extended.h"
#include "easycores/can/utils/canframe_standard.h"
#include "easycores/can/utils/canframe_ptr.h"
#include "easycores/can/test/sync/CanSyncTestFpga.h"
#include "utils/log/log.h"
#include "utils/unittest/tester.h"

can_sync_test_fpga_ptr fpga = std::make_shared<CanSyncTestFpga>();
uint32_t progress = 0;

void receiveISR(void) {
    Log().Get(INFO) << "ENTER CAN CORE#2 RECEIVE ISR";

    can_ptr c2 = fpga->getCanCore2();

    Can::INTERRUPT interrupt = Can::INTERRUPT::WAKE_UP;
    c2->identifyInterrupt(&interrupt);

    if (interrupt == Can::INTERRUPT::RECEIVE) {
        canframe_ptr msg = nullptr;
        if (c2->getReceivedFrame(msg)) {
            Log().Get(INFO) << "Message received!";
            progress++;
        }
    }
    else {
        Log().Get(INFO) << "An unexpected interrupt has been thrown!";
    }

    fpga->enableInterrupts();
}

/**
 * \brief Tests the sync functionality of the Can easyCore
 */
class CanSyncTest : public Tester
{
    std::string testName(void) {
        return "Sync test functionality of can";
    }

    bool testMethod(void) {
        if (!fpga->init(0, "CanSyncTestFpga.bin")) {
            Log().Get(ERROR) << "Possible reasons: no connection / faulty binary upload!";
            return false;
        }

        can_ptr c1 = fpga->getCanCore1();
        can_ptr c2 = fpga->getCanCore2();
        can_ptr c3 = fpga->getCanCore3();

        c1->init(Can::BITRATE::BITRATE_1M, Can::USAGE_MODE::BASIC_MODE);
        c2->init(Can::BITRATE::BITRATE_1M, Can::USAGE_MODE::BASIC_MODE);
        c3->init(Can::BITRATE::BITRATE_1M, Can::USAGE_MODE::BASIC_MODE);

        c1->setAcceptanceMask(0x00);
        c1->setAcceptanceCode(0x11);
        c2->setAcceptanceMask(0xFF);
        c2->setAcceptanceCode(0x22);

        c2->enableInterrupt(Can::INTERRUPT::RECEIVE);
        c2->registerCallback(receiveISR);

        fpga->enableInterrupts();

        /* construct message */
        byte msgData[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
        canframe_ptr msg = std::make_shared<CanFrameStandard>(0x22, msgData, 5); /* << 3: 11 bits, 8 MSB are evaluated */
        canframe_ptr msgRTR = std::make_shared<CanFrameStandard>(0x22);
        canframe_ptr msgExt = std::make_shared<CanFrameStandard>(0x56, msgData, 5);

        c1->logStatus();
        c2->logStatus();

        /* transmit data message */
        Log().Get(INFO) << "Will now send data message...";
        assert(c1->transmit(msg));
        usleep(300000);
        fpga->handleReplies();

        c1->logStatus();
        c2->logStatus();

        /* transmit RTR */
        Log().Get(INFO) << "Will now send RTR message...";
        c1->transmit(msgRTR);
        usleep(300000);
        fpga->handleReplies();

        c1->logStatus();
        c2->logStatus();

        /* transmit extended */
        Log().Get(INFO) << "Will now send extended message that should be ignored...";
        c1->transmit(msgExt);
        usleep(300000);
        fpga->handleReplies();

        c1->logStatus();
        c2->logStatus();

        if (progress == 2) {
            return true;
        }
        else {
            return false;
        }
    }
};

int main(int argc, char** argv)
{
    CanSyncTest test;
    return (uint32_t)test.runTest();
}

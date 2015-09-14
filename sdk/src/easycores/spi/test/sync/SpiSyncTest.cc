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

#include "SpiSyncTestFpga.h"

#include "easyfpga/easycores/spi/spi.h"
#include "easyfpga/easycores/spi/spi_ptr.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"
#include "easycores/register.h"

/**
 * \brief Tests sync functionality of the Spi core
 *
 * A simple demonstration and test of the SPI easyCore. Due to the lack of an SPI slave,
 * the master is operated with a loopback. Thus, only parts of the functionality can be
 * tested.
 *
 * Note: Place a jumper accross Bank0.3 and Bank0.5 to connect MISO and MOSI
 */
class SpiSyncTest : public Tester
{
    std::string testName(void)
    {
        return "spi sync test";
    }

    /*
     * \brief Use transceive method to send, receive and then compare
     */
    bool testTransceive(const uint32_t testRuns, spi_ptr spiMaster)
    {
        for (uint32_t run=0; run<testRuns; run++) {
            byte dataOut = (byte)(run % 256);
            byte dataIn = (byte)0x00;
            Log().Get(DEBUG) << "Will send data: " << (uint32_t)dataOut;
            if (!spiMaster->transceive(dataOut, &dataIn)) {
                Log().Get(ERROR) << "Transceive method returned false";
                return false;
            }
            if (!(dataOut == dataIn)) {
                Log().Get(ERROR) << "Transceive method compare fail. Did you connect MOSI and MISO?";
                return false;
            }
        }
        return true;
    }

    /**
     * \brief Call transmit method up to ten times, then receive only and expect dummy bytes
     */
    bool testTransmitReceive(const uint32_t testRuns, spi_ptr spiMaster)
    {
        for (uint32_t run=0; run<testRuns; run++) {
            /* Transmit up to ten bytes without receiving */
            uint32_t txTime;
            for (txTime=0; txTime<run%10; txTime++) {
                if (!spiMaster->transmit(run%256)) {
                    Log().Get(ERROR) << "Failed to transmit " << txTime << " bytes";
                    return false;
                }
            }
            Log().Get(DEBUG) << "Transmitted " << txTime << " bytes without receiving";

            /* Receive should always give the dummy byte 0x00 */
            uint32_t rxTime;
            for (rxTime=0; rxTime<run%10; rxTime++) {
                byte dataIn = (byte)0x00;
                if (!spiMaster->receive(&dataIn)) {
                    Log().Get(ERROR) << "Receive method returned false";
                    return false;
                }
                if (dataIn!=(byte) 0x00) {
                    Log().Get(ERROR) << "Expected dummy byte, received 0x" << std::hex << dataIn;
                    return false;
                }
            }
            Log().Get(DEBUG) << "Received " << rxTime << " dummy bytes";
        }
        return true;
    }

    /**
     * \brief Init the spi master and call all test methods
     */
    bool testCombination(uint32_t testRuns, spi_ptr spiMaster, Spi::SPI_MODE mode, Spi::CLOCK_SPEED clockSpeed)
    {
        /* Init spi master */
        if (!spiMaster->init(mode, clockSpeed)) {
            Log().Get(ERROR) << "Init method returned false";
            return false;
        }

        /* Test transceive */
        if (!testTransceive(testRuns, spiMaster)) {
            Log().Get(ERROR) << "Transceive test failed";
            return false;
        }

        /* Test transmit-only and receiving dummy bytes */
        if (!testTransmitReceive(testRuns, spiMaster)) {
            Log().Get(ERROR) << "Transmit-Receive test failed";
            return false;
        }
        return true;
    }

    bool testMethod(void)
    {
        /* Setup fpga and get spi master */
        spi_test_fpga_ptr fpga = std::make_shared<SpiSyncTestFpga>();
        if (!fpga->init(0, "SpiSyncTestFpga.bin")) {
            Log().Get(ERROR) << "FPGA init failed";
            return false;
        }
        spi_ptr spiMaster = fpga->getSpiCore();

        /* Run tests */
        const uint32_t TEST_RUNS = 300;
        Log().Get(INFO) << "SPI-Master Test : " << TEST_RUNS << " test runs";

        Log().Get(INFO) << "Testing with MODE 0 and 19531 Hz ...";
        if (!testCombination(TEST_RUNS, spiMaster,
            Spi::SPI_MODE::MODE_0, Spi::CLOCK_SPEED::SCK_19531_HZ)) {
            Log().Get(ERROR) << "Tests with MODE 0 and 19531 Hz failed";
            return false;
        }
        Log().Get(INFO) << "Success!";

        Log().Get(INFO) << "Testing with MODE 1 and 312500 Hz ...";
        if (!testCombination(TEST_RUNS, spiMaster,
            Spi::SPI_MODE::MODE_1, Spi::CLOCK_SPEED::SCK_312500_HZ)) {
            Log().Get(ERROR) << "Tests with MODE 1 and 312500 Hz failed";
            return false;
        }
        Log().Get(INFO) << "Success!";

        Log().Get(INFO) << "Testing with MODE 2 and 2500 kHz ...";
        if (!testCombination(TEST_RUNS, spiMaster,
            Spi::SPI_MODE::MODE_2, Spi::CLOCK_SPEED::SCK_2500_KHZ)) {
            Log().Get(ERROR) << "Tests with MODE 2 and 2500 kHz failed";
            return false;
        }
        Log().Get(INFO) << "Success!";

        Log().Get(INFO) << "Testing with MODE 3 and 40 MHz ...";
        if (!testCombination(TEST_RUNS, spiMaster,
            Spi::SPI_MODE::MODE_3, Spi::CLOCK_SPEED::SCK_40_MHZ)) {
            Log().Get(ERROR) << "Tests with MODE 3 and 40 MHz failed";
            return false;
        }
        Log().Get(INFO) << "Success!";

        return true;
    }
};

int main(int argc, char** argv)
{
    SpiSyncTest test;
    return (uint32_t)test.runTest();
}

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

#include "EepromAt25xxxb.h"
#include "SpiEepromTestFpga.h"

#include "easyfpga/easycores/gpio/gpio8.h"
#include "easyfpga/easycores/gpio/gpio8_ptr.h"
#include "easyfpga/easycores/register.h"
#include "easyfpga/easycores/spi/spi.h"
#include "easyfpga/easycores/spi/spi_ptr.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

#include <algorithm>

spi_eeprom_test_fpga_ptr fpga = std::make_shared<SpiEepromTestFpga>();

gpio8_ptr gpio = fpga->getGpio8Core();
spi_ptr spiMaster = fpga->getSpiCore();

/**
 * \brief Test case: Writes an EEPROM image to an AT25256B and later
 *        readout the written image
 *
 * Prerequisites for a successful execution:
 * - EEPROM: AT25128B or AT25256B
 * - Connected pins:
 *
 * VCC: 3,3V (e.g. the FPGA JTAG pin)
 * GND
 * CS: bank 2, gpio 1
 * SCK: bank 2, gpio 0
 * MOSI: bank 2, gpio 2
 * MISO: bank 2, gpio 3
 */
class SpiEepromTest : public Tester
{
    std::string testName(void) {
        return "spi eeprom read / write test";
    }

    bool testMethod(void) {
        if (!fpga->init(0, "SpiEepromTestFpga.bin")) {
            return false;
        }
        EepromAt25xxxb* eeprom = new EepromAt25xxxb();

        /* declare GPIO0 (chip select) as output pin */
        gpio->makeAllPinsOutput();
        /* disable chip select */
        gpio->setAllPins((byte)0x01);

        /* initialize SPI core */
        spiMaster->init(Spi::SPI_MODE::MODE_0, Spi::CLOCK_SPEED::SCK_20_MHZ);

        byte data = (byte)0x00;

        /* Try to enable write */
        Log().Get(INFO) << "Enable write and check success";
        if (!eeprom->enableWrite()) {
            Log().Get(ERROR) << "Enable write returned false";
            return false;
        }

        /* Check if the enable write operation was successful */
        if (!eeprom->readStatus(&data)) {
            Log().Get(ERROR) << "Read status returned false";
            return false;
        }
        if (!(data == (byte)0x02)) {
            Log().Get(ERROR) << "Write operations could not be enabled!";
            return false;
        }

        /* Write a single byte to address 0x00 */
        Log().Get(INFO) << "Write 0xAA to address 0x00";
        if (!eeprom->write((byte)0xAA, (uint16_t)0)) {
            Log().Get(ERROR) << "Writing single byte failed.";
            return false;
        }

        /*
         * Check if the write operation was successful by read back a
         * the before written byte on address 0x00.
         */
        Log().Get(INFO) << "Read address 0x00";
        if (!eeprom->read(&data, (uint16_t)0)) {
            Log().Get(ERROR) << "Reading single byte failed.";
            return false;
        }
        if (data != (byte)0xAA) {
            return false;
        }

        /* Create random test data array */
        uint8_t length = 20;
        byte testData[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,17,18,19,20};
        std::srand(time(0));
        std::random_shuffle(&testData[0], &testData[length-1]);
        uint16_t startAddress = 0x23;

        /* Write multiple bytes and read back single bytes */
        Log().Get(INFO) << "Write " << (uint32_t)length << " bytes, beginning at address 0x" <<
                           std::hex << startAddress;
        if (!eeprom->write(testData, length, startAddress)) {
            Log().Get(ERROR) << "Failed to write " << (uint32_t)length << " bytes";
            return false;
        }

        Log().Get(INFO) << "Read back " << (uint32_t)length << " bytes";
        for (uint8_t byteNo=0; byteNo<length; byteNo++) {
            if (!eeprom->read(&data, startAddress+byteNo)) {
                Log().Get(ERROR) << "Failed to read byte #" << (uint32_t)byteNo;
                return false;
            }
            if (data != testData[byteNo]) {
                Log().Get(ERROR) << "Byte #" << (uint32_t)byteNo << " differs from written byte";
                return false;
            }
        }

        /* Read back the entire sequence at once and compare */
        Log().Get(INFO) << "Read back " << (uint32_t)length << " bytes at once";
        byte receiveData[length];
        if (!eeprom->read(receiveData, length, startAddress)) {
            Log().Get(ERROR) << "Failed to read " << (uint32_t)length << " bytes at once";
            return false;
        }
        for (uint8_t byteNo=0; byteNo<length; byteNo++) {
            if (receiveData[byteNo] != testData[byteNo]) {
                Log().Get(ERROR) << "Byte #" << (uint32_t)byteNo << " differs from written byte";
                return false;
            }
        }

        /* Write a file */
        std::string fileName = "binary.eep";
        Log().Get(INFO) << "Writing file " << fileName;
        if (!eeprom->write(fileName)) {
            Log().Get(ERROR) << "Writing file " << fileName << " failed";
            return false;
        }

        /* Load file for verification */
        File bin(fileName);
        uint64_t size;
        bin.getSize(&size);
        byte fileContent[size];
        bin.writeIntoByteBuffer(fileContent);

        /* Read back and compare */
        Log().Get(INFO) << "Reading back " << size << " bytes";
        byte eepromByte;
        for (uint32_t byteNo=0; byteNo < size; byteNo++) {
            if(!eeprom->read(&eepromByte, byteNo)) {
                Log().Get(ERROR) << "Failed to read " << (uint32_t)size << " bytes at once";
                return false;
            }
            if (eepromByte != fileContent[byteNo]) {
                Log().Get(ERROR) << "Compare missmatch at byte #" << byteNo;
                return false;
            }
            Log().Get(INFO) << "Compared byte #" << byteNo;
        }

        Log().Get(INFO) << size << " bytes compared, all matching";

        /* Finally log the status register */
        byte spsr = (byte)0x00;
        spiMaster->getRegister(Spi::REGISTER::SPSR)->readSync(&spsr);
        Log().Get(INFO) << "Status register: 0x" << std::hex << (uint32_t)spsr;

        return true;
    }
};

int main(int argc, char** argv)
{
    SpiEepromTest test;
    return (uint32_t)test.runTest();
}

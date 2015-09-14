/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Author: Johannes Hein <support@os-cillation.de>
 *          Simon Gansen
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

#include "easyfpga/easycores/gpio/gpio8.h"
#include "easyfpga/easycores/gpio/gpio8_ptr.h"
#include "easyfpga/easycores/register.h"
#include "easyfpga/easycores/spi/spi.h"
#include "easyfpga/easycores/spi/spi_ptr.h"
#include "easyfpga/utils/log/log.h"

/* defined in SpiEepromTest.cc */
extern gpio8_ptr gpio;
extern spi_ptr spiMaster;

bool EepromAt25xxxb::enableWrite(void)
{
    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* send opcode to eeprom and discard the received data */
    if (!spiMaster->transmit(OPCODE_ENABLE_WRITE)) {
        Log().Get(WARNING) << "Failed to transmit opcode enable write";
        return false;
    }

    /* disable chip select */
    if (!disableChipSelect()) {
        return false;
    }

    return true;
}

bool EepromAt25xxxb::disableWrite(void)
{
    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* send opcode to eeprom and discard the received data */
    if (!spiMaster->transmit(OPCODE_DISABLE_WRITE)) {
        Log().Get(WARNING) << "Failed to transmit opcode disable write";
        return false;
    }

    /* disable chip select */
    if (!disableChipSelect()) {
        return false;
    }

    return true;
}

bool EepromAt25xxxb::readStatus(byte* status)
{
    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* send opcode to eeprom and write the received data to user space */
    if (!spiMaster->transmit(OPCODE_READ_STATUS)) {
        Log().Get(WARNING) << "Failed to transmit opcode read status";
        return false;
    }
    if (!spiMaster->receive(status)) {
        Log().Get(WARNING) << "Failed to receive status register data";
        return false;
    }

    /* disable chip select */
    if (!disableChipSelect()) {
        return false;
    }

    return true;
}

bool EepromAt25xxxb::write(byte data, uint16_t eepromAddress)
{
    /* parameter check */
    if (eepromAddress > ADDRESS_MAX) {
        Log().Get(WARNING) << "Exceeded maximum address: 0x" << std::hex << ADDRESS_MAX;
        return false;
    }

    enableWrite();

    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* send opcode, address and data to be written to eeprom */
    if (!spiMaster->transmit(OPCODE_WRITE)) {
        Log().Get(WARNING) << "Failed to transmit opcode write";
        return false;
    }
    if (!spiMaster->transmit((eepromAddress & 0xFF00) >> 8)) {
        Log().Get(WARNING) << "Failed to transmit address high byte";
        return false;
    }
    if (!spiMaster->transmit(eepromAddress & 0x00FF)) {
        Log().Get(WARNING) << "Failed to transmit address low byte";
        return false;
    }
    if (!spiMaster->transmit(data)) {
        Log().Get(WARNING) << "Failed to transmit byte to write";
        return false;
    }

    /*
     * disable chip select
     * (now the eeprom writes data into the memory)
     */
    if (!disableChipSelect()) {
        return false;
    }

    /* wait until the eeprom becomes ready */
    byte status = (byte)0x00;
    if (!readStatus(&status)) {
        Log().Get(WARNING) << "Failed to read status";
        return false;
    }
    while (setBitTest(status, 0)) {
        if (!readStatus(&status)) {
            Log().Get(WARNING) << "Failed to read status";
            return false;
        }
        usleep(1);
        Log().Get(INFO) << "Wait for eeprom becomes ready";
    }

    return true;
}

bool EepromAt25xxxb::write(byte* data, uint8_t length, uint16_t startAddress)
{
    /* parameter check */
    if (length > PAGE_SIZE) {
        Log().Get(WARNING) << "Exceeded maximum length of " << PAGE_SIZE << " bytes";
        return false;
    }
    else if (length < 2) {
        Log().Get(WARNING) << "This method should be used to write at least 2 bytes";
        return false;
    }
    else if (startAddress > ADDRESS_MAX) {
        Log().Get(WARNING) << "Exceeded maximum address: 0x" << std::hex << ADDRESS_MAX;
        return false;
    }

    enableWrite();

    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* transmit opcode and address */
    if (!spiMaster->transmit(OPCODE_WRITE)) {
        Log().Get(WARNING) << "Failed to transmit opcode write";
        return false;
    }
    if (!spiMaster->transmit((startAddress & 0xFF00) >> 8)) {
        Log().Get(WARNING) << "Failed to transmit address high byte";
        return false;
    }
    if (!spiMaster->transmit(startAddress & 0x00FF)) {
        Log().Get(WARNING) << "Failed to transmit address low byte";
        return false;
    }

    /* transmit data */
    if (!spiMaster->transmit(data, length)) {
        Log().Get(WARNING) << "Failed to transmit multiple data bytes";
        return false;
    }

    /*
     * disable chip select
     * (now the eeprom writes data into the memory)
     */
    if (!disableChipSelect()) {
        return false;
    }

    /* wait until the eeprom becomes ready */
    byte status = (byte)0x00;
    if (!readStatus(&status)) {
        Log().Get(WARNING) << "Failed to read status";
        return false;
    }
    while (setBitTest(status, 0)) {
        if (!readStatus(&status)) {
            Log().Get(WARNING) << "Failed to read status";
            return false;
        }
        usleep(1);
        Log().Get(INFO) << "Wait for eeprom becomes ready";
    }

    return true;
}

bool EepromAt25xxxb::write(std::string fileName)
{
    /* create file and check existance */
    File bin(fileName);
    if (!bin.exists()) {
        Log().Get(WARNING) << "File does not exist";
        return false;
    }

    /* check size */
    uint64_t size;
    if (!bin.getSize(&size)) {
        Log().Get(WARNING) << "Failed to check file size";
        return false;
    }
    Log().Get(INFO) << "Filesize: " << (uint32_t)size << " bytes";

    /* create an array of bytes from the file */
    byte byteArray[size];
    if (!bin.writeIntoByteBuffer(byteArray)) {
        Log().Get(WARNING) << "Failed to read file";
        return false;
    }

    /* determine page count and check size */
    uint32_t pageCount = size / PAGE_SIZE;
    if (size % PAGE_SIZE != 0) {
        Log().Get(WARNING) << "Please make sure, the file fits into pages";
        return false;
    }
    /**
     * \todo Make it possible to write files that do not fit exactly into pages
     */

    enableWrite();

    /* write full pages */
    for (uint32_t pageNo=0; pageNo<pageCount; pageNo++) {
        Log().Get(INFO) << "Writing " << pageNo << "/" << pageCount;

        /* copy a single page */
        byte currentPage[PAGE_SIZE];
        uint32_t startIndex = pageNo * PAGE_SIZE;
        uint32_t endIndex = startIndex + PAGE_SIZE;
        std::copy(byteArray+startIndex, byteArray+endIndex, currentPage);

        /* write page */
        write(currentPage, PAGE_SIZE, startIndex);
    }

    return true;
}

bool EepromAt25xxxb::read(byte* data, uint16_t eepromAddress)
{
    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* send opcode and address */
    if (!spiMaster->transmit(OPCODE_READ)) {
        Log().Get(WARNING) << "Failed to transmit opcode read";
        return false;
    }
    if (!spiMaster->transmit((eepromAddress & 0xFF00) >> 8)) {
        Log().Get(WARNING) << "Failed to transmit address high byte";
        return false;
    }
    if (!spiMaster->transmit(eepromAddress & 0x00FF)) {
        Log().Get(WARNING) << "Failed to transmit address low byte";
        return false;
    }

    /* receive */
    if (!spiMaster->receive(data)) {
        Log().Get(WARNING) << "Failed to receive data";
        return false;
    }

    /* disable chip select */
    if (!disableChipSelect()) {
        return false;
    }

    return true;
}

bool EepromAt25xxxb::read(byte* data, uint16_t length, uint16_t startAddress)
{
    /* parameter check */
    if (length < 2) {
        Log().Get(WARNING) << "This method should be used to read at least 2 bytes";
        return false;
    }

    /* log when wrapping around */
    if (startAddress + length > ADDRESS_MAX) {
        Log().Get(INFO) << "Read operation will wrap around the address space";
    }

    /* enable chip select */
    if (!enableChipSelect()) {
        return false;
    }

    /* send opcode and address */
    if (!spiMaster->transmit(OPCODE_READ)) {
        Log().Get(WARNING) << "Failed to transmit opcode read";
        return false;
    }
    if (!spiMaster->transmit((startAddress & 0xFF00) >> 8)) {
        Log().Get(WARNING) << "Failed to transmit address high byte";
        return false;
    }
    if (!spiMaster->transmit(startAddress & 0x00FF)) {
        Log().Get(WARNING) << "Failed to transmit address low byte";
        return false;
    }

    /* receive */
    for (uint16_t byteNo=0; byteNo<length; byteNo++) {
        if (!spiMaster->receive(&data[byteNo])) {
            Log().Get(WARNING) << "Failed to receive byte #" << (uint32_t)byteNo;
            return false;
        }
    }

    /* disable chip select */
    if (!disableChipSelect()) {
        return false;
    }
    return true;
}

bool EepromAt25xxxb::enableChipSelect(void)
{
    if (!gpio->setAllPins((byte)0x00)) {
        Log().Get(WARNING) << "Failed to enable chip select";
        return false;
    }
    return true;
}

bool EepromAt25xxxb::disableChipSelect(void)
{
    if (!gpio->setAllPins((byte)0x01)) {
        Log().Get(WARNING) << "Failed to disable chip select";
        return false;
    }
    return true;
}

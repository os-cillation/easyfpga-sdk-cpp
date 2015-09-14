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

#ifndef SDK_EASYCORES_SPI_TEST_EEPROM_READ_WRITE_EEPROMAT25XXXB_H_
#define SDK_EASYCORES_SPI_TEST_EEPROM_READ_WRITE_EEPROMAT25XXXB_H_

#include "easyfpga/utils/hardwaretypes.h" /* byte */
#include "easyfpga/utils/os/file.h"

class EepromAt25xxxb
{
    public:
        /**
         * \brief Send write enable command
         *
         * \return true if successful,\n
         *         false otherwise
         */
        bool enableWrite(void);

        /**
         * \brief Send write disable command
         *
         * \return true if successful,\n
         *         false otherwise
         */
        bool disableWrite(void);

        /**
         * \brief Read the status register
         *
         * \param status The status register of the eeprom
         *
         * \return true if successful,\n
         *         false otherwise
         */
        bool readStatus(byte* status);

        /**
         * \brief Write a single byte
         *
         * \param data Data byte to be written
         *
         * \param eepromAddress Destination address (0 .. 0x7FFF)
         *
         * \return true if successful,\n
         *         false otherwise
         */
        bool write(byte data, uint16_t eepromAddress);

        /**
         * \brief Write multiple bytes
         *
         * Note that the address will wrap around at block borders. Hence
         * when writing an entire block it is recommended to start at the
         * beginning of that block.
         *
         * \param data Array of bytes to be written
         *
         * \param length Number of bytes to be written (2 .. 64)
         *
         * \param startAddress Destination address (0 .. 0x7FFF)
         */
        bool write(byte* data, uint8_t length, uint16_t startAddress);

        /**
         * \brief Write an entire binary file into the eeprom
         *
         * \param fileName Name of the file to read from
         */
        bool write(std::string fileName);

        /**
         * \brief Read a single byte
         *
         * \param data Data byte read from the eeprom
         *
         * \param eepromAddress Source address (0 .. 0x7FFF)
         *
         * \return true if successful,\n
         *         false otherwise
         */
        bool read(byte* data, uint16_t eepromAddress);

        /**
         * \brief Read multiple bytes
         *
         * Note that when reaching the end of eeprom space, the
         * eeprom will continue to read from the beginning.
         *
         * \param data Data bytes read from the eeprom
         *
         * \param length Number of bytes to be written (>2)
         *
         * \param startAddress Source address (0 .. 0x7FFF)
         *
         * \return true if successful,\n
         *         false otherwise
         */
        bool read(byte* data, uint16_t length, uint16_t startAddress);

    private:
        static const byte OPCODE_WRITE = (byte)0x02;
        static const byte OPCODE_READ = (byte)0x03;
        static const byte OPCODE_WRITE_STATUS = (byte)0x01;
        static const byte OPCODE_READ_STATUS = (byte)0x05;
        static const byte OPCODE_DISABLE_WRITE = (byte)0x04;
        static const byte OPCODE_ENABLE_WRITE = (byte)0x06;

        static const uint8_t PAGE_SIZE = 64;
        static const uint16_t ADDRESS_MAX = (uint16_t)0x7FFF;

        bool enableChipSelect();
        bool disableChipSelect();
};
#endif  // SDK_EASYCORES_SPI_TEST_EEPROM_READ_WRITE_EEPROMAT25XXXB_H_

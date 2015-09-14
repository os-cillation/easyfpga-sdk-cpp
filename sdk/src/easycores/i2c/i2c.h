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

#ifndef SDK_EASYCORES_I2C_I2C_H_
#define SDK_EASYCORES_I2C_I2C_H_

#include "easycores/easycore.h"
#include "utils/hardwaretypes.h"

/**
 * \brief An I2C master core.
 *
 * <b>Features</b>
 *
 * - Compatible with Philips I2C bus standard
 * - Multi-Master Operation
 * - Software programmable timing
 * - Clock stretching and wait state generation
 * - Interrupt or bit-polling driven byte-by-byte data-transfers
 * - Arbitration lost interrupt, with automatic transfer cancelation
 * - (Repeated)Start/Stop signal generation/detection
 * - Bus busy detection
 * - Supports 7 and 10bit addressing
 * - Fully static and synchronous design
 * - Fully synthesisable
 *
 * \see http://opencores.org/project,i2c
 *
 * <b>Usage</b>
 *
 * The core can either be initialized in standard or in fast mode: In
 * standard mode the clock frequency will be approx. 90 kHz, in fast
 * mode 350 kHz.
 *
 * For typical read and write operations, you can use the I2C::readByte()
 * or I2C::writeByte() methods.
 *
 * The I2C::writeByte() method initiates three transfers:
 * -# Start condition; Write device address
 * -# Write register address
 * -# Write data; Stop condition
 *
 * The I2C::readByte() method performs a typical I2C four-transfer-read:
 * -# Start condition; Write device address
 * -# Write register address
 * -# Repeated start condition; Write device address
 * -# Read register content; Stop condition; Send nack
 *
 * In case these shorthand methods can not be applied, there is also the
 * I2C::transfer() method giving full control of the core. Both methods
 * above use this generic transfer method.
 *
 * <b>Interrupt handling</b>
 *
 * This easyCore currently supports no interrupts.
 */
class I2C : public EasyCore
{
    public:
        I2C();
        ~I2C();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 5;

        /**
         * \brief Defines a set of paths to predefined hdl files which
         *        describes this ip core.
         *
         * These files will be included in the generation process and be
         * located in the github repository os-cillation/easyfpga-soc.
         * (You already cloned this subrepository by cloning the main
         * respository easyfpga-sdk-cpp.)
         *
         * \return A formatted string containing relative paths from an
         *         soc directory to the corresponding hdl files.
         */
        std::string getHdlInjections(void);

        /**
         * \brief Returns a hdl template for this ip core.
         *
         * This template provides a kind of interface to the injected hdl
         * files by getHdlInjections(). It contains several placeholders
         * overwritten at the generation process.
         *
         * \return A core specific hdl template interface as string.
         */
        std::string getHdlTemplate(void);

        /* THE CORE'S ACCESSIBLE HARDWARE */
        /**
         * \brief Defines the available pins of this easyCore.
         *
         * Please note that both pins are bidirectional and have to be
         * connected directly to the GPIO pins in the FPGA definition!
         */
        enum PIN : PinConst {
            /**
             * \brief The bidirectional data pin
             *
             * Type: input / sink and output / source
             *
             * \hideinitializer
             */
            SDA = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief The bidirectional clock pin
             *
             * Type: input / sink and output / source
             *
             * \hideinitializer
             */
            SCL = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1
        };

        /**
         * \brief Defines the addressable registers of this easyCore.
         */
        enum REGISTER : RegisterConst {
            /**
             * Clock prescale register (low byte)
             * <br>Access: read / write
             * <br>Usage purpose: Setting the bus clock.
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            PREREG_LOW = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0,

            /**
             * GPIO output register (high byte)
             * <br>Access: read / write
             * <br>Usage purpose: Setting the bus clock.
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            PREREG_HIGH = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+1,

            /**
             * Control register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            CTRL = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+2,

            /**
             * Transmit register
             * <br>Access: write only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            TX = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+3,

            /**
             * Receive register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            RX = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+4,

            /**
             * Command register
             * <br>Access: write only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            CR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+5,

            /**
             * Status register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            SR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+6
        };

        /* CORE SETTINGS */
        /**
         * \brief Defines the two possible speed modes.
         */
        enum CLOCK_SPEED : int32_t {
            /**
             * Standard mode; SCL clocked at 90kHz
             */
            MODE_STANDARD,

            /**
             * Fast mode; SCL clocked at 350kHz
             */
            MODE_FAST
        };

        /**
         * \brief Initializes the I2C master core and set a certain
         *        transmission speed
         *
         * \param speed Possible values are values from I2C::CLOCK_SPEED.
         *
         * \return true if all settings could be successfully set,<br>
         *         false otherwise
         */
        bool init(CLOCK_SPEED speed);

        /* INTERRUPT HANDLING */

        /* SPECIAL CORE METHODS */
        /**
         * \brief A generic transfer method
         *
         * \param data
         *        - 8 bits of data, or
         *        - 7 bit slave address with R/W bit as LSB
         *
         * \param write
         *        - true when writing data to a slave,
         *        - false when reading data from a slave
         *
         * \param start
         *        - = 0: assert start condition before transmission
         *        - = 1: assert stop condition after transmission
         *        - = 2: transmission in between.
         *
         * \param nack
         *        - true when sending nack after receiving (the param
         *          write have to be false therefore!),
         *        - false don't sending a nack
         *
         * \param reply A byte pointer. The pointed location will contain:
         *        - 0x01 if an ACK received after transmitting data and
         *          write was true, or
         *        - 0x00 if a NACK received after transmitting data and
         *          write was true, or
         *        - the read value if write was false
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transfer(byte data, bool write, uint8_t start, bool nack, byte* reply);

        /**
         * \brief Executes a typical single byte write operation
         *
         * \param deviceAddress A 7 bit device address
         *
         * \param registerAddress An 8 bit register address
         *
         * \param data A data byte to written to the bus line.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool writeByte(uint8_t deviceAddress, uint8_t registerAddress, byte data);

        /**
         * \brief Executed a typical single byte read operation
         *
         * \param deviceAddress A 7 bit device address
         *
         * \param registerAddress An 8 bit register address
         *
         * \param data A byte pointer. The pointed location will contain
         *        the read byte
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool readByte(uint8_t deviceAddress, uint8_t registerAddress, byte* data);
};

#endif  // SDK_EASYCORES_I2C_I2C_H_

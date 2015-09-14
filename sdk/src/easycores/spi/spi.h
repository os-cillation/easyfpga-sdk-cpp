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

#ifndef SDK_EASYCORES_SPI_SPI_H_
#define SDK_EASYCORES_SPI_SPI_H_

#include "easycores/easycore.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief An SPI master core with variable clock frequency up to 40 MHz.
 *
 * <b>Features</b>
 *
 * - Compatible with Motorola’s SPI specifications
 * - Enhanced M68HC11 Serial Peripheral Interface
 * - 4 entries deep read FIFO
 * - 4 entries deep write FIFO
 * - Interrupt generation after 1, 2, 3, or 4 transferred bytes
 * - 8 bit WISHBONE RevB.3 Classic interface
 * - Operates from a wide range of input clock frequencies
 * - Static synchronous design
 * - Fully synthesizable
 *
 * \see http://www.urel.feec.vutbr.cz/MPLD/PDF/simple_spi.pdf
 *
 * <b>Usage</b>
 *
 * Before the core can be used, the Spi::init() method has to be called
 * to specify the SPI mode and clock frequency divider.
 *
 * There are three methods for communicating with an SPI slave:
 * - Spi::transceive(),
 * - Spi::transmit() (this method is overloaded for transmitting one
 *   single byte or an array of bytes) and
 * - Spi::receive()
 *
 * SPI generally operates in full-duplex mode: With each clock cycle one
 * bit is transfered from master to slave over the MOSI line and one
 * from slave to master on the MISO line. In case only one direction is
 * of interrest, the method Spi::transmit() or Spi::receive() should be
 * used. In order to transmit and receive at the same time, use the
 * Spi::transceive() method. The SPI communication methods usually work
 * byte-wise.
 *
 * <b>Interrupt handling</b>
 *
 * This easyCore currently supports no interrupts.
 */
class Spi : public EasyCore
{
    public:
        Spi();
        ~Spi();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 7;

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
         */
        enum PIN : PinConst {
            /**
             * \brief SPI clock
             *
             * "SCK is generated by the master device and synchronizes
             * data movement in and out of the device through the MOSI
             * and MISO lines. The SPI clock is generated by dividing
             * the WISHBONE clock. The division factor is programmable."
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            SCK = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief Master out slave in
             *
             * "The Master Out Slave In line is a unidirectional serial
             * data signal. It is an output from a master device and an
             * input to a slave device."
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            MOSI = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1,

            /**
             * \brief Master in slave out
             *
             * "The Master In Slave Out line is a unidirectional serial
             * data signal. It is an output from a slave device and an
             * input to a master device."
             *
             * Type: input / sink
             *
             * \hideinitializer
             */
            MISO = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+2
        };

        /**
         * \brief Defines the addressable registers of this easyCore.
         */
        enum REGISTER : RegisterConst {
            /**
             * Control register
             * <br>Access: read / write
             * <br>Usage purpose: Configures clock rate, phase, polarity
             * and enables interrupts.
             * <br>Initial value: 0x10
             *
             * \hideinitializer
             */
            SPCR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0,

            /**
             * Status register
             * <br>Access: read / write
             * <br>Usage purpose: Contains several flag for indicating
             * the status of the core and its fifos.
             * <br>Initial value: 0x05
             *
             * \hideinitializer
             */
            SPSR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+1,

            /**
             * Serial peripheral data register
             * <br>Access: read / write
             * <br>Usage purpose: Provides access to the send- and receive fifo.
             * <br>Initial value: undefined
             *
             * \hideinitializer
             */
            SPDR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+2,

            /**
             * Serial peripheral extensions register
             * <br>Access: read / write
             * <br>Usage purpose: Contains transfer block size and
             * extended clock rate select.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            SPER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+3
        };

        /* CORE SETTINGS */
        /**
         * \brief Defines the possible serial clock speed modes.
         */
        enum CLOCK_SPEED : int32_t {
            /**
             * SCK clocked at 19531 Hz
             */
            SCK_19531_HZ,

            /**
             * SCK clocked at 39063 Hz
             */
            SCK_39063_HZ,

            /**
             * SCK clocked at 78125 Hz
             */
            SCK_78125_HZ,

            /**
             * SCK clocked at 156250 Hz
             */
            SCK_156250_HZ,

            /**
             * SCK clocked at 312500 Hz
             */
            SCK_312500_HZ,

            /**
             * SCK clocked at 625000 Hz
             */
            SCK_625_KHZ,

            /**
             * SCK clocked at 1250000 Hz
             */
            SCK_1250_KHZ,

            /**
             * SCK clocked at 2500000 Hz
             */
            SCK_2500_KHZ,

            /**
             * SCK clocked at 5 MHz
             */
            SCK_5_MHZ,

            /**
             * SCK clocked at 10 MHz
             */
            SCK_10_MHZ,

            /**
             * SCK clocked at 20 MHz
             */
            SCK_20_MHZ,

            /**
             * SCK clocked at 40 MHz
             */
            SCK_40_MHZ
        };

        /**
         * \brief Select the possible spi transfer modes.
         *
         * CPOL: Means the clock polarity
         * <br>CPOL=0: no clock means low level on line, a clock period
         * starts with 0-1 egde
         * <br>CPOL=1: no clock means high level on line, a clock period
         * starts with 1-0 egde
         *
         * CPHA: clock phase
         * <br>CPHA=0: The master places the information onto the MOSI
         * line a half-cycle before the clock edge.
         * <br>CPHA=1: The master places the information onto the MOSI
         * line at the clock edge.
         */
        enum SPI_MODE : int32_t {
            /**
             * SPI mode 0
             *
             * CPOL = 0, CPHA = 0
             */
            MODE_0,

            /**
             * SPI mode 1
             *
             * CPOL = 0, CPHA = 1
             */
            MODE_1,

            /**
             * SPI mode 2
             *
             * CPOL = 1, CPHA = 0
             */
            MODE_2,

            /**
             * SPI mode 3
             *
             * CPOL = 1, CPHA = 1
             */
            MODE_3
        };

        /**
         * \brief Initializes and enables the core. Clears both FIFOs.
         *
         * \param mode Possible values are from MODE_0 to MODE_3
         *
         * \param speed Possible values are all values of type CLOCK_SPEED
         *
         * \return true if all settings could be successfully set,<br>
         *         false otherwise
         *
         * \see SPI_MODE, CLOCK_SPEED
         */
        bool init(SPI_MODE mode, CLOCK_SPEED speed);

        /* INTERRUPT HANDLING */

        /* SPECIAL CORE METHODS */
        /**
         * \brief Transmits and receives a byte at the same time.
         *
         * \param txData A byte to send
         *
         * \param rxData The pointed location will contain the received
         *        byte
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transceive(byte txData, byte* rxData);

        /**
         * \brief Transmits a byte and discards the received byte.
         *
         * \param txData A byte to send
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transmit(byte txData);

        /**
         * \brief Transmits multiple bytes and discards all received bytes.
         *
         * \param txData An array of bytes to send
         *
         * \param length Number of bytes to send
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transmit(byte* txData, uint8_t length);

        /**
         * \brief Transmits a dummy byte (0x00) in order to receive a byte.
         *
         * \param rxData A byte pointer. The pointed location contains
         *        the received byte.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool receive(byte* rxData);

        /**
         * \brief Flushes the input and output FIFOs.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool resetBuffers(void);

    protected:
        /**
         * \brief Enables transmission and reception.
         */
        bool enableCore(void);

        /**
         * \brief Disables transmission and reception.
         */
        bool disableCore(void);

        /**
         * \brief Checks whether write/transmit FIFO is full
         *
         * \return true if write FIFO is full
         */
        bool isWriteFifoFull(void);

        /**
         * \brief Checks whether write/transmit FIFO is empty
         *
         * \return true if write/transmit FIFO is empty
         */
        bool isWriteFifoEmpty(void);

        /**
         * \brief Number of bytes transmitted without reading from
         *        the receive buffer.
         *
         * Required for the transmit method that only writes to the
         * TX FIFO without reading from the RX FIFO. This results in
         * unwanted bytes remaining in the RX FIFO. For beeing able
         * to align the RX FIFO pointer the next time the user wants
         * to read data, this number is used to determine the number
         * of required dummy reads prior to reading meaningful data.
         */
        uint8_t _transmittedOnly;
};

#endif  // SDK_EASYCORES_SPI_SPI_H_
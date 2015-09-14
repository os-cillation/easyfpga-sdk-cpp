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

#ifndef SDK_EASYCORES_UART_UART_H_
#define SDK_EASYCORES_UART_UART_H_

#include "easycores/easycore.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

#include <string>

/**
 * \brief An 16750 UART with automatic CTS/RTS hardware flow control and
 *        64-byte FIFO buffers.
 *
 * <b>Features</b>
 *
 * The core's internal behavior is comparable to the integrated circuit
 * TL16C750. Please look at the manuel below for an extended feature
 * description.
 *
 * \see http://www.ti.com/lit/ds/symlink/tl16c750.pdf
 *
 * <b>Usage</b>
 *
 * The UART core has to be initialized before beeing used. For this
 * purpose, call the method Uart::init().
 *
 * The UART core is capable of managing hardware CTS/RTS flow control
 * internally. If your application incorporates hardware flow control,
 * call the method Uart::enableAutoHardwareFlowControl().
 *
 * The SDK offers the overloaded Uart::transmit() method to send data
 * through the UART core. These method are able to transmit a single
 * byte or an array of bytes.
 *
 * Reception of data that have been stored in the UART's receive buffer
 * involves the overloaded Uart::receive() method.  In case the given
 * length is greater than the number of characters in the receive buffer,
 * the result will contain trailing binary zeroes.
 *
 * There are two auxiliary outputs that can be used independently of
 * the actual UART.
 *
 * <b>Interrupt handling</b>
 *
 * The currently supported interrupts are shown in Uart::INTERRUPT.
 *
 * Interrupts are managed using the following methods:
 * - Uart::enableInterrupts()
 * - Uart::enableInterrupt()
 * - Uart::disableInterrupts()
 * - Uart::disableInterrupt() and
 * - Uart::identifyInterrupt()
 *
 * The written value to the overgiven pointer to Uart::identifyInterrupt()
 * can be compared to an interrupt type in your interrupt service
 * routine. In case there is no interrupt pending,
 * - this method will return false if the framework works in a
 *   synchronous mode, or
 * - EasyFpga::handleReplies() will return false if the framework works
 *   in an asynchronous mode.
 *
 * The receive buffer trigger level configures how many received words
 * are required to issue an Uart::INTERRUPT::RX_AVAILABLE interrupt.
 * There are four levels allowed: 1, 16, 32 and 56. For convenience you
 * have to use the constants defined in Uart::RX_TRIGGER_LEVEL.
 */
class Uart : public EasyCore
{
    public:
        Uart();
        ~Uart();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 4;

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
             * \brief Serial receiver
             *
             * Type: input / sink
             *
             * \hideinitializer
             */
            RXD = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief Serial transmitter
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            TXD = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1,

            /**
             * \brief Clear to send
             *
             * Type: input / sink
             *
             * \hideinitializer
             */
            CTSn = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+2,

            /**
             * \brief Request to send
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            RTSn = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+3,

            /**
             * \brief Data set ready
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            DSRn = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+4,

            /**
             * \brief Ring indicator
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            RIn = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+5,

            /**
             * \brief Data carrier detect
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            DCDn = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+6,

            /**
             * \brief Data terminal ready
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            DTRn = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+7,

            /**
             * \brief MCR auxiliary 1
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            AUX1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+8,

            /**
             * \brief MCR auxiliary 2
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            AUX2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+9
        };

        /**
         * \brief Defines the addressable registers of this easyCore.
         */
        enum REGISTER : RegisterConst {
            /**
             * Receiver buffer register
             * <br>Access: read only
             * <br>Usage purpose: Access the Rx FIFO (receive buffer).
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            RX = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0,

            /**
             * Transmitter buffer register
             * <br>Access: write only
             * <br>Usage purpose: Access the Tx FIFO (transmit buffer).
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            TX = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+1,

            /**
             * Interrupt enable register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            IER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+2,

            /**
             * Interrupt identification register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            IIR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+3,

            /**
             * FIFO control register
             * <br>Access: write only, bit #5 is protected by the divisor latch access bit.
             * <br>Usage purpose: Affect the interrupt behavior.
             * <br>Initial value: 0x21
             *
             * \hideinitializer
             */
            FCR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+4,

            /**
             * Line control register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            LCR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+5,

            /**
             * Modem control register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            MCR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+6,

            /**
             * Line status register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            LSR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+7,

            /**
             * Modem status register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            MSR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+8,

            /**
             * Scratch register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            SCR = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+9,

            /**
             * Divisor latch (baud rate generator LSB)
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            DLL = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+10,

            /**
             * Divisor latch (baud rate generator MSB)
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            DLM = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+11
        };

        /* CORE SETTINGS */
        /**
         * \brief Defines the possible lengths of the transmitted
         *        characters.
         */
        enum WORD_LENGTH : int32_t {
            /**
             * Transmitted characters are 5 bits wide.
             */
            C5,

            /**
             * Transmitted characters are 6 bits wide.
             */
            C6,

            /**
             * Transmitted characters are 7 bits wide.
             */
            C7,

            /**
             * Transmitted characters are 8 bits wide.
             */
            C8
        };

        /**
         * \brief Defines the hardware parity check possibilities for the
         *        UART.
         */
        enum PARITY : int32_t {
            /**
             * If set in init(), no parity check will be executed.
             */
            NO_PARITY,

            /**
             * If set in init(), am odd parity check will be executed in
             * hardware after each transmission.
             */
            ODD_PARITY,

            /**
             * If set in init(), am even parity check will be executed in
             * hardware after each transmission.
             */
            EVEN_PARITY
        };

        /**
         * \brief Defines the possible stop bits.
         */
        enum STOP_BIT_COUNT : int32_t {
            /**
             * If set in init(), characters will be transmitted with one
             * stop bit.
             */
            ONE_BIT,

            /**
             * If set in init(), characters will be transmitted with 2
             * stop bits.
             */
            TWO_BITS
        };

        /**
         * \brief Sets the baudrate, wordlength, parity and the stop bit
         *        count this UART core.
         *
         * \param baudrate int values from 77 to 5.000.000 (specified in bit/s)
         * \param wLength all values from the enum Uart::WORD_LENGTH
         * \param parity all values from the enum Uart::PARITY
         * \param stopBits all values from the enum Uart::STOP_BIT_COUNT
         *
         * \return true if all settings could be successfully set,<br>
         *         false otherwise
         *
         * \see Uart::setBaudrate()
         */
        bool init(uint32_t baudrate, WORD_LENGTH wLength, PARITY parity, STOP_BIT_COUNT stopBits);

        /**
         * \brief Sets the baudrate of this UART core.
         *
         * \param baudrate possible values from 77bps to 5000000bps
         *        (bps = bits per second)
         *
         * \return true if the baudrate could be successfully set,<br>
         *         false otherwise
         */
        bool setBaudrate(uint32_t baudrate);

        /**
         * \brief Values for the possible fill levels of the RX fifo
         *        buffer for triggering an RX_AVAILABLE interrupt.
         */
        enum RX_TRIGGER_LEVEL : int32_t {
            /**
             * Receive buffer will trigger an RX_AVAILABLE interrupt if
             * it contains 1 byte.
             */
            RX_TRIGGER_LEVEL_1 = 1,

            /**
             * Receive buffer will trigger an RX_AVAILABLE interrupt if
             * it contains 16 byte.
             */
            RX_TRIGGER_LEVEL_16 = 16,

            /**
             * Receive buffer will trigger an RX_AVAILABLE interrupt if
             * it contains 32 byte.
             */
            RX_TRIGGER_LEVEL_32 = 32,

            /**
             * Receive buffer will trigger an RX_AVAILABLE interrupt if
             * it contains 56 byte.
             */
            RX_TRIGGER_LEVEL_56 = 56
        };

        /**
         * \brief Sets the fill level of the RX fifo buffer when an
         *        RX_AVAILABLE interrupt will be triggered.
         *
         * \param level all values from the enum RX_TRIGGER_LEVEL
         *
         * \return true if the RX trigger level could be successfully adjusted,<br>
         *         false otherwise
         *
         * \see RX_TRIGGER_LEVEL
         */
        bool setRxTriggerLevel(RX_TRIGGER_LEVEL level);

        /**
         * \brief Enables the hardware flow control (RTS / CTS handshake).
         *
         * Without a hardware flow control the user have to carry about
         * that a receiver / transmitter will not be overloaded.
         *
         * \return true if the hardware flow control could be successfully set,<br>
         *         false otherwise
         */
        bool enableAutoHardwareFlowControl(void);

        /* INTERRUPT HANDLING */
        /**
         * \brief Defines the diffrent types of interrupts which can be
         *        triggered by this easyCore.
         */
        enum INTERRUPT : InterruptConst {
            /**
             * \brief Receive buffer has reached its trigger level.
             *
             * The level can be influenced with the method setRxTriggerLevel().
             *
             * Clearance: Automatically if buffer drops below the trigger
             * level.
             *
             * Activation needed: yes
             */
            RX_AVAILABLE = 400,

            /**
             * \brief Transmit buffer is empty.
             *
             * Clearance: Automatically through writing to the transmitter
             * buffer register or reading the interrupt identification
             * register.
             *
             * Activation needed: yes
             */
            TX_EMPTY = 401,

            /**
             * \brief An error (parity, data overrun, framing error) or
             *        break occured.
             *
             * Clearance: Automatically by reading the line status
             * register.
             *
             * Activation needed: yes
             */
            RX_LINE_STATUS = 402,

            /**
             * \brief Either CTS, SDR, RI or DCD have changed.
             *
             * Clearance: Automatically by reading the modem status
             * register.
             *
             * Activation needed: yes
             */
            MODEM_STATUS = 403,

            /**
             * \brief Transmit buffer is empty.
             *
             * Clearance: Automatically by reading the receive buffer
             * register.
             *
             * Activation needed: yes (Actually no, but the Uart::init()
             * method disables this interrupt.)
             */
            CHARACTER_TIMEOUT = 404
        };

        /**
         * \brief Enables all interrupt sources.
         *
         * Here, we don't need explicitly calls for clearing interrupts.
         * The hardware will clear an pending automatically if the user
         * executes a certain operation. (What to do if an interrupt was
         * triggered is documented in Uart::INTERRUPT.)
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool enableInterrupts(void);

        /**
         * \brief Enables one certain interrupt source.
         *
         * Here, we don't need explicitly calls for clearing interrupts.
         * The hardware will clear an pending automatically if the user
         * executes a certain operation. (What to do if an interrupt was
         * triggered is documented in Uart::INTERRUPT.)
         *
         * \param interrupt Specifies the interrupt to enable.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool enableInterrupt(INTERRUPT interrupt);

        /**
         * \brief Disables all interrupt sources.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         *
         * \see INTERRUPT
         */
        bool disableInterrupts(void);

        /**
         * \brief Disables one specified interrupt source.
         *
         * \param interrupt Determines the interrupt source to disable.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         *
         * \see INTERRUPT
         */
        bool disableInterrupt(INTERRUPT interrupt);

        /**
         * \brief Detects which interrupt was triggered.
         *
         * \param target An application space decleared pointer. The
         *        pointed location will contain the identified interrupt
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool identifyInterrupt(INTERRUPT* target);

        /* SPECIAL CORE METHODS */
        /**
         * \brief Transmits a single byte.
         *
         * \param b A byte to send through the UART core.

         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transmit(byte b);

        /**
         * \brief Transmits a byte array automatically.
         *
         * \param buffer Pointer to a byte array.
         *
         * \param byteCount Number of bytes to be transmitted.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transmit(byte* buffer, uint32_t byteCount);

        /**
         * \brief Receives a single byte.
         *
         * \param b A byte pointer. The pointed location will contain
         *        the received byte
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool receive(byte* b);

        /**
         * \brief Receives a byte array automatically.
         *
         * In case the given length is greater than the number of
         * characters in the receive buffer, the result will contain
         * trailing zero bytes.
         *
         * \param targetBuffer A byte array pointer. The pointed location
         *        will contain the received byte array
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \param byteCount Specifies the number of bytes to be received.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool receive(byte* targetBuffer, uint32_t byteCount);

        /**
         * \brief Flushes the input and output FIFOs.
         *
         * \return true if the flush was successful,<br>
         *         false otherwise
         */
        bool clearBuffers(void);

        /**
         * \brief Sets a low or high level to the first AUX pin.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool setAuxiliaryOutput1(LogicLevel level);

        /**
         * \brief Sets a low or high level to the second AUX pin.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool setAuxiliaryOutput2(LogicLevel level);

    protected:
        /**
         * \brief Storing the last from the user adjusted RX trigger
         *        level.
         *
         * It's important to remember because the trigger level have to
         * be setted automatically in the clearBuffers method again.
         */
        RX_TRIGGER_LEVEL _rxTriggerLevel;
};

#endif  // SDK_EASYCORES_UART_UART_H_

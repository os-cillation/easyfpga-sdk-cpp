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

#ifndef SDK_EASYCORES_PWM_PWM16_H_
#define SDK_EASYCORES_PWM_PWM16_H_

#include "easycores/easycore.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief A pulse width modulation core with 16-bit resolution.
 *
 * <b>Features</b>
 *
 * No further description neccessary.
 *
 * <b>Usage</b>
 *
 * By default the clock input is connected to an 80 MHz clock. This
 * results in a constant output frequency of 1.22 kHz on the PWM_OUT
 * pin. (The counting cycle is determined by the resolution and the
 * counting frequency by the input clock.) For decreasing the input
 * frequency, a FrequencyDivider core can be connected to the clock input.
 *
 * The duty cycle can be set using the setDutyCycle() method with
 * - 2 bytes,
 * - integer values of range [0, 65535] or
 * - floating values of range [0, 100]
 *
 * which respectively equals to a duty cycle from 0% to 100%.
 *
 * For checking the current set duty cycle the method getDutyCycle()
 * can be used.
 *
 * <b>Interrupt handling</b>
 *
 * This easyCore currently supports no interrupts.
 */
class Pwm16 : public EasyCore
{
    public:
        Pwm16();
        ~Pwm16();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 3;

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

        /* THE CORE'S ACCISSABLE HARDWARE */
        /**
         * \brief Defines the available pins of this easyCore.
         */
        enum PIN : PinConst {
            /**
             * \brief Pulse width modulated output
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            PWM_OUT = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief Optional clock input
             *
             * Equivalent to 80 MHz if left unconnected in your binary
             * description.
             *
             * Type: input / sink
             *
             * \hideinitializer
             */
            CLK_IN = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1
        };

        /**
         * \brief Defines the addressable registers of this easyCore.
         */
        enum REGISTER : RegisterConst {
            /**
             * PWM compare register (low byte)
             * <br>Access: read / write
             * <br>Usage purpose: Holds the duty cycle (LSB).
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            DUTYCYCLE_LOW = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0,

            /**
             * PWM compare register (high byte)
             * <br>Access: read / write
             * <br>Usage purpose: Holds the duty cycle (MSB).
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            DUTYCYCLE_HIGH = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+1
        };

        /* CORE SETTINGS */

        /* INTERRUPT HANDLING */

        /* SPECIAL CORE METHODS */
        /**
         * \brief Sets a 16-bit wide duty cycle.
         *
         * \param lowByte Values of range [0x00, 0xFF]
         *
         * \param highByte Values of range [0x00, 0xFF]
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool setDutyCycle(byte lowByte, byte highByte);

        /**
         * \brief Sets a 16-bit wide duty cycle.
         *
         * \param cycle Integer values of range [0, 65535]
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool setDutyCycle(uint16_t cycle);

        /**
         * \brief Sets a 16-bit wide duty cycle.
         *
         * \param percentage Floating values of range [0.0, 100.0]
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool setDutyCycle(float percentage);

        /**
         * \brief Gets the currently set duty cycle (as two bytes).
         *
         * \param lowByte See highByte
         *
         * \param highByte The pointed location will contain the requested
         *        byte
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool getDutyCycle(byte* lowByte, byte* highByte);

        /**
         * \brief Gets the currently set duty cycle (as an integer value
         *        between 0 and 65535).
         *
         * \param cycle The pointed location will contain the requested
         *        value
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool getDutyCycle(uint16_t* cycle);

        /**
         * \brief Gets the currently set duty cycle (as a floating value
         *        between 0 and 100).
         *
         * \param percentage The pointed location will contain the requested
         *        value
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool getDutyCycle(float* percentage);
};

#endif  // SDK_EASYCORES_PWM_PWM16_H_

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

#ifndef SDK_EASYCORES_FREQUENCYDIVIDER_FREQUENCYDIVIDER_H_
#define SDK_EASYCORES_FREQUENCYDIVIDER_FREQUENCYDIVIDER_H_

#include "easycores/easycore.h"

/**
 * \brief A configurable frequency divider core capable of dividing a
 *        clock signal by an 8-bit integer or deactivating the output.
 *
 * <b>Features</b>
 *
 * No further description neccessary.
 *
 * <b>Usage</b>
 *
 * Leaving the input pin unconnected results in a connection to the
 * global 80 MHz clock.
 *
 * The division factor can be set:
 * - in the range of 0 to 255 using the setDivisor() method,
 * - to 1 by method named bypass(),
 * - to 0 by stopClock() or
 * - to values higher than 255 by chaining some FrequencyDivider cores
 *   in the FPGA description method EasyFpga::defineStructure().
 *
 * <b>Interrupt handling</b>
 *
 * This easyCore currently supports no interrupts.
 */
class FrequencyDivider : public EasyCore
{
    public:
        FrequencyDivider();
        ~FrequencyDivider();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 6;

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
             * \brief Divided or deactivated clock output
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            OUT = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief Clock input
             *
             * Equivalent to 80 MHz if left unconnected in your binary
             * description.
             *
             * Type: input / sink
             *
             * \hideinitializer
             */
            IN = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1
        };

        /**
         * \brief Defines the addressable registers of this easyCore.
         */
        enum REGISTER : RegisterConst {
            /**
             * Clock devisor register
             * <br>Access: read / write
             * <br>Usage purpose: Sets the devision factor of the clock
             * signal.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            DIV = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0
        };

        /* CORE SETTINGS */

        /* INTERRUPT HANDLING */

        /* SPECIAL CORE METHODS */
        /**
         * \brief Sets the clock devisor to 0, which means to stop the
         *        clock signal.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool stopClock(void);

        /**
         * \brief Sets the clock devisor to 1. This leaves the original
         *        clock signal unchanged.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool bypass(void);

        /**
         * \brief Sets a integer clock devisor.
         *
         * \param divisor Integer values between [0, 255]
         *        - 0: has the same effect like calling stopClock(),
         *        - 1: has the same effect like calling bypass(),
         *        - N>1: increases the clock cycle period by a factor N.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool setDivisor(uint8_t divisor);
};

#endif  // SDK_EASYCORES_FREQUENCYDIVIDER_FREQUENCYDIVIDER_H_

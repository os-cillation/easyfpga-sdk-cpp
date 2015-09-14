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

#ifndef SDK_EASYCORES_CAN_CAN_H_
#define SDK_EASYCORES_CAN_CAN_H_

#include "easycores/can/utils/canframe_ptr.h"
#include "easycores/easycore.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

#include <string>

/**
 * \brief A CAN-Bus controller core with an SJA1000 compatible interface.
 *
 * <b>Important note</b>
 *
 * Due to the patents owned by Bosch, the hdl sources of this core are
 * not deployed with the easyFPGA SDK. These can be downloaded from
 * opencores.org. Further note that for commercial use, the purchase
 * of a CAN protocol license is mandatory.
 *
 * For setting up the CAN hardware description sources:
 * -# Create an account and login at opencores.org
 * -# Navigate to the can controllers project page
 * -# Download the cores RTL written in Verilog
 * -# Extract the archive to the \a soc/easy_cores/can directory of your
 *    cloned easyFPGA project.
 *
 * Then you should be able to use the can controller.
 *
 * \see http://opencores.org/project,can
 *
 * <b>Features</b>
 *
 * \todo
 * - When in basic CAN mode, the reception of an extended frame generates,
 *   if enabled, a receive interrupt. This behavior differs from the
 *   SJA1000 specification. Therefore it is recommended to operate the
 *   core in extended mode.
 *
 * \see http://www.nxp.com/documents/data_sheet/SJA1000.pdf for a
 *      detailed feature description of this core.
 *
 * <b>Usage</b>
 *
 * In order to initialize the core, call the Can::init() method setting
 * the mode of the core and passing one of the bitrate constants. If
 * the core should be able to transmit frames in the extended format, it
 * has to be initialized in extended mode.
 *
 * The CAN controller is capable of filtering frames by means of their
 * identifiers. Frames that are filtered will not be stored in the
 * receive buffer and will not cause any interrupts. The filter is
 * configured using acceptance code and acceptance mask by calling
 * - Can::setAcceptanceCode() and
 * - Can::setAcceptanceMask().
 *
 * The acceptance mask defines which bits of the acceptance code
 * (or identifier) should be compared to the identifier of an incoming
 * frame. The bits that are set in the mask are not used for filtering.
 *
 * In basic mode, only the eight most significant bits of the 11-bit
 * identifier are compared for filtering. In extended mode, the entire
 * 9-bit identifier is taken into account.
 *
 * The frames transmitted over the CAN bus are represented by instances
 * of the CanFrame class. The CAN core supports both: the basic- and
 * extended frame format. Furthermore a frame can either carry data or
 * request a transmission from another node on the bus.
 *
 * After constructing a frame, it can be transmitted using the
 * Can::transmit() method. Once the core has received a frame, it can be
 * fetched by calling the method Can::getReceivedFrame().
 *
 * <b>Interrupt handling</b>
 *
 * The currently supported interrupts are shown in Can::INTERRUPT.
 *
 * In order to enable or disable a certain interrupt type, call either
 * Can::enableInterrupt() or Can::disableInterrupt().
 *
 * After the core has issued an interrupt, the method
 * Can::identifyInterrupt() is used to determine the type of pending
 * interrupt.
 */
class Can : public EasyCore
{
    public:
        Can();
        ~Can();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 8;

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
             * \brief Receive data pin
             *
             * Type: input / sink
             *
             * \hideinitializer
             */
            RX = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief Transmit data pin
             *
             * Type: output / source
             *
             * \hideinitializer
             */
            TX = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1
        };

        /**
         * \brief Defines the registers of this easyCore
         *        addressable in the basic mode.
         */
        enum REGISTER_BASIC_MODE : RegisterConst {
            /**
             * Control segment, the control register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "mode" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_CONTROL = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0,

            /**
             * Control segment, the command register
             * <br>Access: write only (reading results in 0xFF)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_COMMAND = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+1,

            /**
             * Control segment, the status register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_STATUS = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+2,

            /**
             * Control segment, the interrupt register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_INTERRUPT = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+3,

            /**
             * Control segment, the acceptance code register
             * <br>Access: read / write (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_ACCEPTANCE_CODE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+4,

            /**
             * Control segment, the acceptance mask register
             * <br>Access: read / write (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_ACCEPTANCE_MASK = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+5,

            /**
             * Control segment, the bus timing register 0
             * <br>Access: read / write (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_BUS_TIMING_0 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+6,

            /**
             * Control segment, the bus timing register 1
             * <br>Access: read / write (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_BUS_TIMING_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+7,

            /**
             * Transmit buffer segment, the TX identifier 1 [10..3]
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_0" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_IDENTIFIER_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+8,

            /**
             * Transmit buffer segment, the TX identifier 2 [2..0] (RTR and DLC)
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_1" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_IDENTIFIER_2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+9,

            /**
             * Transmit buffer segment, data byte 1
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_2" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+10,

            /**
             * Transmit buffer segment, data byte 2
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_3" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+11,

            /**
             * Transmit buffer segment, data byte 3
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_4" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_3 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+12,

            /**
             * Transmit buffer segment, data byte 4
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_5" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_4 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+13,

            /**
             * Transmit buffer segment, data byte 5
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_6" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_5 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+14,

            /**
             * Transmit buffer segment, data byte 6
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_7" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_6 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+15,

            /**
             * Transmit buffer segment, data byte 7
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_8" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_7 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+16,

            /**
             * Transmit buffer segment, data byte 8
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * This register is called "tx_data_9" in "can_registers.v".
             *
             * \hideinitializer
             */
            BM_TX_DATA_8 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+17,

            /**
             * Receive buffer segment, the RX identifier 1 [10..3]
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_IDENTIFIER_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+18,

            /**
             * Receive buffer segment, the RX identifier 2 [2..0] (RTR and DLC)
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_IDENTIFIER_2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+19,

            /**
             * Receive buffer segment, data byte 1
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+20,

            /**
             * Receive buffer segment, data byte 2
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+21,
            /**
             * Receive buffer segment, data byte 3
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_3 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+22,

            /**
             * Receive buffer segment, data byte 4
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_4 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+23,

            /**
             * Receive buffer segment, data byte 5
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_5 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+24,

            /**
             * Receive buffer segment, data byte 6
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_6 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+25,

            /**
             * Receive buffer segment, data byte 7
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_7 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+26,

            /**
             * Receive buffer segment, data byte 8
             * <br>Access: read / write (only in operating mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            BM_RX_DATA_8 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+27,

            /**
             * Clock divider register
             * <br>Access: read / write (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
             BM_CLOCK_DIVIDER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+28
        };

        /**
         * \brief Defines the in the extendend mode addressable registers
         *        of this easyCore.
         */
        enum REGISTER_EXTENDEND_MODE : RegisterConst {
            /**
             * Mode register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_MODE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+50,

            /**
             * Command register
             * <br>Access: write only (reading results in 0x00)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_COMMAND = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+51,

            /**
             * Status register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_STATUS = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+52,

            /**
             * Interrupt flag register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_INTERRUPT = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+53,

            /**
             * Interrupt enable register
             * <br>Access: read / write
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_INTERRUPT_ENABLE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+54,

            /**
             * Bus timing register 0
             * <br>Access: write only (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_BUS_TIMING_0 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+56,

            /**
             * Bus timing register 1
             * <br>Access: write only (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_BUS_TIMING_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+57,

            /**
             * Arbitration lost capture register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_ARBITRATION_LOST_CAPTURE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+58,

            /**
             * Error code capture register
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_ERROR_CODE_CAPTURE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+59,

            /**
             * Error warning limit register
             * <br>Access: write only (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_ERROR_WARNING_LIMIT = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+60,

            /**
             * RX error counter register
             * <br>Access: write only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_RX_ERROR_COUNTER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+61,

            /**
             * TX error counter register
             * <br>Access: write only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_TX_ERROR_COUNTER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+62,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             *  - reset mode read / write: get / set acceptance code 0
             *  - operation mode read: RX frame information
             *  - operation mode write: TX frame information
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_FRAME_INFORMATION = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+63,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance code 1
             * - operation mode read: RX identifier 1
             * - operation mode write: RX identifier 1
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_IDENTIFIER_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+64,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance code 2
             * - operation mode read: RX identifier 2
             * - operation mode write: TX identifier 2
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_IDENTIFIER_2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+65,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance code 3
             * - operation mode SFF read: RX data 1
             * - operation mode SFF write: TX data 1
             * - operation mode EFF read: TX identifier 3
             * - operation mode EFF write: TX identifier 3
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+66,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance mask 0
             * - operation mode SFF read: RX data 2
             * - operation mode SFF write: TX data 2
             * - operation mode EFF read: TX identifier 4
             * - operation mode EFF write: TX identifier 4
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+67,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance mask 1
             * - operation mode SFF read: RX data 3
             * - operation mode SFF write: TX data 3
             * - operation mode EFF read: RX data 1
             * - operation mode EFF write: TX data 1
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_3 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+68,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance mask 2
             * - operation mode SFF read: RX data 4
             * - operation mode SFF write: TX data 4
             * - operation mode EFF read: RX data 2
             * - operation mode EFF write: TX data 2
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_4 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+69,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - reset mode read / write: get / set acceptance mask 3
             * - operation mode SFF read: RX data 5
             * - operation mode SFF write: TX data 5
             * - operation mode EFF read: RX data 3
             * - operation mode EFF write: TX data 3
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_5 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+70,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - operation mode SFF read: RX data 6
             * - operation mode SFF write: TX data 6
             * - operation mode EFF read: RX data 4
             * - operation mode EFF write: TX data 4
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_6 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+71,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - operation mode SFF read: RX data 7
             * - operation mode SFF write: TX data 7
             * - operation mode EFF read: RX data 5
             * - operation mode EFF write: TX data 5
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_7 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+72,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - operation mode SFF read: RX data 8
             * - operation mode SFF write: TX data 8
             * - operation mode EFF read: RX data 6
             * - operation mode EFF write: TX data 6
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_8 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+73,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - operation mode EFF read: RX data 7
             * - operation mode EFF write: TX data 7
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_9 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+74,

            /**
             * This register has lots of opportunities.
             * <br>Access: read / write
             * <br>Usage purpose:
             * - operation mode EFF read: RX data 8
             * - operation mode EFF write: TX data 8
             *
             * Initial value: TODO
             *
             * \hideinitializer
             */
            EM_DATA_10 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+75,

            /**
             * RX frame counter
             * <br>Access: read only
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_RX_MESSAGE_COUNTER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+76,

            /**
             * RX buffer start address
             * <br>Access: write only (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_RX_BUFFER_START_ADDRESS = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+77,

            /**
             * Clock divider register
             * <br>Access: write only (only in reset mode)
             * <br>Usage purpose: TODO
             * <br>Initial value: TODO
             *
             * \hideinitializer
             */
            EM_CLOCK_DIVIDER = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+78,
        };

        /* CORE SETTINGS */
        /**
         * \brief Defines the possible bitrate constants.
         */
        enum BITRATE : int32_t {
            /**
             * 125 Kbit/s
             */
            BITRATE_125K,

            /**
             * 250 Kbit/s
             */
            BITRATE_250K,

            /**
             * 1 Mbit/s
             */
            BITRATE_1M
        };

        /**
         * \brief Defines the possible CAN usage modes.
         *
         * Please take a look to
         * http://www.nxp.com/documents/data_sheet/SJA1000.pdf if you
         * wish more information about the possible usage modes.
         */
        enum USAGE_MODE : int32_t {
            /**
             * Not yet initialized.
             */
            UNDEFINED,

            /**
             * Basic mode
             */
            BASIC_MODE,

            /**
             * Extendend mode
             */
            EXTENDEND_MODE
        };

        /**
         * \brief Initializes and enables the core with a specific bit
         *        rate and usage mode.
         *
         * \param bitrate all values of enum Can::BITRATE
         * \param mode all values of enum Can::USAGE_MODE
         *
         * \return true if all settings could be successfully set,<br>
         *         false otherwise
         */
        bool init(BITRATE bitrate, USAGE_MODE mode);

        /**
         * \brief Returns the currently configured usage mode of this
         *        CAN core.
         *
         * \return A value of enum Can::USAGE_MODE
         */
        USAGE_MODE getUsageMode(void);

        /* INTERRUPT HANDLING */
        /**
         * \brief Defines the different types of interrupts which can be
         *        triggered by this easyCore.
         */
        enum INTERRUPT : InterruptConst {
            /**
             * \brief A bus error occured. <b>Extended mode only!</b>
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed:
             * - basic mode: not available
             * - extended mode: yes
             */
            BUS_ERROR = 800,

            /**
             * \brief Arbitration lost <b>Extended mode only!</b>
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed:
             * - basic mode: not available
             * - extended mode: yes
             */
            ARBITRATION_LOST = 801,

            /**
             * \brief The error status changes from error active to error
             *        passive or vice-versa. <b>Extended mode only!</b>
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed:
             * - basic mode: not available
             * - extended mode: yes
             */
            ERROR_PASSIVE = 802,

            /**
             * \brief The sleep mode has been left.
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed:
             * - basic mode: no, always activated
             * - extended mode: yes
             */
            WAKE_UP = 803,

            /**
             * \brief An incoming frame has been lost due to a receive
             *        buffer overrun.
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed: yes
             */
            DATA_OVERRUN = 804,

            /**
             * \brief Either error status or bus status bit has changed.
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed: yes
             */
            ERROR = 805,

            /**
             * \brief The transmit buffer is not locked anymore.
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed: yes
             */
            TRANSMIT = 806,

            /**
             * \brief There is data in the receive buffer.
             *
             * Clearance: All pending interrupts will be cleared by
             * reading the BM_INTERRUPT / EM_INTERRUPT register.
             *
             * Activation needed: yes
             */
            RECEIVE = 807
        };

        /**
         * \brief Enables all interrupt sources.
         *
         * Here, we don't need explicit calls for clearing interrupts.
         * The hardware will clear a pending interrupt automatically
         * when the user executes a certain operation. What to do if an
         * interrupt has been triggered is documented in Can::INTERRUPT.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         *
         * \see INTERRUPT
         */
        bool enableInterrupts(void);

        /**
         * \brief Enables a single interrupt source.
         *
         * Here, we don't need explicit calls for clearing interrupts.
         * The hardware will clear a pending interrupt automatically
         * when the user executes a certain operation. What to do if an
         * interrupt has been triggered is documented in Can::INTERRUPT.
         *
         * \param interrupt Determines the interrupt source to enable.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         *
         * \see INTERRUPT
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
         * \brief Disables a single interrupt source.
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
         * \brief Detects which interrupt has been triggered.
         *
         * \param target An application space decleared pointer. The
         *        pointed location will contain the identified interrupt
         *        - after this method has returned if the framework works in
         *          synchronous mode, or
         *        - after calling EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool identifyInterrupt(INTERRUPT* target);

        /* SPECIAL CORE METHODS */
        /**
         * \brief Stores a CAN frame in send queue which will be
         *        transmitted over the bus.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool transmit(canframe_ptr frame);

        /**
         * \brief Gets a frame that has been received.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool getReceivedFrame(canframe_ptr& frame);

        /**
         * \brief Enter the sleep mode if no interrupt is pending and
         *        there is no bus activity.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool goToSleep(void);

        /**
         * \brief Exit sleep mode and continue operation.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool wakeUp(void);

        /**
         * \brief Configures the core's acceptance code that filters - in
         *        conjunction with the acceptance mask - frames before
         *        being stored in the receive buffer.
         *
         * In basic mode, the eight most significant bits of the frame's
         * identifier must be equal to the acceptance code for acceptance.
         *
         * \param code
         *        - basic mode: 8 bits will be used (0 .. 0xFF)
         *        - extended mode: 29 bits will be used (0 .. 0x1FFFFFFF)
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool setAcceptanceCode(uint32_t code);

        /**
         * \brief Sets a mask that qualifies which of the corresponding
         *        bits of the acceptance code are relevant.
         *
         * Bits that are set in the mask are not used for filtering.
         *
         * \param mask
         *        - basic mode: 8 bits will be used (0 .. 0xFF)
         *        - extended mode: 29 bits will be used (0 .. 0x1FFFFFFF)
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool setAcceptanceMask(uint32_t mask);

        /**
         * \brief Reads the status register and write its content to the
         *        log.
         *
         * \return true if the exchange could be processed successfully,<br>
         *         false otherwise
         */
        bool logStatus(void);

    protected:
        /**
         * \brief Stores the last CAN usage mode of this core set by the
         *        user.
         */
        USAGE_MODE _mode;

    private:
        bool enterResetMode(void);
        bool enterOperationMode(void);

        bool setBusTiming(
            uint8_t prescaler,
            uint8_t syncronizationJumpWidth,
            bool sampling,
            uint8_t timeSegment1,
            uint8_t timeSegment2
        );
};

#endif  // SDK_EASYCORES_CAN_CAN_H_

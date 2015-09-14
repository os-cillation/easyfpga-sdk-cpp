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

#ifndef SDK_EASYCORES_GPIO_GPIO8_H_
#define SDK_EASYCORES_GPIO_GPIO8_H_

#include "easycores/easycore.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief A versatile GPIO core with 8 pins that can either be used as
 *        in- or outputs.
 *
 * <b>Features</b>
 *
 * This easyCore offers 8 general purpose I/O pins to be directly
 * connected to hardware pins only within EasyFpga::defineStructure().
 *
 * <b>Usage</b>
 *
 * During runtime, either a single or all pins can be configured either
 * as out- or inputs by:
 * - Gpio8::makePinInput()
 * - Gpio8::makeAllPinsInput()
 * - Gpio8::makePinOutput()
 * - Gpio8::makeAllPinsOutput()
 *
 * After that, you can get / set the levels of from either one or all pins
 * togehter by means of:
 * - Gpio8::setPin()
 * - Gpio8::setAllPins()
 * - Gpio8::getPin()
 * - Gpio8::getAllPins(byte* level)
 *
 * <b>Interrupt handling</b>
 *
 * Input pins are capable of generating interrupts. (Actually output
 * pins too...)
 *
 * First, setup the pins which should trigger interrupts. You have to
 * specify on which edge interrupts should be generated (falling or
 * rising edge).
 *
 * For activating interrupts:
 * -# Implement an interrupt service routine (ISR) and register it to the
 *    framework by calling EasyCore::registerCallback() of the
 *    corresponding easyCore instance.
 *    <br>Note: Since you can register only one ISR per core, you have
 *    to determine within your ISR which pins are responsible for that
 *    interrupt notification. The method
 *    Gpio8::identifyInterruptingPins() might help you.
 * -# Enable interrupt triggering for this easyCore with
 *    Gpio8::releaseInterrupts().
 * -# Let the FPGA sending interrupt notifications to the host by
 *    calling EasyFpga::enableInterrupts().
 *
 * Your ISR should do at least the following things:
 * - Call EasyFpga::enableInterrupts() to allow the FPGA sending more
 *   interrupt notifications.
 * - Clear the corresponding interrupts of all processed pins. (Or of
 *   all pins at the same time for decreasing the count of communication
 *   operations.)
 *
 * Once an interrupt was generated and a notification was sent from the
 * FPGA to the host, your registered ISR will be invoked by calling
 * EasyFpga::handleReplies().
 */
class Gpio8 : public EasyCore
{
    public:
        Gpio8();
        ~Gpio8();

        /* HDL DESCRIPTION */
        static const CoreIndex UNIQUE_CORE_NUMBER = 1;

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
             * \brief General purpose I/O 0
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO0 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+0,

            /**
             * \brief General purpose I/O 1
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO1 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+1,

            /**
             * \brief General purpose I/O 2
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO2 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+2,

            /**
             * \brief General purpose I/O 3
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO3 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+3,

            /**
             * \brief General purpose I/O 4
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO4 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+4,

            /**
             * \brief General purpose I/O 5
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO5 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+5,

            /**
             * \brief General purpose I/O 6
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO6 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+6,

            /**
             * \brief General purpose I/O 7
             *
             * Type: dependent on setting either input / sink or output / source
             *
             * \hideinitializer
             */
            GPIO7 = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+7
        };

        /**
         * \brief Defines the addressable registers of this easyCore.
         */
        enum REGISTER : RegisterConst {
            /**
             * GPIO input register
             * <br>Access: read only
             * <br>Usage purpose: Get level of all as input pins
             * declared gpio pins.
             * <br>Initial value: undefined
             *
             * \hideinitializer
             */
            IN = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+0,

            /**
             * GPIO output register
             * <br>Access: write only
             * <br>Usage purpose: Set level of all as output pins
             * declared gpio pins.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            OUT = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+1,

            /**
             * Output enable register
             * <br>Access: read / write
             * <br>Usage purpose: Defines which gpio pin operates as
             * input or output pin. When bit is set, the corresponding
             * pin will act as an output.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            OE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+2,

            /**
             * Interrupt enable register
             * <br>Access: read / write
             * <br>Usage purpose: Enables interrupt generation of the
             * corresponding gpio pin configured as an input.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            INTE = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+3,

            /**
             * Edge trigger register
             * <br>Access: read / write
             * <br>Usage purpose: Adjusts the type of edge that triggers
             * an interrupt for all 8 gpio pins. A set bit implies an
             * interrupt on a rising edge and a cleared bit on a falling
             * edge.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            PTRIG = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+4,

            /**
             * Global control register
             * <br>Access: read / write
             * <br>Usage purpose: Enables interrupt triggering for this
             * core and allows checking if any interrupt is pending.
             * - Bit #0: If set, the interrupts are enabled
             * - Bit #1: If set, any interrupt is pending (determine the
             *   respective interrupt(s) with REGISTER::INTS)
             *
             * Initial value: 0x00
             *
             * \hideinitializer
             */
            CTRL = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+5,

            /**
             * Interrupt status register
             * <br>Access: read / write
             * <br>Usage purpose: Indicates that the nth input has
             * caused an interrupt if the nth bit is set.
             * <br>Initial value: 0x00
             *
             * \hideinitializer
             */
            INTS = UNIQUE_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT+6
        };

        /* CORE SETTINGS */
        /**
         * \brief Tests if a single pin is decleared as an input pin.
         *
         * \param pin A pin const for specifing the pin to be tested.
         *
         * \param target The pointed location will contain the requested
         *        boolean
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         *
         * @see PIN
         */
        bool isPinInput(PIN pin, bool* target);

        /**
         * \brief Tests if a all pins are decleared as input pins.
         *
         * \param target The pointed location will contain the requested
         *        boolean
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool arePinsInput(bool* target);

        /**
         * \brief Declares a single pin as input pin.
         *
         * \param pin A pin const for specifing the pin to be decleared.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         *
         * @see PIN
         */
        bool makePinInput(PIN pin);

        /**
         * \brief Sets all pins as input pins.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool makeAllPinsInput(void);

        /**
         * \brief Declares a single pin as output pin.
         *
         * \param pin A pin const for specifing the pin to be decleared.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         *
         * @see PIN
         */
        bool makePinOutput(PIN pin);

        /**
         * \brief Sets all pins as output pins.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool makeAllPinsOutput(void);

        /* INTERRUPT HANDLING */
        /**
         * \brief Defines the different types of interrupts which can be
         *        triggered by this easyCore.
         */
        enum INTERRUPT : InterruptConst {
            /**
             * An input pin has caused an interrupt.
             * <br>Clearance: Calling either the clearInterrupt() method
             * with the pin that caused the interrupt or by calling the
             * clearInterrupts() method to clear the interrupts of all
             * pins. (Otherwise no more interrupts will be recognized
             * for this pin.)
             * <br>Activation needed: yes
             */
            GPIO_PIN_CHANGED = 100
        };

        /**
         * \brief Enables interrupt triggering for this easyCore.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool releaseInterrupts(void);

        /**
         * \brief Disables interrupt triggering for this easyCore.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool lockInterrupts(void);

        /**
         * \brief Enables all pins at a certain trigger edge as possible
         *        interrupt source.
         *
         * \param risingEdge true for trigger interrupts when rising
         *        egdes occured,<br>
         *        false for trigger on falling edges
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool enableAllPinInterrupts(bool risingEdge);

        /**
         * \brief Enables one specified pin as interrupt source at a
         *        futhermore specified trigger edge.
         *
         * \param pin A value from Gpio8::PIN determining the pin for
         *        which the interrupt should be emabled.
         *
         * \param risingEdge true for trigger interrupts on rising
         *        egdes of this pin,<br>
         *        false if interrupts should be triggered on falling
         *        edges
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool enablePinInterrupt(PIN pin, bool risingEdge);

        /**
         * \brief Disables all pins as interrupt sources.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool disableAllPinInterrupts(void);

        /**
         * \brief Disables one certain pin as interrupt source.
         *
         * \param pin A value from Gpio8::PIN for the pin the interrupt
         *        should be disabled.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool disablePinInterrupt(PIN pin);

        /**
         * \brief Clears all pending interrupts.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool clearInterrupts(void);

        /**
         * \brief Clears the pending interrupt from one pin.
         *
         * \param pin Determines the interrupting pin to be cleared.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool clearInterrupt(PIN pin);

        /**
         * \brief Detects which pin(s) causes an interrupt.
         *
         * \param pins A list of interrupting pins which will contain
         *        the requested pin constants
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool identifyInterruptingPins(std::list<PIN>& pins);

        /* SPECIAL CORE METHODS */
        /**
         * \brief Sets the logic level of a specified output pin.
         *
         * \param pin Specifies a pin constant from Gpio8::PIN
         *
         * \param level HIGH or LOW
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool setPin(PIN pin, LogicLevel level);

        /**
         * \brief Sets the logic level of all pins if there were
         *        configured as output pins.
         *
         * \param level Byte values of range [0x00, 0xFF]
         *        (The nth bit of parameter level sets the corresponding
         *        nth pin if this was an output pin.)
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool setAllPins(byte level);

        /**
         * \brief Gets the logic level of a specified input pin.
         *        (Outputs possible too.)
         *
         * \param pin Specifies a pin constant from Gpio8::PIN
         *
         * \param level The pointed location will contain the requested
         *        logic level
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool getPin(PIN pin, LogicLevel* level);

        /**
         * \brief Gets the logic level of all pins.
         *        (Outputs possible too.)
         *
         * \param level The pointed location will contain all requested
         *        logic levels as byte
         *        - after this method call if the framework works in
         *          synchronous mode, or
         *        - after call of EasyFpga::handleReplies() if the
         *          framework works in asynchronous mode.
         *
         * \return true if this exchange could be processed successfully,\n
         *         false otherwise
         */
        bool getAllPins(byte* level);
};

#endif  // SDK_EASYCORES_GPIO_GPIO8_H_

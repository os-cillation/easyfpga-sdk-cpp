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

#ifndef SDK_EASYCORES_EASYCORE_H_
#define SDK_EASYCORES_EASYCORE_H_

#include "communication/communicator_ptr.h"
#include "easycores/pin_ptr.h"
#include "easycores/register_ptr.h"
#include "easycores/types.h"

#include <map>
#include <list>
#include <string>

/**
 * \brief Defines the basic functionality of an easyCore.
 *
 * Each easyCore has a list of its own pins and registers.
 *
 * With the Pin instances the user can connect the core's pins to other
 * pins of
 * - the same core,
 * - other cores or
 * - to the EasyFpga's GPIO pins.
 *
 * These connect information will be evaluated and analyzed by the
 * framework at compile time of the user application.
 *
 * At runtime, the framework communicates with an easyCore using the read
 * and write methods read of the Register instances. The high level methods
 * of the inherited classes implement this communication logic by a
 * sequence of read / write operations on the easyCore's registers.
 *
 * Note: Since this is an abstract class, an user can not instantiate
 * this basic class directly, only indirectly through the inherited
 * classes like Gpio8, Pwm8 or Uart.
 *
 * <b>For Developerss:</b> Each easyCore has to inherit from EasyCore
 * and has to implement the following aspects:
 * - the hdl entity files to be injected in the binary build process
 *   (which contains the main structural information about this core) by
 *   implementing the member getHdlInjections(),
 * - the hdl definition of the top level enitity (setable parameters
 *   for the injected hdl templates in the build process) by
 *   implementing the member getHdlTemplate(),
 * - in the constructor: the user accessible set of pins and registers as
 *   well as the generic hdl map of this easyCore and
 * - the core specific logic (a set of core class variables and
 *   functions - called "specific core methods").
 */
class EasyCore
{
    public:
        /**
         * \brief Creates an easyCore instance associated with the creation
         *        of all pins, registers and the generic hdl map.
         *
         * \param uniqueNumber An unique number for every different kinds
         *        of easyCores. (Defined by the subclasses.)
         */
        EasyCore(const CoreIndex uniqueNumber);
        ~EasyCore();

        /**
         * \brief Returns an unique core id of this type of easyCore.
         */
        const CoreIndex& getTheGlobalCoreNumber(void);

        /**
         * \brief Sets an unique core index.
         */
        void setIndex(CoreIndex index);

        /**
         * \brief Returns an unique core index.
         */
        CoreIndex getIndex(void);

        /**
         * \brief Sets the reference to the high level communication
         *        interface used by the core's associated registers.
         */
        void setCommunicator(communicator_ptr c);

        /**
         * \brief Returns a reference to the high level communication
         *        interface.
         */
        const communicator_ptr& getCommunicator(void);

        /**
         * \brief Returns a given pin associated with this easyCore.
         *
         * \return a shared pointer if the pin instance was found, or<br>
         *         nullptr otherwise
         */
        pin_ptr getPin(PinConst pin);

        /**
         * \brief Returns all pins associated with this easyCore.
         */
        std::list<pin_ptr> getAllPins(void);

        /**
         * \brief Returns all to this easyCore asscociated pins with an
         *        specific type.
         *
         * @see PIN_DIRECTION_TYPE
         */
        std::list<pin_ptr> getPinsWithType(PIN_DIRECTION_TYPE type);

        /**
         * \brief Returns a given register asscociated with this easyCore.
         *
         * \return a shared pointer if the pin instance was found, or<br>
         *         nullptr otherwise
         */
        register_ptr getRegister(RegisterConst reg);

        /**
         * \brief Should return a list of hdl source file injections as
         *        a string
         */
        virtual std::string getHdlInjections(void) = 0;

        /**
         * \brief Should return the core's specific parameterized hdl
         *        template.
         */
        virtual std::string getHdlTemplate(void) = 0;

        /**
         * \brief Returns a generic hdl map.
         *
         * Under certain circumstances it's neccessary to enable a certain
         * hdl setting. According to this settings, the hdl compiler will
         * translating some hdl description different.
         *
         * \return A map of a list of generic options assigned to an
         *         easyCore's pin.
         */
        generic_hdl_map getGenericMap(void);

        /**
         * \brief Sets a function pointer to an interrupt service routine
         *        that will be invoked if this core generated an interrupt.
         *
         * Per core, only one interrupt service routine is supported. Thus,
         * the user has to determine the kind of interrupt and perform
         * corresponding actions in this routine. Note, that global interrupts
         * will be disabled after an interrupt and have to be re-enabled by
         * calling the EasyFpga::enableInterrupts() method.
         *
         * \param functionPointer Any user's space defined function.
         *
         * \return true if the function pointer could be assigned successfully,<br>
         *         false otherwise (e.g. if an other pointer assigned before)
         */
        bool registerCallback(interrupt_callback_function functionPointer);

        /**
         * \brief Calls the interrupt service routine assigned by the
         *        registerCallback() method.
         *
         * \return true if interrupt service routine could be executed
         *         successfully,<br>
         *         false otherwise (e.g. if no pointer assigned before)
         */
        bool executeCallback(void);

    protected:
        /**
         * \brief An unique number for every different kinds of easyCores.
         *
         * Currently useful for:
         * - To decide at the generation process how often the hdl
         *   injections have to be included. (Only neccessary one time
         *   per different easyCore.)
         * - To check, if the user uses the correct core's pin and register
         *   constants.
         */
        const CoreIndex _GLOBAL_CORE_NUMBER;

        /**
         * \brief Holds the framework's operation mode currently set.
         *
         * Constant which will be instantiated in the constructor
         * by parsing the configuration file, thus the user should not
         * set this member manually.
         *
         * @see OPERATION_MODE
         */
        const OPERATION_MODE _OPERATION_MODE;

        /**
         * \brief The global core's index number assigned by addEasyCore()
         *        at runtime.
         *
         * @see CoreIndex
         */
        CoreIndex _index;

        /**
         * \brief The core's reference to the communication interface.
         */
        communicator_ptr _communicator;

        /**
         * \brief Stores a reference to an user's space decleared interrupt
         *        service routine.
         *
         * @see interrupt_callback_function
         */
        interrupt_callback_function _callback;

        /**
         * \brief Holds the core's pins.
         *
         * @see the class's constructor EasyCore()
         */
        std::map<PinConst, pin_ptr> _pinMap;

        /**
         * \brief Holds the core's registers.
         *
         * @see the class's constructor EasyCore()
         */
        std::map<RegisterConst, register_ptr> _registerMap;

        /**
         * \brief Holds a core specific generic hdl map.
         *
         * @see the class's constructor EasyCore()
         */
        generic_hdl_map _genericMap;
};

#endif  // SDK_EASYCORES_EASYCORE_H_

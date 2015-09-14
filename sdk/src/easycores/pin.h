/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Author: Johannes Hein <support@os-cillation.de>
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

#ifndef SDK_EASYCORES_PIN_H_
#define SDK_EASYCORES_PIN_H_

#include "easycores/pin_ptr.h"
#include "easycores/types.h"

#include <string>

/**
 * \brief Represents a pin of an easyCore.
 *
 * This class is currently used for:
 * - offering connect() methods of the easyCore's and the EasyFpga's
 *   pins and
 * - replacing the placeholders of the hdl templates with the gained
 *   information from the connecting process.
 */
class Pin : public std::enable_shared_from_this<Pin>
{
    public:
        Pin(std::string name, CoreIndex* core, PinConst pinNumber, PIN_DIRECTION_TYPE type);
        Pin(const Pin& pin);
        ~Pin();

        /**
         * \brief Returns the hdl name of this pin.
         */
        std::string getName(void);

        /**
         * \brief Returns the hdl name of this pin for logging purposes.
         */
        std::string getLogName(void);

        /**
         * \brief Returns the index from the assigned EasyCore.
         */
        const CoreIndex* const getCoreIndex(void);

        /**
         * \brief Returns the pin's assigned pin constant.
         */
        const PinConst getPinNumber(void);

        /**
         * \brief Returns the pin's direction type.
         */
        PIN_DIRECTION_TYPE getType(void);

        /**
         * \brief Sets the direction of this pin.
         */
        bool setType(PIN_DIRECTION_TYPE type);

        /**
         * \brief Checks if this pin has the specified direction.
         *
         * \return true if the core's direction equals the specified
         *         direction, or<br>
         *         false otherwise
         */
        bool hasType(PIN_DIRECTION_TYPE type);

        /**
         * \brief Checks if this pin is already backward connected to
         *        another pin.
         *
         * \return true if this pin is backward connected to any pin, or<br>
         *         false otherwise
         */
        bool isBackwardConnected(void);

        /**
         * \brief Connects this pin to another pin.
         *
         * \return true if all pin connecting constraints will be
         *         statisfied and this pin wasn't connted to another pin
         *         before, or<br>
         *         false otherwise
         */
        bool connectTo(pin_ptr pin);

        /**
         * \brief Returns a reference to the connected pin.
         */
        pin_ptr getConnection(void);

        /**
         * \brief Checks if this pin is backward connected to the
         *        specified pin.
         *
         * \return true if this pin is backward connected to the
         *         specified pin, or<br>
         *         false otherwise
         */
        bool isBackwardConnectedTo(pin_ptr target);

    protected:
        /**
         * \brief Holds the hdl pin name.
         */
        const std::string _name;

        /**
         * \brief Holds the assigned easyCore.
         *
         * This member have to be pointer because the coreIndex changes
         * with the addEasyCore() method.
         */
        CoreIndex* _coreIndex;

        /**
         * \brief Holds the assigned pin constant.
         */
        const PinConst _pinNumber;

        /**
         * \brief Holds the pin's direction type.
         */
        PIN_DIRECTION_TYPE _type;

        /**
         * \brief Stores the information whether this pin is connected
         *        to another pin or not.
         */
        bool _isBackwardConnected;

        /**
         * \brief Stores a reference to the connected pin.
         */
        pin_ptr _backwardConnection;
};

#endif  // SDK_EASYCORES_PIN_H_

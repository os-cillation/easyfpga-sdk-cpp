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

#ifndef SDK_EASYCORES_GPIOPIN_H_
#define SDK_EASYCORES_GPIOPIN_H_

#include "easycores/pin.h"
#include "easycores/types.h"

#include <string>

/**
 * \brief Represents a gpio pin of the easyFPGA board.
 *
 * The gpio pins are stored in the class EasyFpga.
 */
class GpioPin : public Pin
{
    public:
        GpioPin(std::string name, CoreIndex* core, PinConst pinNumber, PIN_DIRECTION_TYPE type, uint8_t bank, uint8_t pin);
        GpioPin(const GpioPin& gpioPin);
        ~GpioPin();

        /**
         * \brief Returns the bank number.
         */
        uint8_t getBankNumber(void);

        /**
         * \brief Returns the pin number.
         */
        uint8_t getPinNumber(void);

    protected:
        /**
         * \brief Holds the to this GPIO pin associated bank.
         */
        uint8_t _bank;

        /**
         * \brief Holds the pin number of the bank.
         */
        uint8_t _pin;
};

#endif  // SDK_EASYCORES_GPIOPIN_H_

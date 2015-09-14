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

#ifndef SDK_EASYCORES_GPIO_CALLBACKS_INPUTTEST_H_
#define SDK_EASYCORES_GPIO_CALLBACKS_INPUTTEST_H_

#include "easycores/callback.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

class Gpio8InputTestCallback : public Callback
{
    public:
        Gpio8InputTestCallback(PinConst pin, bool all, bool* resultTarget);
        ~Gpio8InputTestCallback();

        bool call(void);

    private:
        PinConst _pin;
        bool _all;
        bool* _resultTarget;
};

#endif  // SDK_EASYCORES_GPIO_CALLBACKS_INPUTTEST_H_

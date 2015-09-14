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

#ifndef SDK_EASYCORES_UART_CALLBACKS_INITMETHOD_H_
#define SDK_EASYCORES_UART_CALLBACKS_INITMETHOD_H_

#include "easycores/callback.h"
#include "easycores/uart/uart_fwd.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

/* init fifo control register (enable 64 byte fifo) */
class UartInitiationCallback : public Callback
{
    public:
        UartInitiationCallback(Uart* core);
        ~UartInitiationCallback();

        bool call(void);

    private:
        Uart* _core;
};

#endif  // SDK_EASYCORES_UART_CALLBACKS_INITMETHOD_H_

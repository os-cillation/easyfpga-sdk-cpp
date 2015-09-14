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

#ifndef SDK_COMMUNICATION_PROTOCOL_SPECIFICATION_H_
#define SDK_COMMUNICATION_PROTOCOL_SPECIFICATION_H_

/*
 * Makes furthermore protocol specifications which do not concern one
 * special exchange.
 */

#include "communication/types.h" /* timeoutval */

/**
 * \brief A receive timeout for the device detection procedure.
 *
 * (All other operation specific timeouts are defined in thier respective
 * classes.)
 */
const timeoutval DETECT_TIMEOUT = 200000; // = 200 ms

/**
 * \brief Declares a waiting time between a mcu is configuring reply and
 *        a new detect request.
 *
 * See "DETECT PROCEDURE" in the documentation for more information.
 */
const timeoutval WAITING_TIME_BETWEEN_DETECT_REQUESTS = 500000; // = 500 ms

/**
 * \brief Declares a waiting time between an soc-mcu or vice versa context
 *        switch.
 *
 * If we would send another request directly after a context switch request,
 * the fpga could not recognize this request while switching. (So we have
 * to wait some time until it becomes ready.) Therefore, this definition
 * declares the neccessary waiting time after switch requests.
 */
const timeoutval WAITING_TIME_AFTER_SWITCH = 1000; // = 1 ms

/**
 * \brief Declares an amount of time to wait while flushing the serial
 *        interface.
 *
 * The usb fifo buffer of the easyFPGA board can hold some bytes for an
 * maximum amount of 20 ms. So this constant contains a sufficently large
 * timeout value to ensure that no bytes can arrive meanwhile.
 */
const timeoutval FLUSHING_TIMEOUT = 50000; // = 50 ms

#endif  // SDK_COMMUNICATION_PROTOCOL_SPECIFICATION_H_

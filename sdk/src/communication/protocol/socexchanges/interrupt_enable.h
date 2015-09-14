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

#ifndef SDK_COMMUNICATOR_PROTOCOL_SOCEXCHANGES_INTERRUPTENABLE_H_
#define SDK_COMMUNICATOR_PROTOCOL_SOCEXCHANGES_INTERRUPTENABLE_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"

/**
 * \brief Defines an soc operation for a global release of all interrupts.
 *        (An soc context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 *
 * The fpga can only trigger interrupts after execution of this InterruptEnable
 * exchange. All interrupt sources of all cores will be activated at the
 * same time. Maybe there will be more fine-grained interrupt settings at
 * the corresponding easyCore.
 *
 * After each interrupt tripping the fpga disables all interrupt sources.
 * (So, they have to be activated agian in an interrupt service routine.)
 *
 * <b>IMPORTANT: There are interrupt priorities in the order how an easyCore
 * was added to the the EasyFpga.</b>
 */
class InterruptEnable : public Exchange
{
    public:
        /**
         * \brief Creates an InterruptEnable exchange
         *
         * \param callback
         */
        InterruptEnable(callback_ptr callback) :
            Exchange(3, 1000000, 3, 4, Exchange::SHARED_REPLY_CODES::ACK, callback)
        {
        }

        ~InterruptEnable()
        {
        }

        /**
         * \brief Gets a pre-defined request frame for this exchange
         *
         * \return A pointer to the request frame
         */
        frame_ptr getRequest(void)
        {
            if (_request == NULL) {
                byte requestBuffer[_REQUEST_LENGTH];
                requestBuffer[0] = (byte)0xAA;
                requestBuffer[1] = (byte)_id;
                requestBuffer[2] = Calculator::calculateXorParity(requestBuffer, 2);
                this->setRequest(requestBuffer);
            }

            return _request;
        }

        /**
         * \brief Checks if no parity error occured in case of a successful
         *        exchange execution.
         *
         * \return true if the transmitted parity equals the calculated one,\n
         *         false otherwise
         */
        bool successChecksumIsCorrect(byte* data)
        {
            return true;
        }

        /**
         * \brief Checks if no parity error occured in case of a faulty
         *        exchange execution.
         *
         * \return true if the transmitted parity equals the calculated one,\n
         *         false otherwise
         */
        bool errorChecksumIsCorrect(byte* data)
        {
            return true;
        }

        /**
         * \brief Write the reply from the easyFPGA board from this frame
         *        back into the user's memory (if a reply will be expected).
         */
        void writeResults(void)
        {
        }
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_SOCEXCHANGES_INTERRUPTENABLE_H_

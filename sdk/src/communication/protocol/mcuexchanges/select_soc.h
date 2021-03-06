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

#ifndef SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SELECTSOC_H_
#define SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SELECTSOC_H_

#include "communication/protocol/exchange.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"

/**
 * \brief Defines an mcu operation for switching to fgpa communication
 *        (An mcu context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 *
 * After execution of SelectSoc the easyFPGA is in a soc conext. That means,
 * that the fpga will process all sent frames via the serial interface and
 * the mcu will ignore them.
 */
class SelectSoc : public Exchange
{
    public:
        /**
         * \brief Creates an SelectSoc exchange
         *
         * \param callback
         */
        SelectSoc(callback_ptr callback) :
            Exchange(1, 1000000, 1, 1, Exchange::SHARED_REPLY_CODES::ACK, callback)
        {
        }

        ~SelectSoc()
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
                requestBuffer[0] = (byte)0x44;
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

#endif  // SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SELECTSOC_H_

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

#ifndef SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SERIALWRITE_H_
#define SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SERIALWRITE_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"

/**
 * \brief Defines an mcu operation for setting the adjustable serial number
 *        of the currently connected easyFPGA. (An mcu context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 *
 * The serial number ist stored by the mcu in its non-volatile memory.
 */
class SerialWrite : public Exchange
{
    public:
        /**
         * \brief Creates an SerialWrite exchange
         *
         * \param serialNumber
         *
         * \param callback
         */
        SerialWrite(uint32_t serialNumber, callback_ptr callback) :
            Exchange(6, 1000000, 1, 1, Exchange::SHARED_REPLY_CODES::ACK, callback),
            _serialNumber(serialNumber)
        {
        }

        ~SerialWrite()
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
                requestBuffer[0] = (byte)0xDD;
                requestBuffer[1] = (_serialNumber & 0xFF);
                requestBuffer[2] = (_serialNumber & 0xFF00) >> 8;
                requestBuffer[3] = (_serialNumber & 0xFF0000) >> 16;
                requestBuffer[4] = (_serialNumber & 0xFF000000) >> 24;
                requestBuffer[5] = Calculator::calculateXorParity(requestBuffer, 5);
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

    private:
        uint32_t _serialNumber;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SERIALWRITE_H_

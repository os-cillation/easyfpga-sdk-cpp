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

#ifndef SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SERIALREAD_H_
#define SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SERIALREAD_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"
#include "utils/log/log.h"

/**
 * \brief Defines an mcu operation for getting the adjustable serial number
 *        of the currently connected easyFPGA. (An mcu context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 *
 * The serial number ist stored by the mcu in its non-volatile memory.
 */
class SerialRead : public Exchange
{
    public:
        /**
         * \brief Creates an SerialRead exchange
         *
         * \param serial
         *
         * \param callback
         */
        SerialRead(uint32_t* serial, callback_ptr callback) :
            Exchange(1, 1000000, 6, 1, (byte)0xD9, callback),
            _serial(serial)
        {
        }

        ~SerialRead()
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
                requestBuffer[0] = (byte)0xD3;
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
            byte calculatedParity = Calculator::calculateXorParity(data, _REPLY_LENGTH_AT_SUCCESS-1);
            Log().Get(DEBUG) << "Calculated parity byte: 0x" << std::hex << (uint32_t)calculatedParity;
            byte transmittedParity = data[_REPLY_LENGTH_AT_SUCCESS-1];
            Log().Get(DEBUG) << "Transmitted parity byte: 0x" << std::hex << (uint32_t)transmittedParity;
            return (calculatedParity == transmittedParity);
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
            uint16_t dataLength = _reply->getTotalFrameLength();
            byte buffer[dataLength];
            _reply->getFrameRawData(buffer);

            *(_serial) = (*(buffer+4) << 24) | (*(buffer+3) << 16) | (*(buffer+2) << 8) | (*(buffer+1) << 0);
        }

    private:
        uint32_t* _serial;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SERIALREAD_H_

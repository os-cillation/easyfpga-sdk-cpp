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

#ifndef SDK_COMMUNICATOR_PROTOCOL_SOCEXCHANGES_READMULTIREGISTER_H_
#define SDK_COMMUNICATOR_PROTOCOL_SOCEXCHANGES_READMULTIREGISTER_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"
#include "utils/log/log.h"

/**
 * \brief Defines an soc operation for reading a single register multiple times.
 *        (An soc context is neccessary.)
 */
class ReadMultiRegister : public Exchange
{
    public:
        /**
         * \brief Creates an ReadMultiRegister exchange
         *
         * \param length
         *
         * \param target
         *
         * \param core
         *
         * \param registerAddress
         *
         * \param callback
         */
        ReadMultiRegister(uint8_t length, byte* target, byte core, byte registerAddress, callback_ptr callback) :
            Exchange(6, 1000000, 3+length, 4, (byte)0x93, callback),
            _length(length),
            _target(target),
            _core(core),
            _registerAddress(registerAddress)
        {
        }

        ~ReadMultiRegister()
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
                requestBuffer[0] = (byte)0x73;
                requestBuffer[1] = (byte)_id;
                requestBuffer[2] = _core;
                requestBuffer[3] = _registerAddress;
                requestBuffer[4] = (byte)_length;
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
        bool successChecksumIsCorrect(byte* data) {
            byte calculatedParity = Calculator::calculateXorParity(data, (uint32_t)(_REPLY_LENGTH_AT_SUCCESS-1));
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
        bool errorChecksumIsCorrect(byte* data) {
            byte calculatedParity = Calculator::calculateXorParity(data, (uint32_t)(_REPLY_LENGTH_AT_ERROR-1));
            Log().Get(DEBUG) << "Calculated parity byte: 0x" << std::hex << (uint32_t)calculatedParity;
            byte transmittedParity = data[_REPLY_LENGTH_AT_ERROR-1];
            Log().Get(DEBUG) << "Transmitted parity byte: 0x" << std::hex << (uint32_t)transmittedParity;
            return (calculatedParity == transmittedParity);
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
            memcpy(_target, buffer+2, _length);
        }

    private:
        byte _length;
        byte* _target;
        byte _core;
        byte _registerAddress;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_SOCEXCHANGES_READMULTIREGISTER_H_

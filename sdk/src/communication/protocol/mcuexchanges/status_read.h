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

#ifndef SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_STATUSREAD_H_
#define SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_STATUSREAD_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"
#include "utils/log/log.h"

/**
 * \brief Defines an mcu operation for getting the status of the currently
 *        connected easyFPGA. (An mcu context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 *
 * Over the status of the mcu we can distinguish whether<br>
 * - the fpga is currently configured with an binary, and<br>
 * - the mcu stores the desired binary in its memory (decidable by a hash value).
 */
class StatusRead : public Exchange
{
    public:
        /**
         * \brief Creates an StatusRead exchange
         *
         * \param isFpgaConfigured
         *
         * \param adler32hash
         *
         * \param callback
         */
        StatusRead(bool* isFpgaConfigured, uint32_t* adler32hash, callback_ptr callback) :
            Exchange(1, 1000000, 13, 1, (byte)0xC9, callback),
            _isFpgaConfigured(isFpgaConfigured),
            _adler32hash(adler32hash)
        {
        }

        ~StatusRead()
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
                requestBuffer[0] = (byte)0xC3;
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
            /*
             * The parity bit is calculated without the opcode (first byte).
             * The last byte of the transmitted reply corresponds to the
             * transmitted parity.
             * Thats why we have to calculate the parity with length
             * _REPLY_LENGTH_AT_SUCCESS - 2!
             */
            byte calculatedParity = Calculator::calculateXorParity(data+1, (uint32_t)(_REPLY_LENGTH_AT_SUCCESS-2));
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

            *(_isFpgaConfigured) = (((*(buffer+1) & 0x04) >> 2) == 1);
            *(_adler32hash) = (buffer[11] << 24) | (buffer[10] << 16) | (buffer[9] << 8) | (buffer[8] << 0);
        }

    private:
        bool* _isFpgaConfigured;
        uint32_t* _adler32hash;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_STATUSREAD_H_

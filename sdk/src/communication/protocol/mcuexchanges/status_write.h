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

#ifndef SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_STATUSWRITE_H_
#define SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_STATUSWRITE_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"

/**
 * \brief Defines an mcu operation for setting the status of the currently
 *        connected easyFPGA. (An mcu context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 *
 * Over the status of the mcu we can set<br>
 * - whether an binary is uploaded at the mcu memory, and<br>
 * - what size the binary measures (total number of bytes), and<br>
 * - a adler32 hash value of the binary.
 */
class StatusWrite : public Exchange
{
    public:
        /**
         * \brief Creates an StatusWrite exchange
         *
         * \param socIsUploaded
         *
         * \param binarySize
         *
         * \param adler32hash
         *
         * \param callback
         */
        StatusWrite(bool socIsUploaded, uint32_t binarySize, uint32_t adler32hash, callback_ptr callback) :
            Exchange(13, 1000000, 1, 1, Exchange::SHARED_REPLY_CODES::ACK, callback),
            _socIsUploaded(socIsUploaded),
            _binarySize(binarySize),
            _adler32hash(adler32hash)
        {
        }

        ~StatusWrite()
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

                /* byte #1: operation code */
                requestBuffer[0] = (byte)0xCC;

                /* byte #2: flags */
                requestBuffer[1] = _socIsUploaded ? (byte)0x01 : (byte)0x00;

                /* byte #3: low byte from sector adress */
                requestBuffer[2] = (byte)0x00;

                /* byte #4: high byte from sector adress */
                requestBuffer[3] = (byte)0x00;

                requestBuffer[4] = (_binarySize & 0xFF);
                requestBuffer[5] = (_binarySize & 0xFF00) >> 8;
                requestBuffer[6] = (_binarySize & 0xFF0000) >> 16;
                requestBuffer[7] = (_binarySize & 0xFF000000) >> 24;

                requestBuffer[8]  = (_adler32hash & 0xFF);
                requestBuffer[9]  = (_adler32hash & 0xFF00) >> 8;
                requestBuffer[10] = (_adler32hash & 0xFF0000) >> 16;
                requestBuffer[11] = (_adler32hash & 0xFF000000) >> 24;

                requestBuffer[12] = Calculator::calculateXorParity(requestBuffer+1, 11);

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
        bool _socIsUploaded;
        uint32_t _binarySize;
        uint32_t _adler32hash;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_STATUSWRITE_H_

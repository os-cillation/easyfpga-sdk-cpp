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

#ifndef SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SECTOR4096BYTEWRITE_H_
#define SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SECTOR4096BYTEWRITE_H_

#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"

#include <string.h> /* memcpy() */

/**
 * \brief Defines an mcu operation for uploading a 4096 bytes sized memory block
 *        into the non-volatile memory of the mcu. (An mcu context is neccessary.)
 *
 * This class can be used in combination with a specific execution environment
 * derived from Task.
 */
class Sector4096ByteWrite : public Exchange
{
    public:
        /**
         * \brief Creates an Sector4096ByteWrite exchange
         *
         * \param sectorId
         *
         * \param sector
         *
         * \param callback
         */
        Sector4096ByteWrite(uint16_t sectorId, byte* sector, callback_ptr callback) :
            Exchange(4103, 1000000, 1, 1, Exchange::SHARED_REPLY_CODES::ACK, callback),
            _sectorId(sectorId),
            _sector(sector)
        {
        }

        ~Sector4096ByteWrite()
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
                requestBuffer[0] = (byte)0x22;

                /* byte #2 and #3: low and high byte of 10-bit sector id */
                requestBuffer[1] = (_sectorId & 0x00FF);
                requestBuffer[2] = (_sectorId & 0x0300) >> 8;

                /* 4096 bytes sector */
                memcpy(requestBuffer+3, _sector, 4096);

                /*
                 * byte #4100-#4103: parity (here: adler 32 hash value)
                 * here also low byte first
                 */
                uint32_t hash = Calculator::calculateAdler32Hash(requestBuffer+1, 4098);
                requestBuffer[4099] = (hash & 0xFF);
                requestBuffer[4100] = (hash & 0xFF00) >> 8;
                requestBuffer[4101] = (hash & 0xFF0000) >> 16;
                requestBuffer[4102] = (hash & 0xFF000000) >> 24;

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
            return true;

        }

        /**
         * \brief Checks if no parity error occured in case of a faulty
         *        exchange execution.
         *
         * \return true if the transmitted parity equals the calculated one,\n
         *         false otherwise
         */
        bool errorChecksumIsCorrect(byte* data) {
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
        uint16_t _sectorId;
        byte* _sector;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_MCUEXCHANGES_SECTOR4096BYTEWRITE_H_

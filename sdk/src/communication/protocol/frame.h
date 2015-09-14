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

#ifndef SDK_COMMUNICATION_PROTOCOL_FRAME_H_
#define SDK_COMMUNICATION_PROTOCOL_FRAME_H_

#include <communication/types.h>
#include <utils/hardwaretypes.h>

/**
 * \brief Container for holding all neccessary information about one data
 *        transfer over the serial connection line.
 */
class Frame
{
    public:
        /**
         * \brief Creates and initializes an Frame with the specified
         *        values.
         *
         * \param frameData An byte buffer pointer for copying the
         *        containing values into the frame byte array.
         *
         * \param totalFrameLength Size of the turned over byte buffer
         */
        Frame(byte* frameData, uint16_t totalFrameLength);
        ~Frame();

        /**
         * \brief Returns the first byte of the frame's raw data which
         *        is always defined as the operation code.
         *
         * \return An operation code
         */
        byte getOperationCode(void);

        /**
         * \deprecated Ids don't have to be used.
         *
         * \brief Get the id of this frame. (The second byte of the frame's
         *        raw data if it exists.)
         *
         * \return an 8-bit wide id if this frame has stored an id,\n
         *         0 otherwise
         */
        idval getId(void);

        /**
         * \brief Get the length of the frame's raw byte array.
         *
         * \return an 16-bit wide integer
         */
        uint16_t getTotalFrameLength(void);

        /**
         * \brief Copies the frame's raw data into a byte array.
         *
         * \param destByteArray Pointer to an byte array which have to
         */
        void getFrameRawData(byte* destByteArray);

    private:
        byte* _data;
        uint16_t _frameLength;
};

#endif  // SDK_COMMUNICATION_PROTOCOL_FRAME_H_

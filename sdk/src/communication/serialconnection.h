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

#ifndef SDK_COMMUNICATION_SERIALCONNECTION_H_
#define SDK_COMMUNICATION_SERIALCONNECTION_H_

#include "communication/types.h"
#include "protocol/frame_ptr.h"
#include "utils/hardwaretypes.h"
#include "utils/os/types.h"

#include <string>

/**
 * \brief Connection to the virtual com port. Provides low
 *        level functions for a serial device.
 */
class SerialConnection
{
    public:
        SerialConnection();
        ~SerialConnection();

        /**
         * \brief Opens a device (neccessary before any send / receive
         *        requests).
         *
         * \param device Absolute path to a serial device abstraction (a
         *        file) of the operating system.
         *
         * \return true if the operating system could open the serial
         *         device successfully,\n
         *         false otherwise
         */
        bool openDevice(std::string device);

        /**
         * \brief Closes a before opened device.
         *
         * \return true if the operating system could close the serial
         *         device successfully,\n
         *         false otherwise
         */
        bool closeDevice(void);

        /**
         * \brief Takes all remaining bytes out of the send and receive
         *        buffers of the currently opened serial device.
         *
         * \return true if flush was successful,\n
         *         false otherwise
         */
        bool flushBuffers(void);

        /**
         * \brief Sends a frame's raw data.
         *
         * \param f Pointer to a before instantiated Frame.
         *
         * \return true if the data could send successfully,\n
         *         false otherwise
         */
        bool send(frame_ptr f);

        /**
         * \brief Sends a specific byte array.
         *
         * \param byteArray Pointer to a byte array.
         *
         * \param byteArrayLength Total number of bytes to be sent.
         *
         * \return true if the byte array could send successfully,\n
         *         false otherwise
         */
        bool send(byte* byteArray, uint32_t byteArrayLength);

        /**
         * \brief Returns the currently number of bytes existing in the
         *        send queue.
         *
         * \return The number of bytes greater or equal to 0 remaining
         *         in the send buffer,\n
         *         -1 if an error occurs
         */
        int32_t getSendQueueSize(void);

        /**
         * \brief Receive a given number of bytes.
         *
         * \param byteArray Pointer to an byte array to write in the
         *        received bytes.
         *
         * \param byteArrayLength Total number of bytes to receive.
         *
         * \param timeoutus A timeout for receiving specified in us.
         *
         * \return true if the number of requested bytes could received
         *         successfully (e.g. no error or timeout occured),\n
         *         false otherwise
         */
        bool receive(byte* byteArray, uint32_t byteArrayLength, timeoutval timeoutus);

        /**
         * \brief Returns the currently number of bytes available in the
         *        receive queue.
         *
         * \return The number of bytes greater or equal to 0 available
         *         in the receive buffer,\n
         *         -1 if an error occurs
         */
        int32_t getReceiveQueueSize(void);

    private:
        /**
         * linux file descriptor for writing and reading data
         */
        returnval _fd;
};

#endif  // SDK_COMMUNICATION_SERIALCONNECTION_H_

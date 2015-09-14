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

#include "configuration.h"
#include "communication/serialconnection.h"
#include "protocol/frame.h"
#include "protocol/specification.h"
#include "utils/log/log.h"
#include "utils/os/types.h"

#include <unistd.h> /* select() */
#include <termios.h> /* termios, cfsetospeed(), cfsetispeed(), tcflush() */
#include <fcntl.h> /* open(), close() */
#include <cstring> /* memset() */
#include <sys/ioctl.h> /* ioctl() */
#include <errno.h> /* errno */
#include <cstring> /* strerror(1) */

/*
 * All filedescriptors greater 2 are valid
 * (0, 1, 2 are standard IO; -1 is an error)
 */
#define CONNECTED (_fd > 2)
#define NOTCONNECTED (_fd < 0)

SerialConnection::SerialConnection() :
    _fd(-1)
{
}

SerialConnection::~SerialConnection()
{
}

bool SerialConnection::openDevice(std::string device)
{
    if (NOTCONNECTED) {
        /*
         * O_RDWR = read / write access to file
         * O_NOCTTY = no process controlled terminal
         * O_NDELAY = nonblocking mode
         */
        _fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

        if (CONNECTED) {
            struct termios tio;
            std::memset(&tio, 0, sizeof(tio));

            /*
             * some people can do alot of crazy things here with
             * tio.c_iflag - input mode
             * tio.c_oflag - output mode
             */

             /*
              * CX - Character size x mask (CS8 = 8 bit characters)
              * CREAD - enable receiver
              * CLOCAL - Ignore modem control lines
              * CRTSCTS - enable RTS/CTS (hardware) flow control
              * HUPCL - Lower modem control lines after last process
              *         closes the device (hang up)
              */
            tio.c_cflag = CS8 | CLOCAL | CREAD | CRTSCTS | HUPCL;
            cfsetospeed(&tio, CONNECTIONSPEED);
            cfsetispeed(&tio, CONNECTIONSPEED);

            /*
             * TCSANOW = transmit settings changes immediately
             * (not after transmitting / writing anything)
             */
            tcsetattr(_fd, TCSANOW, &tio);

            return true;
        }
        else if (_fd == -1) {
            Log().Get(ERROR) << "Error while opening serial device '" << device << "': " << strerror(errno);
        }
    }
    else {
        Log().Get(WARNING) << "Opening serial device failed. This serial connection is already opened. You have to call closeDevice() first.";
    }

    return false;
}

bool SerialConnection::closeDevice(void)
{
    this->flushBuffers();

    if (CONNECTED) {
        returnval success = close(_fd);

        if (success == 0) {
            _fd = -1;
            return true;
        }
        else if (success == -1) {
            Log().Get(ERROR) << "Error while closing serial device: " << strerror(errno);
        }
    }
    else {
        Log().Get(WARNING) << "Closing serial device failed. This serial connection is already closed. You have to call openDevice() first.";
    }

    _fd = -1;
    return false;
}

bool SerialConnection::flushBuffers(void)
{
    if (CONNECTED) {
        Log().Get(DEBUG) << "Flushing buffers...";
        Log().Get(DEBUG) << "Send queue size before flushing: " << std::dec << (int32_t)this->getSendQueueSize();
        Log().Get(DEBUG) << "Receive queue size before flushing: " << std::dec << (int32_t)this->getReceiveQueueSize();

        /*
         * The function tcflush() should be used to flush the serial in-
         * and / or output queues, but tests performed have shown that
         * tcflush() doesn't work in every case! See the documentation
         * for more details.
         *
         * Thats why we have to fetch manually all bytes from the recieve
         * buffer (in case of remaining bytes in the queues). Nevertheless
         * we execute this system call for back-/forward compatibility too.
         *
         * TCIOFLUSH = flushes input and output buffers
         */
        returnval success = tcflush(_fd, TCIOFLUSH);

        Log().Get(DEBUG) << "Send queue size after flushing: " << std::dec << (int32_t)this->getSendQueueSize();
        Log().Get(DEBUG) << "Receive queue size after flushing: " << std::dec << (int32_t)this->getReceiveQueueSize();
        if (this->getReceiveQueueSize() > 0) {
            /* fetch manually all bytes from the recieve buffer */
            byte trashByte = (byte)0x00;
            Log().Get(WARNING) << "You can ignore the following timeout warning...";
            while (this->receive(&trashByte, 1, FLUSHING_TIMEOUT)) {
                Log().Get(WARNING) << "A byte removed from recieve buffer while flushing: 0x" << std::hex << (int32_t)trashByte;
            }
        }

        if ((success == 0) && (this->getReceiveQueueSize() == 0)) {
            return true;
        }
    }
    else {
        Log().Get(WARNING) << "Flushing buffers of serial connection failed. No device opened!";
    }

    Log().Get(ERROR) << "Flushing buffers not successful!";
    return false;
}

bool SerialConnection::send(frame_ptr f)
{
    if (CONNECTED) {
        uint16_t dataLength = f->getTotalFrameLength();

        byte framedata[dataLength];
        f->getFrameRawData(framedata);

        returnval bytesSend = write(_fd, framedata, dataLength);

        if (bytesSend == dataLength) {
            return true;
        }
    }
    else {
        Log().Get(WARNING) << "Send failed. No device opened!";
    }

    return false;
}

bool SerialConnection::send(byte* byteArray, uint32_t byteArrayLength)
{
    if (CONNECTED) {
        returnval bytesSend = write(_fd, byteArray, byteArrayLength);

        if ((uint32_t)bytesSend == byteArrayLength) {
            return true;
        }
    }
    else {
        Log().Get(WARNING) << "Send failed. No device opened!";
    }

    return false;
}

int32_t SerialConnection::getSendQueueSize(void)
{
    int32_t bytes = -1;

    if (CONNECTED) {
        ioctl(_fd, TIOCOUTQ, &bytes);
    }
    else {
        Log().Get(WARNING) << "Get send buffer size failed. No device opened!";
    }

    return bytes;
}

bool SerialConnection::receive(byte* byteArray, uint32_t byteArrayLength, timeoutval timeoutus)
{
    if (CONNECTED) {
        fd_set readFdSet;

        /* create and init buffer */
        byte buffer[byteArrayLength];
        for (uint32_t i=0; i<byteArrayLength; i++) {
            *(buffer+i) = (byte)0x00;
        }

        /* get values for timeval struct from function parameters */
        uint32_t timeoutSeconds = timeoutus / 1000000;
        uint32_t timeoutMicroSeconds = timeoutus % 1000000;

        /*
         * byteRemains contains number of bytes to receive and will be
         * count down for every byte read
         */
        uint32_t byteRemains = byteArrayLength;

        while (byteRemains > 0) {
            /*
             * set timeout for the select() function (creating a new
             * timeout struct is necessary every time before to call select)
             */
            timeval timeout;
            timeout.tv_sec = timeoutSeconds;
            timeout.tv_usec = timeoutMicroSeconds;

            /* reset fd_set and associate it to file descriptor */
            FD_ZERO(&readFdSet);
            FD_SET(_fd, &readFdSet);

            /* synchronous function call! */
            returnval available = select(_fd+1, &readFdSet, NULL, NULL, &timeout);

            if (available > 0) {
                returnval bytesRead = read(_fd, buffer+(byteArrayLength-byteRemains), byteRemains);

                if (bytesRead > 0) {
                    byteRemains -= bytesRead;
                }
            }
            else if (available == 0) {
                Log().Get(WARNING) << "TIMEOUT EXPIRED WHILE READING!";
                break;
            }
            else {
                Log().Get(ERROR) << "Error while select() call: " << strerror(errno);
                break;
            }
        }

        FD_CLR(_fd, &readFdSet);

        if (byteRemains == 0) {
            memcpy(byteArray, buffer, byteArrayLength);
            return true;
        }
    }
    else {
        Log().Get(WARNING) << "Receive failed. No device opened!";
    }

    return false;
}

int32_t SerialConnection::getReceiveQueueSize(void)
{
    int32_t bytes = -1;

    if (CONNECTED) {
        ioctl(_fd, TIOCINQ, &bytes);
    }
    else {
        Log().Get(WARNING) << "Get receive buffer size failed. No device opened!";
    }

    return bytes;
}

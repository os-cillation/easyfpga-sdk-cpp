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

#ifndef SDK_COMMUNICATION_PROTOCOL_EXCHANGE_H_
#define SDK_COMMUNICATION_PROTOCOL_EXCHANGE_H_

#include "communication/types.h"
#include "communication/protocol/frame_ptr.h"
#include "easycores/callback_ptr.h"
#include "utils/hardwaretypes.h"
#include "utils/idmanager_fwd.h"

/**
 * \brief Container for holding all neccessary information about a single
 *        communication exchange.
 *
 * An exchange consists basically of 2 frames:
 * - a request frame (data sent from the framework to the easyFPGA board)
 * - a reply frame (replied data sent from the easyFPGA board to the framework)
 *
 * and some more constants holding information about
 * - how long is the expected reply length
 * - how long is the length of a mightly replied error frame
 * - the dimensioned timeout to wait if no reply returns from the board
 *   for a certain user demand.
 *
 * The framworks usually starts (except in case of interrupts) every
 * communication with the easyFPGA board. It constructs a exchange
 * according to the user demand. Then the request frame will be sent to
 * the board. Depending on the framework's operation mode (sync / async)
 * the framework will wait for an answer or process the next user demands.
 *
 * The request frame will be stored in this exchange object. That's
 * neccessary because in cases of errors the request have to be resent.
 *
 * Always if the frameworks receives some data, it maps the received byte
 * array into a reply frame. Afterwards the framework processes the reply
 * (e.g. do some error checking) and returns the gained information back
 * into the user's program space.
 */
class Exchange
{
    public:
        Exchange(
            uint16_t requestLength,
            timeoutval receivingTimeout,
            uint16_t replySuccessLength,
            uint16_t replyErrorLength,
            byte expectedOpcode,
            callback_ptr callback
        );
        ~Exchange();

        /**
         * \brief Defines global answer codes for a reply frame.
         */
        enum SHARED_REPLY_CODES : byte {
            /**
             * Acknowlegdement, request processed successfully
             * <br>(According to the communication protocol, an ACK can
             * have different lengths!)
             */
            ACK = (byte)0x00,

            /**
             * No acknowlegdement, the request was associated with some
             * errors
             * <br>The next bytes specifies the error type
             * <br>(According to the communication protocol, a NACK can
             * have different lengths!)
             */
            NACK = (byte)0x11,

            /**
             * A core triggered an interrupt
             * <br>Further information can be found in the next bytes.
             */
            INTERRUPT = (byte)0x99
        };

        /**
         * \brief Associates an id to this exchange.
         *
         * Normally this member isn't useful for communication with the
         * easyFPGA board. The exchange's id will be always set to zero.
         * It's not neccessary to reset the id to any other value.
         *
         * \param id An positive interger in [1, 255]
         */
        void setId(idval id);

        /**
         * \brief Returns the current exchange's id (usually zero).
         *
         * \return An positive interger in [0, 255]
         */
        idval getId(void);

        /**
         * \brief Returns the request frame's length.
         */
        uint16_t getRequestLength(void);

        /**
         * \brief Returns the currently set request frame.
         *
         * \return A shared pointer to a Frame set by setRequest()
         */
        virtual frame_ptr getRequest(void) = 0; /* implements by inherited classes! */

        /**
         * \brief Returns the expected answer opcode to the request frame.
         */
        byte getExpectedOpcode(void);

        /**
         * \brief Returns the receive timeout for this request.
         */
        timeoutval getReceiveTimeout(void);

        /**
         * \brief Returns the expected length of a successful reply.
         */
        uint16_t getReplySuccessLength(void);

        /**
         * \brief Checks if the transmitted checksum is correct.
         *
         * \return true if the calculated checksum matches the transmitted
         *         checksum,<br>
         *         false otherwise
         */
        virtual bool successChecksumIsCorrect(byte* data) = 0; /* implements by inherited classes! */

        /**
         * \brief Sets the replied answer to this request and maps this
         *        into the reply frame.
         */
        void setSuccessReply(byte* data);

        /**
         * \brief Returns the expected length of an error reply.
         */
        uint16_t getReplyErrorLength(void);

        /**
         * \brief Checks if the transmitted checksum is correct.
         *
         * \return true if the calculated checksum matches the transmitted
         *         checksum,<br>
         *         false otherwise
         */
        virtual bool errorChecksumIsCorrect(byte* data) = 0; /* implements by inherited classes! */

        /**
         * \brief Sets the replied error to this request and maps this
         *        into the reply frame.
         */
        void setErrorReply(byte* data);

        /**
         * \brief Should write the gained information from the reply frame
         *        back into the user's memory (if this is neccessary).
         */
        virtual void writeResults(void) = 0; /* implements by inherited classes! */

        /**
         * \brief Returns the reply frame.
         */
        frame_ptr getReply(void);

        /**
         * \brief Returns true if the framework registered an callback
         *        object to this exchange.
         */
        bool hasACallback(void);

        /**
         * \brief Executes an beforehand registered callback object.
         */
        bool executeCallback(void);

    protected:
        /**
         * \brief Generates a specific request to be sent to the board.
         */
        void setRequest(byte* data);

        /**
         * \brief Holds the exchange's id.
         */
        idval _id;

        /*
         * following const values will set by inherited classes
         */

        /**
         * \brief Stores the request.
         */
        frame_ptr _request;

        /**
         * \brief Stores the request's length.
         */
        const uint16_t _REQUEST_LENGTH;

        /**
         * \brief Stores the replied answer to the request.
         */
        frame_ptr _reply;

        /**
         * \brief Stores the receive timeout.
         */
        const timeoutval _RECEIVING_TIMEOUT;

        /**
         * \brief Stores the expected length of the reply in case of success.
         */
        const uint16_t _REPLY_LENGTH_AT_SUCCESS;

        /**
         * \brief Stores the expected length of the reply in case of error.
         */
        const uint16_t _REPLY_LENGTH_AT_ERROR;

        /**
         * \brief Stores the expected opcode of the reply in case of success.
         */
        const byte _EXPECTED_REPLY_OPCODE;

        /**
         * \brief Stores the callback object.
         */
        callback_ptr _callback;
};

#endif  // SDK_COMMUNICATION_PROTOCOL_EXCHANGE_H_

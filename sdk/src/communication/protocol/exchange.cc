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

#include "configuration.h" /* assert() */
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame.h"
#include "easycores/callback.h"
#include "utils/log/log.h"

Exchange::Exchange(
    uint16_t requestLength,
    timeoutval receivingTimeout,
    uint16_t replySuccessLength,
    uint16_t replyErrorLength,
    byte expectedOpcode,
    callback_ptr callback) :
    _id(0),
    _request(NULL),
    _REQUEST_LENGTH(requestLength),
    _reply(NULL),
    _RECEIVING_TIMEOUT(receivingTimeout),
    _REPLY_LENGTH_AT_SUCCESS(replySuccessLength),
    _REPLY_LENGTH_AT_ERROR(replyErrorLength),
    _EXPECTED_REPLY_OPCODE(expectedOpcode),
    _callback(callback)
{
}

Exchange::~Exchange()
{
}

void Exchange::setId(idval id) {
    assert(_id == 0);
    _id = id;
}

idval Exchange::getId() {
    return _id;
}

uint16_t Exchange::getRequestLength(void)
{
    return _REQUEST_LENGTH;
}

byte Exchange::getExpectedOpcode(void)
{
    return _EXPECTED_REPLY_OPCODE;
}

timeoutval Exchange::getReceiveTimeout(void)
{
    return _RECEIVING_TIMEOUT;
}

uint16_t Exchange::getReplySuccessLength(void)
{
    return _REPLY_LENGTH_AT_SUCCESS;
}

uint16_t Exchange::getReplyErrorLength(void)
{
    return _REPLY_LENGTH_AT_ERROR;
}

void Exchange::setSuccessReply(byte* data)
{
    _reply = std::make_shared<Frame>(data, _REPLY_LENGTH_AT_SUCCESS);
}

void Exchange::setErrorReply(byte* data)
{
    _reply = std::make_shared<Frame>(data, _REPLY_LENGTH_AT_ERROR);
}

frame_ptr Exchange::getReply(void)
{
    return _reply;
}

bool Exchange::hasACallback(void)
{
    return (_callback != NULL);
}

bool Exchange::executeCallback(void)
{
    /* protection */
    assert(this->hasACallback());

    if (_callback != nullptr) {
        bool success = _callback->call();
        Log().Get(WARNING) << "Attempt to get a new id wasn't successful!";

        return success;
    }
    else {
        Log().Get(ERROR) << "A task callback shoul a not !";
        return false;
    }
}

void Exchange::setRequest(byte* data)
{
    _request = std::make_shared<Frame>(data, _REQUEST_LENGTH);
}

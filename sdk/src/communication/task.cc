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

#include "configuration.h" /* assert(1) */
#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/protocol/frame.h"
#include "communication/serialconnection.h"
#include "communication/task.h"
#include "utils/log/log.h"
#include "utils/hardwaretypes.h"

#include <algorithm> /* max(2) */

Task::Task(std::string name, serialconnection_ptr sc, exchange_ptr ex, CoreIndex* interruptTriggeringCore, tasknumberval number) :
    _sendState(Task::SEND_STATE::SEND_NOT_EXECUTED),
    _receiveState(Task::RECEIVE_STATE::RECEIVE_NOT_EXECUTED),
    _name(name),
    _serialConnection(sc),
    _exchange(ex),
    _executionAttempt(1),
    _taskNumber(number),
    _interruptTriggeringCore(interruptTriggeringCore)
{
}

Task::Task(const Task& task) :
    _sendState(task._sendState),
    _receiveState(task._receiveState),
    _name(task._name),
    _serialConnection(task._serialConnection),
    _exchange(task._exchange),
    _executionAttempt(task._executionAttempt),
    _taskNumber(task._taskNumber),
    _interruptTriggeringCore(task._interruptTriggeringCore)
{
}

Task::~Task()
{
}

std::string Task::getName(void)
{
    std::stringstream ss;
    ss << "'" << _name << "/" << _taskNumber << "'";
    return ss.str();
}

exchange_ptr Task::getExchange(void)
{
    return _exchange;
}

retryval Task::getExecutionCount(void)
{
    return _executionAttempt;
}

tasknumberval Task::getNumber(void)
{
    return _taskNumber;
}

void Task::setNumber(tasknumberval newNumber)
{
    _taskNumber = newNumber;
}

Task::SEND_STATE Task::getSendState(void)
{
    return _sendState;
}

Task::RECEIVE_STATE Task::getReceiveState(void)
{
    return _receiveState;
}

bool Task::send(void)
{
    /* Increment execution counter */
    _executionAttempt++;

    /* Actual sending procedure */
    frame_ptr request = _exchange->getRequest();
    byte buffer[request->getTotalFrameLength()];
    request->getFrameRawData(buffer);
    Log().Get(DEBUG) << "Send: " << std::hex << (int32_t)buffer[0];

    if (_serialConnection->send(request)) {
        _sendState = Task::SEND_STATE::SEND_SUCCESS;
        return true;
    }
    else {
        _sendState = Task::SEND_STATE::SEND_FAILURE;
        return false;
    }
}

bool Task::receive(void) {
    return this->receive(0);
}

bool Task::receive(uint8_t recursiveDepth)
{
    /*
     * While receiving the correct bytes from the queue, we determine at
     * first which opcode was responsed from the device. Depending on
     * that opcode, maybe we have to fetch a varying number of bytes.
     * So that every byte fits into the local buffer, we calculate the
     * maximum byte count of all possibilities.
     */
    uint16_t maxByteCount = std::max(_exchange->getReplySuccessLength(), std::max(_exchange->getReplyErrorLength(), (uint16_t)3));
    assert(maxByteCount > 1);

    byte reply[maxByteCount];

    if (_serialConnection->receive(reply, 1, _exchange->getReceiveTimeout())) {
        Log().Get(DEBUG) << "Received opcode: 0x" << std::hex << (int32_t)reply[0];
        uint16_t rest = 0;
        Log().Get(DEBUG) << "Expected opcode: 0x" << std::hex << (int32_t)_exchange->getExpectedOpcode();

        if (reply[0] == _exchange->getExpectedOpcode()) {
            /* -1 because we already fetched the first byte (opcode) */
            rest = _exchange->getReplySuccessLength() - 1;
            if (rest > 0) {
                Log().Get(DEBUG) << "Fetch the remaining " << (int32_t)rest << " byte(s)...";
                _serialConnection->receive(reply+1, rest, _exchange->getReceiveTimeout());
            }

            if (_exchange->successChecksumIsCorrect(reply)) {
                _exchange->setSuccessReply(reply);
                _receiveState = Task::RECEIVE_STATE::RECEIVE_SUCCESS;
                return true;
            }
            else {
                _receiveState = Task::RECEIVE_STATE::RECEIVE_CHECKSUM_ERROR;
                return false;
            }
        }
        else if (reply[0] == Exchange::SHARED_REPLY_CODES::NACK) {
            /* -1 because we already fetched the first byte (opcode) */
            rest = _exchange->getReplyErrorLength() - 1;
            if (rest > 0) {
                Log().Get(DEBUG) << "Fetch the remaining " << (int32_t)rest << " byte(s)...";
                _serialConnection->receive(reply+1, rest, _exchange->getReceiveTimeout());
            }

            if (_exchange->errorChecksumIsCorrect(reply)) {
                _exchange->setErrorReply(reply);
                _receiveState = Task::RECEIVE_STATE::RECEIVE_FAILURE;
                return false;
            }
            else {
                _receiveState = Task::RECEIVE_STATE::RECEIVE_CHECKSUM_ERROR;
                return false;
            }
        }
        else if (reply[0] == Exchange::SHARED_REPLY_CODES::INTERRUPT) {
            /*
             * Interrupts can't occur right after an other. The user or
             * the framework have to enable the interrupts agian before
             * it can be triggered a next one.
             *
             * Furthermore: A protection against an endless loop.
             */
            assert(recursiveDepth < 1);

            Log().Get(DEBUG) << "Fetch the remaining 2 bytes...";
            if (_serialConnection->receive(reply+1, 2, _exchange->getReceiveTimeout())) {
                byte calculatedParity = reply[1];
                Log().Get(DEBUG) << "Calculated parity byte: 0x" << std::hex << (uint32_t)calculatedParity;
                byte transmittedParity = reply[2];
                Log().Get(DEBUG) << "Transmitted parity byte: 0x" << std::hex << (uint32_t)transmittedParity;
                if (calculatedParity == transmittedParity) {
                    *(_interruptTriggeringCore) = (CoreIndex)reply[1];
                    Log().Get(DEBUG) << "The corresponding interrupt routine will be executed after next call of fetchAsyncReplies().";
                }
                else {
                    Log().Get(WARNING) << "Interrupt request recognized. Parity check failed. Because of that won't be executed the corresponding interrupt routine!";
                }
            }
            else {
                Log().Get(WARNING) << "Interrupt request recognized. Serial connection refused to get the triggering core! Because of that won't be executed the corresponding interrupt routine!";
            }

            /* Get the actual bytes assigned to this task in the receive queue. */
            return this->receive(++recursiveDepth);
        }
        else {
            _receiveState = Task::RECEIVE_STATE::RECEIVE_UNEXPECTED_OPCODE_ERROR;
            return false;
        }
    }
    else {
        _receiveState = Task::RECEIVE_STATE::RECEIVE_CONNECTION_ERROR;
        return false;
    }
}

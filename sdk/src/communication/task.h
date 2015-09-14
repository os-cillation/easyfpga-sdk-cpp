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

#ifndef SDK_COMMUNICATION_TASK_H_
#define SDK_COMMUNICATION_TASK_H_

#include "communication/protocol/exchange_ptr.h"
#include "communication/serialconnection_ptr.h"
#include "communication/types.h"
#include "easycores/types.h"

#include <string>

/**
 * \brief Defines a base class for tasks executed by the TaskExecutor
 *
 * The framework offers two kinds of communication ways with the easyFPGA
 * board: synchronous and asynchronous. That's why the framework contains
 * the two higher level task classes SyncTask and AsyncTask.
 *
 * This class is serving a base class for the two kinds of tasks above.
 *
 * Task provides some lower level functions like send() or receive()
 * which are combined to higher level member functions in the derived
 * classes.
 *
 * We know it's possible, but you shouldn't instantiate an object of
 * this class manually. Therefore exists SyncTask and AsyncTask.
 */
class Task
{
    public:
        /**
         * \brief Defines all possible send states in which a task can be.
         */
        enum SEND_STATE : uint32_t {
            /**
             * No request has been sent yet.
             *
             * Type: initial state
             */
            SEND_NOT_EXECUTED,

            /**
             * The sending procedure with the given data from an exchange
             * was successful.
             *
             * Type: case of success
             */
            SEND_SUCCESS,

            /**
             * The sending procedure sent at least one request but this
             * process was related to some errors.
             *
             * Type: case of failure
             */
            SEND_FAILURE
        };

        /**
         * \brief Defines all possible receive states in which a task can be.
         */
        enum RECEIVE_STATE : uint32_t {
            /**
             * There was no attempt to receive a reply so far.
             *
             * Type: initial state
             */
            RECEIVE_NOT_EXECUTED,

            /**
             * All Exchange related bytes could receive successfully.
             * (That means that the receive buffer contained the right
             * opcode followed by the remaining exchange specifc bytes.)
             *
             * Type: case of success
             */
            RECEIVE_SUCCESS,

            /**
             * The fpga couldn't process the desired request. Reasons
             * could be e.g.:
             * - the fgpa couldn't interpret the requested opcode
             * - the fpga's parity check fails
             * - the desired core / register ist not available
             *   (a wishbone timeout occured)
             *
             * Type: case of failure
             *
             * Error handling: retry operation
             */
            RECEIVE_FAILURE,

            /**
             * 1) The right opcode and the exchange specifc bytes, or
             * 2) an ACK received,
             * but the parity check of the received reply fails.
             *
             * Type: case of failure
             *
             * Error handling: retry operation
             */
            RECEIVE_CHECKSUM_ERROR,

            /**
             * Another opcode received instead of the expected one.
             *
             * Type: a really hard error
             *
             * Error handling: generally unspecified but we decided to
             * retry the already started operation (But we are afraid
             * that after this error no more operation will be successful
             * anyway.)
             *
             * Note 1: The easyFPGA board answers all requests in the
             * same order as they were requested to it. Only an interrupt
             * notification can be pushed in between.
             *
             * Note 2: Interrupt notifications are handled in a subroutine
             * of receive(), so they are invisible out of the receive's
             * scope! Consequence: Interrupt notifications aren't a reason
             * for that type of error.
             */
            RECEIVE_UNEXPECTED_OPCODE_ERROR,

            /**
             * No bytes could received. Reasons could be e.g.:
             * - a timeout occured (the fpga doesn't receive any request
             *   so it doesn't reply to any)
             * - no device was opened by the SerialConnection
             *
             * Type: case of failure
             *
             * Error handling: retry operation
             */
            RECEIVE_CONNECTION_ERROR
        };

        Task(
            std::string name,
            serialconnection_ptr sc,
            exchange_ptr ex,
            CoreIndex* interruptTriggeringCore,
            tasknumberval number
        );
        Task(const Task& task);
        ~Task();

        /**
         * \brief Gets the Task's name for logging purposes
         *
         * \return A string
         */
        std::string getName(void);

        /**
         * \brief Gets the Task's assignment to an Exchange
         *
         * \return A shared pointer to an Exchange object
         */
        exchange_ptr getExchange(void);

        /**
         * \brief Gets the count of the Task's execution retries.
         *
         * \return A poistive integer
         */
        retryval getExecutionCount(void);

        /**
         * \brief Gets the Task's globally unique identification number
         *
         * \return A positive integer
         */
        tasknumberval getNumber(void);

        /**
         * \brief Sets the Task's globally unique identification number
         *
         * \param newNumber An unique poisitive integer
         */
        void setNumber(tasknumberval newNumber);

        /**
         * \brief Gets the Task's currently setted send state
         *
         * \return A value of SEND_STATE
         *
         * @see SEND_STATE
         */
        SEND_STATE getSendState(void);

        /**
         * \brief Gets the Task's currently setted receive state
         *
         * \return A value of RECEIVE_STATE
         *
         * @see RECEIVE_STATE
         */
        RECEIVE_STATE getReceiveState(void);

    protected:
        /**
         * \brief Sends a request.
         *
         * \return true if the request could sent successfully to the
         *         easyFPGA board,\n
         *         false otherwise
         */
        bool send(void);

        /**
         * \brief Receives a reply.
         *
         * \return true if the reception was successful and no error
         *         occured,\n
         *         false otherwise
         */
        bool receive(void);

        /**
         * \brief Holds the send state of this task.
         *
         * Initialization: Task::SEND_STATE::SEND_NOT_EXECUTED
         */
        SEND_STATE _sendState;

        /**
         * \brief Holds the receive state of this task.
         *
         * Initialization: Task::RECEIVE_STATE::RECEIVE_NOT_EXECUTED
         */
        RECEIVE_STATE _receiveState;

        /**
         * \brief Remembers the task's name for logging purposes.
         */
        std::string _name;

        /**
         * \brief Stores the reference to the serial device.
         */
        serialconnection_ptr _serialConnection;

        /**
         * \brief Stores the assignment to an Exchange which holds the
         * operation specific byte codes.
         */
        exchange_ptr _exchange;

        /**
         * \brief Holds the retried count of executions.
         *
         * Initialization: 1
         *
         * This value is important for retries. We don't want to
         */
        retryval _executionAttempt;

        /**
         * \brief Holds the execution count of this task.
         *
         * This member will set by a determined value of the TaskExecutor.
         */
        tasknumberval _taskNumber;

        /**
         * \brief Holds a reference to an EasyCore.
         *
         * This member will set by a determined value of the TaskExecutor.
         */
        CoreIndex* _interruptTriggeringCore;

    private:
        bool receive(uint8_t recursiveDepth);
};

#endif  // SDK_COMMUNICATION_TASK_H_

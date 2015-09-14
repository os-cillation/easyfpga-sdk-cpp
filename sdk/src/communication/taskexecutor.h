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

#ifndef SDK_COMMUNICATION_TASKEXECUTOR_H_
#define SDK_COMMUNICATION_TASKEXECUTOR_H_

#include "communication/asynctask.h"
#include "communication/protocol/exchange_ptr.h"
#include "communication/serialconnection_ptr.h"
#include "communication/types.h"
#include "easycore_map_ptr.h"

#ifdef USE_IDS_FOR_ASYNC_OPS
#include "utils/idmanager.h"
#endif

#include <string>
#include <map>
#include <set>
#include <queue>

/**
 * \brief Execution environment for tasks
 *
 * This class manages the execution of a Task instance including all
 * execution associated features like error handling (e.g. possible
 * retries).
 *
 * With the symbol USE_IDS_FOR_ASYNC_OPS can be decided whether this
 * class should use ids for asynchrounous communication or not.
 */
class TaskExecutor
{
    public:
        /**
         * \brief Creates a TaskExecutor instance.
         *
         * \param sc A pointer to an instance of SerialConnection
         *
         * \param coreMap A pointer to the EasyFpga's map of easyCores, or<br>
         *        NULL if no interrupts should be served.
         *
         */
        TaskExecutor(serialconnection_ptr sc, easycore_map_ptr coreMap);
        ~TaskExecutor();

        /* SYNC */
        /**
         * \brief Sending a synchronous request and try to get a good reply.
         *
         * Which bytes transfered over the serial connection is defined
         * in the handed over Exchange instance.
         *
         * This method attempt to retry this synchronous request until
         * communication errors occurs or MAX_RETRIES_ALLOWED reached.
         *
         * \param taskName A name of the operation to be executed (for
         *         logging purposes)
         *
         * \param operation A pointer of Exchange
         *
         * \return true if the synchronous task could be completly executed
         *          and was successful,<br>
         *          false otherwise
         */
        bool doSyncTask(std::string taskName, exchange_ptr operation);

        /* ASYNC */
        /**
         * \brief Sends a asynchronous request.
         *
         * Which bytes transfered over the serial connection is defined
         * in the handed over Exchange instance.
         *
         * If a sending attempt fails, this method retries it a fixed
         * amount. (Setable over the option MAX_RETRIES_ALLOWED in the
         * project depended configfile.)
         *
         * \param taskName A name of the operation to be executed (for
         *        logging purposes)
         *
         * \param operation A pointer of Exchange
         *
         * \param dependency A global task number returned by other
         *        calls of this member before
         *
         * \return An positive integer (the task number) if the request
         *         of this async task could be sent successfully,<br>
         *         0 otherwise
         */
        tasknumberval startAsyncTask(std::string taskName, exchange_ptr operation, tasknumberval dependency);

        /**
         * \brief Handles all yet dispatched asynchronous requests and
         *        interrupt notifications.
         *
         * If a receiving attempt of one operation fails, this method
         * retries the completely operation a fixed amount. This will
         * include a new send attempt. (How often an operation will be
         * repeated ist setable over the option MAX_RETRIES_ALLOWED in
         * the project depended configfile.)
         *
         * \return true if all replies ot the requests could be received
         *         successfully and no recoverable errors occured in the
         *         meanwhile,<br>
         *         else otherwise
         */
        bool fetchAsyncReplies(void);

        /**
         * \brief Gets the number of all started asynchronous requests.
         *
         * \return A positve integer greater or equal to zero
         */
        uint32_t getNumberOfPendingRequests(void);

        /**
         * \brief Gets the number of all handled asynchronous requests
         *        by fetchAsyncReplies().
         *
         * \return A positve integer greater or equal to zero
         */
        uint32_t getNumberOfFinishedRequests(void);

        /**
         * \brief Writes back all by fetchAsyncReplies() handled user
         *        requests replies into the user's defined memory.
         */
        void writeReplies(void);

    private:
        bool interruptOccured(void);
        CoreIndex getTriggeringCore(void);

        serialconnection_ptr _connection;

        easycore_map_ptr _easyCoreMapPointer;

        CoreIndex _triggeringCore;

        #ifdef USE_IDS_FOR_ASYNC_OPS
        IdManager<idval>* _idManager;
        #endif

        /* buffer 1 */
        std::map<tasknumberval, std::queue<AsyncTask>> _pendingAsyncTasks;

        /* buffer 2 */
        std::queue<AsyncTask> _runningAsyncTasks;
        std::set<tasknumberval> _dependendTaskNumbers;

        /* buffer 3 */
        std::queue<AsyncTask> _finishedAsyncTasks;

        /* operation counters */
        tasknumberval _syncOperationCounter;
        tasknumberval _asyncOperationCounter;

        const retryval _MAX_RETRIES_ALLOWED;
};

#endif  // SDK_COMMUNICATION_TASKEXECUTOR_H_

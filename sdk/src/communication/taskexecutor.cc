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

#include "configuration.h" /* assert(), USE_IDS_FOR_ASYNC_OPS */
#include "communication/protocol/calculator.h"
#include "communication/protocol/exchange.h"
#include "communication/serialconnection.h"
#include "communication/synctask.h"
#include "communication/taskexecutor.h"
#include "easycores/easycore.h"
#include "utils/config/configurationfile.h"
#include "utils/log/log.h"

TaskExecutor::TaskExecutor(serialconnection_ptr sc, easycore_map_ptr coreMap) :
    _connection(sc),
    _easyCoreMapPointer(coreMap),
    _triggeringCore(SPECIAL_CORE_INDICES::NO_FPGA_ASSOCIATION),
    _syncOperationCounter(0),
    _asyncOperationCounter(0),
    _MAX_RETRIES_ALLOWED(ConfigurationFile::getInstance().getMaximumRetriesAllowed())
{
    #ifdef USE_IDS_FOR_ASYNC_OPS
    _idManager = new IdManager<idval>();
    #endif
}

TaskExecutor::~TaskExecutor()
{
    #ifdef USE_IDS_FOR_ASYNC_OPS
    delete _idManager;
    _idManager = NULL;
    #endif
}

bool TaskExecutor::doSyncTask(std::string taskName, exchange_ptr operation)
{
    _syncOperationCounter++;

    /*
     * The correct procedure would be now to disable globally the
     * interrupts. Because of there is no existing exchange for the soc
     * doing this (only enable interrupts is possible), that seems to be
     * a big disadvantage of the communication protocol between host and
     * easyFPGA. But here in our case is that not really neccessary.
     * Why? Look ahead. ;-)
     *
     * If an interrupt would be triggered while sync operation execution,
     * the receive() method intercept this and write a notification to
     * this class. Then the member fetchAsyncReplies() check at the end
     * whether an interrupt occured and might call a user defined callback.
     * That's the standard procedure how the framework implements the
     * interrupt system.
     *
     * So, due to the asynchronous character of interrupts, anyway the
     * user have to call explicit the member fetchAsyncReplies() for
     * executing own implemented interrupt service routines.
     */

    /*
     * This function will receive the requested bytes immediately after
     * sending. Thats why we have to handle all requested async tasks at
     * first which is accociated by fetching all received bytes from the
     * receive queue. (Left bytes from async operations would disturb.)
     *
     * Yes, here could be called a callback of a triggered interrupt for
     * a recent time until the end of this method.
     */
    this->fetchAsyncReplies();

    /*
     * Next, create and execute a Task. As long as errors occur, retry
     * this task for certain times. (MAX_RETRIES_ALLOWED defines the
     * maximum number of retries.)
     */
    SyncTask task(taskName, _connection, operation, &_triggeringCore, _syncOperationCounter);

    Log().Get(DEBUG) << "Start " << task.getName();

    do {
        Log().Get(DEBUG) << "Attempt " << (int32_t)task.getExecutionCount() << "/" << (int32_t)_MAX_RETRIES_ALLOWED;

        task.execute();

        switch (task.getReceiveState()) {
            case Task::RECEIVE_STATE::RECEIVE_NOT_EXECUTED:
                Log().Get(DEBUG) << "The request couldn't sent, so a receive is impossible. Try it once more...";
                break;

            case Task::RECEIVE_STATE::RECEIVE_SUCCESS:
                /*
                 * Write received reply if there is anything to write.
                 */
                task.getExchange()->writeResults();
                Log().Get(DEBUG) << "Task " << task.getName() << " successfully executed.";
                return true;

            case Task::RECEIVE_STATE::RECEIVE_FAILURE:
                Log().Get(DEBUG) << "The fpga couldn't process or understand the request. Try it once more...";
                break;

            case Task::RECEIVE_STATE::RECEIVE_CHECKSUM_ERROR:
                Log().Get(DEBUG) << "The checksum check failed! Retry the request...";
                break;

            case Task::RECEIVE_STATE::RECEIVE_UNEXPECTED_OPCODE_ERROR:
                /**
                 * \todo What to do in this case?
                 */
                assert(false);
                return false;

            case Task::RECEIVE_STATE::RECEIVE_CONNECTION_ERROR:
                /**
                 * \todo What to do in this case?
                 *
                 * Reason 1: Timeout too small
                 * Reason 2: Connection lost...
                 */
                assert(false);
                return false;
        }
    } while (task.getExecutionCount() <= _MAX_RETRIES_ALLOWED);

    Log().Get(WARNING) << "No execution success of task " << task.getName() << " within " << (int32_t)(task.getExecutionCount()-1) << " retries.";
    return false;
}

tasknumberval TaskExecutor::startAsyncTask(std::string taskName, exchange_ptr operation, tasknumberval dependency)
{
    _asyncOperationCounter++;

    AsyncTask task(taskName, _connection, operation, &_triggeringCore, _asyncOperationCounter);

    Log().Get(DEBUG) << "Start " << task.getName();
    Log().Get(DEBUG) << "Attempt " << (int32_t)task.getExecutionCount() << "/" << (int32_t)_MAX_RETRIES_ALLOWED;

    #ifdef USE_IDS_FOR_ASYNC_OPS
    idval id = 0;
    if (_idManager->getFreeId(&id)) {
        Log().Get(DEBUG) << "This task has been assigned the id " << (int32_t)id << ".";
        operation->setId(id);
    } else {
        Log().Get(WARNING) << "The IdManager has no more available ids. Try to free all used ones...";
        this->fetchAsyncReplies();

        if (_idManager->getFreeId(&id)) {
            Log().Get(DEBUG) << "Attempt to get a new id successful!";
            Log().Get(DEBUG) << "This task has been assigned the id " << (int32_t)id << ".";
            operation->setId(id);
        }
        else {
            Log().Get(WARNING) << "Attempt to get a new id wasn't successful!";
            Log().Get(WARNING) << "Task " << task.getName() << " could not be started!";
            return 0;
        }
    }
    #endif

    if ((dependency > 0) && (_dependendTaskNumbers.find(dependency) != _dependendTaskNumbers.end())) {
        Log().Get(DEBUG) << "This task have to be retained! [Dependency to ongoing task " << (int32_t)dependency << " found]";
        _pendingAsyncTasks[dependency].push(task);
        return _asyncOperationCounter;
    }
    else {
        if (dependency > 0) {
            Log().Get(DEBUG) << "This task can be executed. [Task " << (int32_t)dependency << " to which it depends already completed]";
        }
        else {
            Log().Get(DEBUG) << "This task can be executed. [No Dependencies]";
        }

        task.executeSend();

        switch (task.getSendState()) {
            case Task::SEND_STATE::SEND_SUCCESS:
                _dependendTaskNumbers.insert(_asyncOperationCounter);
                _runningAsyncTasks.push(task);
                Log().Get(DEBUG) << "Request of task " << task.getName() << " successfully sent.";
                return _asyncOperationCounter;

            case Task::SEND_STATE::SEND_FAILURE:
                Log().Get(ERROR) << "Request of task " << task.getName() << " not successfully sent!";
                Log().Get(ERROR) << "There might be problems with the serial connection...";
                return 0;

            default:
                /* This case must not occur! */
                Log().Get(DEBUG) << "An internal error occured!";
                assert(false);
                return 0;
        }
    }
}

bool TaskExecutor::fetchAsyncReplies(void)
{
    bool success = true;

    /*
     * Check whether a task was executed and we didn't fetch a reply yet.
     * For this case we remembered all executed tasks in the queue
     * _runningAsyncTasks.
     *
     * If there will be no running tasks, we don't regard the other task
     * buffer _pendingAsyncTasks. This buffer will only hold at least one
     * element if there were executed tasks.
     */
    while (_runningAsyncTasks.size() > 0) {
        assert(!_runningAsyncTasks.empty());

        /* Get the first sent but not yet processed task. */
        AsyncTask task(_runningAsyncTasks.front());
        _runningAsyncTasks.pop();

        /* Try to receive a reply. (Here can occur an interrupt!) */
        task.executeReceive();

        if (task.getReceiveState() == Task::RECEIVE_STATE::RECEIVE_SUCCESS) {
            #ifdef USE_IDS_FOR_ASYNC_OPS
            idval id = task.getExchange()->getId();
            _idManager->releaseId(id);
            Log().Get(DEBUG) << "The id " << (int32_t)id << " was released.";
            #endif

            auto it = _dependendTaskNumbers.find(task.getNumber());
            if (it != _dependendTaskNumbers.end()) {
                _dependendTaskNumbers.erase(it);
                auto q(_pendingAsyncTasks[task.getNumber()]);
                _pendingAsyncTasks.erase(task.getNumber());

                if (task.getExchange()->hasACallback()) {
                    task.getExchange()->writeResults();
                    task.getExchange()->executeCallback();
                }
                else {
                    _finishedAsyncTasks.push(task);
                }

                while (q.size() > 0) {
                    AsyncTask task2(q.front());
                    q.pop();

                    task2.executeSend();

                    if (task2.getSendState() == Task::SEND_STATE::SEND_SUCCESS) {
                        _dependendTaskNumbers.insert(task2.getNumber());
                        _runningAsyncTasks.push(task2);
                        Log().Get(DEBUG) << "Request of task " << task2.getName() << " successfully sent.";
                    }
                    else {
                        Log().Get(ERROR) << "Request of task " << task2.getName() << " not successfully sent!";
                    }
                }
            }
            else {
                if (task.getExchange()->hasACallback()) {
                    /*
                     * Write back the reply of this task. We might need
                     * that step because the callback could be process
                     * some received data.
                     */
                    task.getExchange()->writeResults();

                    /* Execute the associated callback. */
                    task.getExchange()->executeCallback();

                    /*
                     * So, at this point the task is finished. We don't
                     * have to put it into the buffer _finishedAsyncTasks
                     * as we do with all other tasks without a callback.
                     * (Because of the task's results are already written.)
                     */
                }
                else {
                    _finishedAsyncTasks.push(task);
                }
            }
        }
        else {
            if (task.getExecutionCount() <= _MAX_RETRIES_ALLOWED) {
                Log().Get(DEBUG) << "Async task " << task.getName() << " not successfully executed. Start this task once again.";
                Log().Get(DEBUG) << "Attempt " << (int32_t)task.getExecutionCount() << "/" << (int32_t)_MAX_RETRIES_ALLOWED;
                task.executeSend();
                if (task.getSendState() == Task::SEND_STATE::SEND_SUCCESS) {
                    Log().Get(DEBUG) << "Request of task " << task.getName() << " successfully sent.";
                    _runningAsyncTasks.push(task);
                }
                else {
                    Log().Get(ERROR) << "Request of task " << task.getName() << " not successfully sent!";
                    success = false;
                }
            }
            else {
                Log().Get(ERROR) << "Max execution retries for async task " << task.getName() << " reached. This operation will be aborted now.";
                success = false;
            }
        }
    }

    int32_t remainingBytes = _connection->getReceiveQueueSize();
    if (remainingBytes > 0) {
        Log().Get(DEBUG) << "Method fetchAsyncReplies() executed, but there are still bytes in the receive queue!";
        Log().Get(DEBUG) << "Bytes in the receive queue: " << remainingBytes;
        Log().Get(DEBUG) << "Pending ops: " << _pendingAsyncTasks.size();
        Log().Get(DEBUG) << "Running ops: " << _runningAsyncTasks.size();
        Log().Get(DEBUG) << "Finished ops: " << _finishedAsyncTasks.size();

        if (remainingBytes < 3) {
            /**
             * \todo What to do in this case?
             *
             * The only possibility seems to be:
             * 1) Flush the send / receive queues
             * 2) Complete reset of all data structures
             * 3) Retry all current pending tasks
             */

            byte reply[2];
            if (!_connection->receive(reply, 2, 1000000)) {
                assert(false);
            }

            for (int32_t i=1; i<=remainingBytes; i++) {
                Log().Get(DEBUG) << "Bytes in the receive queue: " << std::hex << (uint32_t)reply[i];
            }

            assert(false);
        }
        else {
            /* Here can hide an interrupt! */

            byte reply[3];
            if (!_connection->receive(reply, 1, 1000000)) {
                assert(false);
            }

            if (reply[0] == Exchange::SHARED_REPLY_CODES::INTERRUPT) {
                if (_connection->receive(reply+1, 2, 1000000)) {
                    byte calculatedParity = reply[1];
                    Log().Get(DEBUG) << "Calculated parity byte: 0x" << std::hex << (uint32_t)calculatedParity;
                    byte transmittedParity = reply[2];
                    Log().Get(DEBUG) << "Transmitted parity byte: 0x" << std::hex << (uint32_t)transmittedParity;
                    if (calculatedParity == transmittedParity) {
                        _triggeringCore = (CoreIndex)reply[1];
                    }
                    else {
                        assert(false);
                    }
                }
                else {
                    assert(false);
                }
            }
            else {
                assert(false);
            }
        }
    }

    if (this->interruptOccured() && (_easyCoreMapPointer != NULL)) {
        CoreIndex i = this->getTriggeringCore();
        Log().Get(DEBUG) << "Core " << (int32_t)i << " has triggered an interrupt!";

        auto it = _easyCoreMapPointer->find(i);
        assert(it != _easyCoreMapPointer->end());
        if (it != _easyCoreMapPointer->end()) {
            if (it->second->executeCallback()) {
                Log().Get(DEBUG) << "Callback routine successfully executed.";
            }
            else {
                Log().Get(WARNING) << "For this interrupt was no callback registered!";
            }
        }
    }

    return success;
}

uint32_t TaskExecutor::getNumberOfPendingRequests(void)
{
    uint32_t pendingNumber = 0;
    for (auto it=_pendingAsyncTasks.begin(); it!=_pendingAsyncTasks.end(); ++it) {
        pendingNumber += it->second.size();
    }
    Log().Get(DEBUG) << "Pending requests: " << pendingNumber;

    uint32_t runningNumber = _runningAsyncTasks.size();
    Log().Get(DEBUG) << "Running requests: " << runningNumber;

    return pendingNumber + runningNumber;
}

uint32_t TaskExecutor::getNumberOfFinishedRequests(void)
{
    Log().Get(DEBUG) << "Finished requests: " << _finishedAsyncTasks.size();
    return _finishedAsyncTasks.size();
}

void TaskExecutor::writeReplies(void)
{
    while (_finishedAsyncTasks.size() > 0) {
        AsyncTask task(_finishedAsyncTasks.front());

        task.getExchange()->writeResults();
        Log().Get(DEBUG) << "Task " << task.getName() << " successfully executed.";

        _finishedAsyncTasks.pop();
    }
}

bool TaskExecutor::interruptOccured(void)
{
    if (_triggeringCore > 0) {
        return true;
    }
    else {
        return false;
    }
}

CoreIndex TaskExecutor::getTriggeringCore(void)
{
    CoreIndex tc = _triggeringCore;
    _triggeringCore = SPECIAL_CORE_INDICES::NO_FPGA_ASSOCIATION;
    return tc;
}

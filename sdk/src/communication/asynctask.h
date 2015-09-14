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

#ifndef SDK_COMMUNICATION_ASYNCTASK_H_
#define SDK_COMMUNICATION_ASYNCTASK_H_

#include "communication/protocol/exchange_ptr.h"
#include "communication/serialconnection_ptr.h"
#include "communication/task.h"
#include "communication/types.h"
#include "easycores/types.h"

#include <string>

/**
 * \brief Asynchronous execution environment for an Exchange
 *
 * Implements a asynchronous task.
 */
class AsyncTask : public Task
{
    public:
        AsyncTask(std::string name, serialconnection_ptr sc, exchange_ptr ex, CoreIndex* interruptTriggeringCore, uint64_t number);
        AsyncTask(const AsyncTask& task);
        ~AsyncTask();

        /**
         * \brief Send a asynchronous request specified by the pointer to
         *        an Exchange.
         */
        void executeSend(void);

        /**
         * \brief Receives a asynchronous request specified by the pointer
         *        to an Exchange.
         */
        void executeReceive(void);
};

#endif  // SDK_COMMUNICATION_ASYNCTASK_H_

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

#ifndef SDK_COMMUNICATION_SYNCTASK_H_
#define SDK_COMMUNICATION_SYNCTASK_H_

#include "communication/protocol/exchange_ptr.h"
#include "communication/serialconnection_ptr.h"
#include "communication/task.h"
#include "communication/types.h"
#include "easycores/types.h"


#include <string>

/**
 * \brief Synchronous execution environment for an Exchange
 *
 * Implements a synchronous task.
 */
class SyncTask : public Task
{
    public:
        SyncTask(std::string name, serialconnection_ptr sc, exchange_ptr ex, CoreIndex* interruptTriggeringCore, uint64_t number);
        SyncTask(const SyncTask& task);
        ~SyncTask();

        /**
         * \brief Attempt to completely execute a synchronous task.
         *
         * This method sends a request and receives the corresponding reply.
         */
        void execute(void);
};

#endif  // SDK_COMMUNICATION_SYNCTASK_H_

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

#include "utils/os/time_helper.h"
#include "utils/unittest/tester.h"
#include "utils/log/log.h"

bool Tester::runTest(void)
{
    Log().Get(INFO) << "START " << this->testName();
    timevalue start = getCurrentTimeInMillis();

    bool success = this->testMethod();

    timevalue end = getCurrentTimeInMillis();
    if (success) {
        Log().Get(INFO) << "TEST SUCCESSFUL";
    }
    else {
        Log().Get(INFO) << "TEST FAILED";
    }
    Log().Get(INFO) << "Test has taken " << std::dec << (int32_t)(end-start) << "ms.";

    return !success;
}

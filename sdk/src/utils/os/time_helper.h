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

#ifndef SDK_UTILS_OS_TIMEHELPER_H_
#define SDK_UTILS_OS_TIMEHELPER_H_

/**
 * \file src/utils/os/time_helper.h
 *
 * \brief Operating system depending timing types and functions
 */

#include <time.h>
#include <string>

/**
 * \brief Stores a timestamp or a time difference.
 *
 * How this type have to be interpreted depends on the use case.
 */
typedef int64_t timevalue;

/**
 * \brief Determines the current unix timestamp with an accuracy of
 *        milliseconds.
 *
 * \return A unix timestamp
 */
inline timevalue getCurrentTimeInMillis(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME , &ts) == 0) {
        return (timevalue)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
    }
    else {
        return -1;
    }
}

/**
 * \brief Return an formatted time string.
 *
 * \return A string with format "HH:MM:SS"
 */
inline std::string getCurrentTimeString(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        /*
         * struct tm contains all possible time values
         * (seconds, hour, day, year, timezone ...)
         */
        tm r;

        /* mapping timestamp to tm */
        localtime_r(&ts.tv_sec, &r);

        char buffer[50];
        /* specifier "%X" correspond to time format "14:55:02" */
        strftime(buffer, sizeof(buffer), "%X", &r);

        return buffer;
    }
    else {
        return "";
    }
}

#endif  // SDK_UTILS_OS_TIMEHELPER_H_

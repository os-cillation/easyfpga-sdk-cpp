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

#ifndef SDK_UTILS_LOG_LOG_H_
#define SDK_UTILS_LOG_LOG_H_

#include "utils/log/types.h"

#include <cstdio>
#include <sstream>

/**
 * \brief Defines a class which is constructed at every debugging output to
 *        standard in-/output.
 *
 * To set the outputted log level you have to modify the LogLevel constant
 * MIN_LOG_LEVEL_OUTPUT in your configuration.h.
 */
class Log {
    public:
        /**
         * Creates an Log object.
         */
        Log();

        /**
         * The destructor prints out the log lines.
         */
        ~Log();

        /**
         * \brief Get an output stream for inserting a log message.
         *
         * \return A stream object to print out. Adds first important
         * infoormation before the actual log line.
         */
        std::ostringstream& Get(LogLevel level = DEBUG);

        /**
         * Convert a constant LogLevel to an LogLevel output string.
         */
        static std::string toString(LogLevel level);

    private:
        /**
         * A private copy constructor for access protection issues.
         */
        Log(const Log&);

        /**
         * A private assignment constructor for access protection issues.
         */
        Log& operator= (const Log&);

        /**
         * A stream object to remember the message to be outputted.
         */
        std::ostringstream _os;

        /**
         * Stores the given log level for this log message at the time
         * of call.
         */
        LogLevel _messageLevel;

        FILE* const _LOG_OUTPUT_TARGET;

        const LogLevel _LOG_MIN_OUTPUT_LEVEL;
};

#endif  // SDK_UTILS_LOG_LOG_H_

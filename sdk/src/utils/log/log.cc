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

#include "utils/config/configurationfile.h"
#include "utils/log/log.h"
#include "utils/os/time_helper.h" /* getCurrentTimeString() */

Log::Log() :
    _LOG_OUTPUT_TARGET(ConfigurationFile::getInstance().getLogOutputTarget()),
    _LOG_MIN_OUTPUT_LEVEL(ConfigurationFile::getInstance().getMinimumLogOutputLevel())
{
}

Log::~Log()
{
    if (_messageLevel >= _LOG_MIN_OUTPUT_LEVEL) {
        _os << std::endl;

        fprintf(_LOG_OUTPUT_TARGET, "%s", _os.str().c_str());
        fflush(_LOG_OUTPUT_TARGET);
    }
}

std::ostringstream& Log::Get(LogLevel level)
{
    _os << "+ " << getCurrentTimeString() << "  " << this->toString(level) << "\t";
    _messageLevel = level;
    return _os;
}

std::string Log::toString(LogLevel level)
{
    static const char* const buffer[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    return buffer[level];
}

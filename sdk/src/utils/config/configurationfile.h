/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Authors: Johannes Hein <support@os-cillation.de>
 *           Simon Gansen
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

#ifndef SDK_UTILS_CONFIG_CONFIGURATIONFILE_H_
#define SDK_UTILS_CONFIG_CONFIGURATIONFILE_H_

#include "communication/types.h"
#include "easycores/types.h"
#include "utils/log/types.h"

#include <cstdio> /* FILE* */
#include <string>

/**
 * \brief A configuration file manager (singleton)
 */
class ConfigurationFile
{
    public:
        /**
         * \brief Returns an unique instance of this class.
         *
         * Note: The class's private constructor sets the default values
         * of the configuration file.
         */
        static ConfigurationFile& getInstance(void);

        /**
         * \brief Returns the location of the soc hdl sources.
         */
        std::string getSocDirectory(void);

        /**
         * \brief Returns the location of the shared library
         */
        std::string getLibraryDirectory(void);

        /**
         * \brief Returns the location of the header files.
         */
        std::string getHeaderDirectory(void);

        /**
         * \brief Returns the location of the template files.
         */
        std::string getTemplatesDirectory(void);

        /**
         * \brief Returns an absolute path to the directory where the
         *        operating system mounts all connected serial devices.
         */
        std::string getUsbDevicesPath(void);

        /**
         * \brief Returns a operating system specific file name pattern
         *        by with the framework can filter the usb devices out of
         *        all serial devices.
         */
        std::string getUsbDeviceIdentifier(void);

        /**
         * \brief Returns a maximum number of operation retries if errors
         *        occur.
         */
        retryval getMaximumRetriesAllowed(void);

        /**
         * \brief Returns the log target set by the user.
         */
        FILE* getLogOutputTarget(void);

        /**
         * \brief Returns the log level set by the user.
         */
        LogLevel getMinimumLogOutputLevel(void);

        /**
         * \brief Returns the current operation mode of the framework.
         *        (If the framework executes all exchanges synchronous
         *        or asynchronous.)
         */
        OPERATION_MODE getOperationMode(void);

    private:
        ConfigurationFile();
        ~ConfigurationFile();

        /*
         * Dont forget to declare the next two functions. You want to
         * make sure they are unacceptable otherwise you may accidentally
         * get copies of your singleton appearing.
         */

        /*
         * The private copy constructor
         * Don't implement this!
         */
        ConfigurationFile(const ConfigurationFile& c);

        /*
         * The private assignment constructor
         * Don't implement this!
         */
        void operator=(const ConfigurationFile& c);

        bool createConfigurationFile(std::string path);

        bool parseConfigurationFile(void);
        bool parse(std::string content, std::string paramName, std::string& paramValue);

        std::string _SOC_DIRECTORY;
        std::string _LIBRARY_DIRECTORY;
        std::string _HEADER_DIRECTORY;
        std::string _TEMPLATES_DIRECTORY;

        std::string _USB_DEVICE_PATH;
        std::string _USB_DEVICE_IDENTIFIER;

        std::string _MAX_RETRIES_ALLOWED;

        std::string _LOG_OUTPUT_TARGET;
        FILE* _currentLogOutputTarget;
        std::string _LOG_MIN_OUTPUT_LEVEL;

        std::string _FRAMEWORK_OPERATION_MODE;
};

#endif  // SDK_UTILS_CONFIG_CONFIGURATIONFILE_H_

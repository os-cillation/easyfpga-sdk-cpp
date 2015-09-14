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

#include "configuration.h" /* assert(1), USE_ONE_GLOBAL_CONFIGFILE */
#include "utils/config/configurationfile.h"
#include "utils/os/file.h"
#include "utils/os/types.h"

#include <cstdio> /* FILE*, stdout */
#include <iostream>
#include <list>
#include <sstream>
#include <initializer_list>

static bool configFileAlreadyParsed = false;

ConfigurationFile& ConfigurationFile::getInstance(void)
{
    /*
     * _instance is guaranteed to be destroyed. It will be instantiated
     * on first use.
     */
    static ConfigurationFile _instance;

    return _instance;
}

ConfigurationFile::ConfigurationFile() :
    _SOC_DIRECTORY("/usr/local/share/easyfpga/soc"),
    _LIBRARY_DIRECTORY("/usr/local/lib"),
    _HEADER_DIRECTORY("/usr/local/include/easyfpga"),
    _TEMPLATES_DIRECTORY("/usr/local/share/easyfpga/templates"),
    _USB_DEVICE_PATH("/dev/"),
    _USB_DEVICE_IDENTIFIER("ttyUSB"),
    _MAX_RETRIES_ALLOWED("3"),
    _LOG_OUTPUT_TARGET("STDOUT"),
    _currentLogOutputTarget(NULL),
    _LOG_MIN_OUTPUT_LEVEL("0"),
    _FRAMEWORK_OPERATION_MODE("sync")
{
}

ConfigurationFile::~ConfigurationFile()
{
    if (_currentLogOutputTarget != stdout) {
        fclose(_currentLogOutputTarget);
    }
}

bool ConfigurationFile::parseConfigurationFile(void)
{
    std::cout << "+ CONFIGURATION SETTINGS" << std::endl;

    #ifdef USE_ONE_GLOBAL_CONFIGFILE

    std::string path = GLOBAL_CONFIGFILE_ABSOLUTE_PATH;

    #else

    FILE* pipe = popen("pwd", "r");
    std::string path = "";
    char c;
    while ((c = fgetc(pipe)) != EOF) {
        if (c != '\n') {
            path += c;
        }
    }
    pclose(pipe);
    path += "/project.conf";

    #endif

    std::cout << "+ Selected file: " << path << std::endl;

    /**
     * \todo It's your turn: make this pasing shorter, nicer and more
     * efficient.
     * (E.g. storing each option in a suitable data structure. Then
     * something like the following could help.)
     *
     * std::initializer_list<std::string> paramNamesInit = {
     *     "SOC_DIRECTORY",
     *     "LIBRARY_DIRECTORY",
     *     "GENERATOR_DIRECTORY",
     *     "USB_DEVICE_PATH",
     *     "USB_DEVICE_IDENTIFIER",
     *     "MAX_RETRIES_ALLOWED",
     *     "LOG_OUTPUT_TARGET",
     *     "MIN_LOG_LEVEL_OUTPUT"
     * };
     * std::list<std::string> paramNames(paramNamesInit);
     */

    File configFile(path);
    if (configFile.exists()) {
        std::cout << "+ Status: File exists. Parse it..." << std::endl;

        std::string content;

        configFile.writeIntoString(content);

        bool success = true;
        success &= this->parse(content, "SOC_DIRECTORY", _SOC_DIRECTORY);
        success &= this->parse(content, "LIBRARY_DIRECTORY", _LIBRARY_DIRECTORY);
        success &= this->parse(content, "HEADER_DIRECTORY", _HEADER_DIRECTORY);
        success &= this->parse(content, "TEMPLATES_DIRECTORY", _TEMPLATES_DIRECTORY);
        success &= this->parse(content, "USB_DEVICE_PATH", _USB_DEVICE_PATH);
        success &= this->parse(content, "USB_DEVICE_IDENTIFIER", _USB_DEVICE_IDENTIFIER);
        success &= this->parse(content, "MAX_RETRIES_ALLOWED", _MAX_RETRIES_ALLOWED);
        success &= this->parse(content, "LOG_OUTPUT_TARGET", _LOG_OUTPUT_TARGET);
        success &= this->parse(content, "MIN_LOG_LEVEL_OUTPUT", _LOG_MIN_OUTPUT_LEVEL);
        success &= this->parse(content, "FRAMEWORK_OPERATION_MODE", _FRAMEWORK_OPERATION_MODE);

        return success;
    }
    else {
        std::cout << "+ Status: File not found. Create a new project.conf file..." << std::endl;
        return this->createConfigurationFile(path);
    }
}

bool ConfigurationFile::parse(const std::string content, std::string paramName, std::string& paramValue)
{
    size_t digit = content.find(paramName);

    if (digit != std::string::npos) {
        digit += paramName.length();

        if (content[digit++] == '=') {
            std::string newString;
            while (content[digit] != '\n') {
                newString += content[digit];
                digit++;
            }

            paramValue = newString;
            return true;
        }
    }

    return false;
}

std::string ConfigurationFile::getSocDirectory(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return _SOC_DIRECTORY;
}

std::string ConfigurationFile::getLibraryDirectory(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return _LIBRARY_DIRECTORY;
}

std::string ConfigurationFile::getHeaderDirectory(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return _HEADER_DIRECTORY;
}

std::string ConfigurationFile::getTemplatesDirectory(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return _TEMPLATES_DIRECTORY;
}

std::string ConfigurationFile::getUsbDevicesPath(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return _USB_DEVICE_PATH;
}

std::string ConfigurationFile::getUsbDeviceIdentifier(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return _USB_DEVICE_IDENTIFIER;
}

retryval ConfigurationFile::getMaximumRetriesAllowed(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    return (retryval)std::stoi(_MAX_RETRIES_ALLOWED);
}

FILE* ConfigurationFile::getLogOutputTarget(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
        if (_LOG_OUTPUT_TARGET.compare("STDOUT") != 0) {
            std::cout << "All logging messages will be written to '" << _LOG_OUTPUT_TARGET << "'." << std::endl;
        }
    }

    if (_currentLogOutputTarget == NULL) {
        if (_LOG_OUTPUT_TARGET.compare("STDOUT") == 0) {
            _currentLogOutputTarget = stdout;
        }
        else {
            /*
             * Option "w" means overriding a mightly existing log.
             */
            _currentLogOutputTarget = fopen(_LOG_OUTPUT_TARGET.c_str(), "w");
        }

        assert(_currentLogOutputTarget != NULL);
    }

    return _currentLogOutputTarget;
}

LogLevel ConfigurationFile::getMinimumLogOutputLevel(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    switch (std::stoi(_LOG_MIN_OUTPUT_LEVEL)) {
        case 1:
            return LogLevel::INFO;
        case 2:
            return LogLevel::WARNING;
        case 3:
            return LogLevel::ERROR;

        default:
            return LogLevel::DEBUG;
    }
}

OPERATION_MODE ConfigurationFile::getOperationMode(void)
{
    if (!configFileAlreadyParsed) {
        this->parseConfigurationFile();
        configFileAlreadyParsed = true;
    }

    if (_FRAMEWORK_OPERATION_MODE.compare("sync") == 0) {
        return OPERATION_MODE::SYNC;
    } else if (_FRAMEWORK_OPERATION_MODE.compare("async") == 0) {
        return OPERATION_MODE::ASYNC;
    }
    else {
        assert(false);
        exit(-1);
    }
}

bool ConfigurationFile::createConfigurationFile(std::string path)
{
    File newConfigFile(path);

    std::stringstream ss;
    ss << "# easyFPGA PROJECT CONFIGURATION FILE" << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# VHDL BINARY GENERATION" << std::endl;
    ss << "# Path to the SOC repository" << std::endl;
    ss << "SOC_DIRECTORY=" << _SOC_DIRECTORY << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# Location of the shared library" << std::endl;
    ss << "LIBRARY_DIRECTORY=" << _LIBRARY_DIRECTORY << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# Location of the header files" << std::endl;
    ss << "HEADER_DIRECTORY=" << _HEADER_DIRECTORY << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# Location of the template files" << std::endl;
    ss << "TEMPLATES_DIRECTORY=" << _TEMPLATES_DIRECTORY << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# SETTINGS FOR FINDING AN EASYFGPA BOARD" << std::endl;
    ss << "# Location of the system devices in the filesystem." << std::endl;
    ss << "# Value: /an/absolute/path/to/a/directory/" << std::endl;
    ss << "USB_DEVICE_PATH=" << _USB_DEVICE_PATH << std::endl;
    ss << "# Special name pattern to find an device in the directory of USB_DEVICE_PATH" << std::endl;
    ss << "USB_DEVICE_IDENTIFIER=" << _USB_DEVICE_IDENTIFIER << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# COMMUNICATION SETTINGS" << std::endl;
    ss << "# The maximum permissible number of retries for one operation (if e.g." << std::endl;
    ss << "# errors or timeouts occurs)." << std::endl;
    ss << "# Values between 0 and 255 are possible." << std::endl;
    ss << "MAX_RETRIES_ALLOWED=" << _MAX_RETRIES_ALLOWED << std::endl;
    ss << "# Decide whether to use a synchronous or asynchronous operation mode." << std::endl;
    ss << "# Values of set {sync, async} are possible." << std::endl;
    ss << "FRAMEWORK_OPERATION_MODE=" << _FRAMEWORK_OPERATION_MODE << std::endl;
    ss << std::endl;
    ss << std::endl;
    ss << "# LOGGING SETTINGS" << std::endl;
    ss << "# Sets the output target for the log." << std::endl;
    ss << "# Possible values:" << std::endl;
    ss << "# - STDOUT: for the terminal" << std::endl;
    ss << "# - /absolute/path/to/a/file" << std::endl;
    ss << "LOG_OUTPUT_TARGET=" << _LOG_OUTPUT_TARGET << std::endl;
    ss << "# Defines from which level the log messages appears. The larger the log" << std::endl;
    ss << "# level the less messages will appear but they are the more important ones." << std::endl;
    ss << "# For a productive use of the framework should be used 1." << std::endl;
    ss << "# Possible values:" << std::endl;
    ss << "# - 0: all messages including debug messages" << std::endl;
    ss << "# - 1: all messages excluding debug messages" << std::endl;
    ss << "# - 2: all warnings and errors" << std::endl;
    ss << "# - 3: only errors" << std::endl;
    ss << "MIN_LOG_LEVEL_OUTPUT=" << _LOG_MIN_OUTPUT_LEVEL << std::endl;
    ss << "" << std::endl;

    return newConfigFile.createWithContent(ss.str());
}

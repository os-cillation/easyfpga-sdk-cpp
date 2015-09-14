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

#ifndef SDK_CONFIGURATION_H_
#define SDK_CONFIGURATION_H_

/*
 * DEBUG / RELEASE USAGE OF THE FRAMEWORK
 *
 * Defining the following symbol has an impact on including the libary
 * assert.h. So, no assertions will be compiled if you #define NDEBUG.
 */

//#define NDEBUG
#undef NDEBUG
#include <assert.h>

/*
 * CONFIGURATION FILE USAGE
 *
 * With the symbol USE_ONE_GLOBAL_CONFIGFILE you can decide if the
 * framework should use one global configuration file for all of your
 * easyFPGA projects or project dependend ones.
 *
 * In case of an global configuration file you have specify with the
 * symbol GLOBAL_CONFIGFILE_ABSOLUTE_PATH where the framework finds the
 * configuration file (absolute path).
 * In case of project dependend configuration files the framework look
 * at the current working directory for the config file. So you have to
 * change into the desired directory before executing your different
 * projects.
 */

#undef USE_ONE_GLOBAL_CONFIGFILE

#ifdef USE_ONE_GLOBAL_CONFIGFILE
#define GLOBAL_CONFIGFILE_ABSOLUTE_PATH "~/.config/easyfpga-cpp.conf"
#endif

/*
 * COMMUNICATION / SERIAL CONNECTION SETTINGS
 *
 * Decide whether to use ids for async operations or not. Use therefore:
 * - #define USE_IDS_FOR_ASYNC_OPS to enable id usage
 * - #undef USE_IDS_FOR_ASYNC_OPS to disable id usage
 */

#undef USE_IDS_FOR_ASYNC_OPS

/*
 * Hardware specifications
 */

#include <cstdint> /* special ints */

static const uint8_t BANK_COUNT = 3;
static const uint8_t PIN_COUNT = 24;

static const uint32_t WISHBONE_CLOCK_FREQUENCY = 80000000; // = 80 MHz

/* Usb connection speed */
#include <termios.h> /* speed_t, several B connection speeds */
//static const speed_t CONNECTIONSPEED = B115200; // = 115200 baud
static const speed_t CONNECTIONSPEED = 3000000; // = 3000000 baud

#endif  // SDK_CONFIGURATION_H_

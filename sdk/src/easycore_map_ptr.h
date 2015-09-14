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

#ifndef SDK_EASYCORE_MAP_PTR_H_
#define SDK_EASYCORE_MAP_PTR_H_

/**
 * \file src/easycore_map_ptr.h
 *
 * \brief Defines a type for assigning a CoreIndex to an EasyCore instance.
 */

#include "easycores/easycore_ptr.h"
#include "easycores/types.h"

#include <map>
#include <memory>

/**
 * \brief Type with which the class EasyFpga stores its easyCores.
 *
 * Note: Outsourcing this type is important because it will be used at
 * many points.
 */
typedef std::shared_ptr<std::map<CoreIndex, easycore_ptr>> easycore_map_ptr;

#endif  // SDK_EASYCORE_MAP_PTR_H_

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

#include "configuration.h" /* assert(1) */
#include "easycores/easycore.h"
#include "easycores/pin.h"
#include "utils/config/configurationfile.h"

EasyCore::EasyCore(const CoreIndex uniqueNumber) :
    _GLOBAL_CORE_NUMBER(uniqueNumber),
    _OPERATION_MODE(ConfigurationFile::getInstance().getOperationMode()),
    _index(SPECIAL_CORE_INDICES::NO_FPGA_ASSOCIATION),
    _communicator(nullptr),
    _callback(NULL)
{
}

EasyCore::~EasyCore()
{
}

const CoreIndex& EasyCore::getTheGlobalCoreNumber(void)
{
    /* 0 is reserved for the GPIO pins of EasyFpga */
    assert(_GLOBAL_CORE_NUMBER != 0);
    return _GLOBAL_CORE_NUMBER;
}

void EasyCore::setIndex(CoreIndex index)
{
    _index = index;
}

CoreIndex EasyCore::getIndex(void)
{
    return _index;
}

void EasyCore::setCommunicator(communicator_ptr c)
{
    assert(c != nullptr);
    _communicator = c;
    assert(_communicator != nullptr);
}

const communicator_ptr& EasyCore::getCommunicator(void)
{
    assert(_communicator != nullptr);
    return _communicator;
}

pin_ptr EasyCore::getPin(PinConst pin)
{
    /*
     * If this method gets a non compatible pin constant for this kind
     * of easyCore, find(1) will return std::map::end. Since the pin
     * constants are globally unique for every type of easyCore, there
     * isn't any further checking (e.g. with assert or if) at runtime
     * neccessary.
     *
     * Nevertheless, for a development usage of the framework, we decided
     * to state an assert(1) here to find bugs faster.
     */
    assert(((_GLOBAL_CORE_NUMBER*MAX_GLOBAL_OFFSET) <= pin) && (pin < ((_GLOBAL_CORE_NUMBER+1)*MAX_GLOBAL_OFFSET-MAX_GLOBAL_PIN_COUNT)));

    auto it = _pinMap.find(pin);

    if (it != _pinMap.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

std::list<pin_ptr> EasyCore::getAllPins(void)
{
    std::list<pin_ptr> pinList;

    for (auto it=_pinMap.begin(); it!=_pinMap.end(); ++it) {
        pinList.push_back(it->second);
    }

    return pinList;
}

std::list<pin_ptr> EasyCore::getPinsWithType(PIN_DIRECTION_TYPE type)
{
    std::list<pin_ptr> pinList;

    for (auto it=_pinMap.begin(); it!=_pinMap.end(); ++it) {
        if (it->second->getType() == type) {
            pinList.push_back(it->second);
        }
    }

    return pinList;
}

register_ptr EasyCore::getRegister(RegisterConst reg)
{
    /*
     * If this method gets a non compatible register constant for this
     * kind of easyCore, find(1) will return std::map::end. Since the
     * register constants are globally unique for every type of easyCore,
     * there isn't any further checking (e.g. with assert or if) at
     * runtime neccessary.
     *
     * Nevertheless, for a development usage of the framework, we decided
     * to state an assert(1) here to find bugs faster.
     */
    assert(((_GLOBAL_CORE_NUMBER*MAX_GLOBAL_OFFSET+MAX_GLOBAL_PIN_COUNT) <= reg) && (reg < (_GLOBAL_CORE_NUMBER+1)*MAX_GLOBAL_OFFSET));

    auto it = _registerMap.find(reg);

    if (it != _registerMap.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

generic_hdl_map EasyCore::getGenericMap(void)
{
    return _genericMap;
}

bool EasyCore::registerCallback(interrupt_callback_function functionPointer)
{
    if (_callback == NULL) {
        _callback = functionPointer;
        return true;
    }
    else {
        return false;
    }
}

bool EasyCore::executeCallback(void)
{
    if (_callback != NULL) {
        /* Invoke the function pointer. */
        (*_callback)();
        return true;
    }
    else {
        return false;
    }
}

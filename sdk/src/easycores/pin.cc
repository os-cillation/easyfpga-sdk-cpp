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

#include "easycores/pin.h"
#include "utils/log/log.h"

#include <sstream>

Pin::Pin(std::string name, CoreIndex* core, PinConst pinNumber, PIN_DIRECTION_TYPE type) :
    _name(name),
    _coreIndex(core),
    _pinNumber(pinNumber),
    _type(type),
    _isBackwardConnected(false),
    _backwardConnection(NULL)
{
}

Pin::Pin(const Pin& pin) :
    _name(pin._name),
    _coreIndex(pin._coreIndex),
    _pinNumber(pin._pinNumber),
    _type(pin._type),
    _isBackwardConnected(pin._isBackwardConnected),
    _backwardConnection(pin._backwardConnection)
{
}

Pin::~Pin()
{
}

std::string Pin::getName(void)
{
    return _name;
}

std::string Pin::getLogName(void)
{
    std::stringstream ss;
    ss << "'" << _name << "'";
    return ss.str();
}

const CoreIndex* const Pin::getCoreIndex(void)
{
    return _coreIndex;
}

const PinConst Pin::getPinNumber(void)
{
    return _pinNumber;
}

PIN_DIRECTION_TYPE Pin::getType(void)
{
    return _type;
}

bool Pin::setType(PIN_DIRECTION_TYPE type)
{
    if ((_type == PIN_DIRECTION_TYPE::UNDEFINED) && (type != PIN_DIRECTION_TYPE::UNDEFINED)) {
        _type = type;
        return true;
    }

    return false;
}

bool Pin::hasType(PIN_DIRECTION_TYPE type)
{
    return (_type == type);
}

bool Pin::isBackwardConnected(void)
{
    return _isBackwardConnected;
}

bool Pin::connectTo(pin_ptr pin)
{
    if (pin == shared_from_this()) {
        Log().Get(WARNING) << "Connecting the pin " << this->getLogName() << " with itself does not have any effect. This instruction will be ignored.";
        return false;
    }

    if (this->isBackwardConnectedTo(pin) || pin->isBackwardConnectedTo(shared_from_this())) {
        Log().Get(WARNING) << "The two pins " << this->getLogName() << " and " << pin->getLogName() << " are already connected. This line will be skipped.";
        return false;
    }

    if (pin != nullptr) {
        if (this->_isBackwardConnected) {
            pin_ptr backwardConnection = _backwardConnection;
            while (backwardConnection->hasType(PIN_DIRECTION_TYPE::UNDEFINED) || backwardConnection->hasType(PIN_DIRECTION_TYPE::IN)) {
                if (pin->hasType(PIN_DIRECTION_TYPE::IN)) {
                    Log().Get(ERROR) << "The input pin " << pin->getLogName() << " from core " << (uint32_t)*(pin->getCoreIndex()) << " can't be driven from multiple GPIO pins or pins of type OUT. Maximum one is acceptable.";
                    exit(-1);
                }

                if (backwardConnection->isBackwardConnected()) {
                    backwardConnection = backwardConnection->getConnection();
                }
                else {
                    backwardConnection->_backwardConnection = pin;
                    backwardConnection->_isBackwardConnected = true;
                    return true;
                }
            }

            Log().Get(ERROR) << "Pin " << this->getLogName() << " is already connected to " << _backwardConnection->getLogName() << ". So we can't connect this one to pin " << pin->getLogName() << "! (We would connect 2 output pins together.)";
            exit(-1);
        }
        else {
            _backwardConnection = pin;
            _isBackwardConnected = true;
            return true;
        }
    }
    else {
        Log().Get(ERROR) << "The pin instance is invalid to which the pin " << this->getLogName() << " should be connected.";
        exit(-1);
    }

    return false;
}

pin_ptr Pin::getConnection(void)
{
    return _backwardConnection;
}

bool Pin::isBackwardConnectedTo(pin_ptr target)
{
    if (_isBackwardConnected && (target != NULL)) {
        return (*(_backwardConnection->getCoreIndex()) == *(target->getCoreIndex())) && (_backwardConnection->getPinNumber() == target->getPinNumber());
    }
    else {
        return false;
    }
}

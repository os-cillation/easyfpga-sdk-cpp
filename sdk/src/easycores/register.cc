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
#include "communication/communicator.h"
#include "easycores/register.h"
#include "easycores/easycore.h"
#include "utils/log/log.h"

#include <bitset>

Register::Register(EasyCore* core, byte address, REGISTER_ACCESS_TYPE type) :
    _core(core),
    _address(address),
    _type(type),
    _dependency(0)
{
}

Register::~Register()
{
}

bool Register::readSync(byte* target)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->readRegister(target, _core->getIndex(), _address);
}

bool Register::readMultiTimesSync(byte* target, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->readMultiRegister(target, _core->getIndex(), _address, number);
}

bool Register::readAutoAddressIncrementSync(byte* target, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->readAutoAdressIncrementRegister(target, _core->getIndex(), _address, number);
}

bool Register::writeSync(byte content)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeRegister(content, _core->getIndex(), _address);
}

bool Register::changeBitSync(uint8_t bitPosition, bool set)
{
    assert((0 <= bitPosition) && (bitPosition <= 7));

    byte buffer = (byte)0x00;

    if (this->readSync(&buffer)) {
        if (set) {
            setBit(buffer, bitPosition);
        }
        else {
            clrBit(buffer, bitPosition);
        }

        return this->writeSync(buffer);
    }

    return false;
}

bool Register::writeMultiTimesSync(byte* content, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeMultiRegister(content, _core->getIndex(), _address, number);
}

bool Register::writeAutoAddressIncrementSync(byte* content, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeAutoAdressIncrementRegister(content, _core->getIndex(), _address, number);
}

bool Register::readAsync(byte* target)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->readRegisterAsync(target, _core->getIndex(), _address, _dependency);
}

bool Register::readAsync(byte* target, callback_ptr callback)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    auto dependency = _core->getCommunicator()->readRegisterAsync(target, _core->getIndex(), _address, callback, _dependency);

    if (dependency > 0) {
        /**
         * \todo Update _dependency only if Callback is writing.
         */
        //if (Callback is writing) {
            _dependency = dependency;
        //}
        return true;
    }
    else {
        return false;
    }
}

/*
bool Register::readMultiTimesAsync(byte* target, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return false;
}

bool Register::readMultiTimesAsync(byte* target, uint8_t number, callback_ptr callback)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return false;
}

bool Register::readAutoAddressIncrementAsync(byte* target, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return false;
}

bool Register::readAutoAddressIncrementAsync(byte* target, uint8_t number, callback_ptr callback)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return false;
}
*/

bool Register::writeAsync(byte content)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeRegisterAsync(content, _core->getIndex(), _address, _dependency);
}

bool Register::writeAsync(byte content, callback_ptr callback)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    auto dependency = _core->getCommunicator()->writeRegisterAsync(content, _core->getIndex(), _address, callback, _dependency);

    if (dependency > 0) {
        /**
         * \todo Update _dependency only if Callback is writing.
         */
        //if (Callback is writing) {
            _dependency = dependency;
        //}
        return true;
    }
    else {
        return false;
    }
}

bool Register::writeMultiTimesAsync(byte* content, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeMultiRegisterAsync(content, _core->getIndex(), _address, number, _dependency);
}

/*
bool Register::writeMultiTimesAsync(byte* content, uint8_t number, callback_ptr callback)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeMultiRegisterAsync(content, _core->getIndex(), _address, number, callback, _dependency);
}
*/

bool Register::writeAutoAddressIncrementAsync(byte* content, uint8_t number)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeAutoAdressIncrementRegisterAsync(content, _core->getIndex(), _address, number, _dependency);
}

/*
bool Register::writeAutoAddressIncrementAsync(byte* content, uint8_t number, callback_ptr callback)
{
    assert(_core!=NULL);
    assert(_core->getCommunicator()!=nullptr);

    return _core->getCommunicator()->writeAutoAdressIncrementRegisterAsync(content, _core->getIndex(), _address, number, callback, callback);
}
*/

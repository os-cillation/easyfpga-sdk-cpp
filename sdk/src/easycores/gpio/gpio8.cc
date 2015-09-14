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
#include "easycores/gpio/gpio8.h"
#include "easycores/gpio/callbacks/byte_to_logic_level.h"
#include "easycores/gpio/callbacks/input_test.h"
#include "easycores/gpio/callbacks/read_modify_write.h"
#include "easycores/pin.h"
#include "easycores/register.h"
#include "utils/log/log.h"

#include <sstream>

Gpio8::Gpio8() :
    EasyCore(UNIQUE_CORE_NUMBER)
{
    _pinMap.insert(std::make_pair(PIN::GPIO0, std::make_shared<Pin>("gpio0", &_index, PIN::GPIO0, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO1, std::make_shared<Pin>("gpio1", &_index, PIN::GPIO1, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO2, std::make_shared<Pin>("gpio2", &_index, PIN::GPIO2, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO3, std::make_shared<Pin>("gpio3", &_index, PIN::GPIO3, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO4, std::make_shared<Pin>("gpio4", &_index, PIN::GPIO4, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO5, std::make_shared<Pin>("gpio5", &_index, PIN::GPIO5, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO6, std::make_shared<Pin>("gpio6", &_index, PIN::GPIO6, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::GPIO7, std::make_shared<Pin>("gpio7", &_index, PIN::GPIO7, PIN_DIRECTION_TYPE::INOUT)));

    _registerMap.insert(std::make_pair(REGISTER::IN, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER::OUT, std::make_shared<Register>(this, (byte)0x04, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER::OE, std::make_shared<Register>(this, (byte)0x08, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::INTE, std::make_shared<Register>(this, (byte)0x0C, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::PTRIG, std::make_shared<Register>(this, (byte)0x10, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::CTRL, std::make_shared<Register>(this, (byte)0x18, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::INTS, std::make_shared<Register>(this, (byte)0x1C, REGISTER_ACCESS_TYPE::READWRITE)));
}

Gpio8::~Gpio8()
{
}

std::string Gpio8::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/gpio/gpio8.vhd\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/gpio/gpio_defines.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/gpio/gpio_top.v\"" << std::endl;

    return ss.str();
}

std::string Gpio8::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "GPIO8%d : entity work.gpio8"                                                     << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      --clk_in   => gclk_s,"                                                     << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool Gpio8::isPinInput(PIN pin, bool* target)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::OE)->readSync(&buffer);
            *(target) = setBitTest(buffer, pin%MAX_GLOBAL_PIN_COUNT);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8InputTestCallback>(pin, false, target);
            return getRegister(REGISTER::OE)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::arePinsInput(bool* target)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0xFF;
            success &= getRegister(REGISTER::OE)->readSync(&buffer);
            if (buffer == (byte)0x00) {
                *(target) = true;
            }
            else {
                *(target) = false;
            }

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8InputTestCallback>(0, true, target);
            return getRegister(REGISTER::OE)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::makePinInput(PIN pin)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::OE)->readSync(&buffer);
            clrBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            success &= getRegister(REGISTER::OE)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8ModifiedWriteCallback>(this, pin, LOW);
            return getRegister(REGISTER::OE)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::makeAllPinsInput(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::OE)->writeSync((byte)0x00);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::OE)->writeAsync((byte)0x00);
    }

    return false;
}

bool Gpio8::makePinOutput(PIN pin)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::OE)->readSync(&buffer);
            setBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            success &= getRegister(REGISTER::OE)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8ModifiedWriteCallback>(this, pin, HIGH);
            return getRegister(REGISTER::OE)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::makeAllPinsOutput(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::OE)->writeSync((byte)0xFF);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::OE)->writeAsync((byte)0xFF);
    }

    return false;
}

bool Gpio8::releaseInterrupts(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::CTRL)->writeSync((byte)0x01);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::CTRL)->writeAsync((byte)0x01);
    }

    return false;
}

bool Gpio8::lockInterrupts(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::CTRL)->writeSync((byte)0x00);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::CTRL)->writeAsync((byte)0x00);
    }

    return false;
}

bool Gpio8::enableAllPinInterrupts(bool risingEdge)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = getRegister(REGISTER::INTE)->writeSync((byte)0xFF);

            if (risingEdge) {
                success &= getRegister(REGISTER::PTRIG)->writeSync((byte)0xFF);
            }
            else {
                success &= getRegister(REGISTER::PTRIG)->writeSync((byte)0x00);
            }

            return success;

        case OPERATION_MODE::ASYNC:
            success = getRegister(REGISTER::INTE)->writeAsync((byte)0xFF);

            if (risingEdge) {
                success &= getRegister(REGISTER::PTRIG)->writeAsync((byte)0xFF);
            }
            else {
                success &= getRegister(REGISTER::PTRIG)->writeAsync((byte)0x00);
            }

            return success;
    }

    return false;
}

bool Gpio8::enablePinInterrupt(PIN pin, bool risingEdge)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;
            buffer = (byte)0x00;

            /* Activates the interrupt source. */
            success &= getRegister(REGISTER::INTE)->readSync(&buffer);
            setBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            success &= getRegister(REGISTER::INTE)->writeSync(buffer);

            /* Sets the trigger edge. */
            success &= getRegister(REGISTER::PTRIG)->readSync(&buffer);
            if (risingEdge) {
                setBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            }
            else {
                clrBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            }
            success &= getRegister(REGISTER::PTRIG)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c1 = std::make_shared<Gpio8ModifiedWriteCallback>(this, pin, HIGH);
            success = getRegister(REGISTER::INTE)->readAsync(c1->getBuffer(), c1);

            callback_ptr c2 = std::make_shared<Gpio8ModifiedWriteCallback>(this, pin, risingEdge);
            success &= getRegister(REGISTER::PTRIG)->readAsync(c2->getBuffer(), c2);

            return success;
    }

    return false;
}

bool Gpio8::clearInterrupts(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::INTS)->writeSync((byte)0x00);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::INTS)->writeAsync((byte)0x00);
    }

    return false;
}

bool Gpio8::clearInterrupt(PIN pin)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;
            buffer = (byte)0x00;

            /* Deactivate the interrupt source. */
            success &= getRegister(REGISTER::INTS)->readSync(&buffer);
            clrBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            success &= getRegister(REGISTER::INTS)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8ModifiedWriteCallback>(this, pin, LOW);
            return getRegister(REGISTER::INTS)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::identifyInterruptingPins(std::list<PIN>& pins)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            buffer = (byte)0x00;
            success = getRegister(REGISTER::INTS)->readSync(&buffer);

            if (success) {
                success = false;

                for (uint8_t i=0; i<8; i++) {
                    if (setBitTest(buffer, i)) {
                        pins.push_back((PIN)(UNIQUE_CORE_NUMBER*MAX_GLOBAL_PIN_COUNT+i));
                        success = true;
                    }
                }
            }

            return success;

        /** \todo Support async mode */
        case OPERATION_MODE::ASYNC:
            Log().Get(WARNING) << "Async mode not yet supported. Aborting ...";
            return false;
    }

    return false;
}

bool Gpio8::setPin(PIN pin, LogicLevel level)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::OUT)->readSync(&buffer);

            if (level) {
                setBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            }
            else {
                clrBit(buffer, pin%MAX_GLOBAL_PIN_COUNT);
            }

            success &= getRegister(REGISTER::OUT)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8ModifiedWriteCallback>(this, pin, level);
            return getRegister(REGISTER::OUT)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::setAllPins(byte level)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::OUT)->writeSync(level);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::OUT)->writeAsync(level);
    }

    return false;
}

bool Gpio8::getPin(PIN pin, LogicLevel* level)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;
            buffer = (byte)0x00;
            success &= getRegister(REGISTER::OUT)->readSync(&buffer);

            *(level) = setBitTest(buffer, pin%MAX_GLOBAL_PIN_COUNT);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<Gpio8ByteToLogicLevelCallback>(pin, level);
            return getRegister(REGISTER::IN)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Gpio8::getAllPins(byte* level)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::OUT)->readSync(level);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::OUT)->readAsync(level);
    }

    return false;
}

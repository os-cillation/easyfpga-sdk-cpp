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
#include "easycores/pwm/pwm16.h"
#include "easycores/register.h"
#include "utils/log/log.h"

#include <sstream>

Pwm16::Pwm16() :
    EasyCore(UNIQUE_CORE_NUMBER)
{
    _pinMap.insert(std::make_pair(PIN::PWM_OUT, std::make_shared<Pin>("pwm_out", &_index, PIN::PWM_OUT, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::CLK_IN, std::make_shared<Pin>("clk_in", &_index, PIN::CLK_IN, PIN_DIRECTION_TYPE::IN)));

    _registerMap.insert(std::make_pair(REGISTER::DUTYCYCLE_LOW, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::DUTYCYCLE_HIGH, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::READWRITE)));
}

Pwm16::~Pwm16()
{
}

std::string Pwm16::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/pwm/wb_pwm16.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/pwm/pwm16.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/wishbone_slave/wbs_dual_out.vhd\"" << std::endl;

    return ss.str();
}

std::string Pwm16::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "PWM16%d : entity work.wb_pwm16"                                                  << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool Pwm16::setDutyCycle(byte lowByte, byte highByte)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    byte buf[2];
    buf[0] = lowByte;
    buf[1] = highByte;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::DUTYCYCLE_LOW)->writeAutoAddressIncrementSync(buf, 2);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::DUTYCYCLE_LOW)->writeAutoAddressIncrementAsync(buf, 2);
    }

    return false;
}

bool Pwm16::setDutyCycle(uint16_t cycle)
{
    return this->setDutyCycle((byte)(cycle & 0xFF), (byte)((cycle & 0xFF00) >> 8));
}

bool Pwm16::setDutyCycle(float percentage)
{
    return this->setDutyCycle((uint16_t)((float)(percentage / 100) * (float)UINT16_MAX));
}

bool Pwm16::getDutyCycle(byte* lowByte, byte* highByte)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success = true;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success &= getRegister(REGISTER::DUTYCYCLE_LOW)->readSync(lowByte);
            success &= getRegister(REGISTER::DUTYCYCLE_HIGH)->readSync(highByte);

            return success;

        case OPERATION_MODE::ASYNC:
            success &= getRegister(REGISTER::DUTYCYCLE_LOW)->readAsync(lowByte);
            success &= getRegister(REGISTER::DUTYCYCLE_HIGH)->readAsync(highByte);

            return success;
    }

    return false;
}

bool Pwm16::getDutyCycle(uint16_t* cycle)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    /** \todo Implement this function */
    Log().Get(WARNING) << "getDutyCycle(uint16_t*) is not yet implemented. Aborting ...";
    return false;
}

bool Pwm16::getDutyCycle(float* percentage)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    /** \todo Implement this function */
    Log().Get(WARNING) << "getDutyCycle(float*) is not yet implemented. Aborting ...";
    return false;
}

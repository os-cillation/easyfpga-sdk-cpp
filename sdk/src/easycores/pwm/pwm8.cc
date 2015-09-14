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
#include "easycores/pwm/pwm8.h"
#include "easycores/register.h"

#include <sstream>

Pwm8::Pwm8() :
    EasyCore(UNIQUE_CORE_NUMBER)
{
    _pinMap.insert(std::make_pair(PIN::PWM_OUT, std::make_shared<Pin>("pwm_out", &_index, PIN::PWM_OUT, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::CLK_IN, std::make_shared<Pin>("clk_in", &_index, PIN::CLK_IN, PIN_DIRECTION_TYPE::IN)));

    _registerMap.insert(std::make_pair(REGISTER::DUTYCYCLE, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));

    _genericMap = {
        { PIN::CLK_IN, std::list<std::string>({ std::string("USER_CLK => TRUE") }) }
    };
}

Pwm8::~Pwm8()
{
}

std::string Pwm8::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/pwm/wb_pwm8.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/pwm/pwm8.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/wishbone_slave/wbs_single_reg.vhd\"" << std::endl;

    return ss.str();
}

std::string Pwm8::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "PWM8%d : entity work.wb_pwm8"                                                    << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool Pwm8::setDutyCycle(byte cycle)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return this->getRegister(REGISTER::DUTYCYCLE)->writeSync(cycle);

        case OPERATION_MODE::ASYNC:
            return this->getRegister(REGISTER::DUTYCYCLE)->writeAsync(cycle);
    }

    return false;
}

bool Pwm8::getDutyCycle(byte* cycle)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return this->getRegister(REGISTER::DUTYCYCLE)->readSync(cycle);

        case OPERATION_MODE::ASYNC:
            return this->getRegister(REGISTER::DUTYCYCLE)->readAsync(cycle);
    }

    return false;
}

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

#include "easycores/frequencydivider/frequencydivider.h"
#include "easycores/pin.h"
#include "easycores/register.h"

#include <sstream>

FrequencyDivider::FrequencyDivider() :
    EasyCore(UNIQUE_CORE_NUMBER)
{
    _pinMap.insert(std::make_pair(PIN::OUT, std::make_shared<Pin>("clk_out", &_index, PIN::OUT, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::IN, std::make_shared<Pin>("clk_in", &_index, PIN::IN, PIN_DIRECTION_TYPE::IN)));

    _registerMap.insert(std::make_pair(REGISTER::DIV, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));

    _genericMap = {
        { PIN::IN, std::list<std::string>({ std::string("USER_CLK => TRUE") }) }
    };
}

FrequencyDivider::~FrequencyDivider()
{
}

std::string FrequencyDivider::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/wishbone_slave/wbs_single_reg.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/fdiv/wb_fdiv.vhd\"" << std::endl;

    return ss.str();
}

std::string FrequencyDivider::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "FDIV%d : entity work.wb_fdiv"                                                    << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      --clk_in   => gclk_s,"                                                     << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool FrequencyDivider::stopClock(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return this->getRegister(REGISTER::DIV)->writeSync((byte)0x00);

        case OPERATION_MODE::ASYNC:
            return this->getRegister(REGISTER::DIV)->writeAsync((byte)0x00);
    }

    return false;
}

bool FrequencyDivider::bypass(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return this->getRegister(REGISTER::DIV)->writeSync((byte)0x01);

        case OPERATION_MODE::ASYNC:
            return this->getRegister(REGISTER::DIV)->writeAsync((byte)0x01);
    }

    return false;
}

bool FrequencyDivider::setDivisor(uint8_t divisor)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return this->getRegister(REGISTER::DIV)->writeSync(divisor);

        case OPERATION_MODE::ASYNC:
            return this->getRegister(REGISTER::DIV)->writeAsync(divisor);
    }

    return false;
}

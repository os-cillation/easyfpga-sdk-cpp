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

/**
 * \brief Forward declaration of the FrequencyDividerTestFpga
 */
class FrequencyDividerTestFpga;

#include <memory>

/**
 * \brief A shared pointer of the FrequencyDividerTestFpga
 */
typedef std::shared_ptr<FrequencyDividerTestFpga> frequency_divider_test_fpga_ptr;

#include "easyfpga/easyfpga.h"
#include "easyfpga/easycores/frequencydivider/frequencydivider_ptr.h"
#include "easyfpga/easycores/pwm/pwm8_ptr.h"

/**
 * \brief Fpga description for the FrequencyDividerTest
 */
class FrequencyDividerTestFpga : public EasyFpga
{
    public:
        FrequencyDividerTestFpga();
        ~FrequencyDividerTestFpga();

        void defineStructure(void);

        pwm8_ptr getPwm1Core(void);
        pwm8_ptr getPwm2Core(void);

        frequencydivider_ptr getFreqDiv1Core(void);
        frequencydivider_ptr getFreqDiv2Core(void);
        frequencydivider_ptr getFreqDiv3Core(void);
        frequencydivider_ptr getFreqDiv4Core(void);

    private:
        pwm8_ptr _pwm8_1;
        pwm8_ptr _pwm8_2;
        frequencydivider_ptr _f1;
        frequencydivider_ptr _f2;
        frequencydivider_ptr _f3;
        frequencydivider_ptr _f4;
};

EASY_FPGA_DESCRIPTION_CLASS(FrequencyDividerTestFpga)

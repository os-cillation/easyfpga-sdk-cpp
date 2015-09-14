/*
 * This file is part of the easyFPGA project.
 *
 * (C) Copyright 2013-2015 os-cillation GmbH
 * Author: Johannes Hein <support@os-cillation.de>
 *
 * easyFPGA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * easyFPGA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with easyFPGA. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SDK_EASYCORES_CAN_UTILS_CANFRAME_STANDARD_H_
#define SDK_EASYCORES_CAN_UTILS_CANFRAME_STANDARD_H_

#include "easycores/can/utils/canframe.h"
#include "easycores/can/utils/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief A can message.
 */
class CanFrameStandard : public CanFrame
{
    public:
        CanFrameStandard(uint32_t identifier);
        CanFrameStandard(uint32_t identifier, byte* dataToCopy, uint8_t length);
        ~CanFrameStandard();

        void getDescriptor(byte* descriptorTarget);

        uint8_t getDescriptorLength(void);
};

#endif  // SDK_EASYCORES_CAN_UTILS_CANFRAME_STANDARD_H_

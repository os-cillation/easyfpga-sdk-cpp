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

#ifndef SDK_EASYCORES_CAN_UTILS_CANFRAME_H_
#define SDK_EASYCORES_CAN_UTILS_CANFRAME_H_

#include "easycores/can/utils/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief A can message.
 */
class CanFrame
{
    public:
        /**
         * \brief Constructs a remote transfer request
         */
        CanFrame(uint32_t identifier, CAN_MESSAGE_FORMAT_TYPE format);

        /**
         * \brief Constructs a data transfer request
         */
        CanFrame(uint32_t identifier, byte* dataToCopy, uint8_t dataLength, CAN_MESSAGE_FORMAT_TYPE format);
        ~CanFrame();

        /**
         * \brief Returns the frame identifier.
         */
        uint32_t getIdentifier(void);

        /**
         * \brief Returns the frame message type.
         */
        CAN_MESSAGE_FORMAT_TYPE getMessageType(void);

        /**
         * \brief Returns the request type.
         */
        bool isRemoteTransferRequest(void);

        /**
         * \brief Returns the a frame information field. (Only neccessary
         * for the extended usage mode of the can core.)
         */
        void getFrameInfomationField(byte* target);

        /**
         * \brief Returns the frame's descriptor.
         */
        virtual void getDescriptor(byte* descriptorTarget) = 0;

        /**
         * \brief Returns the frame's descriptor length.
         */
        virtual uint8_t getDescriptorLength(void) = 0;

        /**
         * \brief Returns the frame's data.
         */
        void getData(byte* dataArrayTarget);

        /**
         * \brief Returns the frame's data length.
         */
        uint8_t getDataLength(void);

    protected:
        uint32_t _identifier;

        CAN_MESSAGE_FORMAT_TYPE _frameFormatType;

        bool _isRemoteTransfer;

        byte _frameInformation;

        byte* _descriptor;

        byte* _data;

        uint8_t _dataLength;
};

#endif  // SDK_EASYCORES_CAN_UTILS_CANFRAME_H_

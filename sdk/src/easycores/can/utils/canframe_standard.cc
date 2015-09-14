#include "easycores/can/utils/canframe_standard.h"

#include <cstdlib> /* NULL */
#include <cstring> /* memcpy(3) */

CanFrameStandard::CanFrameStandard(uint32_t identifier) :
    CanFrame(identifier, CAN_MESSAGE_FORMAT_TYPE::STANDARD_FRAME)
{
    /* BUILD THE FRAME INFORMATION FIELD */
    _frameInformation = _dataLength & 0x0F;
    clrBit(&_frameInformation, 7);
    setBit(&_frameInformation, 6);

    /* BUILD THE FRAME DESCRIPTOR */
    _descriptor = new byte[2];

    _descriptor[0] = (identifier & 0x000007F8) >> 3;  // _descriptor[0] contains as MSB ID28 and as LSB ID21
    _descriptor[1] = (identifier & 0x00000007) << 5;  // _descriptor[1] contains ID20 + ID19 + ID18 + RTR bit + 4 data length bits
    setBit(&_descriptor[1], 4);
    _descriptor[1] |= _dataLength & 0x0F;
}

CanFrameStandard::CanFrameStandard(uint32_t identifier, byte* dataToCopy, uint8_t length) :
    CanFrame(identifier, CAN_MESSAGE_FORMAT_TYPE::STANDARD_FRAME)
{
    /* BUILD THE FRAME INFORMATION FIELD */
    _frameInformation = _dataLength & 0x0F;
    clrBit(&_frameInformation, 7);
    clrBit(&_frameInformation, 6);

    /* BUILD THE FRAME DESCRIPTOR */
    _descriptor = new byte[2];

    _descriptor[0] = (identifier & 0x000007F8) >> 3;  // _descriptor[0] contains as MSB ID28 and as LSB ID21
    _descriptor[1] = (identifier & 0x00000007) << 5;  // _descriptor[1] contains ID20 + ID19 + ID18 + RTR bit + 4 data length bits
    clrBit(&_descriptor[1], 4);
    _descriptor[1] |= _dataLength & 0x0F;
}

CanFrameStandard::~CanFrameStandard()
{
    delete[] _descriptor;
}

void CanFrameStandard::getDescriptor(byte* descriptorTarget)
{
    memcpy(descriptorTarget, _descriptor, 2);
}

uint8_t CanFrameStandard::getDescriptorLength(void)
{
    return 2;
}

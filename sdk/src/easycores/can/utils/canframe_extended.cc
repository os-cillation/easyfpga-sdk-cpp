#include "easycores/can/utils/canframe_extended.h"

#include <cstdlib> /* NULL */
#include <cstring> /* memcpy(3) */

CanFrameExtended::CanFrameExtended(uint32_t identifier) :
    CanFrame(identifier, CAN_MESSAGE_FORMAT_TYPE::EXTENDED_FRAME)
{
    /* BUILD THE FRAME INFORMATION FIELD */
    _frameInformation = _dataLength & 0x0F;
    setBit(&_frameInformation, 7);
    setBit(&_frameInformation, 6);

    /* BUILD THE FRAME DESCRIPTOR */
    _descriptor = new byte[4];

    _descriptor[0] = (identifier & 0x1FE00000) >> 21; // _descriptor[0] contains as MSB ID28 and as LSB ID21
    _descriptor[1] = (identifier & 0x001FE000) >> 13; // _descriptor[1] contains ID20 .. ID13
    _descriptor[2] = (identifier & 0x00001FE0) >> 5;  // _descriptor[2] contains ID12 .. ID5
    _descriptor[3] = (identifier & 0x0000001F) << 3;  // _descriptor[3] contains ID4  .. ID0 and 3x don't care in the end as LSBs
    // for compatibility reasons we should set the don't care bits according to the following scheme
    setBit(&_descriptor[3], 2);
    clrBit(&_descriptor[3], 1);
    clrBit(&_descriptor[3], 0);
}

CanFrameExtended::CanFrameExtended(uint32_t identifier, byte* dataToCopy, uint8_t length) :
    CanFrame(identifier, CAN_MESSAGE_FORMAT_TYPE::EXTENDED_FRAME)
{
    /* BUILD THE FRAME INFORMATION FIELD */
    _frameInformation = _dataLength & 0x0F;
    setBit(&_frameInformation, 7);
    clrBit(&_frameInformation, 6);

    /* BUILD THE FRAME DESCRIPTOR */
    _descriptor = new byte[4];

    _descriptor[0] = (identifier & 0x1FE00000) >> 21; // _descriptor[0] contains as MSB ID28 and as LSB ID21
    _descriptor[1] = (identifier & 0x001FE000) >> 13; // _descriptor[1] contains ID20 .. ID13
    _descriptor[2] = (identifier & 0x00001FE0) >> 5;  // _descriptor[2] contains ID12 .. ID5
    _descriptor[3] = (identifier & 0x0000001F) << 3;  // _descriptor[3] contains ID4  .. ID0 and 3x don't care in the end as LSBs
    // for compatibility reasons we should set the don't care bits according to the following scheme
    clrBit(&_descriptor[3], 2);
    clrBit(&_descriptor[3], 1);
    clrBit(&_descriptor[3], 0);
}

CanFrameExtended::~CanFrameExtended()
{
    delete[] _descriptor;
}

void CanFrameExtended::getDescriptor(byte* descriptorTarget)
{
    memcpy(descriptorTarget, _descriptor, 4);
}

uint8_t CanFrameExtended::getDescriptorLength(void)
{
    return 4;
}

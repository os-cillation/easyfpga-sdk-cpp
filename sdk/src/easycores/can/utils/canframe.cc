#include "easycores/can/utils/canframe.h"

#include <cstdlib> /* NULL */
#include <cstring> /* memcpy(3) */

CanFrame::CanFrame(uint32_t identifier, CAN_MESSAGE_FORMAT_TYPE format) :
    _identifier(identifier),
    _frameFormatType(format),
    _isRemoteTransfer(true),
    _data(NULL),
    _dataLength(0)
{
    /* MESSAGE WITHOUT DATA -> NO COPY */
}

CanFrame::CanFrame(uint32_t identifier, byte* dataToCopy, uint8_t length, CAN_MESSAGE_FORMAT_TYPE format) :
    _identifier(identifier),
    _frameFormatType(format),
    _isRemoteTransfer(false),
    _data(NULL),
    _dataLength(length)
{
    /* MESSAGE WITH DATA -> COPY */
    _data = new byte[8];
    memcpy(_data, dataToCopy, _dataLength);
}

CanFrame::~CanFrame()
{
    delete[] _data;
}

uint32_t CanFrame::getIdentifier(void)
{
    return _identifier;
}

CAN_MESSAGE_FORMAT_TYPE CanFrame::getMessageType(void)
{
    return _frameFormatType;
}

bool CanFrame::isRemoteTransferRequest(void)
{
    return _isRemoteTransfer;
}

void CanFrame::getFrameInfomationField(byte* target)
{
    *target = _frameInformation;
}

void CanFrame::getData(byte* dataArrayTarget)
{
    memcpy(dataArrayTarget, _data, _dataLength);
}

uint8_t CanFrame::getDataLength(void)
{
    return _dataLength;
}

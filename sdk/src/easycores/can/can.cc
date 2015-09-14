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
#include "easycores/can/can.h"
#include "easycores/can/utils/canframe.h"
#include "easycores/can/utils/canframe_extended.h"
#include "easycores/can/utils/canframe_standard.h"
#include "easycores/pin.h"
#include "easycores/register.h"
#include "utils/log/log.h"

#include <sstream>
#include <cstring>

Can::Can() :
    EasyCore(UNIQUE_CORE_NUMBER),
    _mode(Can::USAGE_MODE::UNDEFINED)
{
    _pinMap.insert(std::make_pair(PIN::RX, std::make_shared<Pin>("can_rx_in", &_index, PIN::RX, PIN_DIRECTION_TYPE::IN)));
    _pinMap.insert(std::make_pair(PIN::TX, std::make_shared<Pin>("can_tx_out", &_index, PIN::TX, PIN_DIRECTION_TYPE::OUT)));

    /*
     * 32 Registers for the basic mode
     */
    // CONTROL SEGMENT
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_CONTROL, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_COMMAND, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_STATUS, std::make_shared<Register>(this, (byte)0x02, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_INTERRUPT, std::make_shared<Register>(this, (byte)0x03, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_ACCEPTANCE_CODE, std::make_shared<Register>(this, (byte)0x04, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in reset mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_ACCEPTANCE_MASK, std::make_shared<Register>(this, (byte)0x05, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in reset mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_BUS_TIMING_0, std::make_shared<Register>(this, (byte)0x06, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in reset mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_BUS_TIMING_1, std::make_shared<Register>(this, (byte)0x07, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in reset mode only
    // address 0x08 not used here (output control register) // accessible in reset mode only
    // address 0x09 not used here (test register)
    // address 0x1E not used here (a read access results always in 0xFF, write operations have undefined behaviour)
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_CLOCK_DIVIDER, std::make_shared<Register>(this, (byte)0x1F, REGISTER_ACCESS_TYPE::READWRITE))); // bits 7 and 6 only writeable in reset mode

    // TRANSMIT BUFFER SEGMENT
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_IDENTIFIER_1, std::make_shared<Register>(this, (byte)0x0A, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_IDENTIFIER_2, std::make_shared<Register>(this, (byte)0x0B, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_1, std::make_shared<Register>(this, (byte)0x0C, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_2, std::make_shared<Register>(this, (byte)0x0D, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_3, std::make_shared<Register>(this, (byte)0x0E, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_4, std::make_shared<Register>(this, (byte)0x0F, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_5, std::make_shared<Register>(this, (byte)0x10, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_6, std::make_shared<Register>(this, (byte)0x11, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_7, std::make_shared<Register>(this, (byte)0x12, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_TX_DATA_8, std::make_shared<Register>(this, (byte)0x13, REGISTER_ACCESS_TYPE::READWRITE))); // accessible in operation mode only

    // RECEIVE BUFFER SEGMENT
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_IDENTIFIER_1, std::make_shared<Register>(this, (byte)0x14, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_IDENTIFIER_2, std::make_shared<Register>(this, (byte)0x15, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_1, std::make_shared<Register>(this, (byte)0x16, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_2, std::make_shared<Register>(this, (byte)0x17, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_3, std::make_shared<Register>(this, (byte)0x18, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_4, std::make_shared<Register>(this, (byte)0x19, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_5, std::make_shared<Register>(this, (byte)0x1A, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_6, std::make_shared<Register>(this, (byte)0x1B, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_7, std::make_shared<Register>(this, (byte)0x1C, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_BASIC_MODE::BM_RX_DATA_8, std::make_shared<Register>(this, (byte)0x1D, REGISTER_ACCESS_TYPE::READWRITE)));



    /*
     * 32 egisters for the extendend mode
     */
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_MODE, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_COMMAND, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_STATUS, std::make_shared<Register>(this, (byte)0x02, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_INTERRUPT, std::make_shared<Register>(this, (byte)0x03, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_INTERRUPT_ENABLE, std::make_shared<Register>(this, (byte)0x04, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_BUS_TIMING_0, std::make_shared<Register>(this, (byte)0x06, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_BUS_TIMING_1, std::make_shared<Register>(this, (byte)0x07, REGISTER_ACCESS_TYPE::WRITEONLY)));

    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_ARBITRATION_LOST_CAPTURE, std::make_shared<Register>(this, (byte)0x0B, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_ERROR_CODE_CAPTURE, std::make_shared<Register>(this, (byte)0x0C, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_ERROR_WARNING_LIMIT, std::make_shared<Register>(this, (byte)0x0D, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_RX_ERROR_COUNTER, std::make_shared<Register>(this, (byte)0x0E, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_TX_ERROR_COUNTER, std::make_shared<Register>(this, (byte)0x0F, REGISTER_ACCESS_TYPE::WRITEONLY)));

    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION, std::make_shared<Register>(this, (byte)0x10, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_IDENTIFIER_1, std::make_shared<Register>(this, (byte)0x11, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_IDENTIFIER_2, std::make_shared<Register>(this, (byte)0x12, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_1, std::make_shared<Register>(this, (byte)0x13, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_2, std::make_shared<Register>(this, (byte)0x14, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_3, std::make_shared<Register>(this, (byte)0x15, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_4, std::make_shared<Register>(this, (byte)0x16, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_5, std::make_shared<Register>(this, (byte)0x17, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_6, std::make_shared<Register>(this, (byte)0x18, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_7, std::make_shared<Register>(this, (byte)0x19, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_8, std::make_shared<Register>(this, (byte)0x1A, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_9, std::make_shared<Register>(this, (byte)0x1B, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_DATA_10, std::make_shared<Register>(this, (byte)0x1C, REGISTER_ACCESS_TYPE::READWRITE)));

    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_RX_MESSAGE_COUNTER, std::make_shared<Register>(this, (byte)0x1D, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_RX_BUFFER_START_ADDRESS, std::make_shared<Register>(this, (byte)0x1E, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER_EXTENDEND_MODE::EM_CLOCK_DIVIDER, std::make_shared<Register>(this, (byte)0x1F, REGISTER_ACCESS_TYPE::WRITEONLY)));
}

Can::~Can()
{
}

std::string Can::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/can_wrapper/can.vhd\"" << std::endl;

    ss << "verilog work \"%dir/easy_cores/can/can_acf.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_bsp.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_btl.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_crc.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_defines.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_fifo.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_ibo.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_register.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_register_asyn.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_register_asyn_syn.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_registers.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_register_syn.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/can/can_top.v\"" << std::endl;

    return ss.str();
}

std::string Can::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "CAN%d : entity work.can"                                                         << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool Can::init(BITRATE bitrate, USAGE_MODE mode)
{
    bool success = true;

    uint8_t prescaler;

    /* PARAMETER CHECK */
    switch (bitrate) {
        case BITRATE::BITRATE_125K:
            prescaler = 32;
            break;

        case BITRATE::BITRATE_250K:
            prescaler = 16;
            break;

        case BITRATE::BITRATE_1M:
            prescaler = 4;
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The birate parameter is not valid.";
            return false;
    }

    _mode = mode;

    success &= this->enterResetMode();

    switch (mode) {
        case USAGE_MODE::BASIC_MODE:
            /* enter the specified CAN mode */
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_CLOCK_DIVIDER)->changeBitSync(7, false);

            /* set a possible default timing */
            success &= this->setBusTiming(prescaler, 2, false, 5, 2);
            break;

        case USAGE_MODE::EXTENDEND_MODE:
            /* enter the specified CAN mode */
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_CLOCK_DIVIDER)->changeBitSync(7, false);

            /* set a possible default timing */
            success &= this->setBusTiming(prescaler, 2, false, 5, 2);

            /* set single-filter frame filtering */
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_MODE)->changeBitSync(3, true);
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The usage mode parameter is not valid.";
            return false;
    }

    /* enable core */
    success &= this->enterOperationMode();

    /* Set internal mode to UNDEFINED if initialization was not successful. */
    if (!success) {
        _mode = Can::USAGE_MODE::UNDEFINED;
        Log().Get(WARNING) << "Init method failed! The usage mode will not set...";
    }

    return success;
}

Can::USAGE_MODE Can::getUsageMode(void)
{
    return _mode;
}

bool Can::enableInterrupts(void)
{
    bool success;

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            success = true;
            success &= this->enableInterrupt(INTERRUPT::RECEIVE);
            success &= this->enableInterrupt(INTERRUPT::TRANSMIT);
            success &= this->enableInterrupt(INTERRUPT::ERROR);
            success &= this->enableInterrupt(INTERRUPT::DATA_OVERRUN);
            success &= this->enableInterrupt(INTERRUPT::WAKE_UP);
            return success;

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_INTERRUPT_ENABLE)->writeSync((byte)0xFF);

        default:
            Log().Get(WARNING) << "You have to init this core first before you can enable the interrupts!";
            return false;
    }
}

bool Can::enableInterrupt(INTERRUPT interrupt)
{
    uint8_t bitPosition;

    switch (interrupt) {
        case INTERRUPT::BUS_ERROR:
            bitPosition = 7;
            break;

        case INTERRUPT::ARBITRATION_LOST:
            bitPosition = 6;
            break;

        case INTERRUPT::ERROR_PASSIVE:
            bitPosition = 5;
            break;

        case INTERRUPT::WAKE_UP:
            bitPosition = 4;
            break;

        case INTERRUPT::DATA_OVERRUN:
            bitPosition = 3;
            break;

        case INTERRUPT::ERROR:
            bitPosition = 2;
            break;

        case INTERRUPT::TRANSMIT:
            bitPosition = 1;
            break;

        case INTERRUPT::RECEIVE:
            bitPosition = 0;
            break;

        default:
            Log().Get(WARNING) << "Unknown CAN interrupt constant!";
            return false;
    }

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            if (4 < bitPosition) {
                Log().Get(WARNING) << "This interrupt can be only used in the extendend mode! No interrupt will be enabled.";
                return false;
            }
            else {
                bitPosition++; /* Because digit 0 represents the mode setting bit. */
                return this->getRegister(REGISTER_BASIC_MODE::BM_CONTROL)->changeBitSync(bitPosition, true);
            }

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_INTERRUPT_ENABLE)->changeBitSync(bitPosition, true);

        default:
            Log().Get(WARNING) << "You have to init this core first before you can enable the interrupts!";
            return false;
    }
}

bool Can::disableInterrupts(void)
{
    bool success;

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            success = true;
            success &= this->disableInterrupt(INTERRUPT::RECEIVE);
            success &= this->disableInterrupt(INTERRUPT::TRANSMIT);
            success &= this->disableInterrupt(INTERRUPT::ERROR);
            success &= this->disableInterrupt(INTERRUPT::DATA_OVERRUN);
            success &= this->disableInterrupt(INTERRUPT::WAKE_UP);
            return success;

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_INTERRUPT_ENABLE)->writeSync((byte)0x00);

        default:
            Log().Get(WARNING) << "You have to init this core first before you can enable the interrupts!";
            return false;
    }
}

bool Can::disableInterrupt(INTERRUPT interrupt)
{
    uint8_t bitPosition;

    switch (interrupt) {
        case INTERRUPT::BUS_ERROR:
            bitPosition = 7;
            break;

        case INTERRUPT::ARBITRATION_LOST:
            bitPosition = 6;
            break;

        case INTERRUPT::ERROR_PASSIVE:
            bitPosition = 5;
            break;

        case INTERRUPT::WAKE_UP:
            bitPosition = 4;
            break;

        case INTERRUPT::DATA_OVERRUN:
            bitPosition = 3;
            break;

        case INTERRUPT::ERROR:
            bitPosition = 2;
            break;

        case INTERRUPT::TRANSMIT:
            bitPosition = 1;
            break;

        case INTERRUPT::RECEIVE:
            bitPosition = 0;
            break;

        default:
            Log().Get(WARNING) << "Unknown CAN interrupt constant!";
            return false;
    }

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            if (4 < bitPosition) {
                Log().Get(WARNING) << "This interrupt can be only used in the extendend mode! No interrupt will be disabled.";
                return false;
            }
            else {
                bitPosition++; /* Because digit 0 represents the mode setting bit. */
                return this->getRegister(REGISTER_BASIC_MODE::BM_CONTROL)->changeBitSync(bitPosition, false);
            }

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_INTERRUPT_ENABLE)->changeBitSync(bitPosition, false);

        default:
            Log().Get(WARNING) << "You have to init this core first before you can enable the interrupts!";
            return false;
    }
}

bool Can::identifyInterrupt(INTERRUPT* target)
{
    bool success;
    byte buffer = (byte)0x00;

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            success = this->getRegister(REGISTER_BASIC_MODE::BM_INTERRUPT)->readSync(&buffer);
            break;

        case USAGE_MODE::EXTENDEND_MODE:
            success = this->getRegister(REGISTER_EXTENDEND_MODE::EM_INTERRUPT)->readSync(&buffer);
            break;

        default:
            Log().Get(WARNING) << "You have to call init() first before you can try to identify any interrupt!";
            return false;
    }

    if (success) {
        if (setBitTest(buffer, 0)) {
            *(target) = Can::INTERRUPT::RECEIVE;
        }
        else if (setBitTest(buffer, 1)) {
            *(target) = Can::INTERRUPT::TRANSMIT;
        }
        else if (setBitTest(buffer, 2)) {
            *(target) = Can::INTERRUPT::ERROR;
        }
        else if (setBitTest(buffer, 3)) {
            *(target) = Can::INTERRUPT::DATA_OVERRUN;
        }
        else if (setBitTest(buffer, 4)) {
            *(target) = Can::INTERRUPT::WAKE_UP;
        }
        else if (setBitTest(buffer, 5)) {
            *(target) = Can::INTERRUPT::ERROR_PASSIVE;
        }
        else if (setBitTest(buffer, 6)) {
            *(target) = Can::INTERRUPT::ARBITRATION_LOST;
        }
        else if (setBitTest(buffer, 7)) {
            *(target) = Can::INTERRUPT::BUS_ERROR;
        }
        else {
            Log().Get(WARNING) << "No interrupt could be identified!";
            return false;
        }

        return true;
    }

    return false;
}

bool Can::transmit(canframe_ptr frame)
{
    assert(frame->getDataLength() <= 8);

    bool success = true;

    uint8_t descriptorLength = frame->getDescriptorLength();
    //uint8_t dataLength = frame->getDataLength();

    byte data[13];
    for (uint8_t i=0; i<13; i++) {
        data[i] = (byte)0x00;
    }

    byte status = (byte)0x00;

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            assert(frame->getMessageType() != CAN_MESSAGE_FORMAT_TYPE::EXTENDED_FRAME);

            /* wait as long as the transmit buffer locked */
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_STATUS)->readSync(&status);
            while (success && !setBitTest(status, 2)) {
                success &= this->getRegister(REGISTER_BASIC_MODE::BM_STATUS)->readSync(&status);
                Log().Get(INFO) << "Transmit buffer : " << std::boolalpha << setBitTest(status, 2);
                usleep(10);
            }

            frame->getDescriptor(data);
            frame->getData(data+descriptorLength);

            //success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_IDENTIFIER_1)->writeAutoAddressIncrementSync(data, descriptorLength+dataLength);

            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_IDENTIFIER_1)->writeSync(data[0]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_IDENTIFIER_2)->writeSync(data[1]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_1)->writeSync(data[2]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_2)->writeSync(data[3]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_3)->writeSync(data[4]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_4)->writeSync(data[5]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_5)->writeSync(data[6]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_6)->writeSync(data[7]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_7)->writeSync(data[8]);
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_TX_DATA_8)->writeSync(data[9]);

            // request transmission
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_COMMAND)->writeSync((byte)0x01);
            break;

        case USAGE_MODE::EXTENDEND_MODE:
            /* wait as long as the transmit buffer locked */
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_STATUS)->readSync(&status);
            while (success && !setBitTest(status, 2)) {
                success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_STATUS)->readSync(&status);
                Log().Get(INFO) << "Transmit buffer : " << std::boolalpha << setBitTest(status, 2);
                usleep(10);
            }

            frame->getFrameInfomationField(data);
            frame->getDescriptor(data+1);
            frame->getData(data+1+descriptorLength);

            //success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION)->writeAutoAddressIncrementSync(data, descriptorLength+dataLength+1);

            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION)->writeSync(data[0]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+1)->writeSync(data[1]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+2)->writeSync(data[2]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+3)->writeSync(data[3]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+4)->writeSync(data[4]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+5)->writeSync(data[5]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+6)->writeSync(data[6]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+7)->writeSync(data[7]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+8)->writeSync(data[8]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+9)->writeSync(data[9]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+10)->writeSync(data[10]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+11)->writeSync(data[11]);
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION+12)->writeSync(data[12]);


            // request transmission
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_COMMAND)->writeSync((byte)0x01);
            break;

        default:
            return false;
    }


    return success;
}

bool Can::getReceivedFrame(canframe_ptr& frame)
{
    bool success = true;

    byte data[15];
    for (uint8_t i=0; i<15; i++) {
        data[i] = (byte)0x00;
    }

    byte status = 0x00;

    byte frameInformation;
    byte identifier1;
    byte identifier2;
    byte identifier3;
    byte identifier4;
    uint32_t identifier;
    uint8_t dataLength;

    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_STATUS)->readSync(&status);

            if (!setBitTest(status, 0)) {
                frame = nullptr;
                return false;
            }

            success &= this->getRegister(REGISTER_BASIC_MODE::BM_RX_IDENTIFIER_1)->readAutoAddressIncrementSync(data, 10);

            identifier1 = data[0];
            identifier2 = data[1];
            identifier = (identifier1 << 3) | ((identifier2 & 0xE0) >> 5);

            /* determine if RTR or data transmission */
            if (setBitTest(identifier2, 4)) {
                frame = std::make_shared<CanFrameStandard>(identifier);
            }
            else {
                /* determine data length */
                dataLength = identifier2 & 0x0F;
                assert(dataLength <= 8);

                frame = std::make_shared<CanFrameStandard>(identifier, data+3, dataLength);
            }

            // release receive buffer
            success &= this->getRegister(REGISTER_BASIC_MODE::BM_COMMAND)->writeSync((byte)0x04);
            break;

        case USAGE_MODE::EXTENDEND_MODE:
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_STATUS)->readSync(&status);

            if (!setBitTest(status, 0)) {
                frame = nullptr;
                return false;
            }

            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION)->readAutoAddressIncrementSync(data, 14);

            frameInformation = data[0];
            dataLength = frameInformation & 0x0F;
            assert(dataLength <= 8);

            /* Test if an standard or extended frame is avialable */
            if (!setBitTest(frameInformation, 7)) {
                identifier1 = data[1];
                identifier2 = data[2];

                identifier = (identifier1 << 3) | ((identifier2 & 0xE0) >> 5);

                /* determine if RTR or data transmission */
                if (setBitTest(identifier2, 4)) {
                    frame = std::make_shared<CanFrameExtended>(identifier);
                }
                else {
                    frame = std::make_shared<CanFrameExtended>(identifier, data+3, dataLength);
                }
            }
            else {
                identifier1 = data[1];
                identifier2 = data[2];
                identifier3 = data[3];
                identifier4 = data[4];

                identifier = (identifier1 << 21) | (identifier2 << 13) |
                             (identifier3 << 5)  | ((identifier4 & 0xF8) >> 3);

                /* determine if RTR or data transmission */
                if (setBitTest(frameInformation, 6)) {
                    frame = std::make_shared<CanFrameExtended>(identifier);
                }
                /* if data transmission */
                else {
                    frame = std::make_shared<CanFrameExtended>(identifier, data+6, dataLength);
                }
            }

            // release receive buffer
            success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_COMMAND)->writeSync((byte)0x04);
            break;

        default:
            Log().Get(WARNING) << "You have to call init() first before you can try to receive any can message!";
            return false;
    }

    return success;
}

bool Can::goToSleep(void)
{
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            return this->getRegister(REGISTER_BASIC_MODE::BM_COMMAND)->writeSync((byte)0x10);

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_COMMAND)->changeBitSync(4, true);

        default:
            return false;
    }
}

bool Can::wakeUp(void)
{
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            return this->getRegister(REGISTER_BASIC_MODE::BM_COMMAND)->writeSync((byte)0x10);

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_COMMAND)->changeBitSync(4, false);

        default:
            return false;
    }
}

bool Can::setAcceptanceCode(uint32_t code)
{
    /* PARAMETER CHECK */
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            if (code > (uint32_t)0xFF) {
                Log().Get(WARNING) << "Illegal acceptance code used: " << code << " (In basic mode are only values between 0 and 255 possible.)";
                return false;
            }
            break;

        case USAGE_MODE::EXTENDEND_MODE:
            if (code > (uint32_t)0x1FFFFFFF) {
                Log().Get(WARNING) << "Illegal acceptance code used: " << code << " (In extendend mode are only values between 0 and 536870911 possible.)";
                return false;
            }
            break;

        default:
            Log().Get(WARNING) << "You have to call init() first before you set an acceptance mask!";
            return false;
    }

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success = true;

    success &= this->enterResetMode();

    if (_mode == USAGE_MODE::BASIC_MODE) {
        success &= this->getRegister(REGISTER_BASIC_MODE::BM_ACCEPTANCE_CODE)->writeSync((byte)(code & 0xFF));
    }
    else {
        byte byteArray[4];
        byteArray[0] = (code & 0x1FE00000) >> 21;  /* ACR0 */
        byteArray[1] = (code & 0x001FE000) >> 13;  /* ACR1 */
        byteArray[2] = (code & 0x00001FE0) >> 5;   /* ACR2 */
        byteArray[3] = (code & 0x0000001F) << 3;   /* ACR3 */

        success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_FRAME_INFORMATION)->writeAutoAddressIncrementSync(byteArray, 4);
    }

    success &= this->enterOperationMode();

    return success;
}

bool Can::setAcceptanceMask(uint32_t mask)
{
    /* PARAMETER CHECK */
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            if (mask > (uint32_t)0xFF) {
                Log().Get(WARNING) << "Illegal acceptance code used: " << mask << " (In basic mode are only values between 0 and 255 possible.)";
                return false;
            }
            break;

        case USAGE_MODE::EXTENDEND_MODE:
            if (mask > (uint32_t)0x1FFFFFFF) {
                Log().Get(WARNING) << "Illegal acceptance code used: " << mask << " (In extendend mode are only values between 0 and 536870911 possible.)";
                return false;
            }
            break;

        default:
            Log().Get(WARNING) << "You have to call init() first before you set an acceptance mask!";
            return false;
    }

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success = true;

    success &= this->enterResetMode();

    if (_mode == USAGE_MODE::BASIC_MODE) {
        success &= this->getRegister(REGISTER_BASIC_MODE::BM_ACCEPTANCE_MASK)->writeSync((byte)(mask & 0xFF));
    }
    else {
        byte byteArray[4];
        byteArray[0] = (mask & 0x1FE00000) >> 21;         /* AMR0 */
        byteArray[1] = (mask & 0x001FE000) >> 13;         /* AMR1 */
        byteArray[2] = (mask & 0x00001FE0) >> 5;          /* AMR2 */
        byteArray[3] = ((mask & 0x0000001F) << 3) | 0x7;  /* AMR3 */

        success &= this->getRegister(REGISTER_EXTENDEND_MODE::EM_DATA_2)->writeAutoAddressIncrementSync(byteArray, 4);
    }

    success &= this->enterOperationMode();

    return success;
}

bool Can::logStatus(void)
{
    /* read status register */
    byte statusRegister = (byte)0x00;
    if (!this->getRegister(REGISTER_BASIC_MODE::BM_STATUS)->readSync(&statusRegister)) {
        return false;
    }

    /* extract and log bits */
    Log().Get(INFO) << "CAN CORE #" << (uint32_t)this->getIndex() << " STATUS";
    Log().Get(INFO) << "Bus : " << std::boolalpha << setBitTest(statusRegister, 7);
    Log().Get(INFO) << "Error : " << std::boolalpha << setBitTest(statusRegister, 6);
    Log().Get(INFO) << "Transmit : " << std::boolalpha << setBitTest(statusRegister, 5);
    Log().Get(INFO) << "Receive : " << std::boolalpha << setBitTest(statusRegister, 4);
    Log().Get(INFO) << "Transmission complete : " << std::boolalpha << setBitTest(statusRegister, 3);
    Log().Get(INFO) << "Transmit buffer : " << std::boolalpha << setBitTest(statusRegister, 2);
    Log().Get(INFO) << "Data overrun : " << std::boolalpha << setBitTest(statusRegister, 1);
    Log().Get(INFO) << "Receive buffer : " << std::boolalpha << setBitTest(statusRegister, 0);

    return true;
}

bool Can::enterResetMode(void)
{
    /* change reset request bit depending on mode*/
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            return this->getRegister(REGISTER_BASIC_MODE::BM_CONTROL)->changeBitSync(0, true);

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_MODE)->changeBitSync(0, true);

        default:
            Log().Get(LogLevel::ERROR) << "Internal error: Please call init() first!";
            return false;
    }
}

bool Can::enterOperationMode(void)
{
    /* change reset request bit depending on mode*/
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            return this->getRegister(REGISTER_BASIC_MODE::BM_CONTROL)->changeBitSync(0, false);

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_MODE)->changeBitSync(0, false);

        default:
            Log().Get(LogLevel::ERROR) << "Internal error: Please call init() first!";
            return false;
    }
}

bool Can::setBusTiming(
    uint8_t prescaler,
    uint8_t syncronizationJumpWidth,
    bool sampling,
    uint8_t timeSegment1,
    uint8_t timeSegment2
)
{
    if ((syncronizationJumpWidth < 0) || (syncronizationJumpWidth > 3)) {
        Log().Get(WARNING) << "Init method fails. Invalid synchronization jump width!";
        return false;
    }

    if ((timeSegment1 < 0) || (timeSegment1 > 15)) {
        Log().Get(WARNING) << "Init method fails. Invalid time segment 1!";
        return false;
    }

    if ((timeSegment2 < 0) || (timeSegment2 > 7)) {
        Log().Get(WARNING) << "Init method fails. Invalid time segment 2!";
        return false;
    }

    /* determine bus timing register values */
    byte btr0 = ((syncronizationJumpWidth & 0x03) << 6) | ((prescaler - 1) & 0x3F);
    byte btr1 = ((timeSegment2 & 0x07) << 4) | (timeSegment1 & 0x0F);

    if (sampling) {
        btr1 |= (1 << 7);
    }

    /* write bus timing registers */
    switch (_mode) {
        case USAGE_MODE::BASIC_MODE:
            return this->getRegister(REGISTER_BASIC_MODE::BM_BUS_TIMING_0)->writeSync(btr0) &&
                   this->getRegister(REGISTER_BASIC_MODE::BM_BUS_TIMING_1)->writeSync(btr1);

        case USAGE_MODE::EXTENDEND_MODE:
            return this->getRegister(REGISTER_EXTENDEND_MODE::EM_BUS_TIMING_0)->writeSync(btr0) &&
                   this->getRegister(REGISTER_EXTENDEND_MODE::EM_BUS_TIMING_1)->writeSync(btr1);

        default:
            return false;
    }
}

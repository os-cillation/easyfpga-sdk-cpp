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

#include "configuration.h" /* WISHBONE_CLOCK_FREQUENCY */
#include "easycores/pin.h"
#include "easycores/register.h"
#include "easycores/uart/callbacks/init_method.h"
#include "easycores/uart/callbacks/interrupt_identification.h"
#include "easycores/uart/callbacks/set_baudrate_divisor.h"
#include "easycores/uart/callbacks/read_modify_write.h"
#include "easycores/uart/uart.h"
#include "utils/log/log.h"

#include <sstream>

Uart::Uart() :
    EasyCore(UNIQUE_CORE_NUMBER),
    _rxTriggerLevel(RX_TRIGGER_LEVEL::RX_TRIGGER_LEVEL_1)
{
    _pinMap.insert(std::make_pair(PIN::RXD, std::make_shared<Pin>("RXD_i", &_index, PIN::RXD, PIN_DIRECTION_TYPE::IN)));
    _pinMap.insert(std::make_pair(PIN::TXD, std::make_shared<Pin>("TXD_o", &_index, PIN::TXD, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::CTSn, std::make_shared<Pin>("CTSn_i", &_index, PIN::CTSn, PIN_DIRECTION_TYPE::IN)));
    _pinMap.insert(std::make_pair(PIN::RTSn, std::make_shared<Pin>("RTSn_o", &_index, PIN::RTSn, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::DSRn, std::make_shared<Pin>("DSRn_i", &_index, PIN::DSRn, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::RIn, std::make_shared<Pin>("RIn_i", &_index, PIN::RIn, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::DCDn, std::make_shared<Pin>("DCDn_i", &_index, PIN::DCDn, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::DTRn, std::make_shared<Pin>("DTRn_o", &_index, PIN::DTRn, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::AUX1, std::make_shared<Pin>("AUX1_o", &_index, PIN::AUX1, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::AUX2, std::make_shared<Pin>("AUX2_o", &_index, PIN::AUX2, PIN_DIRECTION_TYPE::OUT)));

    _registerMap.insert(std::make_pair(REGISTER::RX, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER::TX, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER::IER, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::IIR, std::make_shared<Register>(this, (byte)0x02, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER::FCR, std::make_shared<Register>(this, (byte)0x02, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER::LCR, std::make_shared<Register>(this, (byte)0x03, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::MCR, std::make_shared<Register>(this, (byte)0x04, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::LSR, std::make_shared<Register>(this, (byte)0x05, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER::MSR, std::make_shared<Register>(this, (byte)0x06, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER::SCR, std::make_shared<Register>(this, (byte)0x07, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::DLL, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::DLM, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::READWRITE)));
}

Uart::~Uart()
{
}

std::string Uart::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_clock_div.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_counter.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_edge_detect.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_fifo_cyclone2.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_fifo.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_input_filter.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_input_sync.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/slib_mv_filter.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/uart_baudgen.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/uart_interrupt.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/uart_receiver.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/uart_transmitter.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/uart_16750.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/uart16750/uart_16750_wb.vhd\"" << std::endl;

    return ss.str();
}

std::string Uart::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "UART%d : entity work.uart_16750_wb"                                              << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool Uart::init(uint32_t baudrate, WORD_LENGTH wLength, PARITY parity, STOP_BIT_COUNT stopBits)
{
    /* PARAMETER CHECK */
    byte lcrContent = (byte)0x00;

    switch (wLength) {
        case WORD_LENGTH::C5:
            /* dont't set any bits */
            break;

        case WORD_LENGTH::C6:
            setBit(lcrContent, 0);
            break;

        case WORD_LENGTH::C7:
            setBit(lcrContent, 1);
            break;

        case WORD_LENGTH::C8:
            setBit(lcrContent, 0);
            setBit(lcrContent, 1);
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The word length parameter is not valid.";
            return false;
    }

    switch (stopBits) {
        case STOP_BIT_COUNT::ONE_BIT:
            /* dont't set any bits */
            break;

        case STOP_BIT_COUNT::TWO_BITS:
            setBit(lcrContent, 2);
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The number of stop bits is not valid.";
            return false;
    }

    switch (parity) {
        case PARITY::NO_PARITY:
            /* dont't set any bits */
            break;

        case PARITY::ODD_PARITY:
            setBit(lcrContent, 3);
            break;

        case PARITY::EVEN_PARITY:
            setBit(lcrContent, 3);
            setBit(lcrContent, 4);
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The parity selection parameter is not valid.";
            return false;
    }

    /* PERFORM AN ACTION DEPENDING ON MODE */
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            /* write line control register */
            if (!getRegister(REGISTER::LCR)->writeSync(lcrContent)) {
                Log().Get(WARNING) << "Init method could not set the communication line parameters.";
                return false;
            }

            /* set baudrate */
            if (!this->setBaudrate(baudrate)) {
                Log().Get(WARNING) << "Init method could not set the desired baudrate.";
                return false;
            }

            /* init fifo control register (enable 64 byte fifo) */
            //callback_ptr c =  std::make_shared<UartInitiationCallback>(this);
            buffer = (byte)0x00;
            if (!getRegister(REGISTER::LCR)->readSync(&buffer)) {
                return false;
            }
            setBit(buffer, 7);
            if (!getRegister(REGISTER::LCR)->writeSync(buffer)) {
                return false;
            }

            if (!getRegister(REGISTER::FCR)->writeSync((byte)0x21)) {
                Log().Get(WARNING) << "Init method could not enable the 64 byte fifos.";
                return false;
            }

            clrBit(buffer, 7);
            if (!getRegister(REGISTER::LCR)->writeSync(buffer)) {
                return false;
            }

            /* disable tx empty interrupt */
            if (!this->disableInterrupt(INTERRUPT::TX_EMPTY)) {
                Log().Get(WARNING) << "Init method could not disable the INTERRUPT::TX_EMPTY interrupt.";
                return false;
            }

            return true;

        /** \todo Support async mode */
        case OPERATION_MODE::ASYNC:
            Log().Get(WARNING) << "Async mode not yet supported. Aborting ...";
            return false;
    }

    return false;
}

bool Uart::setBaudrate(uint32_t baudrate)
{
    /* PARAMETER CHECK */
    uint32_t baudrateDivisorHelper = (uint32_t)(WISHBONE_CLOCK_FREQUENCY / (baudrate * 16));

    Log().Get(DEBUG) << "Initialized baurate: " << (uint32_t)(WISHBONE_CLOCK_FREQUENCY / (baudrateDivisorHelper * 16));

    /* sanity check of divisor value */
    assert (baudrateDivisorHelper < (uint32_t)0xFFFF);
    if (baudrateDivisorHelper > (uint32_t)0xFFFF) {
        return false;
    }

    uint16_t baudrateDivisor = (uint16_t)baudrateDivisorHelper;

    byte low = (baudrateDivisor & 0xFF);
    byte high = (baudrateDivisor & 0xFF00) >> 8;

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            getRegister(REGISTER::LCR)->readSync(&buffer);
            setBit(buffer, 7);
            success &= getRegister(REGISTER::LCR)->writeSync(buffer);

            /* Write out low and high byte of the baudrate divisor */
            success &= getRegister(REGISTER::DLL)->writeSync(low);
            success &= getRegister(REGISTER::DLM)->writeSync(high);

            /* Reset divisor latch access bit in order to access the buffer registers */
            clrBit(buffer, 7);
            success &= getRegister(REGISTER::LCR)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<UartSetBaudrateDivisorCallback>(this, low, high);
            return getRegister(REGISTER::LCR)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Uart::setRxTriggerLevel(RX_TRIGGER_LEVEL level)
{
    /* PARAMETER CHECK */
    byte value = (byte)0x01;

    switch (level) {
        case Uart::RX_TRIGGER_LEVEL::RX_TRIGGER_LEVEL_1:
            _rxTriggerLevel = RX_TRIGGER_LEVEL_1;
            value = (byte)0x01;

        case Uart::RX_TRIGGER_LEVEL::RX_TRIGGER_LEVEL_16:
            _rxTriggerLevel = RX_TRIGGER_LEVEL_16;
            value = (byte)0x41;

        case Uart::RX_TRIGGER_LEVEL::RX_TRIGGER_LEVEL_32:
            _rxTriggerLevel = RX_TRIGGER_LEVEL_32;
            value = (byte)0x81;

        case Uart::RX_TRIGGER_LEVEL::RX_TRIGGER_LEVEL_56:
            _rxTriggerLevel = RX_TRIGGER_LEVEL_56;
            value = (byte)0xC1;

        default:
            return false;
    }

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::FCR)->writeSync(value);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::FCR)->writeAsync(value);
    }

    return false;
}


bool Uart::enableAutoHardwareFlowControl(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::MCR)->readSync(&buffer);
            setBit(buffer, 1);
            setBit(buffer, 5);
            success &= getRegister(REGISTER::MCR)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            std::list<std::pair<uint8_t, LogicLevel>> pinsToModify;
            pinsToModify.push_back(std::make_pair(1, HIGH));
            pinsToModify.push_back(std::make_pair(5, HIGH));

            callback_ptr c = std::make_shared<UartModifiedWriteCallback>(this, REGISTER::MCR, pinsToModify);
            return getRegister(REGISTER::MCR)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Uart::enableInterrupts(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::IER)->writeSync((byte)0x1F);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::IER)->writeAsync((byte)0x1F);
    }

    return false;
}

bool Uart::enableInterrupt(INTERRUPT interrupt)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;
            buffer = (byte)0x00;
            success &= getRegister(REGISTER::IER)->readSync(&buffer);
            switch (interrupt) {
                case INTERRUPT::RX_AVAILABLE:
                    setBit(buffer, 0);
                    break;

                case INTERRUPT::TX_EMPTY:
                    setBit(buffer, 1);
                    break;

                case INTERRUPT::RX_LINE_STATUS:
                    setBit(buffer, 2);
                    break;

                case INTERRUPT::MODEM_STATUS:
                    setBit(buffer, 3);
                    break;

                case INTERRUPT::CHARACTER_TIMEOUT:
                    setBit(buffer, 4);
                    break;

                default:
                    return false;
            }
            success &= getRegister(REGISTER::IER)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            std::list<std::pair<uint8_t, LogicLevel>> pinsToModify;

            switch (interrupt) {
                case INTERRUPT::RX_AVAILABLE:
                    pinsToModify.push_back(std::make_pair(0, HIGH));
                    break;

                case INTERRUPT::TX_EMPTY:
                    pinsToModify.push_back(std::make_pair(1, HIGH));
                    break;

                case INTERRUPT::RX_LINE_STATUS:
                    pinsToModify.push_back(std::make_pair(2, HIGH));
                    break;

                case INTERRUPT::MODEM_STATUS:
                    pinsToModify.push_back(std::make_pair(3, HIGH));
                    break;

                case INTERRUPT::CHARACTER_TIMEOUT:
                    pinsToModify.push_back(std::make_pair(4, HIGH));
                    break;

                default:
                    return false;
            }

            callback_ptr c = std::make_shared<UartModifiedWriteCallback>(this, REGISTER::IER, pinsToModify);
            return getRegister(REGISTER::IER)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Uart::disableInterrupts(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::IER)->writeSync((byte)0x00);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::IER)->writeAsync((byte)0x00);
    }

    return false;
}

bool Uart::disableInterrupt(INTERRUPT interrupt)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;
            buffer = (byte)0x00;
            success &= getRegister(REGISTER::IER)->readSync(&buffer);
            switch (interrupt) {
                case INTERRUPT::RX_AVAILABLE:
                    clrBit(buffer, 0);
                    break;

                case INTERRUPT::TX_EMPTY:
                    clrBit(buffer, 1);
                    break;

                case INTERRUPT::RX_LINE_STATUS:
                    clrBit(buffer, 2);
                    break;

                case INTERRUPT::MODEM_STATUS:
                    clrBit(buffer, 3);
                    break;

                case INTERRUPT::CHARACTER_TIMEOUT:
                    clrBit(buffer, 4);
                    break;

                default:
                    return false;
            }
            success &= getRegister(REGISTER::IER)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            std::list<std::pair<uint8_t, LogicLevel>> pinsToModify;

            switch (interrupt) {
                case INTERRUPT::RX_AVAILABLE:
                    pinsToModify.push_back(std::make_pair(0, LOW));
                    break;

                case INTERRUPT::TX_EMPTY:
                    pinsToModify.push_back(std::make_pair(1, LOW));
                    break;

                case INTERRUPT::RX_LINE_STATUS:
                    pinsToModify.push_back(std::make_pair(2, LOW));
                    break;

                case INTERRUPT::MODEM_STATUS:
                    pinsToModify.push_back(std::make_pair(3, LOW));
                    break;

                case INTERRUPT::CHARACTER_TIMEOUT:
                    pinsToModify.push_back(std::make_pair(4, LOW));
                    break;

                default:
                    return false;
            }

            callback_ptr c = std::make_shared<UartModifiedWriteCallback>(this, REGISTER::IER, pinsToModify);
            return getRegister(REGISTER::IER)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Uart::identifyInterrupt(Uart::INTERRUPT* target)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::IIR)->readSync(&buffer);

            if ((buffer & 0x0F) == (byte)0x06) {
                *(target) = Uart::INTERRUPT::RX_LINE_STATUS;
            }
            else if ((buffer & 0x0F) == (byte)0x04) {
                *(target) = Uart::INTERRUPT::RX_AVAILABLE;
            }
            else if ((buffer & 0x0F) == (byte)0x0C) {
                *(target) = Uart::INTERRUPT::CHARACTER_TIMEOUT;
            }
            else if ((buffer & 0x0F) == (byte)0x02) {
                *(target) = Uart::INTERRUPT::TX_EMPTY;
            }
            else if ((buffer & 0x0F) == (byte)0x00) {
                *(target) = Uart::INTERRUPT::MODEM_STATUS;
            }
            else {
                return false;
            }

            return true;

        case OPERATION_MODE::ASYNC:
            callback_ptr c = std::make_shared<UartInterruptIdentificationCallback>(target);
            return getRegister(REGISTER::IIR)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Uart::transmit(byte b)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::TX)->writeSync(b);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::TX)->writeAsync(b);
    }

    return false;
}


bool Uart::transmit(byte* buffer, uint32_t byteCount)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    /** \todo Implement this function */
    Log().Get(WARNING) << "transmit(byte*, uint32_t) is not yet implemented. Aborting ...";
    return false;
}


bool Uart::receive(byte* b)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return getRegister(REGISTER::RX)->readSync(b);

        case OPERATION_MODE::ASYNC:
            return getRegister(REGISTER::RX)->readAsync(b);
    }

    return false;
}

bool Uart::receive(byte* targetBuffer, uint32_t byteCount)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    /** \todo Implement this function */
    Log().Get(WARNING) << "receive(byte*, uint32_t) is not yet implemented. Aborting ...";
    return false;
}


bool Uart::clearBuffers(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success = getRegister(REGISTER::FCR)->writeSync((byte)0x07);

    /** \todo receive all remaining bytes and add async mode */

    success &= this->setRxTriggerLevel(_rxTriggerLevel);

    return success;
}

bool Uart::setAuxiliaryOutput1(LogicLevel level)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::MCR)->readSync(&buffer);

            if (level) {
                setBit(buffer, 2);
            }
            else {
                clrBit(buffer, 2);
            }

            success &= getRegister(REGISTER::MCR)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            std::list<std::pair<uint8_t, LogicLevel>> pinsToModify;
            pinsToModify.push_back(std::make_pair(3, level));
            callback_ptr c = std::make_shared<UartModifiedWriteCallback>(this, REGISTER::MCR, pinsToModify);
            return getRegister(REGISTER::MCR)->readAsync(c->getBuffer(), c);
    }

    return false;
}

bool Uart::setAuxiliaryOutput2(LogicLevel level)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success;
    byte buffer;

    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            success = true;

            buffer = (byte)0x00;
            success &= getRegister(REGISTER::MCR)->readSync(&buffer);

            if (level) {
                setBit(buffer, 3);
            }
            else {
                clrBit(buffer, 3);
            }

            success &= getRegister(REGISTER::MCR)->writeSync(buffer);

            return success;

        case OPERATION_MODE::ASYNC:
            std::list<std::pair<uint8_t, LogicLevel>> pinsToModify;
            pinsToModify.push_back(std::make_pair(4, level));
            callback_ptr c = std::make_shared<UartModifiedWriteCallback>(this, REGISTER::MCR, pinsToModify);
            return getRegister(REGISTER::MCR)->readAsync(c->getBuffer(), c);
    }

    return false;
}

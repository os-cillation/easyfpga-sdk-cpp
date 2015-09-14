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
#include "easycores/i2c/i2c.h"
#include "easycores/pin.h"
#include "easycores/register.h"
#include "utils/log/log.h"

#include <sstream>

I2C::I2C() :
    EasyCore(UNIQUE_CORE_NUMBER)
{
    _pinMap.insert(std::make_pair(PIN::SDA, std::make_shared<Pin>("i2c_sda_io", &_index, PIN::SDA, PIN_DIRECTION_TYPE::INOUT)));
    _pinMap.insert(std::make_pair(PIN::SCL, std::make_shared<Pin>("i2c_scl_io", &_index, PIN::SCL, PIN_DIRECTION_TYPE::INOUT)));

    _registerMap.insert(std::make_pair(REGISTER::PREREG_LOW, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::PREREG_HIGH, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::CTRL, std::make_shared<Register>(this, (byte)0x02, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::TX, std::make_shared<Register>(this, (byte)0x03, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER::RX, std::make_shared<Register>(this, (byte)0x03, REGISTER_ACCESS_TYPE::READONLY)));
    _registerMap.insert(std::make_pair(REGISTER::CR, std::make_shared<Register>(this, (byte)0x04, REGISTER_ACCESS_TYPE::WRITEONLY)));
    _registerMap.insert(std::make_pair(REGISTER::SR, std::make_shared<Register>(this, (byte)0x04, REGISTER_ACCESS_TYPE::READONLY)));
}

I2C::~I2C()
{
}

std::string I2C::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/i2c_master/i2c_master_bit_ctrl.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/i2c_master/i2c_master_byte_ctrl.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/i2c_master/i2c_master_top.vhd\"" << std::endl;
    ss << "vhdl work \"%dir/easy_cores/i2c_master/i2c_master.vhd\"" << std::endl;

    return ss.str();
}

std::string I2C::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "I2C_MASTER%d : entity work.i2c_master"                                           << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      --clk_in   => gclk_s,"                                                     << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool I2C::init(CLOCK_SPEED speed)
{
    /* PARAMETER CHECK */
    uint16_t prescale;
    byte prescaleLow;
    byte prescaleHigh;

    switch (speed) {
        case CLOCK_SPEED::MODE_STANDARD:
            prescale = (WISHBONE_CLOCK_FREQUENCY / (5 * 100000)) - 1;
            prescaleLow  = (prescale & 0xFF);
            prescaleHigh = (prescale & 0xFF00) >> 8;
            break;

        case CLOCK_SPEED::MODE_FAST:
            prescale = (WISHBONE_CLOCK_FREQUENCY / (5 * 400000)) - 1;
            prescaleLow  = (prescale & 0xFF);
            prescaleHigh = (prescale & 0xFF00) >> 8;
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The speed parameter is not valid.";
            return false;
    }

    /* PERFORM AN ACTION */
    bool success = true;

    /* Disable core. */
    success &= this->getRegister(REGISTER::CTRL)->changeBitSync(6, false);

    /* Write clock register. */
    success &= this->getRegister(REGISTER::PREREG_LOW)->writeSync(prescaleLow);
    success &= this->getRegister(REGISTER::PREREG_HIGH)->writeSync(prescaleHigh);

    /* Enable core. */
    success &= this->getRegister(REGISTER::CTRL)->changeBitSync(6, true);

    return success;
}

bool I2C::transfer(byte data, bool write, uint8_t start, bool nack, byte* reply)
{
    /* PARAMETER CHECK */
    if (write && nack) {
        Log().Get(WARNING) << "Nack can only be asserted in read transmissions! Abort transfer...";
        return false;
    }

    /* PERFORM AN ACTION */
    bool success = true;

    byte command = (byte)0x00;

    if (start == 0) {
        command |= (1 << 7);
    }
    else {
        command |= (1 << 6);
    }

    if (nack) {
        command |= (1 << 3);
    }

    if (write) {
        /* write data/address + WR-bit to TX register */
        success &= this->getRegister(REGISTER::TX)->writeSync(data);

        command |= (1 << 4); /* WRITE */
    }
    else {
        command |= (1 << 5); /* READ */
    }

    /* write command register */
    success &= this->getRegister(REGISTER::CR)->writeSync(command);

    /* busy wait until finished */
    byte status = (byte)0x00;
    success &= this->getRegister(REGISTER::SR)->readSync(&status);
    while (success && setBitTest(status, 1)) {
        success &= this->getRegister(REGISTER::SR)->readSync(&status);
        usleep(10);
    }

    if (write) {
        success &= this->getRegister(REGISTER::SR)->readSync(&status);

        /* return pseudo-boolean representation of ACK bit if write transmission */
        if (setBitTest(status, 7)) {
            *reply = (byte)0x00;
        }
        else {
            *reply = (byte)0x01;
        }
    }
    else {
        /* return read value, when receiving data */
        success &= this->getRegister(REGISTER::RX)->readSync(reply);
    }

    return success;
}

bool I2C::writeByte(uint8_t deviceAddress, uint8_t registerAddress, byte data)
{
    /* PARAMETER CHECK */
    if ((deviceAddress < 0) || (deviceAddress > 127)) {
        Log().Get(WARNING) << "Device address out of range [0, 127]. Abort write...";
        return false;
    }
    if ((registerAddress < 0) || (registerAddress > 255)) {
        Log().Get(WARNING) << "Register address out of range [0, 255]. Abort write...";
        return false;
    }

    /* PERFORM AN ACTION */
    bool success = true;

    byte ack0 = (byte)0xFF;
    byte ack1 = (byte)0xFF;
    byte ack2 = (byte)0xFF;

    /* Transmit slave address. Start condition, R/W-Bit: 0 */
    success &= this->transfer(deviceAddress << 1, true, 0, false, &ack0);

    /* Transmit register address */
    success &= this->transfer(registerAddress, true, 2, false, &ack1);

    /* Transmit data. Stop condition. */
    success &= this->transfer(data, true, 1, false, &ack2);

    /* Check if no NACK received while transmission. */
    if ((ack0 != 0x01) || (ack1 != 0x01) || (ack2 != 0x01)) {
        Log().Get(WARNING) << "NACK during write byte operation";
        return false;
    }

    return success;
}

bool I2C::readByte(uint8_t deviceAddress, uint8_t registerAddress, byte* data)
{
    /* PARAMETER CHECK */
    if ((deviceAddress < 0) || (deviceAddress > 127)) {
        Log().Get(WARNING) << "Device address out of range [0, 127]. Abort read...";
        return false;
    }
    if ((registerAddress < 0) || (registerAddress > 255)) {
        Log().Get(WARNING) << "Register address out of range [0, 255]. Abort read...";
        return false;
    }

    /* PERFORM AN ACTION */
    bool success = true;

    byte ack0 = (byte)0xFF;
    byte ack1 = (byte)0xFF;
    byte ack2 = (byte)0xFF;
    byte receivedData = (byte)0xFF;

    /* Transmit slave address. Start condition, R/W-Bit: 0 */
    success &= this->transfer(deviceAddress << 1, true, 0, false, &ack0);

    /* Transmit register address */
    success &= this->transfer(registerAddress, true, 2, false, &ack1);

    /* Transmit slave address. Repeated start condition, R/W-Bit: 1 */
    success &= this->transfer((deviceAddress << 1) | 0x01, true, 0, false, &ack2);

    /* Receive register content. Stop condition, send NACK */
    success &= this->transfer(0x00, false, 1, true, &receivedData);

    /* Check if no NACK received while transmission. */
    if ((ack0 != 0x01) || (ack1 != 0x01) || (ack2 != 0x01)) {
        Log().Get(WARNING) << "NACK during read byte operation";
        return false;
    }

    /* Everthing ok. Write back the data! */
    *data = receivedData;

    return success;
}

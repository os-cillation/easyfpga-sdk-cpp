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

#include "configuration.h"
#include "communication/communicator.h"
#include "communication/protocol/calculator.h"
#include "communication/protocol/specification.h"
#include "communication/serialconnection.h"
#include "communication/taskexecutor.h"
#include "utils/config/configurationfile.h"
#include "utils/log/log.h"

#include "communication/protocol/mcuexchanges/configure_fpga.h"
#include "communication/protocol/mcuexchanges/sector_4096byte_write.h"
#include "communication/protocol/mcuexchanges/select_soc.h"
#include "communication/protocol/mcuexchanges/serial_read.h"
#include "communication/protocol/mcuexchanges/serial_write.h"
#include "communication/protocol/mcuexchanges/status_read.h"
#include "communication/protocol/mcuexchanges/status_write.h"

#include "communication/protocol/socexchanges/interrupt_enable.h"
#include "communication/protocol/socexchanges/read_register.h"
#include "communication/protocol/socexchanges/read_register_auto_address_increment.h"
#include "communication/protocol/socexchanges/read_register_multiple_times.h"
#include "communication/protocol/socexchanges/select_mcu.h"
#include "communication/protocol/socexchanges/write_register.h"
#include "communication/protocol/socexchanges/write_register_auto_address_increment.h"
#include "communication/protocol/socexchanges/write_register_multiple_times.h"

#include <unistd.h> /* usleep() */
#include <dirent.h> /* low level c directory functions */
#include <string.h> /* memcpy() */

Communicator::Communicator(easycore_map_ptr cores) :
    _target(COM_TARGET::UNDEFINED),
    _connection(std::make_shared<SerialConnection>()),
    _executor(std::make_shared<TaskExecutor>(_connection, cores)),
    _USB_DEVICE_PATH(ConfigurationFile::getInstance().getUsbDevicesPath()),
    _USB_DEVICE_IDENTIFIER(ConfigurationFile::getInstance().getUsbDeviceIdentifier())
{
    Log().Get(DEBUG) << "Communicator is not initialized. Connection status is undefined.";
}

Communicator::~Communicator()
{
    Log().Get(DEBUG) << "Try closing serial connection...";
    if (_connection->closeDevice()) {
        Log().Get(DEBUG) << "Close serial connection successful.";
    }
    else {
        Log().Get(ERROR) << "Can not close serial connection... :-(";
    }
}

bool Communicator::init(uint32_t serial)
{
    std::pair<std::string, Communicator::COM_TARGET> emptyEasyFpga;
    std::pair<std::string, Communicator::COM_TARGET> device(this->findFirstMatchingEasyFpga(serial));
    if (device != emptyEasyFpga) {
        Log().Get(DEBUG) << "Try to open " << device.first << " as serial connection...";
        if (_connection->openDevice(device.first)) {
            _connection->flushBuffers();

            _target = device.second;
            if (_target == COM_TARGET::MCU) {
                Log().Get(DEBUG) << "Communicator now connected to mcu.";
                return true;
            }
            else if (_target == COM_TARGET::SOC) {
                Log().Get(DEBUG) << "Communicator now connected to soc.";
                return true;
            }
            else {
                Log().Get(ERROR) << "Could not connect to mcu or soc.";
            }
        }
    }
    else {
        Log().Get(WARNING) << "No matching easyFpga found. Communicator not connected to any easyFPGA.";
    }

    return false;
}

bool Communicator::configureFpga(void)
{
    if (!this->switchTo(COM_TARGET::MCU)) {
        return false;
    }

    return _executor->doSyncTask(std::string("configureFpga"), std::make_shared<ConfigureFpga>(nullptr));
}

bool Communicator::writeBinary(byte* binary, uint64_t binaryLength)
{
    /*
     * First, we have to know whether the binary fits exactly into an
     * integer multiple of 4096 bytes. If not, we have to add an
     * additional sector which will filled with the remaining bytes and
     * the remainder with null-bytes (see send loop below).
     */
    uint32_t byteCountofLastSector = binaryLength % 4096;
    Log().Get(DEBUG) << "Number of bytes in last sector: " << (int32_t)byteCountofLastSector;

    uint32_t numOf4096ByteSectors = binaryLength / 4096;
    if (byteCountofLastSector != 0) {
        numOf4096ByteSectors++;
    }

    /* Write sector loop */
    byte* array = binary;
    uint32_t i;
    for (i=0; i<numOf4096ByteSectors; i++) {
        Log().Get(DEBUG) << "Write sector " << (int32_t)(i+1) << "/" << (int32_t)numOf4096ByteSectors;

        if (i == (numOf4096ByteSectors-1)) {
            byte rest[4096];
            memcpy(rest, array, byteCountofLastSector);
            uint32_t j;
            for (j=byteCountofLastSector; j<4096; j++) {
                rest[j] = (byte)0x00;
            }
            array = rest;
        }

        if (!_executor->doSyncTask(std::string("write4096ByteSector"), std::make_shared<Sector4096ByteWrite>(i, array, nullptr))) {
            Log().Get(WARNING) << "Aborting binary upload...";
            return false;
        }

        array += 4096;
    }

    /*
     * If the execution flow reaches this point, the binary writing was
     * succcessful.
     */
    Log().Get(DEBUG) << "Writing binary successful.";
    return true;
}

bool Communicator::writeSerial(uint32_t serial)
{
    if (!this->switchTo(COM_TARGET::MCU)) {
        return false;
    }

    return _executor->doSyncTask(std::string("writeSerial"), std::make_shared<SerialWrite>(serial, nullptr));
}

bool Communicator::readSerial(uint32_t* serial)
{
    if (!this->switchTo(COM_TARGET::MCU)) {
        return false;
    }

    return _executor->doSyncTask(std::string("readSerial"), std::make_shared<SerialRead>(serial, nullptr));
}

bool Communicator::writeStatus(bool socIsUploaded, uint32_t binarySize, uint32_t adler32hash)
{
    if (!this->switchTo(COM_TARGET::MCU)) {
        return false;
    }

    return _executor->doSyncTask(std::string("writeStatus"), std::make_shared<StatusWrite>(socIsUploaded, binarySize, adler32hash, nullptr));
}

bool Communicator::readStatus(bool* isFpgaConfigured, uint32_t* adler32hash)
{
    if (!this->switchTo(COM_TARGET::MCU)) {
        return false;
    }

    return _executor->doSyncTask(std::string("readStatus"), std::make_shared<StatusRead>(isFpgaConfigured, adler32hash, nullptr));
}

bool Communicator::readRegister(byte* reply, byte core, byte registerAddress)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("readRegister"),
        std::make_shared<ReadRegister>(reply, core, registerAddress, nullptr)
    );
}

bool Communicator::readMultiRegister(byte* reply, byte core, byte registerAddress, uint8_t length)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("readMultiRegister"),
        std::make_shared<ReadMultiRegister>(length, reply, core, registerAddress, nullptr)
    );
}

bool Communicator::readAutoAdressIncrementRegister(byte* reply, byte core, byte registerAddress, uint8_t length)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("readAutoAdressIncrementRegister"),
        std::make_shared<ReadAutoAddressIncrementRegister>(length, reply, core, registerAddress, nullptr)
    );
}

bool Communicator::writeRegister(byte data, byte core, byte registerAddress)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("writeRegister"),
        std::make_shared<WriteRegister>(core, registerAddress, data, nullptr)
    );
}

bool Communicator::writeMultiRegister(byte* data, byte core, byte registerAddress, uint8_t length)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("writeMultiRegister"),
        std::make_shared<WriteMultiRegister>(core, registerAddress, length, data, nullptr)
    );
}

bool Communicator::writeAutoAdressIncrementRegister(byte* data, byte core, byte registerAddress, uint8_t length)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("writeAutoAdressIncrementRegister"),
        std::make_shared<WriteAutoAddressIncrementRegister>(core, registerAddress, length, data, nullptr)
    );
}

bool Communicator::enableGlobalInterrupts(void)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->doSyncTask(
        std::string("enableGlobalInterrupts"),
        std::make_shared<InterruptEnable>(nullptr)
    );
}

bool Communicator::readRegisterAsync(byte* reply, byte core, byte registerAddress, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("readRegisterAsync"),
        std::make_shared<ReadRegister>(reply, core, registerAddress, nullptr),
        dep
    );
}

tasknumberval Communicator::readRegisterAsync(byte* reply, byte core, byte registerAddress, callback_ptr callback, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("readRegisterAsync"),
        std::make_shared<ReadRegister>(reply, core, registerAddress, callback),
        dep
    );
}

bool Communicator::readMultiRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("readMultiRegisterAsync"),
        std::make_shared<ReadMultiRegister>(length, reply, core, registerAddress, nullptr),
        dep
    );
}

tasknumberval Communicator::readMultiRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("readMultiRegisterAsync"),
        std::make_shared<ReadMultiRegister>(length, reply, core, registerAddress, callback),
        dep
    );
}

bool Communicator::readAutoAdressIncrementRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("readAutoAdressIncrementRegisterAsync"),
        std::make_shared<ReadAutoAddressIncrementRegister>(length, reply, core, registerAddress, nullptr),
        dep
    );
}

tasknumberval Communicator::readAutoAdressIncrementRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("readAutoAdressIncrementRegisterAsync"),
        std::make_shared<ReadAutoAddressIncrementRegister>(length, reply, core, registerAddress, callback),
        dep
    );
}

bool Communicator::writeRegisterAsync(byte data, byte core, byte registerAddress, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("writeRegisterAsync"),
        std::make_shared<WriteRegister>(core, registerAddress, data, nullptr),
        dep
    );
}

tasknumberval Communicator::writeRegisterAsync(byte data, byte core, byte registerAddress, callback_ptr callback, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("writeRegisterAsync"),
        std::make_shared<WriteRegister>(core, registerAddress, data, callback),
        dep
    );
}

bool Communicator::writeMultiRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("writeMultiRegisterAsync"),
        std::make_shared<WriteMultiRegister>(core, registerAddress, length, data, nullptr),
        dep
    );
}

tasknumberval Communicator::writeMultiRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("writeMultiRegisterAsync"),
        std::make_shared<WriteMultiRegister>(core, registerAddress, length, data, callback),
        dep
    );
}

bool Communicator::writeAutoAdressIncrementRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("writeAutoAdressIncrementRegisterAsync"),
        std::make_shared<WriteAutoAddressIncrementRegister>(core, registerAddress, length, data, nullptr),
        dep
    );
}

tasknumberval Communicator::writeAutoAdressIncrementRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("writeAutoAdressIncrementRegisterAsync"),
        std::make_shared<WriteAutoAddressIncrementRegister>(core, registerAddress, length, data, callback),
        dep
    );
}

bool Communicator::enableGlobalInterruptsAsync(void)
{
    if (!this->switchTo(COM_TARGET::SOC)) {
        return false;
    }

    return _executor->startAsyncTask(
        std::string("enableGlobalInterruptsAsync"),
        std::make_shared<InterruptEnable>(nullptr),
        0
    );
}

bool Communicator::handleRequestReplies(void)
{
    if (_executor->fetchAsyncReplies()) {
        _executor->writeReplies();
        return true;
    }

    return false;
}

uint32_t Communicator::getNumberOfPendingAsyncRequests(void)
{
    return _executor->getNumberOfPendingRequests() + _executor->getNumberOfFinishedRequests();
}

bool Communicator::switchTo(COM_TARGET target)
{
    if ((_target == COM_TARGET::SOC) && (target == COM_TARGET::MCU)) {
        Log().Get(DEBUG) << "Switch to mcu...";
        if (_executor->doSyncTask(std::string("selectMCU"), std::make_shared<SelectMcu>(nullptr))) {
            _target = COM_TARGET::MCU;
            Log().Get(DEBUG) << "Let the hardware " << (int32_t)WAITING_TIME_AFTER_SWITCH << "us time to do this...";
            usleep(WAITING_TIME_AFTER_SWITCH);
            _connection->flushBuffers();
            return true;
        }
        Log().Get(WARNING) << "No success in switching to mcu!";
    }
    else if ((_target == COM_TARGET::MCU) && (target == COM_TARGET::SOC)) {
        Log().Get(DEBUG) << "Switch to soc...";
        if (_executor->doSyncTask(std::string("selectSOC"), std::make_shared<SelectSoc>(nullptr))) {
            _target = COM_TARGET::SOC;
            Log().Get(DEBUG) << "Let the hardware " << (int32_t)WAITING_TIME_AFTER_SWITCH << "us time to do this...";
            usleep(WAITING_TIME_AFTER_SWITCH);
            _connection->flushBuffers();
            return true;
        }
        Log().Get(WARNING) << "No success in switching to soc!";
    }
    else if (((_target == COM_TARGET::SOC) && (target == COM_TARGET::SOC)) ||
             ((_target == COM_TARGET::MCU) && (target == COM_TARGET::MCU))) {
        Log().Get(DEBUG) << "No context switch necessary...";
        return true;
    }

    Log().Get(ERROR) << "Parameters for switching were inconsistent!";
    return false;
}

std::pair<std::string, Communicator::COM_TARGET> Communicator::findFirstMatchingEasyFpga(uint32_t serialNumber)
{
    std::pair<std::string, Communicator::COM_TARGET> easyFpga;

    std::string dir(_USB_DEVICE_PATH);
    DIR* dp = opendir(dir.c_str());

    if (dp != NULL) {
        if (serialNumber > 0) {
            Log().Get(DEBUG) << "Search for an easyFPGA with serial " << (uint32_t)serialNumber << "...";
        }
        else {
            Log().Get(DEBUG) << "Search for an easyFPGA without a specific serial...";
        }

        bool easyFpgaFound = false;
        uint32_t serial;

        struct dirent* dirp = readdir(dp);
        while ((dirp != NULL) && !easyFpgaFound) {
            std::string devicename = std::string(dirp->d_name);
            if (devicename.find(_USB_DEVICE_IDENTIFIER) != std::string::npos) {
                std::string filename(dir);
                filename.append(devicename);

                Log().Get(DEBUG) << "Try " << filename << "...";
                _connection->openDevice(filename);
                _connection->flushBuffers();

                COM_TARGET t = this->testDeviceResponseBehavior();
                switch (t) {
                    case COM_TARGET::UNDEFINED:
                        Log().Get(DEBUG) << "No mcu or soc specific bytes received. This device isnt a easyFPGA...";
                        break;

                    case COM_TARGET::MCU:
                    case COM_TARGET::SOC:
                        Log().Get(DEBUG) << "Mcu / soc specific bytes received; an easyFPGA found!";
                        if (serialNumber > 0) {
                            if (this->readSerial(&serial)) {
                                if (serialNumber == serial) {
                                    Log().Get(DEBUG) << "Serial read 0x" << std::hex << (uint32_t)serial << " matches required serial!";
                                    easyFpga = std::make_pair(filename, t);
                                    easyFpgaFound = true;
                                }
                                else {
                                    Log().Get(DEBUG) << "Serial read 0x" << std::hex << (uint32_t)serial << " doesnt match required serial...";
                                }
                            }
                            else {
                                Log().Get(WARNING) << "Serial of found easyFPGA is not readable...";
                            }
                        } else {
                            easyFpga = std::make_pair(filename, t);
                            easyFpgaFound = true;
                        }
                        break;

                    default:
                        Log().Get(WARNING) << "Function testDeviceResponseBehavior() returned a invalid value.";
                        break;
                }
                _connection->closeDevice();
            }
            dirp = readdir(dp);
        }
    }
    else {
        Log().Get(ERROR) << "Error while opening " << dir;
    }

    closedir(dp);

    return easyFpga;
}

Communicator::COM_TARGET Communicator::testDeviceResponseBehavior()
{
    COM_TARGET target = COM_TARGET::UNDEFINED;
    do {
        byte buffer[1] = {(byte)0xEE};
        _connection->send(buffer, 1);

        byte answer[3];
        for (uint32_t i=0; i<3; i++) {
            answer[i] = (byte)0x00;
        }

        if (_connection->receive(answer, 3, DETECT_TIMEOUT)) {
            if ((answer[0] == (byte)0xFF) && (answer[2] == Calculator::calculateXorParity(answer, 2))) {
                switch (answer[1]) {
                    case (byte)0xEF:
                        target = COM_TARGET::SOC;
                        break;

                    case (byte)0x22:
                        target = COM_TARGET::MCU;
                        break;

                    case (byte)0x33:
                        target = COM_TARGET::MCU_CONF;

                        Log().Get(DEBUG) << "Mcu is configuring. Wait " << (int32_t)(WAITING_TIME_BETWEEN_DETECT_REQUESTS/1000) << "ms and then test device again...";
                        usleep(WAITING_TIME_BETWEEN_DETECT_REQUESTS);
                        break;

                    default:
                        target = COM_TARGET::UNDEFINED;
                }
            }
        }
    } while (target == COM_TARGET::MCU_CONF);

    return target;
}

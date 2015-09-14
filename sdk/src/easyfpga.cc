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

#include "configuration.h" /* BANK_COUNT, PIN_COUNT */
#include "easyfpga.h"
#include "communication/communicator.h"
#include "communication/protocol/calculator.h"
#include "easycores/easycore.h"
#include "easycores/gpiopin.h"
#include "easycores/types.h"
#include "generator/generator.h"
#include "utils/config/configurationfile.h" /* getOperationMode(0) */
#include "utils/idmanager.h"
#include "utils/log/log.h"
#include "utils/os/file.h"
#include "utils/os/directory.h"

#include <sstream>
#include <iomanip> /* setw(), setfill() */

EasyFpga::EasyFpga() :
    _OPERATION_MODE(ConfigurationFile::getInstance().getOperationMode()),
    _gpioCoreIndex(SPECIAL_CORE_INDICES::GPIOPIN_CORE_ASSOCIATION),
    _easyCoreMap(std::make_shared<std::map<CoreIndex, easycore_ptr>>()),
    _com(std::make_shared<Communicator>(_easyCoreMap)),
    _alreadyInstantiatedCores(false)
{
    /*
     * Create the gpio pin map
     */
    PinConst key = GPIOPIN::BANK0_PIN0;
    for (uint8_t i=0; i<BANK_COUNT; i++) {
        for (uint8_t j=0; j<PIN_COUNT; j++) {
            std::stringstream pinName;
            pinName << "gpio_b" << std::setw(1) << (uint32_t)i << "_" << std::setfill('0') << std::setw(2) << (uint32_t)j;
            _gpioPinMap.insert(std::make_pair(key, std::make_shared<GpioPin>(pinName.str(), &_gpioCoreIndex, key, PIN_DIRECTION_TYPE::UNDEFINED, i, j)));
            key++;
        }
    }

    /*
     * Fill the stack of free core indices with values from 1 to 255.
     * (Since the gpio pins are not associated to any easyCore the value
     * zero is reserved for indentifying them.)
     */
    _freeCoreIndices = new IdManager<CoreIndex>();
}

EasyFpga::~EasyFpga()
{
    delete _freeCoreIndices;
    _freeCoreIndices = NULL;
}

bool EasyFpga::init(uint32_t serialNumber, std::string pathToBinary)
{
    Log().Get(INFO) << "Check if the specified binary already exists. If not, try to generate this one...";

    File binary(pathToBinary);
    if (binary.exists()) {
        Log().Get(DEBUG) << "The binary was found at the specified path. Check if the binary description changed...";
    }
    else {
        Log().Get(DEBUG) << "The binary was NOT found at the specified path. Try to generate a binary...";
    }

    /*
     * Truncate the file name from fileName and Determine the absolute
     * file path of path and write it back to path.
     */
    std::string path = Directory(pathToBinary).getName();
    Directory(path).getTheAbsolutePath(path);

    /*
     * generateBinary(3) does:
     * 1. Always generates VHDL and compares it to the generated VHDL
     *    before (if any *.vhd files exists).
     * 2. Checks if a toolchain run will be neccessary and mightly execute
     *    it to build a new binary.
     */
    Log().Get(INFO) << "Binary name: " << binary.getNameWithoutEnding();
    Log().Get(INFO) << "Selected directory: " << path;

    Generator g;

    if (g.generateBinary(binary.getNameWithoutEnding(), shared_from_this(), path.c_str())) {
        Log().Get(INFO) << "Successful attempt for choosing the correct binary! :-D";
    }
    else {
        Log().Get(ERROR) << "Unsuccessful attempt to choose the correct binary. The fgpa initialization method will be aborted. :-(";
        return false;
    }

    Log().Get(INFO) << "Connect to an easyFPGA...";

    if (!this->connectHardwareDevice(serialNumber)) {
        return false;
    }

    Log().Get(INFO) << "Upload the given binary...";

    if (!this->uploadBinaryFile(pathToBinary)) {
        return false;
    }

    Log().Get(DEBUG) << "Configure the to the fpga belonging easyCores...";

    this->instantiateCores();

    Log().Get(INFO) << "The fpga's initialization process was successful!";

    return true;
}

communicator_ptr EasyFpga::getCommunicator(void)
{
    return _com;
}

bool EasyFpga::connectHardwareDevice(uint32_t serialNumber)
{
    if (_com->init(serialNumber)) {
        uint32_t serialRead = 0;
        if (_com->readSerial(&serialRead)) {
            Log().Get(DEBUG) << "Connection to an easyFPGA with serial number 0x" << std::hex << serialRead << " successfully established.";
        }
        return true;
    }
    else {
        Log().Get(WARNING) << "No connection to an easyFPGA established. :-(";
        return false;
    }
}

bool EasyFpga::uploadBinaryFile(std::string pathToBinary)
{
    File binaryFile(pathToBinary);

    uint64_t size = 0;
    if (!binaryFile.getSize(&size)) {
        Log().Get(ERROR) << "The binary (located on the pc) could not be read!";
        Log().Get(ERROR) << "Did you used the right path to the binary in init() / uploadBinaryFile()?";
        return false;
    }

    Log().Get(DEBUG) << "Binary file size: " << size;
    Log().Get(DEBUG) << "Write binary file into internal buffer...";
    byte buffer[size];
    if (!binaryFile.writeIntoByteBuffer(buffer)) {
        Log().Get(ERROR) << "Error while reading binary!";
        return false;
    }

    Log().Get(DEBUG) << "Get status from connected easyFPGA...";
    bool isFpgaConfigured = false;
    uint32_t remoteHash = 0;
    if (!_com->readStatus(&isFpgaConfigured, &remoteHash)) {
        Log().Get(ERROR) << "The status of the easyFPGA board could not be read!";
        return false;
    }

    uint32_t localHash = Calculator::calculateAdler32Hash(buffer, size); // calculate hash from local binary

    Log().Get(DEBUG) << "Is fpga configured: " << std::boolalpha << isFpgaConfigured;
    Log().Get(DEBUG) << "Checksum from host binary: 0x" << std::hex << localHash;
    Log().Get(DEBUG) << "Checksum from easyFPGA binary: 0x" << std::hex << remoteHash;

    if (localHash == remoteHash) {
        Log().Get(DEBUG) << "No new binary upload neccessary.";

        if (!isFpgaConfigured) {
            Log().Get(DEBUG) << "Configure FPGA with the stored binary...";
            if (_com->configureFpga()) {
                Log().Get(DEBUG) << "Success!";
                return true;
            }
            else {
                Log().Get(ERROR) << "Configuration faulty!";
            }
        }
        else {
            Log().Get(DEBUG) << "No new configuration neccessary. Success!";
            return true;
        }
    }
    else {
        Log().Get(DEBUG) << "Try to upload a new binary...";
        if (_com->writeBinary(buffer, size)) {
            Log().Get(DEBUG) << "Binary upload ok. Now write new status to the easyFPGA...";
            if (_com->writeStatus(true, size, localHash)) {
                Log().Get(DEBUG) << "Configure Fpga with uploaded VHDL binary...";
                if (_com->configureFpga()) {
                    Log().Get(DEBUG) << "Configuration done. Success!";
                    return true;
                }
                else {
                    Log().Get(ERROR) << "Configuration faulty!";
                }
            }
            else {
                Log().Get(ERROR) << "Error while writing status!";
            }
        }
        else {
            Log().Get(ERROR) << "Error while binary upload!";
        }
    }

    Log().Get(WARNING) << "Binary could not be uploaded!";
    return false;
}

bool EasyFpga::handleReplies(void)
{
    return _com->handleRequestReplies();
}

bool EasyFpga::enableInterrupts(void)
{
    switch (_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            return _com->enableGlobalInterrupts();

        case OPERATION_MODE::ASYNC:
            return _com->enableGlobalInterruptsAsync();
    }

    return false;
}

void EasyFpga::instantiateCores(void)
{
    if (!_alreadyInstantiatedCores) {
        this->defineStructure();
        _alreadyInstantiatedCores = true;
    }
}

bool EasyFpga::addEasyCore(easycore_ptr core)
{
    assert(core != nullptr);
    if (core != nullptr) {
        if (core->getIndex() == SPECIAL_CORE_INDICES::NO_FPGA_ASSOCIATION) {
            CoreIndex i = SPECIAL_CORE_INDICES::NO_FPGA_ASSOCIATION;
            if (_freeCoreIndices->getFreeId(&i)) {
                _easyCoreMap->insert(std::make_pair(i, core));
                core->setIndex(i);
                core->setCommunicator(_com);
                return true;
            }
            else {
                Log().Get(ERROR) << "This easyCore can not be added to this easyFPGA. Maximum number of easyCores reached!";
            }
        }
        else {
            Log().Get(ERROR) << "This easyCore can not be added to this easyFPGA. The core belongs already to an other easyFPGA!";
        }
    }

    return false;
}

bool EasyFpga::removeEasyCore(easycore_ptr coreToRemove)
{
    if (coreToRemove != NULL) {
        CoreIndex i = coreToRemove->getIndex();
        size_t elementsErased = _easyCoreMap->erase(i);

        if (elementsErased < 1) {
            Log().Get(WARNING) << "This easyCore can not be removed from this fpga because the core either was not added before or it belongs to another core!";
        }
        else if (elementsErased == 1) {
            if (_freeCoreIndices->releaseId(i)) {
                coreToRemove->setIndex(SPECIAL_CORE_INDICES::NO_FPGA_ASSOCIATION);
                coreToRemove->setCommunicator(NULL);
                return true;
            }
            else {
                Log().Get(ERROR) << "Internal Error! There are more core ids in the stack as allocated.";
                assert(false);
            }
        }
        else {
            Log().Get(ERROR) << "Internal Error! There more than one easyCores with one core index.";
            assert(false);
        }
    }
    else {
        Log().Get(ERROR) << "The core instance was invalid.";
    }

    return false;
}

bool EasyFpga::connect(PinConst sourceGpioPin, PinConst sinkGpioPin)
{
    /* PARAMETER CHECK */
    gpiopin_ptr gpio1 = this->getGpioPin(sourceGpioPin);
    gpiopin_ptr gpio2 = this->getGpioPin(sinkGpioPin);

    if ((gpio1 == nullptr) || (gpio2 == nullptr)) {
        Log().Get(ERROR) << "At least one PinConst is faulty!";
        exit(-1);
        return false;
    }

    /* PERFORM A CONNECT IF POSSIBLE */
    return gpio1->connectTo(gpio2);
}

bool EasyFpga::connect(easycore_ptr sourceCore, PinConst sourceCorePin, PinConst sinkGpioPin)
{
    /* PARAMETER CHECK */
    if (sourceCore == nullptr) {
        Log().Get(ERROR) << "The pointer to the easyCore is invalid!";
        exit(-1);
    }

    if (_easyCoreMap->find(sourceCore->getIndex()) == _easyCoreMap->end()) {
        Log().Get(ERROR) << "The easyCore currently doesn't belong to this easyFPGA! Aborting... Did you already added this core?";
        exit(-1);
    }

    pin_ptr pin = sourceCore->getPin(sourceCorePin);
    gpiopin_ptr gpio = this->getGpioPin(sinkGpioPin);

    if ((pin == nullptr) || (gpio == nullptr)) {
        Log().Get(ERROR) << "At least one PinConst is faulty!";
        exit(-1);
    }

    if (pin->hasType(PIN_DIRECTION_TYPE::UNDEFINED)) {
        Log().Get(ERROR) << "An internal error occured! An easyCore implementation will be wrong initialized in the constructor...";
        exit(-1);
    }

    /* PERFORM A CONNECT IF POSSIBLE */
    if (gpio->hasType(PIN_DIRECTION_TYPE::UNDEFINED)) {
        if (pin->hasType(PIN_DIRECTION_TYPE::INOUT)) {
            Log().Get(DEBUG) << "Pin " << gpio->getLogName() << " gets the direction 'INOUT'.";
            gpio->setType(PIN_DIRECTION_TYPE::INOUT);
        }
        else {
            Log().Get(DEBUG) << "Pin " << gpio->getLogName() << " gets the direction 'OUT'.";
            gpio->setType(PIN_DIRECTION_TYPE::OUT);
        }
        return gpio->connectTo(pin);
    }
    else {
        if (pin->hasType(PIN_DIRECTION_TYPE::IN)) {
            return pin->connectTo(gpio);
        }
        else {
            return gpio->connectTo(pin);
        }
    }
}

bool EasyFpga::connect(PinConst sourceGpioPin, easycore_ptr sinkCore, PinConst sinkCorePin)
{
    /* PARAMETER CHECK */
    if (sinkCore == nullptr) {
        Log().Get(ERROR) << "The pointer to the easyCore is invalid!";
        exit(-1);
    }

    if (_easyCoreMap->find(sinkCore->getIndex()) == _easyCoreMap->end()) {
        Log().Get(ERROR) << "The easyCore currently doesn't belong to this easyFPGA! Aborting... Did you already added this core?";
        exit(-1);
    }

    pin_ptr pin = sinkCore->getPin(sinkCorePin);
    gpiopin_ptr gpio = this->getGpioPin(sourceGpioPin);

    if ((pin == nullptr) || (gpio == nullptr)) {
        Log().Get(ERROR) << "At least one PinConst is faulty!";
        exit(-1);
    }

    if (pin->hasType(PIN_DIRECTION_TYPE::UNDEFINED)) {
        Log().Get(ERROR) << "An internal error occured! An easyCore implementation will be wrong initialized in the constructor...";
        exit(-1);
    }

    /* PERFORM A CONNECT IF POSSIBLE */
    if (gpio->hasType(PIN_DIRECTION_TYPE::UNDEFINED)) {
        if (pin->hasType(PIN_DIRECTION_TYPE::INOUT)) {
            Log().Get(DEBUG) << "Pin " << gpio->getLogName() << " gets the direction 'INOUT'.";
            gpio->setType(PIN_DIRECTION_TYPE::INOUT);
        }
        else {
            Log().Get(DEBUG) << "Pin " << gpio->getLogName() << " gets the direction 'IN'.";
            gpio->setType(PIN_DIRECTION_TYPE::IN);
        }
        return gpio->connectTo(pin);
    }
    else {
        if (pin->hasType(PIN_DIRECTION_TYPE::IN)) {
            return pin->connectTo(gpio);
        }
        else {
            return gpio->connectTo(pin);
        }
    }
}

bool EasyFpga::connect(easycore_ptr sourceCore, PinConst sourceCorePin, easycore_ptr sinkCore, PinConst sinkCorePin)
{
    /* PARAMETER CHECK */
    if ((sourceCore == nullptr) || (sinkCore == nullptr)) {
        Log().Get(ERROR) << "At least one easyCore pointer is invalid!";
        exit(-1);
    }

    if ((_easyCoreMap->find(sourceCore->getIndex()) == _easyCoreMap->end()) || (_easyCoreMap->find(sinkCore->getIndex()) == _easyCoreMap->end())) {
        Log().Get(ERROR) << "At least one easyCore doesn't belong to this easyFPGA! Aborting... Do you already added this core?";
        exit(-1);
    }

    pin_ptr pin1 = sourceCore->getPin(sourceCorePin);
    pin_ptr pin2 = sinkCore->getPin(sinkCorePin);

    if ((pin1 == nullptr) || (pin2 == nullptr)) {
        Log().Get(ERROR) << "At least one PinConst is faulty!";
        exit(-1);
    }

    if (pin1->hasType(PIN_DIRECTION_TYPE::INOUT) || pin2->hasType(PIN_DIRECTION_TYPE::INOUT)) {
        Log().Get(ERROR) << "You try to connect at least one INOUT pin (" << pin1->getLogName() << " and/or " << pin2->getLogName() << ") internally! Please make sure, that all INOUT pins in your hardware description are connected to gpio pins only...";
        exit(-1);
    }

    if (!pin1->hasType(PIN_DIRECTION_TYPE::OUT)) {
        Log().Get(ERROR) << "Pin " << pin1->getLogName() << " isn't a source. Correct your binary description please...";
        exit(-1);
    }

    if (!pin2->hasType(PIN_DIRECTION_TYPE::IN)) {
        Log().Get(ERROR) << "Pin " << pin2->getLogName() << " isn't a sink. Correct your binary description please...";
        exit(-1);
    }

    /* PERFORM A CONNECT IF POSSIBLE */
    return pin2->connectTo(pin1);
}

gpiopin_ptr EasyFpga::getGpioPin(PinConst pin)
{
    assert((0 <= pin) && (pin < BANK_COUNT*PIN_COUNT));

    auto it = _gpioPinMap.find(pin);

    if (it != _gpioPinMap.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

std::list<gpiopin_ptr> EasyFpga::getGpioPins(void)
{
    std::list<gpiopin_ptr> ret;

    for (auto it=_gpioPinMap.begin(); it!=_gpioPinMap.end(); ++it) {
        ret.push_back(it->second);
    }

    return ret;
}

std::list<easycore_ptr> EasyFpga::getEasyCores(void)
{
    std::list<easycore_ptr> ret;

    for (auto it=_easyCoreMap->begin(); it!=_easyCoreMap->end(); ++it) {
        ret.push_back(it->second);
    }

    return ret;
}

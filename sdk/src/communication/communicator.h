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

#ifndef SDK_COMMUNICATION_COMMUNICATOR_H_
#define SDK_COMMUNICATION_COMMUNICATOR_H_

#include "communication/types.h"
#include "communication/serialconnection_ptr.h"
#include "communication/taskexecutor_ptr.h"
#include "easycore_map_ptr.h"
#include "easycores/callback_ptr.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

#include <utility> /* pair<2> */
#include <string>

/**
 * \brief Provides an interface and implements the entire high level
 *         communication between framework and easyFPGA board.
 *
 * All operations are offered in a synchronous and asynchronous way.
 * Asynchronous operations were introduced for increasing the throughput
 * on the serial line. By default, all easyCore operations are
 * asynchronous.
 *
 * Note: The communication between the framework an the mcu will mainly
 * be synchronous for simplicity. Mcu communication only takes place
 * during fpga binary upload.
 *
 * Below the communicator's high level operations, the SerialConnection
 * class manages to send and receive raw bytes. SerialConnection acts
 * as a virtual com port (known as vcp; an interface to a usb port
 * offered by the underlying operating system).
 *
 * How the communicator works in a nutshell:<br>
 * The Communicator instantiates a TaskExecutor which actually handles
 * the requested operations. For every request the Communicator creates
 * the desired SyncTask or AsyncTask and hands it over to the
 * TaskExecutor. The TaskExecutor takes care about starting the task
 * and handling communication errors and retries.
 */
class Communicator
{
    public:
        /**
         * \brief Creates a Communicator instance.
         *
         * \param cores A pointer to the EasyFpga's map of easyCores, or<br>
         *         NULL if no interrupts should be served.
         */
        Communicator(easycore_map_ptr cores);
        ~Communicator();

        /**
         * \brief Connect the Communicator to an easyFPGA hardware device.
         *
         * \param serial If serial is not equal to zero, this method will
         *         only connect to an easyFPGA with this serial.<br>
         *         Otherwise if serial equals zero, this method will
         *         connect to any easyFPGA.
         *
         * \return true if an connected could be established,\n
         *         false otherwise
         */
        bool init(uint32_t serial);

        /**
         * \brief Overrides the serial stored in the easyFPGA.
         *
         * \param serial The serial to be written.
         *
         * \return true if the serial was successfully written,\n
         *         false otherwise
         */
        bool writeSerial(uint32_t serial);

        /**
         * \brief Gets the serial stored in the easyFPGA.
         *
         * \param serial Points to an int, where the requested serial have to be written.
         *
         * \return true if the serial could be read successful,\n
         *         false otherwise
         */
        bool readSerial(uint32_t* serial);

        /**
         * \brief Lets the mcu configuring the fpga with the beforehand uploaded binary.
         *
         * \return true if the configuration process was successful\n
         *         false otherwise
         */
        bool configureFpga(void);

        /**
         * \brief Uploads a binary to a non volatile memory of the mcu.
         *
         * \param binary Points to the beginning of an byte array which contains the whole binary.
         * \param binaryLength Determines the length of the binary.
         *
         * \return true if the upload process was successful,\n
         *         false otherwise
         */
        bool writeBinary(byte* binary, uint64_t binaryLength);

        /**
         * \brief Writes a new status to the easyFPGA.
         *
         * \param socIsUploaded True tells the mcu that the framework could write all sectors.
         * \param binarySize Contains the length of the binary.
         * \param adler32hash Contains the adler32 hash of the binary.
         *
         * \return true if the status writing was successful,\n
         *         false otherwise
         */
        bool writeStatus(bool socIsUploaded, uint32_t binarySize, uint32_t adler32hash);

        /**
         * \brief Reads the status of the easyFPGA.
         *
         * \param isFpgaConfigured Location of this pointer contains after method execution true if the fpga is configured with the stored binary.
         * \param adler32hash Location of this pointer contains after method execution the adler32 hash of the stored binary.
         *
         * \return true if the status reading was successful,\n
         *         false otherwise
         */
        bool readStatus(bool* isFpgaConfigured, uint32_t* adler32hash);

        /* SYNC */
        bool readRegister(byte* reply, byte core, byte registerAddress);
        bool readMultiRegister(byte* reply, byte core, byte registerAddress, uint8_t length);
        bool readAutoAdressIncrementRegister(byte* reply, byte core, byte registerAddress, uint8_t length);

        bool writeRegister(byte data, byte core, byte registerAddress);
        bool writeMultiRegister(byte* data, byte core, byte registerAddress, uint8_t length);
        bool writeAutoAdressIncrementRegister(byte* data, byte core, byte registerAddress, uint8_t length);

        bool enableGlobalInterrupts(void);

        /* ASYNC */
        bool readRegisterAsync(byte* reply, byte core, byte registerAddress, tasknumberval dep);
        tasknumberval readRegisterAsync(byte* reply, byte core, byte registerAddress, callback_ptr callback, tasknumberval dep);
        bool readMultiRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, tasknumberval dep);
        tasknumberval readMultiRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep);
        bool readAutoAdressIncrementRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, tasknumberval dep);
        tasknumberval readAutoAdressIncrementRegisterAsync(byte* reply, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep);

        bool writeRegisterAsync(byte data, byte core, byte registerAddress, tasknumberval dep);
        tasknumberval writeRegisterAsync(byte data, byte core, byte registerAddress, callback_ptr callback, tasknumberval dep);
        bool writeMultiRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, tasknumberval dep);
        tasknumberval writeMultiRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep);
        bool writeAutoAdressIncrementRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, tasknumberval dep);
        tasknumberval writeAutoAdressIncrementRegisterAsync(byte* data, byte core, byte registerAddress, uint8_t length, callback_ptr callback, tasknumberval dep);

        bool enableGlobalInterruptsAsync(void);

        bool handleRequestReplies(void);
        uint32_t getNumberOfPendingAsyncRequests(void);

    private:
        /**
         * Define all possible communicating states of an easyFPGA.
         */
        enum COM_TARGET : int32_t {
            UNDEFINED = -1,
            MCU = 0,
            MCU_CONF = 1,
            SOC = 2
        };

        /**
         * Remembers the current state whether the framework talks to the
         * mcu or the soc. In addition, remembering the state reduces the
         * communication overhead.
         *
         * At the beginning the communicator is in state UNDEFINED. By
         * connection to an easyFPGA device, method init() resolves the
         * current fpga state. Every time an operation will be executed
         * this variable have to checked. If the communication context
         * not matches to the needed context of the operation, a context
         * switch will be neccessary. This is done by calling switchTo().
         */
        COM_TARGET _target;

        /**
         * \brief Pointer to the high level serial interface (known as
         *        virtual com port).
         *
         * One SerialConnection belongs to one communicator and will be
         * constructed here therefore.
         */
        serialconnection_ptr _connection;

        /**
         * \brief Pointer the execution unit of the requested tasks.
         *
         * One TaskExecutor belongs to one communicator and will be
         * constructed here therefore.
         */
        taskexecutor_ptr _executor;

        /**
         * \brief A helper function for switching to the desired context.
         *        This inline function reduces the lines of code heavily.
         *
         * \param target The context to be switched. Possible values are
         *        COM_TARGET::MCU or COM_TARGET::SOC. All other values
         *        will be rejected.
         *
         * \return true if the context switch was successful,\n
         *         false otherwise (Especially if there will be used other
         *         params as the 2 mentioned above!)
         */
        bool switchTo(COM_TARGET target);

        /**
         * \brief Finds any easyFPGA or an easyFPGA with an special serial
         *        number (indicated by serialNumber) connected to the host
         *        system.
         *
         * If the serialNumber equals 0 this method returns the first
         * easyFPGA found regardless it's serial number. Otherwise, if the
         * given serial number is higher than 0, only an easyFPGA will be
         * returned by this method with has exactly stored the given
         * serial number.
         *
         * \param serialNumber Possible values are all integers between
         *        0 and UINT32_MAX (specified in <cstdint>).
         *
         * \return A pair containing a decive path string together with
         *         the currently communication context of the easyFPGA or\n
         *         an empty pair if no serial device matches the typical
         *         behavior of an easyFPGA.
         */
        inline std::pair<std::string, COM_TARGET> findFirstMatchingEasyFpga(uint32_t serialNumber);

        /**
         * \brief Checks whether the currently connected serial device
         *        behaves like an easyFPGA and indicates in which mode
         *        it runs.
         *
         * This method implements the DETECT PROCEDURE decribed in the
         * documentation. For accessing the serial port, it uses an
         * internal pointer of SerialConnection (which was instantiated
         * in the constructor).
         *
         * \return COM_TARGET::MCU if an easyFPGA was recognized and the
         *         mcu replies responses,\n
         *         COM_TARGET::SOC if an easyFPGA was recognized and the
         *         fpga replies responses,\n
         *         COM_TARGET::UNDEFINED otherwise
         */
        inline COM_TARGET testDeviceResponseBehavior();

        /*
         * Two constants which will be instantiated in the constructor
         * by parsing the configuration file.
         */
        const std::string _USB_DEVICE_PATH;
        const std::string _USB_DEVICE_IDENTIFIER;
};

#endif  // SDK_COMMUNICATION_COMMUNICATOR_H_

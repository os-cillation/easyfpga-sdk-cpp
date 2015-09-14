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

#ifndef SDK_EASYFPGA_H_
#define SDK_EASYFPGA_H_

#include "communication/communicator_ptr.h"
#include "easycore_map_ptr.h"
#include "easycores/easycore_ptr.h"
#include "easycores/gpiopin_ptr.h"
#include "easycores/pin_ptr.h"
#include "easycores/types.h"
#include "utils/idmanager_fwd.h"

#include <list>
#include <map>
#include <string>
#include <memory>

/**
 * \brief The framework's interface to a physical connected
 *        easyFPGA board.
 *
 * The main usage purpose of this class is to:
 * - describe the structure of an user defined FPGA by inheriting an own
 *   class from this one and implementing the abstract member
 *   EasyFpga::defineStructure(),
 * - connect this application to an physical connected easyFPGA board,
 * - upload an generated binary to the easyFPGA board, and
 * - do some core across communication steps with the connected
 *   FPGA, e.g. enabling a global interrupt generation for all cores.
 *   See members EasyFpga::handleReplies() or EasyFpga::enableInterrupts().
 *
 * Important for the FPGA's structure definition are the member
 * EasyFpga::addEasyCore() and the overloaded EasyFpga::connect() member.
 *
 * At runtime, you have to connect to your easyFPGA board first and
 * upload the fpga binary by using EasyFpga::init(). Thereafter you can
 * use the functions of the easyCore API.
 *
 * Note: For some example, please take a look to the test case
 * implementations of this project.
 */
class EasyFpga : public std::enable_shared_from_this<EasyFpga>
{
    public:
        EasyFpga();
        ~EasyFpga();

        /* FOR COMMUNICATION */
        /**
         * \brief Connects to a physical connected easyFPGA board and
         *        initializes its FPGA with the specified binary.
         *
         * This method is equivalent to the execution of:
         * -# Generator::generateBinary(), <- This method will only
         *    generate a binary if no binary found under the specified
         *    path.
         * -# EasyFpga::connectHardwareDevice(),
         * -# EasyFpga::uploadBinaryFile() and
         * -# EasyFpga::instantiateCores().
         *
         * If this method fails (returns false), you will find some
         * useful hints within the log or the log file.
         *
         * \param serialNumber Specifies to which physical connected
         *        board the framework connects.
         *        - 0: Connects to the first board found.
         *        - >0: Connects only to a board which offers this
         *          specified serial number.
         *
         * \param pathToBinary A path to the binary file to be installed
         *        at the fpga (possible are relative and absolute paths).
         *
         * \return true if the complete initialization process was
         *         successful for the specified parameters,<br>
         *         false otherwise (e.g. no board with this serial found
         *         or the binary could not be uploaded)
         */
        bool init(uint32_t serialNumber, std::string pathToBinary);

        /**
         * \brief Returns a reference to the high level communicator.
         *
         * <b>Important:</b> Handle with care. Using the communicator
         * directly may interfer with ongoing communication conducted by
         * an easyCore. Please use only the wrapper members of this
         * class and thier respective easyCore APIs!
         * <br>This function should only be used by experienced users
         * and will be only useful for:
         * - getting some information of the underlying communication
         *   structure, e.g. How many async request are started since
         *   last call of EasyFpga::handleReplies(), or
         * - if the user doesn't want to use a special framework API
         *   method and implement this one by himself in his application.
         */
        communicator_ptr getCommunicator(void);

        /**
         * \brief Connects the framework to a physical connected easyFPGA
         *        board specified by the given board serial number.
         *
         * \param serialNumber Specifies to which physical connected board
         *        the framework connects.
         *        - 0: Connects to the first board found.
         *        - >0: Connects only to a board which stores this special
         *          serial number.
         *
         * \return true if the initialization process was successful,<br>
         *         false otherwise (e.g. no board with this serial found)
         */
        bool connectHardwareDevice(uint32_t serialNumber);

        /**
         * \brief Uploads the given binary if neccessary.
         *
         * \param pathToBinary The binary file to be installed
         *        at the fpga.
         *
         * \return true if the binary upload was successful or not
         *         neccessary,<br>
         *         false otherwise (e.g. the binary file not exists or
         *         errors occured during upload)
         */
        bool uploadBinaryFile(std::string pathToBinary);

        /**
         * \brief Synchronizes the FPGA's state by handling all yet
         *        dispatched asynchronous requests and sent interrupt
         *        notifications from the board.
         *
         * Calling this method is useful for:
         * - getting the results of asynchronous exchanges by writing
         *   them into the user allocated memory,
         * - recognizing interrupts notifications, and
         * - to make sure, the FPGA has the desired state in an
         *   asynchronous operation mode.
         *
         * If a receive attempt of one operation fails, this method
         * retries the completely operation a fixed amount. (This will
         * include a new send attempt. How often an operation will be
         * repeated can be set over the option \c MAX_RETRIES_ALLOWED in
         * \a project.conf.)
         *
         * \return true if all replies ot the requests could be received
         *         successfully and no non-recoverable errors occured in
         *         the meanwhile,<br>
         *         false otherwise
         */
        bool handleReplies(void);

        /**
         * \brief Enables globally the interrupt generation of all cores.
         *
         * <b>Important note:</b> Interrupts will be globally disabled
         * by the hardware after triggering one. Hence, every interrupt
         * service routine should call this method to avoid blocking
         * further interrupts of the same or other cores!
         *
         * \return true if interrupt enable was successful,<br>
         *         false otherwise (i.e. the mcu is currently selected)
         */
        bool enableInterrupts(void);

        /* FOR BINARY GENERATION */
        /**
         * \brief Defines the available GPIO pins of this easyFPGA board.
         */
        enum GPIOPIN : PinConst {
            BANK0_PIN0 = 0,
            BANK0_PIN1 = 1,
            BANK0_PIN2 = 2,
            BANK0_PIN3 = 3,
            BANK0_PIN4 = 4,
            BANK0_PIN5 = 5,
            BANK0_PIN6 = 6,
            BANK0_PIN7 = 7,
            BANK0_PIN8 = 8,
            BANK0_PIN9 = 9,
            BANK0_PIN10 = 10,
            BANK0_PIN11 = 11,
            BANK0_PIN12 = 12,
            BANK0_PIN13 = 13,
            BANK0_PIN14 = 14,
            BANK0_PIN15 = 15,
            BANK0_PIN16 = 16,
            BANK0_PIN17 = 17,
            BANK0_PIN18 = 18,
            BANK0_PIN19 = 19,
            BANK0_PIN20 = 20,
            BANK0_PIN21 = 21,
            BANK0_PIN22 = 22,
            BANK0_PIN23 = 23,

            BANK1_PIN0 = 24,
            BANK1_PIN1 = 25,
            BANK1_PIN2 = 26,
            BANK1_PIN3 = 27,
            BANK1_PIN4 = 28,
            BANK1_PIN5 = 29,
            BANK1_PIN6 = 30,
            BANK1_PIN7 = 31,
            BANK1_PIN8 = 32,
            BANK1_PIN9 = 33,
            BANK1_PIN10 = 34,
            BANK1_PIN11 = 35,
            BANK1_PIN12 = 36,
            BANK1_PIN13 = 37,
            BANK1_PIN14 = 38,
            BANK1_PIN15 = 39,
            BANK1_PIN16 = 40,
            BANK1_PIN17 = 41,
            BANK1_PIN18 = 42,
            BANK1_PIN19 = 43,
            BANK1_PIN20 = 44,
            BANK1_PIN21 = 45,
            BANK1_PIN22 = 46,
            BANK1_PIN23 = 47,

            BANK2_PIN0 = 48,
            BANK2_PIN1 = 49,
            BANK2_PIN2 = 50,
            BANK2_PIN3 = 51,
            BANK2_PIN4 = 52,
            BANK2_PIN5 = 53,
            BANK2_PIN6 = 54,
            BANK2_PIN7 = 55,
            BANK2_PIN8 = 56,
            BANK2_PIN9 = 57,
            BANK2_PIN10 = 58,
            BANK2_PIN11 = 59,
            BANK2_PIN12 = 60,
            BANK2_PIN13 = 61,
            BANK2_PIN14 = 62,
            BANK2_PIN15 = 63,
            BANK2_PIN16 = 64,
            BANK2_PIN17 = 65,
            BANK2_PIN18 = 66,
            BANK2_PIN19 = 67,
            BANK2_PIN20 = 68,
            BANK2_PIN21 = 69,
            BANK2_PIN22 = 70,
            BANK2_PIN23 = 71
        };

        /**
         * \brief User implementable. Should describe the structure of
         *        the easyFPGA.
         */
        virtual void defineStructure(void) = 0;

        /**
         * \brief This mehtod instantiate the used easyCores. (By meaning
         *        of giving them an CoreIndex and add them to the EasyFPGA
         *        instance.)
         */
        void instantiateCores(void);

        /**
         * \brief Adds an instance of EasyCore to this easyFPGA.
         *
         * \param core A pointer to an instance of EasyCore
         *
         * \return true if the could be successfully added,<br>
         *         false otherwise (e.g. the core already belongs to
         *         another easyFPGA)
         */
        bool addEasyCore(easycore_ptr core);

        /**
         * \brief Removes an EasyCore from this easyFPGA.
         *
         * \param coreToRemove A pointer to an instance of EasyCore
         *
         * \return true if the removal was successful,<br>
         *         false otherwise (e.g. the core never added before)
         */
        bool removeEasyCore(easycore_ptr coreToRemove);

        /**
         * \brief Connects two GPIO pins.
         *
         * \return true if all pin connecting constraints are
         *         satisfied, or<br>
         *         false otherwise
         */
        bool connect(PinConst sourceGpioPin, PinConst sinkGpioPin);

        /**
         * \brief Connects an easyCore's pin to a GPIO pin.
         *
         * \return true if all pin connecting constraints are
         *         satisfied, or<br>
         *         false otherwise
         */
        bool connect(easycore_ptr sourceCore, PinConst sourceCorePin, PinConst sinkGpioPin);

        /**
         * \brief Connects an easyCore's pin to a GPIO pin.
         *
         * \return true if all pin connecting constraints are
         *         satisfied, or<br>
         *         false otherwise
         */
        bool connect(PinConst sourceGpioPin, easycore_ptr sinkCore, PinConst sinkCorePin);

        /**
         * \brief Connects two easyCore pins.
         *
         * \return true if all pin connecting constraints are
         *         satisfied, or<br>
         *         false otherwise
         */
        bool connect(easycore_ptr sourceCore, PinConst sourceCorePin, easycore_ptr sinkCore, PinConst sinkCorePin);

        /**
         * \brief Returns a GPIO pin instance specified by an constant.
         */
        gpiopin_ptr getGpioPin(PinConst pin);

        /**
         * \brief Returns a list of all currently available GPIO pins.
         */
        std::list<gpiopin_ptr> getGpioPins(void);

        /**
         * \brief Returns a list of the currently to this easyFPGA
         *        belonging easyCores.
         */
        std::list<easycore_ptr> getEasyCores(void);

    protected:
        /**
         * \brief Holds the framework's operation mode currently set.
         *
         * Constant which will be instantiated in the constructor
         * by parsing the configuration file, thus the user should not
         * set this member manually.
         *
         * @see OPERATION_MODE
         */
        const OPERATION_MODE _OPERATION_MODE;

    private:
        CoreIndex _gpioCoreIndex;

        /* Range: 1 .. 255; 0 is reserved for identifying gpio pins */
        IdManager<CoreIndex>* _freeCoreIndices;

        std::map<PinConst, gpiopin_ptr> _gpioPinMap;

        easycore_map_ptr _easyCoreMap;

        communicator_ptr _com;

        bool _alreadyInstantiatedCores;
};

/*
 * Defining a macro which inserts a main function to a fpga decription
 * class for executing the binary generator if the compiler defined the
 * BINARY_GENERATION_PROCESS symbol.
 */
#ifdef BINARY_GENERATION_PROCESS

#include "generator/generator.h"

#define EASY_FPGA_DESCRIPTION_CLASS(OWN_CLASS) \
int main(int argc, char** argv) \
{ \
    if (argc == 2) { \
        Generator g; \
        if (g.generateBinary(#OWN_CLASS, new OWN_CLASS(), argv[1])) { \
            return 0; \
        } \
    } \
    return -1; \
}

#else

#define EASY_FPGA_DESCRIPTION_CLASS(OWN_CLASS)

#endif  // BINARY_GENERATION_PROCESS

#endif  // SDK_EASYFPGA_H_

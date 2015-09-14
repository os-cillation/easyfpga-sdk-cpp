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

#ifndef SDK_EASYCORES_REGISTER_H_
#define SDK_EASYCORES_REGISTER_H_

#include "communication/types.h"
#include "easycores/callback_ptr.h"
#include "easycores/easycore_fwd.h"
#include "easycores/easycore_ptr.h"
#include "easycores/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief Represents an accessible register belonging to an easyCore.
 */
class Register
{
    public:
        Register(EasyCore* core, byte address, REGISTER_ACCESS_TYPE type);
        ~Register();

        /**
         * \brief Reads the register one time.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool readSync(byte* target);

        /**
         * \brief Reads the register multiple times.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool readMultiTimesSync(byte* target, uint8_t number);

        /**
         * \brief Reads this register and the next (number-1) registers
         *        in the hardware's register array.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool readAutoAddressIncrementSync(byte* target, uint8_t number);

        /**
         * \brief Writes the register one time.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool writeSync(byte content);

        /**
         * \brief Sets or clears a specified bit in the register.
         *
         * This method executes and read-modify-write operation on this
         * register.
         *
         * \param bitPosition Specifies the bit to be set in the register.
         *        Values in the range [0, 7] possible.
         *
         * \param set Specifies whether the bit have to set or cleared.
         *        - true: The bit will be set.
         *        - false: The bit will be cleared.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool changeBitSync(uint8_t bitPosition, bool set);

        /**
         * \brief Writes the register multiple times.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool writeMultiTimesSync(byte* content, uint8_t number);

        /**
         * \brief Writes this register and the next (number-1) registers
         *        in the hardware's register array.
         *
         * \return true if the request could be completed successfully
         *         and a valid answer is available,<br>
         *         false otherwise
         */
        bool writeAutoAddressIncrementSync(byte* content, uint8_t number);

        /**
         * \brief Reads the register one time asynchronous.
         *
         * \param target Byte pointer which contains the answer after call
         *        of handleRequestReplies().
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool readAsync(byte* target);

        /**
         * \brief Reads the register one time asynchronous.
         *
         * \param target Byte pointer which contains the answer after call
         *        of handleRequestReplies().
         *
         * \param callback Pointer to an callback object for executing
         *        additional logic after the answer to this request arrives.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool readAsync(byte* target, callback_ptr callback);

        /**
         * \brief Reads the register multiple times asynchronous.
         *
         * \param target Byte pointer which contains the answer after call
         *        of handleRequestReplies().
         *
         * \param number How often the register should be read.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool readMultiTimesAsync(byte* target, uint8_t number);

        /**
         * \brief Reads the register multiple times asynchronous.
         *
         * \param target Byte pointer which contains the answer after call
         *        of handleRequestReplies().
         *
         * \param number How often the register should be read.
         *
         * \param callback Pointer to an callback object for executing
         *        additional logic after the answer to this request arrives.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool readMultiTimesAsync(byte* target, uint8_t number, callback_ptr callback);

        /**
         * \brief Reads this register and the next (number-1) registers
         *        in the hardware's register array asynchronous.
         *
         * \param target Byte pointer which contains the answer after call
         *        of handleRequestReplies().
         *
         * \param number How often the register should be read.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool readAutoAddressIncrementAsync(byte* target, uint8_t number);

        /**
         * \brief Reads this register and the next (number-1) registers
         *        in the hardware's register array asynchronous.
         *
         * \param target Byte pointer which contains the answer after call
         *        of handleRequestReplies().
         *
         * \param number How often the register should be read.
         *
         * \param callback Pointer to an callback object for executing
         *        additional logic after the answer to this request arrives.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool readAutoAddressIncrementAsync(byte* target, uint8_t number, callback_ptr callback);

        /**
         * \brief Writes the register one time asynchronous.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool writeAsync(byte content);

        /**
         * \brief Writes the register one time asynchronous.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool writeAsync(byte content, callback_ptr callback);

        /**
         * \brief Writes the register multiple times asynchronous.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool writeMultiTimesAsync(byte* content, uint8_t number);

        /**
         * \brief Writes the register multiple times asynchronous.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool writeMultiTimesAsync(byte* content, uint8_t number, callback_ptr callback);

        /**
         * \brief Writes this register and the next (number-1) registers
         *        in the hardware's register array asynchronous.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool writeAutoAddressIncrementAsync(byte* content, uint8_t number);

        /**
         * \brief Writes this register and the next (number-1) registers
         *        in the hardware's register array asynchronous.
         *
         * \return true if the request could be successfully sent to the
         *         easyFPGA board (not more!),<br>
         *         false otherwise
         */
        bool writeAutoAddressIncrementAsync(byte* content, uint8_t number, callback_ptr callback);

    protected:
        /**
         * \brief Stores a reference to the parental easyCore.
         */
        EasyCore* _core;

        /**
         * \brief Holds the register offset of the hardware's register
         *        array.
         */
        byte _address;

        /**
         * \brief Stores how the framework must access this register.
         */
        REGISTER_ACCESS_TYPE _type;

        /**
         * \brief Stores the globally unique number of the last operation
         *        accesses this register.
         *
         * Important for resolving access dependencies at asynchronous
         * communication. Every write access have to update this member.
         */
        tasknumberval _dependency;
};

#endif  // SDK_EASYCORES_REGISTER_H_

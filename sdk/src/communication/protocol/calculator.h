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

#ifndef SDK_COMMUNICATOR_PROTOCOL_CALCULATOR_H_
#define SDK_COMMUNICATOR_PROTOCOL_CALCULATOR_H_

#include "communication/types.h"
#include "utils/hardwaretypes.h"

/**
 * \brief Calculates parity bytes and hash sums
 */
class Calculator
{
    public:
        /**
         * \brief Calculates a parity byte by XORing all data bytes
         *
         * \param byteArray location of the data in memory
         * \param byteArrayLength length of the data (a byte count)
         */
        static byte calculateXorParity(byte* byteArray, uint32_t byteArrayLength);

        /**
         * \brief Calculates a hash sum with the Adler32 algorithm
         *
         * Implementation of this method copied from
         * http://en.wikipedia.org/wiki/Adler-32
         * on 2014-12-01 at 10:50 am. Look there for further information.
         *
         * \param byteArray location of the data in memory
         * \param byteArrayLength length of the data (a byte count)
         */
        static uint32_t calculateAdler32Hash(byte* byteArray, uint32_t byteArrayLength);

    private:
        static const int32_t _MOD_ADLER = 65521;
};

#endif  // SDK_COMMUNICATOR_PROTOCOL_CHECKSUMCALCULATOR_H_

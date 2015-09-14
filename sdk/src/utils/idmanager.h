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

#ifndef SDK_UTILS_IDMANAGER_H_
#define SDK_UTILS_IDMANAGER_H_

#include "configuration.h" /* assert() */

#include <cstdint> /* special ints and there min- and max-macros */
#include <stack>

/**
 * \brief Id storage for all values within range [1, 255].
 *
 * The id 0 will be never used. In all use cases of this framework the
 * zero-id is reserved for special circumstances. Thats why there are
 * maximum 255 different values available.
 *
 * The IdManager is used by the Communicator for identifying frames at
 * asynchronous communication. If a frame is sent asynchronously to the
 * easyFPGA, the Communicator requests this class for a new ID. After
 * getting the results the Communicator releases the requested id for
 * the next operation. Please note, that the user can turning off /
 * switching on the usage of ids by the communicator through an option
 * in the config file.
 *
 * The EasyFpga uses this class too for assigning core indices to easyCores.
 * Here are as well maximum 255 different easyCores possible.
 */
template <typename T>
class IdManager
{
    private:
        /* a 8-bit id of range 0x01 to 0xFF */
        std::stack<T> _freeIds;

    public:
        /**
         * \brief Creates an id store with 255 different 8-bit wide int values.
         */
        IdManager()
        {
            assert(_freeIds.size() == 0);
            for (int32_t i=UINT8_MAX; i>=1; i--) {
                _freeIds.push(i);
            }
            assert(_freeIds.size() == UINT8_MAX);
        }

        /**
         * \brief Return an unique id into the user defined memory.
         *
         * \param targetId target pointer where the id have to be written
         *
         * \return true at success,<br>
         *         false if id storage was empty
         */
        bool getFreeId(T* targetId)
        {
            assert(!_freeIds.empty());

            if (_freeIds.empty()) {
                return false;
            }
            else {
                *(targetId) = _freeIds.top();
                _freeIds.pop();
                return true;
            }
        }

        /**
         * \brief Puts a beforehand returned id back into id storage.
         *
         * \param id int to release
         *
         * \return true at success,<br>
         *         false if id storage was already full
         */
        bool releaseId(T id)
        {
            assert(_freeIds.size() < UINT8_MAX);

            if (_freeIds.size() < UINT8_MAX) {
                _freeIds.push(id);
                return true;
            }
            else {
                return false;
            }
        }
};

#endif // SDK_UTILS_IDMANAGER_H_

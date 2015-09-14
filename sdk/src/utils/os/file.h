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

#ifndef SDK_UTILS_OS_FILE_H_
#define SDK_UTILS_OS_FILE_H_

#include "utils/hardwaretypes.h"

#include <fstream>
#include <string>

/**
 * \brief Helper class for high level file functions
 */
class File
{
    public:
        /**
         * \brief Create a file helper object.
         *
         * \param fileName A relative or absolute path to a file or just
         *        a file name of the current working directory.
         *
         */
        File(std::string fileName);
        ~File();

        /**
         * \brief Returns the the canonicalized absolute pathname of
         *        this file.
         *
         * \return true if the absolute pathname could be determined, or<br>
         *         false otherwise
         */
        bool getTheAbsolutePath(std::string& resolvedPath);

        /**
         * \brief Returns the file's name only without the directory.
         */
        std::string getName(void);

        std::string getNameWithoutEnding(void);

        /**
         * \brief Returns the file's name for logging purposes.
         */
        std::string getLogName(void);

        /**
         * \brief Checks if this file exists or not.
         *
         * \return true if this file exists,<br>
         *         false otherwise
         */
        bool exists(void);

        /**
         * \brief Determines the file size in bytes.
         *
         * \param size Location where to write the file's size
         *
         * \return true if the file size could be determined,<br>
         *         false otherwise
         */
        bool getSize(uint64_t* size);

        /**
         * \brief Read the file's content. <b>binary mode</b>
         *
         * \param targetBuffer Location where to write the file's content
         *
         * \return true if the file could read correctly,<br>
         *         false otherwise
         */
        bool writeIntoByteBuffer(byte* targetBuffer);

        /**
         * \brief Read the file's content. <b>ascii mode</b>
         *
         * \param fileContent Reference to an string to to be filled with
         *        the file's content
         *
         * \return true if the file could be correctly read,<br>
         *         false otherwise
         */
        bool writeIntoString(std::string& fileContent);

        /**
         * \brief Extracts the class name of an easyFPGA binary description
         *        file.
         *
         * \return the class name as a string if the file content meets
         *         the requirements, or<br>
         *         an empty string otherwise
         */
        std::string getAnEasyFpgaName(void);

        /**
         * \brief Searches for an specified string pattern and replaces
         *        all occurrences with the replacement string
         *
         * \param placeholder String pattern to be searched for
         *
         * \param replacement Tje replacement string
         *
         * \return true if all occurrences could be successfully overwritten, or<br>
         *         false otherwise
         */
        bool findAndReplace(std::string placeholder, std::string replacement);

        /**
         * \brief Create / Opens the file and overrides the current content.
         *
         * \param content The file's content to override
         *
         * \return the class name as a string if the file content meets
         *         the requirements, or<br>
         *         an empty string otherwise
         */
        bool createWithContent(std::string content);

    private:
        void resolveTilde(void);
        std::string _fileName;
        std::fstream* _fileStream;
};

#endif  // SDK_UTILS_OS_FILE_H_

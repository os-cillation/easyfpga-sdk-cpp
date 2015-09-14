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

#ifndef SDK_UTILS_OS_DIRECTORY_H_
#define SDK_UTILS_OS_DIRECTORY_H_

#include <string>
#include <dirent.h>
#include <list>

/**
 * \brief Helper class for high level directory functions
 */
class Directory
{
    public:
        /**
         * \brief Create a directory helper object.
         *
         * \param path A relative or absolute path to a directory or
         *        a file.
         */
        Directory(std::string path);
        ~Directory();

        /**
         * \brief Returns the the canonicalized absolute pathname of
         *        this directory.
         *
         * \return true if the absolute pathname could be determined, or<br>
         *         false otherwise
         */
        bool getTheAbsolutePath(std::string& resolvedPath);

        /**
         * \brief Returns the directory's name. (A mightly existing file
         *        name will be truncated.)
         */
        std::string getName(void);

        /**
         * \brief Returns a list of file names in the specified directory
         *        which endings equals one from a given set of endings.
         *
         * \param endings Specifies a set of endings to test.
         *
         * \return A list of file names.
         */
        std::list<std::string> getAllFileNamesWithEndings(std::list<std::string> endings);

        /**
         * \brief Deletes a subdirectory of the specified directory.
         *
         * \param dirName The name of the subdirectory
         *
         * \return true if the subdirectory could be deleted successfully,<br>
         *         false otherwise
         */
        bool removeSubDirectory(std::string dirName);

        /**
         * \brief Deletes a set of subdirectories of the specified directory.
         *
         * \param dirNames A list of subdirectory names
         *
         * \return true if all subdirectories could be deleted successfully,<br>
         *         false otherwise
         */
        bool removeSubDirectories(std::list<std::string> dirNames);

        /**
         * \brief Deletes any directory.
         *
         * \param dirName An absolute path to the directory
         *
         * \return true if the directory could be deleted successfully,<br>
         *         false otherwise
         */
        static bool remove(std::string dirName);

    private:
        void resolveTilde(void);

        std::string _dirName;

        static bool removeHelper(std::string dirName);
};

#endif  // SDK_UTILS_OS_DIRECTORY_H_

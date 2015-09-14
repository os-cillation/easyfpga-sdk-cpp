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

#include "configuration.h" /* assert(1) */
#include "utils/os/directory.h"
#include "utils/os/types.h"
#include "utils/log/log.h"

#include <regex>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pwd.h> /* getpwuid(1) */

//#include <errno.h> /* errno */
//#include <string.h> /* strerror(1) */

Directory::Directory(std::string path) :
    _dirName(path)
{
    resolveTilde();
}

Directory::~Directory()
{
}

bool Directory::getTheAbsolutePath(std::string& resolvedPath)
{
    char actualpath[PATH_MAX+1];

    if (realpath(_dirName.c_str(), actualpath) != NULL) {
        resolvedPath = actualpath;
        return true;
    }
    else {
        return false;
    }
}

std::string Directory::getName(void)
{
    struct stat fileStat;
    if (stat(_dirName.c_str(), &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            size_t found = _dirName.find_last_of("/");
            if (found != std::string::npos) {
                return _dirName.substr(0, found);
            }
            else {
                return ".";
            }
        }

        return _dirName;
    }

    size_t found = _dirName.find_last_of("/");
    if (found != std::string::npos) {
        return _dirName.substr(0, found);
    }
    else {
        return ".";
    }
}

std::list<std::string> Directory::getAllFileNamesWithEndings(std::list<std::string> endings)
{
    std::list<std::string> fileNames;

    DIR* directory = opendir(_dirName.c_str());

    assert(directory != NULL);
    if (directory == NULL) {
        //Log().Get(ERROR) << "Error while opening directory '" << _dirName << "': " << strerror(errno);
        return fileNames;
    }

    struct dirent* dirp = readdir(directory);
    while (dirp != NULL) {
        std::string fileName = std::string(dirp->d_name);

        /**
         * \todo Only test files! (readdir returns all directory entries;
         * especially other directories)
         */
        for (auto it=endings.begin(); it!=endings.end(); ++it) {
            std::stringstream ss;
            ss << ".*" << *it;
            std::regex ex(ss.str());

            if (std::regex_match(fileName, ex)) {
                fileNames.push_back(fileName);
            }
        }

        dirp = readdir(directory);
    }

    returnval success = closedir(directory);

    assert(success == 0);
    if (success != 0) {
        //Log().Get(ERROR) << "Error while closing directory '" << _dirName << "': " << strerror(errno);
        return fileNames;
    }

    return fileNames;
}

bool Directory::removeSubDirectory(std::string dirName)
{
    std::list<std::string> directoryToRemove;
    directoryToRemove.push_back(dirName);

    return this->removeSubDirectories(directoryToRemove);
}

bool Directory::removeSubDirectories(std::list<std::string> dirNames)
{
    DIR* directory = opendir(_dirName.c_str());

    assert(directory != NULL);
    if (directory == NULL) {
        //Log().Get(ERROR) << "Error while opening directory '" << _dirName << "': " << strerror(errno);
        return false;
    }

    bool everythingSuccessful = true;

    struct dirent* dirp = readdir(directory);
    while (dirp != NULL) {
        std::string dirEntry = std::string(dirp->d_name);

        struct stat dirStat;
        returnval success = stat(dirEntry.c_str(), &dirStat);

        if ((success == 0) && S_ISDIR(dirStat.st_mode)) {
            for (auto it=dirNames.begin(); it!=dirNames.end(); ++it) {
                uint32_t pos = (*it).find("*");
                if (pos != std::string::npos) {
                    if (dirEntry.find((*it).substr(0, pos)) != std::string::npos) {
                        everythingSuccessful &= Directory::removeHelper(dirEntry);
                    }
                }
                else {
                    if (dirEntry.find(*it) != std::string::npos) {
                        everythingSuccessful &= Directory::removeHelper(dirEntry);
                    }
                }
            }
        }

        dirp = readdir(directory);
    }

    returnval success = closedir(directory);

    assert(success == 0);
    if (success != 0) {
        //Log().Get(ERROR) << "Error while closing directory '" << _dirName << "': " << strerror(errno);
        return false;
    }

    return everythingSuccessful;
}

bool Directory::remove(std::string dirName)
{
    struct stat dirStat;
    returnval success = stat(dirName.c_str(), &dirStat);

    assert(success == 0);
    if (success == 0) {
        if (S_ISDIR(dirStat.st_mode)) {
            return Directory::removeHelper(dirName);
        }
    }
    else {
        //Log().Get(ERROR) << "Error while delete directory '" << dirName << "': " << strerror(errno);
    }

    return false;
}

void Directory::resolveTilde(void)
{
    size_t tildePos = _dirName.find('~');
    if (tildePos != std::string::npos) {

        /* determine homedir */
        const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        /* replace tilde */
        _dirName = std::string(homedir).append(_dirName.substr(1));
    }
}

bool Directory::removeHelper(std::string dirName)
{
    uint32_t retries = 10;

    /*
     * With the following code we can hide the output of the shell
     * command "rm".
     */
    std::stringstream ss;
    ss << "rm -r " << dirName << " 2>&1";

    uint32_t i = 0;
    returnval success = -1;
    while ((success != 0) && (i < retries)) {
        FILE* ppp = popen(ss.str().c_str(), "r");
        if (ppp != NULL) {
            success = pclose(ppp);
        }

        if (success == 0) {
            //Log().Get(DEBUG) << "Directory '" << dirName << "' could be deleted after " << i+1 << " attempts.";
            return true;
        }

        i++;

        usleep(500000);
    }

    assert(success == 0);
    //Log().Get(ERROR) << "Directory '" << dirName << "' could not be deleted after " << retries << " attempts!";

    return false;
}

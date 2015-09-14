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

#include "configuration.h" /* assert(1) */
#include "utils/os/file.h"

#include <regex> /* regex, regex_match(2) */
#include <sstream> /* stringstream */
#include <sys/stat.h> /* stat(2) */

#include <limits.h> /* PATH_MAX */
#include <stdlib.h> /* realpath(2) */

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

File::File(std::string fileName) :
    _fileName(fileName)
{
    _fileStream = new std::fstream();
    resolveTilde();
}

File::~File()
{
    delete _fileStream;
    _fileStream = NULL;
}

bool File::getTheAbsolutePath(std::string& resolvedPath)
{
    char actualpath[PATH_MAX+1];

    if (realpath(_fileName.c_str(), actualpath) != NULL) {
        resolvedPath = actualpath;
        return true;
    }
    else {
        return false;
    }
}

std::string File::getName(void)
{
    //struct stat fileStat;
    //if ((stat(_fileName.c_str(), &fileStat) == 0) && S_ISREG(fileStat.st_mode))
    //{
        size_t found = _fileName.find_last_of("/");
        if (found != std::string::npos) {
            return _fileName.substr(found+1, _fileName.size()-found-1);
        }

        return _fileName;
    //}

    //return "";
}

std::string File::getNameWithoutEnding(void)
{
    std::string str = this->getName();

    size_t found = str.find_last_of(".");
    if (found != std::string::npos) {
        return str.substr(0, found);
    }

    return _fileName;
}

std::string File::getLogName(void)
{
    std::stringstream ss;
    ss << "'" << this->getName() << "'";
    return ss.str();
}

bool File::exists(void)
{
    struct stat fileStat;
    if (stat(_fileName.c_str(), &fileStat) != -1)
    {
        return true;
    }

    return false;
}

bool File::getSize(uint64_t* size)
{
    _fileStream->open(_fileName, std::ios::in);

    assert(_fileStream->is_open());

    if (_fileStream->is_open()) {
        _fileStream->seekg(0, std::ios::end);

        *(size) = _fileStream->tellg();

        _fileStream->close();

        return true;
    } else {
        return false;
    }
}

bool File::writeIntoByteBuffer(byte* targetBuffer)
{
    _fileStream->open(_fileName, std::ios::in | std::ios::binary);

    assert(_fileStream->is_open());

    if (_fileStream->is_open()) {
        _fileStream->seekg(0);

        int i = 0;
        while (!_fileStream->eof()) {
            *(targetBuffer+i) = (byte)_fileStream->get();
            i++;
        }

        _fileStream->close();

        return true;
    } else {
        return false;
    }
}

bool File::writeIntoString(std::string& fileContent)
{
    _fileStream->open(_fileName, std::ios::in);

    assert(_fileStream->is_open());

    if (_fileStream->is_open()) {
        _fileStream->seekg(0, std::ios::end);
        size_t size = _fileStream->tellg();
        _fileStream->seekg(0);

        fileContent.resize(size, ' ');
        _fileStream->read(&fileContent[0], size);

        _fileStream->close();

        return true;
    } else {
        return false;
    }
}

std::string File::getAnEasyFpgaName(void)
{
    _fileStream->open(_fileName, std::ios::in);

    assert(_fileStream->is_open());

    if (_fileStream->is_open()) {
        _fileStream->seekg(0, std::ios::end);
        size_t size = _fileStream->tellg();
        _fileStream->seekg(0);

        std::string fileContent(size, ' ');
        _fileStream->read(&fileContent[0], size);

        std::regex ex(".*class.*: public EasyFpga.*");
        if (std::regex_match(fileContent, ex)) {
            uint32_t beginPos = fileContent.find("class ");
            uint32_t endPos = fileContent.find(" : public EasyFpga");
            if ((beginPos != std::string::npos) && (beginPos != std::string::npos)) {
                /* In the following: -6 because the string "class " is 6 chars long */
                std::string easyFpgaName = fileContent.substr(beginPos+6, endPos-beginPos-6);
                _fileStream->close();
                return easyFpgaName;
            }
        }

        _fileStream->close();

        return std::string("");
    } else {
        return std::string("");
    }
}

bool File::findAndReplace(std::string placeholder, std::string replacement)
{
    _fileStream->open(_fileName, std::ios::in);

    assert(_fileStream->is_open());

    if (_fileStream->is_open()) {
        _fileStream->seekg(0, std::ios::end);
        size_t size = _fileStream->tellg();
        _fileStream->seekg(0);

        std::string fileContent(size, ' ');
        _fileStream->read(&fileContent[0], size);
        _fileStream->close();

        int32_t pos;
        while ((pos = fileContent.find(placeholder)) != std::string::npos) {
            fileContent.replace(pos, placeholder.size(), replacement);
        }

        _fileStream->open(_fileName, std::ios::out | std::ios::trunc);
        if (_fileStream->is_open()) {
            _fileStream->write(&fileContent[0], fileContent.size());
            _fileStream->close();
            return true;
        }
    }

    return false;
}

bool File::createWithContent(std::string content)
{
    _fileStream->open(_fileName, std::ios::out | std::ios::trunc);

    assert(_fileStream->is_open());

    if (_fileStream->is_open()) {
        _fileStream->write(&content[0], content.size());
        _fileStream->close();
        return true;
    }

    return false;
}

void File::resolveTilde(void)
{
    size_t tildePos = _fileName.find('~');
    if (tildePos != std::string::npos) {

        /* determine homedir */
        const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        /* replace tilde */
        _fileName = std::string(homedir).append(_fileName.substr(1));
    }
}

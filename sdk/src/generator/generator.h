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

#ifndef SDK_GENERATOR_GENERATOR_H_
#define SDK_GENERATOR_GENERATOR_H_

#include "easyfpga.h"
#include "easyfpga_ptr.h"
#include "generator/types.h"

#include <string>

/**
 * \brief Binary generator for the easyFPGA board
 */
class Generator
{
    public:
        Generator();
        ~Generator();

        /**
         * \brief Determines in a given directory which files are
         *        describing an easyFPGA and tries to generate binaries
         *        from them.
         *
         * \param directory An absolute directory path
         *
         * \return true if all binary descriptions of the given directory
         *         could be processed successfully and no errors occured
         *         for every single binary generation,<br>
         *         false otherwise
         */
        bool generateBinaries(std::string directory);

        /**
         * \brief Tries to generate a single binary.
         *
         * \param binaryName A file name including the suffix ".bin"
         *
         * \param fpga Pointer to any instance of EasyFpga
         *
         * \param directory An absolute directory path
         *
         * \return true if this binary could be created successfully,<br>
         *         false otherwise
         */
        bool generateBinary(std::string binaryName, EasyFpga* fpga, const char* directory);

        bool generateBinary(std::string binaryName, easyfpga_ptr fpga, const char* directory);

    private:
        HDL_GENERATION_STATUS generateHdl(std::string directory, std::string binaryName, easyfpga_ptr fpga);
        bool copyTemplate(std::string sourceFileName, std::string targetPath);
        bool replaceAll(std::string& target, std::string placeholder, std::string replacement);

        bool executeAndWriteToLog(const char* instruction, FILE* logFile);

        /**
         * \brief Executes the XILINX toolchain.
         *
         * Therfore are the generated hdl files in the project directory
         * neccessary!
         */
        bool runToolchain(std::string binaryName, std::string directory);

        /**
         * Deletes all auto-generated files.
         *
         * \param deleteOwnHdl Specifies if the self-generated hdl files
         *        will be deleted or not.
         */
        bool cleanupBuildDirectory(std::string directory, std::string binaryName, bool deleteOwnHdl);

        std::string _SOC_DIRECTORY;
        std::string _LIBRARY_DIRECTORY;
        std::string _HEADER_DIRECTORY;
        std::string _TEMPLATES_DIRECTORY;
};

#endif  // SDK_GENERATOR_GENERATOR_H_

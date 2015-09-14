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

#include "easyfpga/configuration.h" /* assert(1) */
#include "easyfpga/generator/generator.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/os/directory.h"
#include "easyfpga/utils/unittest/tester.h"

#include <list>
#include <sstream>

/**
 * \brief Test the binary generation functionality of the class Generator
 */
class GeneratorTest : public Tester
{
    std::string testName(void) {
        return "generator test";
    }

    bool testMethod(void) {
        Directory dir(".");

        Log().Get(INFO) << "Generate binaries in directory " << dir.getName();

        Log().Get(INFO) << "First, clean directory!";

        std::initializer_list<std::string> endingsListInit = {".vhd", ".bin", ".log"};
        std::list<std::string> interestingEndings(endingsListInit);

        std::list<std::string> filesToDelete = dir.getAllFileNamesWithEndings(interestingEndings);
        for (auto it=filesToDelete.begin(); it!=filesToDelete.end(); ++it) {
            Log().Get(INFO) << "Delete existing file: " << *it;
            std::stringstream ss;
            ss << "rm " << dir.getName() << "/" << *it;
            assert(!system(ss.str().c_str()));
        }

        Log().Get(INFO) << "Now, generate!";

        Generator g;
        if (g.generateBinaries(dir.getName())) {
            Log().Get(INFO) << "Binary generation process was successful! :-D";
            return true;
        }
        else {
            Log().Get(ERROR) << "Binary generation process wasn't successful! :-(";
        }

        return false;
    }
};

int main(int argc, char** argv)
{
    GeneratorTest test;
    return (uint32_t)test.runTest();
}

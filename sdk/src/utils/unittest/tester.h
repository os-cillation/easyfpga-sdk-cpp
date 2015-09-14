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

#ifndef SDK_UTILS_UNITTEST_TESTER_H_
#define SDK_UTILS_UNITTEST_TESTER_H_

#include <string>

/**
 * \brief Base class for an automatic testing environment
 *
 * We integrated an automatic test execution environment in our makefile.
 * This was neccessary to test our implemented functionality. For
 * simplicity and unification we let inherit all test cases from Tester.
 * So we only have to implement the two abstract methods testName() and
 * testMethod().
 *
 * If you want to create a new test case, do the following:
 * -# Copy the file "ClassTest.template" anywhere
 * -# Rename the file (The file name have to end up with "*Test.cc".)
 * -# Adapt the class name within the file (Don't forget the main.)
 * -# Overwrite the members testName() and testMethod() with your test
 *    logic
 *
 * That's all!
 */
class Tester
{
    public:
        /**
         * \brief Executes the test case.
         *
         * \return true if the test case could be executed successfully,<br>
         *         false otherwise
         */
        bool runTest(void);

    protected:
        /**
         * \brief Should return the test case's name.
         *
         * \return a string
         */
        virtual std::string testName(void) = 0;

        /**
         * \brief Should implement the tast case.
         *
         * \return true if the test case processes correctly,<br>
         *         false otherwise
         */
        virtual bool testMethod(void) = 0;
};

#endif  // SDK_UTILS_UNITTEST_TESTER_H_

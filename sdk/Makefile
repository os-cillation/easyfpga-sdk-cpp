#
# This file is part of easyFPGA.
# Copyright 2013-2015 os-cillation GmbH
#
# Authors: Johannes Hein <support@os-cillation.de>
#          Simon Gansen
#
# easyFPGA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# easyFPGA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with easyFPGA.  If not, see <http://www.gnu.org/licenses/>.
#


# shared library name
SHARED_LIBRARY_NAME = EasyFpga



# local paths in our repository root directory
DIR_SOURCES = src/
DIR_BINARIES = bin/
DIR_SHARED_LIBRARY = lib/
DIR_DOCUMENTATION = doc/
DIR_TEST_RESULTS = testoutput/
DIR_SOC = ../soc/



# install paths
INSTALL_DIR_SHARED_LIBRARY = /usr/local/lib/
INSTALL_DIR_HEADERS = /usr/local/include/easyfpga/
INSTALL_DIR_TEMPLATES = /usr/local/share/easyfpga/templates/
INSTALL_DIR_SOC = /usr/local/share/easyfpga/soc/



# terminal messages
MSG_COMPILING = make: *** COMPILE ...
MSG_LINKING = make: *** LINK ...

MSG_STATICCODEANALYSIS = make: *** RUN STATIC CODE ANALYSIS TOOL...

MSG_DOKU = make: *** CREATE DOCUMENTATION OF THE SDK ...
MSG_DOKU_DONE = make: *** CREATE DOCUMENTATION SUCCESS

MSG_BUILD_LIB_DONE = make: *** LIB BUILD SUCCESS

MSG_BUILD_TEST_CASES_DONE = make: *** TEST CASES BUILD SUCCESS

MSG_CLEAN_BINARIES = make: *** DELETE BINARIES ...
MSG_CLEAN_SHARED_LIBRARY = make: *** DELETE SHARED LIBRARY ...
MSG_CLEAN_TEST_RESULTS = make: *** DELETE TEST RESULTS ...
MSG_CLEAN_DOCUMENTATION = make: *** DELETE GENERATED DOCUMENTATION ...
MSG_CLEAN_DONE = make: *** CLEAN SUCCESS

MSG_TEST = make: *** RUN TEST ...

MSG_TESTS = make: *** RUN TESTS ...
MSG_TESTS_CHECK_RESULTS = make: *** CHECK TEST RESULTS ...
MSG_TESTS_ERROR = make: *** ERRORS DURING TESTS
MSG_TESTS_DONE = make: *** ALL TESTS RUN SUCCESSFUL

MSG_COPY_HEADERS = make: *** copy header files ...
MSG_COPY_HDL_TEMPLATES = make: *** copy easycore independent hdl templates ...

MSG_INSTALL = make: *** INSTALL SDK (library, soc templates and headers) ...
MSG_INSTALL_DONE  = make: *** INSTALL SUCCESS
MSG_UNINSTALL = make: *** UNINSTALL SDK ...
MSG_UNINSTALL_DONE = make: *** UNINSTALL SUCCESS



# used programs with all necessary flags
CC = g++
CC_FLAGS = -I $(DIR_SOURCES)
CC_FLAGS += -c
CC_FLAGS += -std=c++0x
CC_FLAGS += -Wall
#CC_FLAGS += -ggdb
CC_FLAGS_LIB = -I $(DIR_SOURCES)
CC_FLAGS_LIB += -std=c++0x
CC_FLAGS_LIB += -fPIC
CC_FLAGS_LIB += -Wall
CC_FLAGS_LIB += -shared
#CC_FLAGS_LIB += -ggdb

# order of included libraries is important; please do not change...
FLAGS_LINKING = -l$(SHARED_LIBRARY_NAME)
FLAGS_LINKING += -lrt

CHANGE_DIR = cd

COPY_FILE = cp
COPY_DIR = cp -r

DELETE_FILE = rm
# we have to force because it might happen that we want to delete a directory which not exists
DELETE_DIR = rm -rf

DOXYGEN = doxygen

STATIC_CODE_ANALYSIS_TOOL = cppcheck
STATIC_CODE_ANALYSIS_TOOL_FLAGS = --quiet



# sources and objects excluding tests
SOURCES = $(shell find $(DIR_SOURCES) -not -wholename '*/test/*' -name '*.cc')
OBJECTS = $(patsubst $(DIR_SOURCES)%,$(DIR_BINARIES)%,$(patsubst %.cc,%.o,$(SOURCES:%.cc=%.o)))

# test case sources and objects
TEST_SOURCES = $(shell find $(DIR_SOURCES) -name '*Test.cc')
TEST_OBJECTS = $(patsubst $(DIR_SOURCES)%,$(DIR_BINARIES)%,$(patsubst %.cc,%.o,$(TEST_SOURCES:%.cc=%.o)))

# build targets
SHARED_LIBRARY_TARGET = $(DIR_SHARED_LIBRARY)lib$(SHARED_LIBRARY_NAME).so
TEST_CASE_TARGETS = $(patsubst $(DIR_SOURCES)%,$(DIR_BINARIES)%,$(patsubst %.cc,%.o,$(TEST_SOURCES:%.cc=%)))



# targets which are always out of date (or: don't refer to filenames)
.PHONY: default all build copyheaders copytemplates install do_install uninstall do_uninstall checksources doc do_doc clean cleantestcases cleanlib cleandoc test runtest runtests checkresults



# SDK development candidate
# The default make target for developing is set to:
# 1. execute an static code analysis tool for finding bugs,
# 2. build the shared library and installing it into the filesystem, and
# 3. build the test cases.
# Please comment / uncomment the following line for your usage purpose.
#default: checksources install buildtestcases

# SDK release candidate
# The default make target for developing is set to:
# 1. build the shared library and installing it into the filesystem, and
# 2. generating the documentation.
# Please comment / uncomment the following line for your usage purpose.
default: install doc



# does a completely new rebuild of everything what's possible (useful
# for developing the easyFPGA library; not really for a productive usage)
# The order of the following targets is important
# (clean -> install -> buildtestcases)
all: clean checksources install buildtestcases doc



buildlib: $(SOURCES)
	$(info )
	$(info $(MSG_COMPILING) $(SHARED_LIBRARY_TARGET))
	@mkdir -p $(DIR_SHARED_LIBRARY)
	$(CC) $(CC_FLAGS_LIB) -o $(SHARED_LIBRARY_TARGET) $(SOURCES)



copyheaders:
	$(info )
	$(info $(MSG_COPY_HEADERS))
	$(foreach header, $(shell find $(DIR_SOURCES) -name '*.h'), \
		$(shell mkdir -p $(dir $(patsubst $(DIR_SOURCES)%, $(DIR_SHARED_LIBRARY)inc/%, $(header))) && \
		$(COPY_FILE) $(header) $(dir $(patsubst $(DIR_SOURCES)%, $(DIR_SHARED_LIBRARY)inc/%, $(header)))) \
	)



copytemplates:
	$(info )
	$(info $(MSG_COPY_HDL_TEMPLATES))
	$(foreach template, $(shell find $(DIR_SOURCES)generator/templates/ -name '*.template'), \
		$(shell mkdir -p $(DIR_SHARED_LIBRARY)templates && \
		$(COPY_FILE) $(template) $(DIR_SHARED_LIBRARY)templates/$(shell basename $(template))) \
	)



# build all test cases
buildtestcases: $(TEST_CASE_TARGETS)
	$(info )
	$(info $(MSG_BUILD_TEST_CASES_DONE))

# link test case object files to binaries
# to determine which cpp file contains a test case, we decided to end up
# all test class file names with "*Test.cc"
$(DIR_BINARIES)%: $(DIR_BINARIES)%.o
	$(foreach testcaseobject, $(shell find $(patsubst $(DIR_BINARIES)%,$(DIR_SOURCES)%,$(@D)) -not -name '*Test.cc' -name '*.cc'), \
		$(info ) \
		$(info $(MSG_COMPILING) $(testcaseobject)) \
		$(shell $(CC) $(CC_FLAGS) $(testcaseobject) -o $(patsubst $(DIR_SOURCES)%, $(DIR_BINARIES)%, $(testcaseobject:%.cc=%.o)))  \
	)
	$(info )
	$(info $(MSG_LINKING) $@)
	$(CC) $@.o $(patsubst $(DIR_SOURCES)%, $(DIR_BINARIES)%, \
		$(patsubst %.cc, %.o, $(shell find $(dir $(patsubst $(DIR_BINARIES)%,$(DIR_SOURCES)%,$<)) -not -name '*Test.cc' -name '*.cc'))) \
		-o $@ $(FLAGS_LINKING)

# compile the test case sources to object files
# TODO: changes at .h files (which will be included by a .cc file) won't
# be recognized by this makefile!
$(DIR_BINARIES)%.o: $(DIR_SOURCES)%.cc
	$(info )
	$(info $(MSG_COMPILING) $<)
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) $< -o $@



# install library, templates and hdl files (requires root privileges)
install: copyheaders copytemplates buildlib do_install
	$(info )
	$(info $(MSG_INSTALL_DONE))

do_install:
	$(info )
	$(info $(MSG_INSTALL))
	$(COPY_FILE) $(DIR_SHARED_LIBRARY)lib$(SHARED_LIBRARY_NAME).so $(INSTALL_DIR_SHARED_LIBRARY)
	mkdir -p $(INSTALL_DIR_HEADERS)
	$(COPY_DIR) $(DIR_SHARED_LIBRARY)inc/* $(INSTALL_DIR_HEADERS)
	mkdir -p $(INSTALL_DIR_TEMPLATES)
	$(COPY_DIR) $(DIR_SHARED_LIBRARY)templates/* $(INSTALL_DIR_TEMPLATES)
	mkdir -p $(INSTALL_DIR_SOC)
	$(COPY_DIR) $(DIR_SOC)* $(INSTALL_DIR_SOC)

# remove library, templates and hdl files (requires root privileges)

uninstall: do_uninstall clean
	$(info )
	$(info $(MSG_UNINSTALL_DONE))

do_uninstall:
	$(info )
	$(info $(MSG_UNINSTALL))
	$(DELETE_FILE) $(INSTALL_DIR_SHARED_LIBRARY)lib$(SHARED_LIBRARY_NAME).so
	$(DELETE_DIR) $(INSTALL_DIR_HEADERS)
	$(DELETE_DIR) $(INSTALL_DIR_TEMPLATES)
	$(DELETE_DIR) $(INSTALL_DIR_SOC)



# executes a static code analysis tool for cpp
checksources:
	$(info )
	$(info $(MSG_STATICCODEANALYSIS))
	$(STATIC_CODE_ANALYSIS_TOOL) $(STATIC_CODE_ANALYSIS_TOOL_FLAGS) $(DIR_SOURCES)



# creates an documentation of the api with doxygen
doc: do_doc
	$(info )
	$(info $(MSG_DOKU_DONE))

do_doc:
	$(info )
	$(info $(MSG_DOKU))
	$(DOXYGEN)



# removes all generated directories and files
clean: cleantestcases cleantestresults cleanlib cleandoc
	$(info )
	$(info $(MSG_CLEAN_DONE))

cleantestcases:
	$(info )
	$(info $(MSG_CLEAN_BINARIES))
	$(DELETE_DIR) $(DIR_BINARIES)

cleantestresults:
	$(info )
	$(info $(MSG_CLEAN_TEST_RESULTS))
	$(DELETE_DIR) $(DIR_TEST_RESULTS)

cleanlib:
	$(info )
	$(info $(MSG_CLEAN_SHARED_LIBRARY))
	$(DELETE_DIR) $(DIR_SHARED_LIBRARY)

cleandoc:
	$(info )
	$(info $(MSG_CLEAN_DOCUMENTATION))
	$(DELETE_DIR) $(DIR_DOCUMENTATION)



# runs a single test
CURRENT_DIR = $(shell pwd)

runtest: buildtestcases
	$(info )
	$(info $(MSG_TEST) $(notdir $(TESTCASE)))
	$(CHANGE_DIR) $(dir $(TESTCASE:bin/%=src/%)) && ../../../../../$(TESTCASE) && $(CHANGE_DIR) $(CURRENT_DIR)



# runs all existing tests, write their results to log files and check
# them for errors
test: buildtestcases runtests checkresults

runtests: $(TEST_CASE_TARGETS)
	$(info )
	$(info $(MSG_TESTS))
	@mkdir -p $(DIR_TEST_RESULTS)
	$(foreach testcase,$(TEST_CASE_TARGETS),./$(testcase) | tee $(DIR_TEST_RESULTS)$(notdir $(testcase)) ;)

checkresults: TESTERRORS = $(shell grep 'FAILED' $(DIR_TEST_RESULTS)*)
checkresults:
	$(info )
	$(info $(MSG_TESTS_CHECK_RESULTS))
	$(info )
	$(if $(findstring FAILED, $(TESTERRORS)), $(info $(MSG_TESTS_ERROR)), $(info $(MSG_TESTS_DONE)))

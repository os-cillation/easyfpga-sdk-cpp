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
#include "easyfpga.h"
#include "easycores/easycore.h"
#include "easycores/gpiopin.h"
#include "generator/generator.h"
#include "utils/config/configurationfile.h"
#include "utils/log/log.h"
#include "utils/os/directory.h"
#include "utils/os/file.h"

#include <unistd.h> /* chdir(1) */
#include <stdlib.h> /* system(1), popen(2) */
#include <sstream>
#include <map>
#include <list>
#include <set>
#include <typeinfo> /* typeid(1) */
#include <iomanip> /* setw(1), setfill(1) */
#include <initializer_list>

Generator::Generator()
{
    /*
     * Function getTheAbsolutePath() determines the absolute file path
     * for a mightly given relative path in configuration file.
     */
    File(ConfigurationFile::getInstance().getSocDirectory()).getTheAbsolutePath(_SOC_DIRECTORY);
    File(ConfigurationFile::getInstance().getLibraryDirectory()).getTheAbsolutePath(_LIBRARY_DIRECTORY);
    File(ConfigurationFile::getInstance().getHeaderDirectory()).getTheAbsolutePath(_HEADER_DIRECTORY);
    File(ConfigurationFile::getInstance().getTemplatesDirectory()).getTheAbsolutePath(_TEMPLATES_DIRECTORY);
}

Generator::~Generator()
{
}

bool Generator::generateBinaries(std::string directory)
{
    if (directory.empty()) {
        Log().Get(ERROR) << "Path to working directory empty...";
        return false;
    }

    Log().Get(INFO) << "Start generation process...";
    Log().Get(DEBUG) << "Project directory '" << directory << "' selected.";

    std::initializer_list<std::string> endingsListInit = {".cc", ".h"};
    std::list<std::string> interestingEndings(endingsListInit);

    bool firstEnding = true;
    std::stringstream endings;
    for (auto it=interestingEndings.begin(); it!=interestingEndings.end(); ++it) {
        if (firstEnding) {
            endings << "'" << *it << "'";
            firstEnding = false;
        }
        else {
            endings << ", '" << *it << "'";
        }
    }
    Log().Get(DEBUG) << "Looking for binary description files with endings " << endings.str() << " in the project directory...";

    Directory dir(directory);
    std::list<std::string> fileNames = dir.getAllFileNamesWithEndings(interestingEndings);

    if (fileNames.size() > 0) {
        std::list<std::string> easyFpgaNames;

        for (auto it2=fileNames.begin(); it2!=fileNames.end(); ++it2) {
            std::string fullFileName(directory);
            fullFileName.append("/").append(*it2);
            File f(fullFileName);
            std::string name = f.getAnEasyFpgaName();
            if (!name.empty()) {
                Log().Get(DEBUG) << f.getLogName() << " contains a binary description!";
                easyFpgaNames.push_back(name);
            } else {
                Log().Get(DEBUG) << f.getLogName() << " doesn't contain a binary description!";
            }
        }

        if (easyFpgaNames.size() > 0) {
            Log().Get(INFO) << "The Generator found " << easyFpgaNames.size() << " easyFPGA description(s).";
        }
        else {
            Log().Get(WARNING) << "In the working directory are cpp files, but no easyFPGA describing classes!";
            return false;
        }

        bool overallSuccess = true;

        for (auto it3=easyFpgaNames.begin(); it3!=easyFpgaNames.end(); ++it3) {
            bool success = true;

            std::string binaryName = *it3;

            Log().Get(INFO) << "Create binary '" << binaryName << ".bin'...";

            if (success) {
                std::stringstream compile;
                compile << "g++ -std=c++0x -I " << directory << " -I " << _HEADER_DIRECTORY << " -c " << directory << "/" << binaryName << ".cc -o " << directory << "/" << binaryName << ".o -DBINARY_GENERATION_PROCESS";
                Log().Get(DEBUG) << "Compile: " << compile.str();
                if (system(compile.str().c_str()) != 0) {
                    Log().Get(ERROR) << "Compilation of the binary generator executable for this binary failed!";
                    success = false;
                }
            }

            if (success) {
                std::stringstream link;
                link << "g++ " << directory << "/" << binaryName << ".o -o " << directory << "/" << binaryName << " -L " << _LIBRARY_DIRECTORY << " -lEasyFpga -lrt";
                Log().Get(DEBUG) << "Link: " << link.str();
                if (system(link.str().c_str()) != 0) {
                    Log().Get(ERROR) << "Unable to link framework to the binary generator!";
                    success = false;
                }
            }

            if (success) {
                std::stringstream execute;
                execute << directory << "/" << binaryName << " " << directory;
                if (system(execute.str().c_str()) != 0) {
                    Log().Get(ERROR) << "At least one error occured during binary generation! Please have a look to the log files...";
                    success = false;
                }
            }

            Log().Get(DEBUG) << "Delete auto-generated files...";
            if (!this->cleanupBuildDirectory(directory, binaryName, !success)) {
                Log().Get(WARNING) << "Not all auto-generated files could be deleted!";
            }

            if (success) {
                Log().Get(INFO) << "The binary '" << binaryName << ".bin' has been created successfully.";
            }
            else {
                Log().Get(WARNING) << "The binary generation of '" << binaryName << ".bin' was NOT successful!";
            }

            overallSuccess &= success;
        }

        return overallSuccess;
    }
    else {
        Log().Get(WARNING) << "There are no interesting cpp files in the project directory! Did you follow the naming conventions?";

        return false;
    }
}

bool Generator::generateBinary(std::string binaryName, EasyFpga* fpga, const char* directory)
{
    Log().Get(DEBUG) << "Generating vhdl...";
    switch (this->generateHdl(std::string(directory), binaryName, std::shared_ptr<EasyFpga>(fpga))) {
        case HDL_GENERATION_STATUS::NEW_BUILD_NECCESSARY:
            Log().Get(DEBUG) << "Run toolchain with generated vhdl...";
            if (this->runToolchain(binaryName, std::string(directory))) {
                Log().Get(DEBUG) << "Toolchain successfully executed!";
                return true;
            }
            else {
                Log().Get(ERROR) << "The toolchain has detected errors! The build process for this binary will be aborted.";
                return false;
            }

        case HDL_GENERATION_STATUS::NO_NEW_BUILD_NECCESSARY:
            Log().Get(DEBUG) << "The hardware description has not changed. Running the toolchain not neccessary!";
            return true;

        case HDL_GENERATION_STATUS::ERRORS_AT_STRUCTURE_DESCRIPTION:
            Log().Get(ERROR) << "The fpga decription was faulty! The build process for this binary will be aborted.";
            return false;

        default:
            return false;
    }
}

bool Generator::generateBinary(std::string binaryName, easyfpga_ptr fpga, const char* directory)
{
    Log().Get(DEBUG) << "Generating vhdl...";

    bool success;

    switch (this->generateHdl(std::string(directory), binaryName, fpga)) {
        case HDL_GENERATION_STATUS::NEW_BUILD_NECCESSARY:
            Log().Get(DEBUG) << "Run toolchain with generated vhdl...";
            if (this->runToolchain(binaryName, std::string(directory))) {
                Log().Get(DEBUG) << "Toolchain successfully executed!";
                success = true;
            }
            else {
                Log().Get(ERROR) << "The toolchain has detected errors! The build process for this binary will be aborted.";
                success = false;
            }

            if (success) {
                Log().Get(INFO) << "The binary '" << binaryName << ".bin' has been created successfully.";
            }
            else {
                Log().Get(WARNING) << "The binary generation of '" << binaryName << ".bin' was NOT successful!";
            }
            break;

        case HDL_GENERATION_STATUS::NO_NEW_BUILD_NECCESSARY:
            Log().Get(DEBUG) << "The hardware description has not changed. Running the toolchain not neccessary!";
            success = true;
            break;

        case HDL_GENERATION_STATUS::ERRORS_AT_STRUCTURE_DESCRIPTION:
            Log().Get(ERROR) << "The fpga decription was faulty! The build process for this binary will be aborted.";
            success = false;
            break;

        default:
            success = false;
    }

    Log().Get(DEBUG) << "Delete auto-generated files...";
    if (!this->cleanupBuildDirectory(directory, binaryName, !success)) {
        Log().Get(WARNING) << "Not all auto-generated files could be deleted!";
        return false;
    }

    return success;
}

HDL_GENERATION_STATUS Generator::generateHdl(std::string directory, std::string binaryName, easyfpga_ptr fpga)
{
    /*
     * STEP 1: Init data structures of the easyFpga class
     */
    fpga->instantiateCores();

    /*
     * STEP 2: Get the easyFpga data structures and define some data
     * structures for the analysis step
     */
    CustomSignalIndex customSignalCounter = 0;

    /*
     * The following data structure holds the main information. Here will
     * be stored which pins are connected together. All pin instances
     * connected together get assigned the same CustomSignalIndex. (The
     * first occurance of any connected pin will be assigned to a next
     * free CustomSignalIndex not used before. All to it's other
     * logically connected pins get assigned this same CustomSignalIndex
     * in STEP 4. For doing that exits customSignalCounter.)
     */
    std::map<pin_ptr, CustomSignalIndex> customSignals;

    std::list<gpiopin_ptr> gpioPinList = fpga->getGpioPins();
    std::list<gpiopin_ptr> notDirectlyConnectedGpioPins;

    std::list<easycore_ptr> easyCoreList = fpga->getEasyCores();
    std::set<CoreIndex> usedCoresTypes;

    /*
     * STEP 3: Define all used placeholder streams
     */

    /* INTERCON */
    std::stringstream hdlCoreInjections;
    std::stringstream wbslavesIntercon;
    bool wbslavesInterconbool = true;
    std::stringstream constants;
    std::stringstream signals;
    std::stringstream csignals;
    std::stringstream drdmultiplexer;
    std::stringstream addresscomparator;
    std::stringstream ackorgate;
    bool ackorgatebool = true;
    std::stringstream stbandgates;
    std::stringstream irqprioritydecoder1;
    bool irqprioritydecoder1bool = true;
    std::stringstream irqprioritydecoder2;
    bool irqprioritydecoder2bool = true;
    std::stringstream irqorgate;
    bool irqorgatebool = true;

    /* TOP LEVEL ENTITY */
    std::stringstream user_gpios;
    std::stringstream wbslavesTle;
    std::stringstream customsignals;
    std::stringstream wbslavesInterconTle;
    bool wbslavesInterconTlebool = true;
    std::stringstream cores;

    /*
     * STEP 4: Analyse data structures and insert the gained information
     * into the placeholder streams
     */

    /*
     * STEP 4.1: Build the INTERCON specific vhdl code for all easyCores
     */
    drdmultiplexer << "   with core_adr_s select wbm_in.dat <=" << std::endl;
    ackorgate << "   wbm_in.ack <= ";
    irqprioritydecoder1 << "   IRQ_DEC : process (";
    irqprioritydecoder2 << "   begin" << std::endl;
    irqorgate << "   wbm_in.girq <= ";

    for (auto it=easyCoreList.begin(); it!=easyCoreList.end(); ++it) {
        int32_t i = (int32_t)((*it)->getIndex());

        if (wbslavesInterconbool) {
            wbslavesIntercon << "wbs" << i << "_out : in wbs_out_type;" << std::endl;
            wbslavesIntercon << "      wbs" << i << "_in : out wbs_in_type";
            wbslavesInterconbool = false;
        }
        else {
            wbslavesIntercon << ";" << std::endl;
            wbslavesIntercon << "      wbs" << i << "_out : in wbs_out_type;" << std::endl;
            wbslavesIntercon << "      wbs" << i << "_in : out wbs_in_type";
        }

        constants << "   constant WBS" << std::dec << i << "_ADR : std_logic_vector(WB_CORE_AW-1 downto 0) := x\"" << std::setw(2) << std::hex << std::setfill('0') << i << "\";" << std::endl;

        signals << "   signal adr_match_" << i << "_s : std_logic;" << std::endl;

        csignals << "   wbs" << i << "_in.dat <= wbm_out.dat;" << std::endl;
        csignals << "   wbs" << i << "_in.adr <= reg_adr_s;" << std::endl;
        csignals << "   wbs" << i << "_in.we <= wbm_out.we;" << std::endl;
        csignals << "   wbs" << i << "_in.cyc <= wbm_out.cyc;" << std::endl;
        csignals << "   wbs" << i << "_in.clk <= clk_in;" << std::endl;
        csignals << "   wbs" << i << "_in.rst <= rst_in;" << std::endl;
        csignals << std::endl;

        drdmultiplexer << "      wbs" << i << "_out.dat when WBS" << i << "_ADR," << std::endl;

        addresscomparator << "   adr_match_" << i << "_s <= '1' when core_adr_s = WBS" << i << "_ADR else '0';" << std::endl;

        if (ackorgatebool) {
            ackorgate << "wbs" << i << "_out.ack";
            ackorgatebool = false;
        }
        else {
            ackorgate << " OR wbs" << i << "_out.ack";
        }

        stbandgates << "   wbs" << i << "_in.stb <= wbm_out.cyc AND wbm_out.stb AND adr_match_" << i << "_s;" << std::endl;

        if (irqprioritydecoder1bool) {
            irqprioritydecoder1 << "wbs" << i << "_out.irq";
            irqprioritydecoder1bool = false;
        }
        else {
            irqprioritydecoder1 << ", wbs" << i << "_out.irq";
        }

        if (irqprioritydecoder2bool) {
            irqprioritydecoder2 << "      if wbs" << i << "_out.irq = '1' then wbm_in.int_adr <= WBS" << i << "_ADR;" << std::endl;
            irqprioritydecoder2bool = false;
        }
        else {
            irqprioritydecoder2 << "      elsif wbs" << i << "_out.irq = '1' then wbm_in.int_adr <= WBS" << i << "_ADR;" << std::endl;
        }

        if (irqorgatebool) {
            irqorgate << "wbs" << i << "_out.irq";
            irqorgatebool = false;
        }
        else {
            irqorgate << " OR wbs" << i << "_out.irq";
        }
    }

    drdmultiplexer << "      (others => '-') when others;" << std::endl;
    ackorgate << ";" << std::endl;
    irqprioritydecoder1 << ")" << std::endl;
    irqprioritydecoder2 << "      else wbm_in.int_adr <= (others => '-');" << std::endl;
    irqprioritydecoder2 << "      end if;" << std::endl;
    irqprioritydecoder2 << "   end process IRQ_DEC;" << std::endl;
    irqorgate << ";" << std::endl;

    /*
     * STEP 4.2: Build the TOP LEVEL ENTITY specific vhdl code for all
     * easyCores
     */

    /* 4.2.1: Fill customSignals with gpio pins */
    for (auto it=gpioPinList.begin(); it!=gpioPinList.end(); ++it) {
        auto pin = *it;

        if (pin->isBackwardConnected()) {
            auto target = pin->getConnection();

            auto found1 = customSignals.find(pin);
            auto found2 = customSignals.find(target);
            if ((found1 == customSignals.end()) && (found2 == customSignals.end())) {
                customSignals.insert(std::make_pair(pin, customSignalCounter));
                customSignals.insert(std::make_pair(target, customSignalCounter));
                customsignals << "   signal c" << customSignalCounter << "_s : std_logic;" << std::endl;

                customSignalCounter++;
            }
            else if ((found1 == customSignals.end()) && (found2 != customSignals.end())) {
                customSignals.insert(std::make_pair(pin, found2->second));
            }
            else if ((found1 != customSignals.end()) && (found2 == customSignals.end())) {
                customSignals.insert(std::make_pair(target, found1->second));
            }
        }
    }

    /* 4.2.2: Fill customSignals with easyCore pins */
    for (auto it=easyCoreList.begin(); it!=easyCoreList.end(); ++it) {
        auto core = *it;

        auto coreInputPins = core->getPinsWithType(PIN_DIRECTION_TYPE::IN);
        for (auto it=coreInputPins.begin(); it!=coreInputPins.end(); ++it) {
            auto pin = *it;

            if (pin->isBackwardConnected()) {
                auto target = pin->getConnection();

                auto found1 = customSignals.find(pin);
                auto found2 = customSignals.find(target);
                if ((found1 == customSignals.end()) && (found2 == customSignals.end())) {
                    customSignals.insert(std::make_pair(pin, customSignalCounter));
                    customSignals.insert(std::make_pair(target, customSignalCounter));
                    customsignals << "   signal c" << customSignalCounter << "_s : std_logic;" << std::endl;

                    customSignalCounter++;
                }
                else if ((found1 == customSignals.end()) && (found2 != customSignals.end())) {
                    customSignals.insert(std::make_pair(pin, found2->second));
                }
                else if ((found1 != customSignals.end()) && (found2 == customSignals.end())) {
                    customSignals.insert(std::make_pair(target, found1->second));
                }
            }
        }

        auto coreInoutPins = core->getPinsWithType(PIN_DIRECTION_TYPE::INOUT);
        for (auto it=coreInoutPins.begin(); it!=coreInoutPins.end(); ++it) {
            auto pin = *it;

            if (pin->isBackwardConnected()) {
                auto target = pin->getConnection();

                auto found1 = customSignals.find(pin);
                auto found2 = customSignals.find(target);
                if ((found1 == customSignals.end()) && (found2 == customSignals.end())) {
                    customSignals.insert(std::make_pair(pin, customSignalCounter));
                    customSignals.insert(std::make_pair(target, customSignalCounter));
                    customsignals << "   signal c" << customSignalCounter << "_s : std_logic;" << std::endl;

                    customSignalCounter++;
                }
                else if ((found1 == customSignals.end()) && (found2 != customSignals.end())) {
                    customSignals.insert(std::make_pair(pin, found2->second));
                }
                else if ((found1 != customSignals.end()) && (found2 == customSignals.end())) {
                    customSignals.insert(std::make_pair(target, found1->second));
                }
            }
        }
    }

    /*
     * 4.2.3: Insert all used GPIO pins. (That a GPIO pin is used is
     * indicated by customSignals if customSignals contains the desired
     * GPIO pin or not. The next lines modifies the "user_gpios"
     * placeholder.)
     */
    for (auto it=gpioPinList.begin(); it!=gpioPinList.end(); ++it) {
        auto pin = *it;

        auto found = customSignals.find(pin);
        if (found != customSignals.end()) {
            switch (pin->getType()) {
                case PIN_DIRECTION_TYPE::IN:
                    user_gpios << "      " << pin->getName() << " : in std_logic;" << std::endl;

                    cores << "c" << found->second << "_s" << " <= " << pin->getName() << ";" << std::endl;
                    cores << std::endl;
                    break;

                case PIN_DIRECTION_TYPE::OUT:
                    user_gpios << "      " << pin->getName() << " : out std_logic;" << std::endl;

                    cores << pin->getName() << " <= c" << found->second << "_s;" << std::endl;
                    cores << std::endl;
                    break;

                case PIN_DIRECTION_TYPE::INOUT:
                    user_gpios << "      " << pin->getName() << " : inout std_logic;" << std::endl;

                    cores << pin->getName() << " <= c" << found->second << "_s;" << std::endl;
                    cores << std::endl;
                    break;

                default:
                    assert(false);
                    break;
            }
        }
    }

    /*
     * 4.2.4: Go through the easyCores again and replace the remaining
     * things.
     */
    for (auto it=easyCoreList.begin(); it!=easyCoreList.end(); ++it) {
        auto core = *it;
        int32_t i = (int32_t)(core->getIndex());

        wbslavesTle << "   signal wbs" << i << "_in_s : wbs_in_type;" << std::endl;
        wbslavesTle << "   signal wbs" << i << "_out_s : wbs_out_type;" << std::endl;

        if (wbslavesInterconTlebool) {
            wbslavesInterconTle << "      wbs" << i << "_out => wbs" << i << "_out_s," << std::endl;
            wbslavesInterconTle << "      wbs" << i << "_in => wbs" << i << "_in_s";
            wbslavesInterconTlebool = false;
        }
        else {
            wbslavesInterconTle << "," << std::endl;
            wbslavesInterconTle << "      wbs" << i << "_out => wbs" << i << "_out_s," << std::endl;
            wbslavesInterconTle << "      wbs" << i << "_in => wbs" << i << "_in_s";
        }

        std::stringstream connections;

        auto pins = core->getAllPins();
        for (auto it=pins.begin(); it!=pins.end(); ++it) {
            auto pin = *it;

            auto found = customSignals.find(pin);
            if (found != customSignals.end()) {
                connections << "," << std::endl << "      " << pin->getName() << " => c" << (int32_t)found->second << "_s";
            }
        }

        std::string coreHdlCode = core->getHdlTemplate();

        /*
         * Handle the generic map information and possibly insert them
         * into the placeholder.
         */
        std::stringstream generic_map;
        bool generic_map_contains_a_element = false;
        auto genericMap = core->getGenericMap();
        for (auto it=genericMap.begin(); it!=genericMap.end(); ++it) {
            auto pin = it->first;
            if (core->getPin(pin)->isBackwardConnected()) {
                if (!generic_map_contains_a_element) {
                    generic_map << "generic map (" << std::endl;
                    generic_map_contains_a_element = true;
                }
                /**
                 * \todo Currently we assume that the list contains
                 * already one element.
                 */
                generic_map << "      " << it->second.front() << std::endl;
            }
        }
        if (generic_map_contains_a_element) {
            generic_map << "   )" << std::endl;
        }

        this->replaceAll(coreHdlCode, "%generic_map", generic_map.str());

        /* Replace the generic core number. */
        this->replaceAll(coreHdlCode, "%d", std::to_string(i));

        /* Replace the core connections. */
        this->replaceAll(coreHdlCode, "%connections", connections.str());

        /* Insert the core modified core template to the cores placeholder. */
        cores << coreHdlCode << std::endl;

        CoreIndex globalCoreNumber = core->getTheGlobalCoreNumber();
        if (usedCoresTypes.find(globalCoreNumber) == usedCoresTypes.end()) {
            usedCoresTypes.insert(globalCoreNumber);
            hdlCoreInjections << core->getHdlInjections();
        }
    }

    Log().Get(DEBUG) << "This easyFpga uses " << usedCoresTypes.size() << " different easyCore(s).";

    /*
     * STEP 5: Generate the hdl sources and the toolchain scripts
     */

    bool noNewToolchainRunRequired = true;

    /* Defining paths to the files to be generated. */
    std::stringstream ss0;
    ss0 << directory << "/xst-script";
    std::string xstScriptPath = ss0.str();

    std::stringstream ss1;
    ss1 << directory << "/xst-project";
    std::string xstProjectPath = ss1.str();

    std::stringstream ss2;
    ss2 << directory << "/" << binaryName << "_intercon.vhd";
    std::string interconPath = ss2.str();

    std::stringstream ss3;
    ss3 << directory << "/" << binaryName << ".vhd";
    std::string topLevelEntityPath = ss3.str();

    /* Generate xst-script (Replace %tle_name in xst-script.template) */
    this->copyTemplate("xst-script.template", xstScriptPath);

    File xstScript(xstScriptPath);
    xstScript.findAndReplace("%tle_name", binaryName);

    /* Generate xst-project (Insert easyCore hdl injections into xst-project) */
    this->copyTemplate("xst-project.template", xstProjectPath);

    File xstProject(xstProjectPath);
    xstProject.findAndReplace("%cores", hdlCoreInjections.str());
    xstProject.findAndReplace("%tle_name", binaryName);
    xstProject.findAndReplace("%dir", _SOC_DIRECTORY);

    std::string oldInterconContent;
    /*
     * The following block limited by the curly braces is important!
     * It ensures that the object oldIntercon will be destructed right
     * after leaving it. (We have to reopen the same file a few lines
     * later with the instruction "File intercon(interconPath);".)
     */
    {
        File oldIntercon(interconPath);
        if (oldIntercon.exists()) {
            oldIntercon.writeIntoString(oldInterconContent);
        }
        else {
            noNewToolchainRunRequired = false;
        }
    }

    /* Create **binaryName**_intercon.vhd (Replace placeholders in intercon.template) */
    this->copyTemplate("intercon.template", interconPath);

    irqprioritydecoder1 << irqprioritydecoder2.str();

    File intercon(interconPath);
    intercon.findAndReplace("%wbslaves", wbslavesIntercon.str());
    intercon.findAndReplace("%constants", constants.str());
    intercon.findAndReplace("%signals", signals.str());
    intercon.findAndReplace("%csignals", csignals.str());
    intercon.findAndReplace("%drdmultiplexer", drdmultiplexer.str());
    intercon.findAndReplace("%addresscomparator", addresscomparator.str());
    intercon.findAndReplace("%ackorgate", ackorgate.str());
    intercon.findAndReplace("%stbandgates", stbandgates.str());
    intercon.findAndReplace("%irqprioritydecoder", irqprioritydecoder1.str());
    intercon.findAndReplace("%irqorgate", irqorgate.str());

    if (noNewToolchainRunRequired) {
        std::string newInterconContent;
        intercon.writeIntoString(newInterconContent);

        noNewToolchainRunRequired = !newInterconContent.compare(oldInterconContent);
    }

    std::string oldTleContent;
    /*
     * The following block limited by the curly braces is important!
     * It ensures that the object oldIntercon will be destructed right
     * after leaving it. (We have to reopen the same file a few lines
     * later with the instruction "File topLevelEntity(topLevelEntityPath);".)
     */
    {
        File oldTopLevelEntity(topLevelEntityPath);
        if (oldTopLevelEntity.exists()) {
            oldTopLevelEntity.writeIntoString(oldTleContent);
        }
        else {
            noNewToolchainRunRequired = false;
        }
    }

    /* Create **binaryName**.vhd (Replace placeholders in tle.template) */
    this->copyTemplate("tle.template", topLevelEntityPath);

    File topLevelEntity(topLevelEntityPath);
    topLevelEntity.findAndReplace("%name", binaryName);
    topLevelEntity.findAndReplace("%user_gpios", user_gpios.str());
    topLevelEntity.findAndReplace("%wbslaves", wbslavesTle.str());
    topLevelEntity.findAndReplace("%customsignals", customsignals.str());
    topLevelEntity.findAndReplace("%wbinterconslaves", wbslavesInterconTle.str());
    topLevelEntity.findAndReplace("%cores", cores.str());

    if (noNewToolchainRunRequired) {
        std::string newTleContent;
        topLevelEntity.writeIntoString(newTleContent);

        noNewToolchainRunRequired = !newTleContent.compare(oldTleContent);
    }

    if (noNewToolchainRunRequired && File(binaryName.append(".bin")).exists()) {
        return NO_NEW_BUILD_NECCESSARY;
    }
    else {
        return NEW_BUILD_NECCESSARY;
    }
}

bool Generator::copyTemplate(std::string sourceFileName, std::string targetPath)
{
    /* delete a possibly existing file */
    std::stringstream ss1;
    ss1 << "rm -rf " << targetPath;
    system(ss1.str().c_str());

    /* create source path */
    std::stringstream ss2;
    ss2 << _TEMPLATES_DIRECTORY << "/" << sourceFileName;
    std::string sourcePath = ss2.str();

    /* copy template */
    std::stringstream ss3;
    ss3 << "cp " << sourcePath << " " << targetPath;
    system(ss3.str().c_str());

    return true;
}

bool Generator::replaceAll(std::string& target, std::string placeholder, std::string replacement)
{
    bool success = false;

    int32_t pos;
    while ((pos = target.find(placeholder)) != std::string::npos) {
        target.replace(pos, placeholder.size(), replacement);
        success = true;
    }

    return success;
}

bool Generator::executeAndWriteToLog(const char* instruction, FILE* logFile)
{
    char c;

    FILE* step = popen(instruction, "r");

    if (step == NULL) {
        return false;
    }

    while ((c = fgetc(step)) != EOF) {
        fprintf(logFile, "%c", c);
    }

    if (pclose(step) == 0) {
        return true;
    }
    else {
        return false;
    }
}

bool Generator::runToolchain(std::string binaryName, std::string directory)
{
    bool success = true;
    std::stringstream ss0;
    ss0 << binaryName << ".log";
    FILE* logFile = fopen(ss0.str().c_str(), "w");

    /* XST */
    Log().Get(INFO) << "Step 1/5: XST";
    //Log().Get(DEBUG) << std::string("xst -ifn ").append(directory).append("/xst-script");
    success = this->executeAndWriteToLog(std::string("xst -ifn ").append(directory).append("/xst-script").c_str(), logFile);

    /* NGDBUILD */
    if (success) {
        Log().Get(INFO) << "Step 2/5: NGDBUILD";
        std::stringstream ss1;
        ss1 << "ngdbuild -uc " << _SOC_DIRECTORY << "/easyFPGA.ucf -aul " << directory << "/" << binaryName << ".ngc " << directory << "/" << binaryName << ".ngd";
        success = this->executeAndWriteToLog(ss1.str().c_str(), logFile);
    }
    else {
        Log().Get(WARNING) << "Step 2/5: NGDBUILD skipped because errors occured!";
    }

    /* MAP */
    if (success) {
        Log().Get(INFO) << "Step 3/5: MAP";
        std::stringstream ss2;
        ss2 << "map -p xc6slx9-tqg144-2 -w -o " << directory << "/" << binaryName << "-before-par.ncd " << directory << "/" << binaryName << ".ngd";
        success = this->executeAndWriteToLog(ss2.str().c_str(), logFile);
    }
    else {
        Log().Get(WARNING) << "Step 3/5: MAP skipped because errors occured!";
    }

    /* PAR */
    if (success) {
        Log().Get(INFO) << "Step 4/5: PAR";
        std::stringstream ss3;
        ss3 << "par -w " << binaryName << "-before-par.ncd " << directory << "/" << binaryName << ".ncd";
        success = this->executeAndWriteToLog(ss3.str().c_str(), logFile);
    }
    else {
        Log().Get(WARNING) << "Step 4/5: PAR skipped because errors occured!";
    }

    /* BITGEN */
    if (success) {
        Log().Get(INFO) << "Step 5/5: BITGEN";
        std::stringstream ss4;
        ss4 << "bitgen -w -g binary:yes -g compress " << directory << "/" << binaryName << ".ncd";
        success = this->executeAndWriteToLog(ss4.str().c_str(), logFile);
    }
    else {
        Log().Get(WARNING) << "Step 5/5: BITGEN skipped because errors occured!";
    }

    fclose(logFile);

    return success;
}

bool Generator::cleanupBuildDirectory(std::string directory, std::string binaryName, bool deleteOwnHdl)
{
    /* delete generated files from our generator */
    std::stringstream xstScript;
    xstScript << "rm -rf " << directory << "/xst-script";
    system(xstScript.str().c_str());

    std::stringstream xstProject;
    xstProject << "rm -rf " << directory << "/xst-project";
    system(xstProject.str().c_str());

    if (deleteOwnHdl) {
        std::stringstream tle;
        tle << "rm -rf " << directory << "/" << binaryName << ".vhd";
        system(tle.str().c_str());

        std::stringstream intercon;
        intercon << "rm -rf " << directory << "/" << binaryName << "_intercon.vhd";
        system(intercon.str().c_str());
    }

    std::stringstream objectFile;
    objectFile << "rm -rf " << directory << "/" << binaryName << ".o";
    system(objectFile.str().c_str());

    std::stringstream generator;
    generator << "rm -rf " << directory << "/" << binaryName;
    system(generator.str().c_str());

    /* delete auto generated files and directories from the toolchain */
    std::list<std::string> directoriesToRemove;
    directoriesToRemove.push_back("_xmsgs");
    directoriesToRemove.push_back("xst");
    directoriesToRemove.push_back("xlnx_*");

    Directory dir(directory);
    dir.removeSubDirectories(directoriesToRemove);

    system(std::string("rm -rf ").append(directory).append("/*.bgn").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.bld").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.drc").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.ncd").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.ngd").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.pad").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.par").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.pcf").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.ptwx").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.unroutes").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.xpi").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.map").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.html").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.xrpt").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.xwbt").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.txt").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.csv").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.ngm").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.xml").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.mrp").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.ngc").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.bit").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.lst").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.srp").c_str());
    system(std::string("rm -rf ").append(directory).append("/*.lso").c_str());
    system(std::string("rm -rf ").append(directory).append("/webtalk.log").c_str());

    return true;
}

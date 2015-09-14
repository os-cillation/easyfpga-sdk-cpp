/*
 *  This file is part of easyFPGA.
 *  Copyright 2013-2015 os-cillation GmbH
 *
 *  Authors: Johannes Hein <support@os-cillation.de>
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

#include "UartTestFpga.h"

#include "easyfpga/communication/communicator.h"
#include "easyfpga/easycores/uart/uart.h"
#include "easyfpga/easycores/uart/uart_ptr.h"
#include "easyfpga/utils/log/log.h"
#include "easyfpga/utils/unittest/tester.h"

#include <unistd.h> /* usleep(1) */

std::string strToSend("Wir lieben Siegen!");
uint32_t progress = 0;
std::string receivedStr("");

uart_test_fpga_ptr fpga = std::make_shared<UartTestFpga>();

void sendISR(void)
{
    uart_ptr sender = fpga->getUart1();
    uart_ptr receiver = fpga->getUart2();

    Uart::INTERRUPT interrupt = Uart::INTERRUPT::RX_AVAILABLE;
    sender->identifyInterrupt(&interrupt);

    byte c = ' ';

    switch (interrupt) {
        case Uart::INTERRUPT::RX_AVAILABLE:
            Log().Get(INFO) << "Sender Interrupt: RX_AVAILABLE";
            break;

        case Uart::INTERRUPT::TX_EMPTY:
            Log().Get(INFO) << "Sender Interrupt: TX_EMPTY";

            if (progress < strToSend.length()) {
                c = strToSend[progress++];
                Log().Get(INFO) << "Sender: Send character '" << c << "' (0x" << std::hex << (int32_t)c << ")";
                sender->transmit(c);
            }

            break;

        case Uart::INTERRUPT::RX_LINE_STATUS:
            Log().Get(INFO) << "Sender Interrupt: RX_LINE_STATUS";
            break;

        case Uart::INTERRUPT::MODEM_STATUS:
            Log().Get(INFO) << "Sender Interrupt: MODEM_STATUS";
            break;

        case Uart::INTERRUPT::CHARACTER_TIMEOUT:
            Log().Get(INFO) << "Sender Interrupt: CHARACTER_TIMEOUT";
            break;

        default:
            Log().Get(INFO) << "Sender: ERROR!";
    }

    sender->disableInterrupts();
    receiver->enableInterrupt(Uart::INTERRUPT::RX_AVAILABLE);

    fpga->enableInterrupts();
}

void receiveISR(void)
{
    uart_ptr sender = fpga->getUart1();
    uart_ptr receiver = fpga->getUart2();

    Uart::INTERRUPT interrupt = Uart::INTERRUPT::RX_AVAILABLE;
    receiver->identifyInterrupt(&interrupt);

    byte b = (byte)0x00;

    switch (interrupt) {
        case Uart::INTERRUPT::CHARACTER_TIMEOUT:
            Log().Get(INFO) << "Receiver Interrupt: CHARACTER_TIMEOUT";

            receiver->receive(&b);

            Log().Get(INFO) << "Receiver: Received character '" << (char)b << "'";
            receivedStr += (char)b;

            break;

        case Uart::INTERRUPT::RX_AVAILABLE:
            Log().Get(INFO) << "Receiver Interrupt: RX_AVAILABLE";

            receiver->receive(&b);

            Log().Get(INFO) << "Receiver: Received character '" << (char)b << "'";
            receivedStr += (char)b;

            break;

        case Uart::INTERRUPT::TX_EMPTY:
            Log().Get(INFO) << "Receiver Interrupt: TX_EMPTY";
            break;

        case Uart::INTERRUPT::RX_LINE_STATUS:
            Log().Get(INFO) << "Receiver Interrupt: RX_LINE_STATUS";
            break;

        case Uart::INTERRUPT::MODEM_STATUS:
            Log().Get(INFO) << "Receiver Interrupt: MODEM_STATUS";
            break;

        default:
            Log().Get(INFO) << "Receiver: ERROR!";
    }

    receiver->disableInterrupts();
    sender->enableInterrupt(Uart::INTERRUPT::TX_EMPTY);

    fpga->enableInterrupts();
}

/**
 * \brief Tests sync functionality of the Uart core
 */
class UartTest : public Tester
{
    std::string testName(void) {
        return "uart sync test";
    }

    bool testMethod(void) {
        if (!fpga->init(0, "~/repositories/easyfpga-sdk-cpp/sdk/src/easycores/uart/test/sync/UartTestFpga.bin")) {
            return false;
        }

        uart_ptr receiver = fpga->getUart2();

        Log().Get(INFO) << "Receiver: Init UART";
        receiver->init(1000000, Uart::WORD_LENGTH::C8, Uart::PARITY::NO_PARITY, Uart::STOP_BIT_COUNT::ONE_BIT);
        Log().Get(INFO) << "Receiver: Register callback function";
        receiver->registerCallback(receiveISR);
        Log().Get(INFO) << "Receiver: Enable all interrupt sources";
        receiver->enableInterrupt(Uart::INTERRUPT::RX_AVAILABLE);

        uart_ptr sender = fpga->getUart1();

        Log().Get(INFO) << "Sender: Init UART";
        sender->init(1000000, Uart::WORD_LENGTH::C8, Uart::PARITY::NO_PARITY, Uart::STOP_BIT_COUNT::ONE_BIT);
        Log().Get(INFO) << "Sender: Register callback function";
        sender->registerCallback(sendISR);
        Log().Get(INFO) << "Sender: Enable all interrupt sources";
        sender->enableInterrupt(Uart::INTERRUPT::TX_EMPTY);

        Log().Get(INFO) << "We want to transmit the following string: '" << strToSend << "'";

        fpga->enableInterrupts();

        while (strToSend.compare(receivedStr) != 0) {
            /*
             * Give back control to the framework for recognizing the
             * interrupts.
             */
            fpga->handleReplies();
            usleep(5);
        }

        return true;
    }
};

int main(int argc, char** argv)
{
    UartTest test;
    return (uint32_t)test.runTest();
}

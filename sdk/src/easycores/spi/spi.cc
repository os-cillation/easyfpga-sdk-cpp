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

#include "easycores/spi/spi.h"
#include "easycores/pin.h"
#include "easycores/register.h"
#include "utils/log/log.h"

#include <sstream>

Spi::Spi() :
    EasyCore(UNIQUE_CORE_NUMBER),
    _transmittedOnly(0)
{
    _pinMap.insert(std::make_pair(PIN::SCK, std::make_shared<Pin>("sck_out", &_index, PIN::SCK, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::MOSI, std::make_shared<Pin>("mosi_out", &_index, PIN::MOSI, PIN_DIRECTION_TYPE::OUT)));
    _pinMap.insert(std::make_pair(PIN::MISO, std::make_shared<Pin>("miso_in", &_index, PIN::MISO, PIN_DIRECTION_TYPE::IN)));

    _registerMap.insert(std::make_pair(REGISTER::SPCR, std::make_shared<Register>(this, (byte)0x00, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::SPSR, std::make_shared<Register>(this, (byte)0x01, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::SPDR, std::make_shared<Register>(this, (byte)0x02, REGISTER_ACCESS_TYPE::READWRITE)));
    _registerMap.insert(std::make_pair(REGISTER::SPER, std::make_shared<Register>(this, (byte)0x03, REGISTER_ACCESS_TYPE::READWRITE)));
}

Spi::~Spi()
{
}

std::string Spi::getHdlInjections(void)
{
    std::stringstream ss;
    ss << "vhdl work \"%dir/easy_cores/spi/spi.vhd\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/spi/simple_spi_top.v\"" << std::endl;
    ss << "verilog work \"%dir/easy_cores/spi/fifo4.v\"" << std::endl;

    return ss.str();
}

std::string Spi::getHdlTemplate(void)
{
    std::stringstream ss;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "SPI%d : entity work.spi"                                                         << std::endl;
    ss << "-------------------------------------------------------------------------------" << std::endl;
    ss << "   %generic_map"                                                                 << std::endl;
    ss << "   port map ("                                                                   << std::endl;
    ss << "      --clk_in   => gclk_s,"                                                     << std::endl;
    ss << "      wbs_in   => wbs%d_in_s,"                                                   << std::endl;
    ss << "      wbs_out  => wbs%d_out_s%connections"                                       << std::endl;
    ss << "   );"                                                                           << std::endl;

    return ss.str();
}

bool Spi::init(SPI_MODE mode, CLOCK_SPEED speed)
{
    /* PARAMETER CHECK */
    /*
     * Mode | CPOL | CPHA
     *   0  |  0   |  0
     *   1  |  0   |  1
     *   2  |  1   |  0
     *   3  |  1   |  1
     */
    bool cpol; /* CPOL */
    bool cpha; /* CPHA */

    switch (mode) {
        case MODE_0:
            cpol = false;
            cpha = false;
            break;

        case MODE_1:
            cpol = false;
            cpha = true;
            break;

        case MODE_2:
            cpol = true;
            cpha = false;
            break;

        case MODE_3:
            cpol = true;
            cpha = true;
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The mode parameter is not valid.";
            return false;
    }

    /*
     * Divider |   SPCR   |   ESPR
     *    2    | XXXXXX00 | XXXXXX00
     *    4    | XXXXXX01 | XXXXXX00
     *    8    | XXXXXX00 | XXXXXX01 <- note this! ;-)
     *    16   | XXXXXX10 | XXXXXX00
     *    32   | XXXXXX11 | XXXXXX00
     *    64   | XXXXXX01 | XXXXXX01
     *   128   | XXXXXX10 | XXXXXX01
     *   256   | XXXXXX11 | XXXXXX01
     *   512   | XXXXXX00 | XXXXXX10
     *   1024  | XXXXXX01 | XXXXXX10
     *   2048  | XXXXXX10 | XXXXXX10
     *   4096  | XXXXXX11 | XXXXXX10
     */
    bool spcr0;
    bool spcr1;
    bool espr0;
    bool espr1;

    switch (speed) {
        case CLOCK_SPEED::SCK_40_MHZ:
            spcr0 = false;
            spcr1 = false;
            espr0 = false;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_20_MHZ:
            spcr0 = true;
            spcr1 = false;
            espr0 = false;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_10_MHZ:
            spcr0 = false;
            spcr1 = false;
            espr0 = true;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_5_MHZ:
            spcr0 = false;
            spcr1 = true;
            espr0 = false;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_2500_KHZ:
            spcr0 = true;
            spcr1 = true;
            espr0 = false;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_1250_KHZ:
            spcr0 = true;
            spcr1 = false;
            espr0 = true;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_625_KHZ:
            spcr0 = false;
            spcr1 = true;
            espr0 = true;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_312500_HZ:
            spcr0 = true;
            spcr1 = true;
            espr0 = true;
            espr1 = false;
            break;

        case CLOCK_SPEED::SCK_156250_HZ:
            spcr0 = false;
            spcr1 = false;
            espr0 = false;
            espr1 = true;
            break;

        case CLOCK_SPEED::SCK_78125_HZ:
            spcr0 = true;
            spcr1 = false;
            espr0 = false;
            espr1 = true;
            break;

        case CLOCK_SPEED::SCK_39063_HZ:
            spcr0 = false;
            spcr1 = true;
            espr0 = false;
            espr1 = true;
            break;

        case CLOCK_SPEED::SCK_19531_HZ:
            spcr0 = true;
            spcr1 = true;
            espr0 = false;
            espr1 = true;
            break;

        default:
            Log().Get(WARNING) << "Init method fails. The speed parameter is not valid.";
            return false;
    }

    /* PERFORM AN ACTION DEPENDING ON MODE */
    bool success = true;

    switch(_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            /* Disables core */
            success &= this->disableCore();

            /* Set SPI mode */
            success &= this->getRegister(REGISTER::SPCR)->changeBitSync(2, cpha);
            success &= this->getRegister(REGISTER::SPCR)->changeBitSync(3, cpol);

            /* Set SPI clock by setting the clock divider */
            success &= this->getRegister(REGISTER::SPCR)->changeBitSync(0, spcr0);
            success &= this->getRegister(REGISTER::SPCR)->changeBitSync(1, spcr1);
            success &= this->getRegister(REGISTER::SPER)->changeBitSync(0, espr0);
            success &= this->getRegister(REGISTER::SPER)->changeBitSync(1, espr1);

            /* Enables core */
            success &= this->enableCore();

            return success;

        /** \todo Support async mode */
        case OPERATION_MODE::ASYNC:
            Log().Get(WARNING) << "Async mode not yet supported. Aborting ...";
            return false;
    }

    return false;
}

bool Spi::transceive(byte txData, byte* rxData)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    byte readDontCare = (byte)0x00;
    byte spsr = (byte)0x00;
    uint8_t dummyReads;

    switch(_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            /* Perform dummy reads if necessary */
            dummyReads = _transmittedOnly%4;
            if (dummyReads > 0) {
                for (uint8_t i = 0; i < dummyReads; i++) {
                    if (!this->getRegister(REGISTER::SPDR)->readSync(&readDontCare)) {
                        return false;
                    }
                }
            }

            /* Receive FIFO will now be aligned */
            _transmittedOnly = 0;

            /* Transmit */
            if (!this->getRegister(REGISTER::SPDR)->writeSync(txData)) {
                return false;
            }

            /* Wait until read FIFO is not empty */
            if (!this->getRegister(REGISTER::SPSR)->readSync(&spsr)) {
                return false;
            }
            while (setBitTest(spsr, 0)) {
                if (!this->getRegister(REGISTER::SPSR)->readSync(&spsr)) {
                    return false;
                }
                usleep(10);
            }

            /* Read from receive FIFO */
            if (!this->getRegister(REGISTER::SPDR)->readSync(rxData)) {
                return false;
            }
            return true;

        /** \todo Support async mode */
        case OPERATION_MODE::ASYNC:
            Log().Get(WARNING) << "Async mode not yet supported. Aborting ...";
            return false;
    }

    return false;
}

bool Spi::transmit(byte txData)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    switch(_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:
            /* Write to TX FIFO */
            if (!this->getRegister(REGISTER::SPDR)->writeSync(txData)) {
                return false;
            }

            /* Remember that foolish byte in receive FIFO */
            _transmittedOnly++;
            return true;

        /** \todo Support async mode */
        case OPERATION_MODE::ASYNC:
            Log().Get(WARNING) << "Async mode not yet supported. Aborting ...";
            return false;
    }

    return false;
}

bool Spi::transmit(byte* txData, uint8_t length)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    uint8_t transmitted = 0;

    switch(_OPERATION_MODE) {
        case OPERATION_MODE::SYNC:

            while(transmitted < length) {

                /* Wait until write fifo is empty */
                while (!isWriteFifoEmpty()) {
                    usleep(10);
                }

                /* Transmit four bytes (FIFO length) */
                byte packet[4];
                std::copy(txData+transmitted, txData+transmitted+4, packet);
                if (!this->getRegister(REGISTER::SPDR)->writeMultiTimesSync(packet, 4)) {
                    Log().Get(WARNING) << "Failed to transmit four byte packet";
                    return false;
                }
                transmitted+=4;
                _transmittedOnly+=4; //actually not required...
            }
            return true;

        /** \todo Support async mode */
        case OPERATION_MODE::ASYNC:
            Log().Get(WARNING) << "Async mode not yet supported. Aborting ...";
            return false;
    }

    return false;
}

bool Spi::receive(byte* rxData)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    /* Use the transceive method and transmit a dummy byte */
    /** \todo Support async mode */
    return transceive((byte)0x00, rxData);
}

bool Spi::resetBuffers(void)
{
    /* PARAMETER CHECK */

    /* PERFORM AN ACTION DEPENDING ON MODE */
    /** \todo Support async mode */
    bool success = true;
    success &= this->disableCore();
    success &= this->enableCore();

    if (success) {
        _transmittedOnly = 0;
    }
    return success;
}

bool Spi::enableCore(void)
{
    return this->getRegister(REGISTER::SPCR)->changeBitSync(6, true);
}

bool Spi::disableCore(void)
{
    return this->getRegister(REGISTER::SPCR)->changeBitSync(6, false);
}

bool Spi::isWriteFifoFull(void)
{
    byte status = (byte)0xFF;
    if (!this->getRegister(REGISTER::SPSR)->readSync(&status)) {
        Log().Get(WARNING) << "Failed to read status register";
    }
    return setBitTest(&status, 3);
}

bool Spi::isWriteFifoEmpty(void)
{
    byte status = (byte)0xFF;
    if (!this->getRegister(REGISTER::SPSR)->readSync(&status)) {
        Log().Get(WARNING) << "Failed to read status register";
    }
    return setBitTest(&status, 2);
}

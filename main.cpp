

/*
 * Copyright (C) 2018 by IMDEA Networks Institute
 *
 * This file is part of Electrosense.
 *
 * Electrosense is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Electrosense is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RTL-Spec.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 	Authors: 	Roberto Calvo-Palomino <roberto.calvo@imdea.org>
 *
 */

#include <iostream>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>

// Workaround issue #4 , complex.h breaks openssl's RSA library
//   include RSA before any mention to complex.h (in rtlsdrDriver.h)
#include "MiscBlocks/AvroSerialization.h"
#include "MiscBlocks/FileSink.h"
#include "MiscBlocks/IQSink.h"
#include "MiscBlocks/IQStream.h"
#include "MiscBlocks/Transmission.h"
#include "ProcessingBlocks/Averaging.h"
#include "ProcessingBlocks/FFT.h"
#include "ProcessingBlocks/PSDFast.h"
#include "ProcessingBlocks/RemoveDC.h"
#include "ProcessingBlocks/RemoveDCRTL.h"
#include "ProcessingBlocks/Windowing.h"
#include "context/OpenRFSenseContext.h"
#include "drivers/Component.h"
#include "drivers/rtlsdr/rtlsdrDriver.h"
#include "generated/version_config.h"

void usage(char *name) {
    fprintf(
        stdout,
        "Usage:\n"
        "  %s sensor_id campaign_id min_freq max_freq\n"
        "  [-h]\n"
        "  [-d <dev_index>]\n"
        "  [-c <clk_off>] [-k <clk_corr_period>]\n"
        "  [-g <gain>]\n"
        "  [-y <hopping_strategy>]\n"
        "  [-s <samp_rate>]\n"
        "  [-q <freq_overlap>]\n"
        "  [-t <monitor_time>] [-r <min_time_res>]\n"
        "  [-l <cmpr_level>]\n"
        "  [-m "
        "<hostname1>:<portnumber1>[;<bandwidth1>],...,<hostnameN>:<portnumberN>"
        "[;"
        "bandwidthN]]\n"
        "  [-n "
        "<hostname1>:<portnumber1>[;<bandwidth1>],...,<hostnameN>:<portnumberN>"
        "[;"
        "<bandwidthN>]#<ca_cert>#<cert>#<key>]\n"
        "  [-z <pipeline>]\n"
        "\n"
        "  PSD Pipeline\n"
        "    [-f <log2_fft_size>] [-b <fft_batchlen>]\n"
        "    [-a <avg_factor>] [-o <soverlap>] \n"
        "    [-w <window>]\n"
        "\n"
        "  IQ Pipeline\n"
        "    [-x <absolute_time>]\n"
        "\n"
        "Arguments:\n"
        "  sensor_id              Unique sensor ID\n"
        "  campaign_id            Unique campaign ID\n"
        "  min_freq               Lower frequency bound in Hz\n"
        "  max_freq               Upper frequency bound in Hz\n"
        "\n"
        "Options:\n"
        "  -h                     Show this help\n"
        "  -d <dev_index>         RTL-SDR device index [default=%i]\n"
        "  -z <pipeline>          Pipeline process: PSD | IQ [default=%s]\n"
        "  -c <clk_off>           Clock offset in PPM [default=%i]\n"
        "  -k <clk_corr_period>   Clock correction period in seconds "
        "[default=%u]\n"
        "                           i.e. perform frequency correction every "
        "'clk_corr_period'\n"
        "                           seconds\n"
        "  -g <gain>              Gain value in dB [default=%.1f]\n"
        "                           -1 for automatic gain\n"
        "  -y <hopping_strategy>  Hopping strategy to use [default=%s]\n"
        "                           sequential\n"
        "                           random\n"
        "                           similarity\n"
        "  -s <samp_rate>         Sampling rate in Hz [default=%u]\n"
        "  -q <freq_overlap>      Frequency overlapping factor [default=%.3f]\n"
        "                           i.e. the frequency width is reduced from "
        "'samp_rate' to\n"
        "                           (1-'freq_overlap')*'samp_rate'\n"
        "  -t <monitor_time>      Time in seconds to monitor [default=%u]\n"
        "                           0 to monitor infinitely\n"
        "  -r <min_time_res>      Minimal time resolution in seconds "
        "[default=%u]\n"
        "                           0 for no time resolution limitation\n"
        "  -w <window>            Windowing function [default=%s]\n"
        "                           hanning\n"
        "                           rectangular\n"
        "                           blackman_harris_4\n"
        "  -l <cmpr_level>        Compression level [default=%u]\n"
        "                           0 for no compression, fastest\n"
        "                           9 for highest compression, slowest\n"
        "  -m <path>              Avro schema file path [default=%s]\n"
        "  -n <hostname>:<portnumber>#<ca_cert>#<cert>#<key>\n"
        "                         SSL/TLS collector host [default=%s]\n"
        "                           0 for no SSL/TLS collector\n"
        "                           SSL/TLS (CA) certificate and private key "
        "files\n"
        "  -p <true|false>        Set FIFO priority to the sampling thread "
        "[default=%s]\n"
        "                           true\n"
        "                           false\n"
        "  -u <filename>          Set filename output where Spectrum "
        "measurements are saved.\n"
        "\n"
        "PSD PIPELINE\n"
        "  -f <log2_fft_size>     Use FFT size of 2^'log2_fft_size' "
        "[default=%u]\n"
        "                           the resulting frequency resolution is\n"
        "                           'samp_rate'/(2^'log2_fft_size')\n"
        "  -b <fft_batchlen>      FFT batch length [default=%u]\n"
        "                           i.e. process FFTs in batches of length "
        "'fft_batchlen'\n"
        "  -a <avg_factor>        Averaging factor [default=%u]\n"
        "                           i.e. average 'avg_factor' segments\n"
        "  -o <soverlap>          Segment overlap [default=%u]\n"
        "                           i.e. number of samples per segment that "
        "overlap\n"
        "                           The time to dwell in seconds at a given "
        "frequency is given by\n"
        "                           "
        "(((1<<'log2_fft_size')-'soverlap')*'avg_factor'+'soverlap')/"
        "'samp_rate'\n"
        "IQ PIPELINE\n"
        "  -x <absolute_time>     Absolute time when receiver must start "
        "sampling\n"
        "\n"
        "",
        name, OpenRFSenseContext::getInstance()->getDevIndex(),
        OpenRFSenseContext::getInstance()->getPipeline().c_str(),
        OpenRFSenseContext::getInstance()->getClkOffset(),
        OpenRFSenseContext::getInstance()->getClkCorrPerior(),
        OpenRFSenseContext::getInstance()->getGain(),
        OpenRFSenseContext::getInstance()->getHoppingStrategy().c_str(),
        OpenRFSenseContext::getInstance()->getSamplingRate(),
        OpenRFSenseContext::getInstance()->getFreqOverlap(),
        OpenRFSenseContext::getInstance()->getMonitorTime(),
        OpenRFSenseContext::getInstance()->getMinTimeRes(),
        OpenRFSenseContext::getInstance()->getWindowing().c_str(),
        OpenRFSenseContext::getInstance()->getComprLevel(),
        OpenRFSenseContext::getInstance()->getSchemaPath().c_str(),
        OpenRFSenseContext::getInstance()->getTlsHosts().c_str(),
        OpenRFSenseContext::getInstance()->isFifoPriority() ? "true" : "false",
        OpenRFSenseContext::getInstance()->getLog2FftSize(),
        OpenRFSenseContext::getInstance()->getFFTbatchlen(),
        OpenRFSenseContext::getInstance()->getAvgFactor(),
        OpenRFSenseContext::getInstance()->getSoverlap());

    exit(0);
}

void parse_args(int argc, char *argv[]) {
    int opt;
    const char *options = "hd:z:c:k:g:y:s:f:b:a:o:x:q:t:r:w:l:m:n:u:p";

    // Option arguments
    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
        case 'a':
            OpenRFSenseContext::getInstance()->setAvgFactor(atol(optarg));
            break;
        case 'b':
            OpenRFSenseContext::getInstance()->setFFTbatchlen(atol(optarg));
            break;
        case 'c':
            OpenRFSenseContext::getInstance()->setClkOff(atoi(optarg));
            break;
        case 'd':
            OpenRFSenseContext::getInstance()->setDevIndex(atoi(optarg));
            break;
        case 'f':
            OpenRFSenseContext::getInstance()->setLog2FftSize(atol(optarg));
            break;
        case 'g':
            if (atof(optarg) < -1) {
                std::cout << "Error usage: Gain can not be negative!\n" << std::endl;
                usage(argv[0]);
                break;
            }
            OpenRFSenseContext::getInstance()->setGain(atof(optarg));
            break;
        case 'h':
            usage(argv[0]);
            break;
        case 'k':
            OpenRFSenseContext::getInstance()->setClkCorrPerior(atoi(optarg));
            break;
        case 'l':
            OpenRFSenseContext::getInstance()->setComprLevel(atol(optarg));
            break;
        case 'o':
            OpenRFSenseContext::getInstance()->setSoverlap(atol(optarg));
            break;
        case 'm':
            OpenRFSenseContext::getInstance()->setSchemaPath(strdup(optarg));
            break;
        case 'n':
            OpenRFSenseContext::getInstance()->setTlsHosts(strdup(optarg));
            break;
        case 'p':
            OpenRFSenseContext::getInstance()->setFifoPriority(1);
            break;
        case 'q':
            OpenRFSenseContext::getInstance()->setFreqOverlap(atof(optarg));
            break;
        case 'r':
            OpenRFSenseContext::getInstance()->setMinTimeRes(atol(optarg));
            break;
        case 's':
            OpenRFSenseContext::getInstance()->setSamplingRate(atol(optarg));
            break;
        case 't':
            OpenRFSenseContext::getInstance()->setMonitorTime(atol(optarg));
            break;
        case 'u':
            OpenRFSenseContext::getInstance()->setOutputFileName(strdup(optarg));
            break;
        case 'w':
            OpenRFSenseContext::getInstance()->setWindowing(strdup(optarg));
            break;
        case 'x':
            OpenRFSenseContext::getInstance()->setStartTimeSampling(atol(optarg));
            break;
        case 'y':
            OpenRFSenseContext::getInstance()->setHoppingStrategy(strdup(optarg));
            break;
        case 'z':
            OpenRFSenseContext::getInstance()->setPipeline(strdup(optarg));
            break;

        default:
            fprintf(stderr, "Unknown option '-%c'\n\n", opt);
            usage(argv[0]);
        }
    }

    // Non-option arguments
    if (optind + 4 != argc) {
        fprintf(stderr, "Malformed arguments\n\n");
        usage(argv[0]);
    } else {
        OpenRFSenseContext::getInstance()->setSensorId(argv[optind]);
        OpenRFSenseContext::getInstance()->setCampaignId(argv[optind + 1]);
        OpenRFSenseContext::getInstance()->setMinFreq(atoll(argv[optind + 2]));
        OpenRFSenseContext::getInstance()->setMaxFreq(atoll(argv[optind + 3]));
    }
}

std::vector<Component *> vComponents;

void shutdown() {
    std::cout << std::endl << "Shutdown components ..." << std::endl;
    for (unsigned int i = 0; i < vComponents.size(); i++) {
        std::cout << "  - Stopping component: " << vComponents.at(i)->getNameId()
                  << std::endl;
        vComponents.at(i)->stop();
    }

    std::cout << "Sensing process finished correctly." << std::endl;
}

void signal_callback_handler(int signum) {
    std::cout << "signal_callback_handler " << signum << std::endl;
    shutdown();
    exit(signum);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    std::cout << std::endl
              << "OpenRFSense sensing application " << orfs::version::version() << " ("
              << orfs::version::compilationTime() << ")" << std::endl
              << std::endl;

    parse_args(argc, argv);
    OpenRFSenseContext::getInstance()->print();

    orfs::RemoveDC *rdcBlock;
    orfs::RemoveDCRTL *rdcRTLBlock;
    orfs::Windowing *winBlock;
    orfs::FFT *fftBlock;
    orfs::Averaging *avgBlock = new orfs::Averaging();
    orfs::PSDFast *psdfastBlock;

    // A few checks before start
    if (OpenRFSenseContext::getInstance()->getPipeline().compare("IQ") == 0) {
        if (OpenRFSenseContext::getInstance()->getMinFreq() !=
            OpenRFSenseContext::getInstance()->getMaxFreq()) {
            std::cerr << "[ERROR] Min and Max frequencies in IQ mode must be "
                         "the same!"
                      << std::endl;
            exit(-1);
        }
    }

    // A few checks before start
    if (OpenRFSenseContext::getInstance()->getPipeline().compare("PSD") == 0) {
        if (OpenRFSenseContext::getInstance()->getMinFreq() >=
            OpenRFSenseContext::getInstance()->getMaxFreq()) {
            std::cerr << "[ERROR] Min frequency must be lower than Max frequency!"
                      << std::endl;
            exit(-1);
        }
    }

    // RTL-SDR Driver
    auto *rtlDriver = new orfs::rtlsdrDriver();
    vComponents.push_back(rtlDriver);

    try {
        rtlDriver->open(OpenRFSenseContext::getInstance()->getDevIndex());
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        shutdown();
        exit(-1);
    }
    
    orfs::IQStream *iqStream = new orfs::IQStream();

    if (OpenRFSenseContext::getInstance()->getPipeline().compare("PSD") == 0) {
        // RemoveDC Block
        rdcBlock = new orfs::RemoveDC();
        rdcBlock->setQueueIn(rtlDriver->getQueueOut());
        vComponents.push_back(rdcBlock);

        // Windowing
        winBlock = new orfs::Windowing(orfs::Windowing::HAMMING);
        winBlock->setQueueIn(rdcBlock->getQueueOut());
        vComponents.push_back(winBlock);

        // FFT
        fftBlock = new orfs::FFT();
        fftBlock->setQueueIn(winBlock->getQueueOut());
        vComponents.push_back(fftBlock);

        // Averaging
        avgBlock->setQueueIn(fftBlock->getQueueOut());
        vComponents.push_back(avgBlock);
    } else if (OpenRFSenseContext::getInstance()->getPipeline().compare("IQ") == 0) {
        // Comment the following to be able to run in RPi0
        /*rdcRTLBlock = new orfs::RemoveDCRTL();
        vComponents.push_back(rdcRTLBlock);
        rdcRTLBlock->setQueueIn(rtlDriver->getQueueOut());
        */
    } else if (OpenRFSenseContext::getInstance()->getPipeline().compare("DEC") == 0) {
        // PSD Fast block
        psdfastBlock = new orfs::PSDFast();

        vComponents.push_back(psdfastBlock);
        psdfastBlock->setQueueIn(rtlDriver->getQueueOut2());

        // IQStream
        vComponents.push_back(iqStream);
        iqStream->setQueueIn(rtlDriver->getQueueOut());
    }

    orfs::Transmission *transBlock;

    // Send measurements to the server.
    if (OpenRFSenseContext::getInstance()->getTlsHosts().compare(DEFAULT_TLS_HOSTS) !=
        0) {
        auto *avroBlock = new orfs::AvroSerialization();

        if (OpenRFSenseContext::getInstance()->getPipeline().compare("PSD") == 0) {
            avroBlock->setQueueIn(avgBlock->getQueueOut());
            vComponents.push_back(avroBlock);
        } else if (OpenRFSenseContext::getInstance()->getPipeline().compare("IQ") == 0) {
            rdcRTLBlock = new orfs::RemoveDCRTL();
            rdcRTLBlock->setQueueIn(rtlDriver->getQueueOut());
            vComponents.push_back(rdcRTLBlock);

            avroBlock->setQueueIn(rdcRTLBlock->getQueueOut());
            vComponents.push_back(avroBlock);
        }

        transBlock = new orfs::Transmission();
        vComponents.push_back(transBlock);
        transBlock->setQueueIn(avroBlock->getQueueOut());
    } else if (
        OpenRFSenseContext::getInstance()->getOutputFileName().compare(
            DEFAULT_OUTPUT_FILENAME) != 0) {
        // Send measurements to a  file.
        if (OpenRFSenseContext::getInstance()->getPipeline().compare("PSD") == 0) {
            auto *fileSink = new orfs::FileSink(
                OpenRFSenseContext::getInstance()->getOutputFileName());
            vComponents.push_back(fileSink);

            fileSink->setQueueIn(avgBlock->getQueueOut());
        } else if (OpenRFSenseContext::getInstance()->getPipeline().compare("IQ") == 0) {
            if (OpenRFSenseContext::getInstance()->getOutputType() == OUTPUT_TYPE_BYTE) {
                auto *iqSink = new orfs::IQSink(
                    OpenRFSenseContext::getInstance()->getOutputFileName());
                iqSink->setQueueIn(rtlDriver->getQueueOut());
                vComponents.push_back(iqSink);
            } else {
                // FLOAT
                rdcRTLBlock = new orfs::RemoveDCRTL();
                rdcRTLBlock->setQueueIn(rtlDriver->getQueueOut());
                vComponents.push_back(rdcRTLBlock);

                auto *iqSink = new orfs::IQSink(
                    OpenRFSenseContext::getInstance()->getOutputFileName());
                iqSink->setQueueIn(rdcRTLBlock->getQueueOut());
                vComponents.push_back(iqSink);
            }
        }
    }

    try {
        std::cout << std::endl << "Starting components ..." << std::endl;
        for (unsigned int i = 0; i < vComponents.size(); i++) {
            std::cout << "  - Starting component: " << vComponents.at(i)->getNameId()
                      << std::endl;
            vComponents.at(i)->start();
        }

        unsigned int current_dec = 0;

        while (1) {
            sleep(1);

            bool watch_components = true;
            for (unsigned int i = 0; i < vComponents.size(); i++) {
                if (!vComponents.at(i)->isRunning()) {
                    std::cout << "[WATCHER] " << vComponents.at(i)->getNameId()
                              << " component is not running. Finishing process ..."
                              << std::endl;
                    watch_components = false;
                }
            }
            if (!watch_components)
                break;

            if (OpenRFSenseContext::getInstance()->getPipeline().compare("DEC") == 0) {
                // Restart IQStream and decoder if 1) decoder changes or 2)
                // decoder (child) dies
                if (OpenRFSenseContext::getInstance()->getDecoder() != current_dec ||
                    (!iqStream->getDecoderState())) {

                    current_dec = OpenRFSenseContext::getInstance()->getDecoder();

                    iqStream->stop();
                    usleep(0.03);

                    if (OpenRFSenseContext::getInstance()->getDecoder() != -1)
                        iqStream->start();
                }
            }
        }
    }

    catch (...) {
        std::cerr << "Connection Server Lost" << std::endl;
    }

    shutdown();

    return 0;
}

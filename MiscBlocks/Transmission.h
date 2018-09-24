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

#ifndef ES_SENSOR_TRANSMISSION_H
#define ES_SENSOR_TRANSMISSION_H

#include <vector>
#include <unistd.h>
#include <string.h>

// Workaround issue #4 , complex.h breaks openssl's RSA library
//   include RSA before any mention to complex.h (in SpectrumSegment.h)
#include "../misc/TLS.h"
#include "../drivers/Component.h"
#include "../drivers/Communication.h"
#include "../context/ElectrosenseContext.h"
#include "../types/SpectrumSegment.h"

#include <netinet/in.h>

namespace electrosense {

    class Transmission: public Component, public Communication<SpectrumSegment*,SpectrumSegment*> {

    public:

        Transmission();

        ~Transmission(){};

        std::string getNameId () { return std::string("Transmission"); };

        int stop();

        ReaderWriterQueue<SpectrumSegment*>* getQueueIn() { return mQueueIn; }
        void setQueueIn (ReaderWriterQueue<SpectrumSegment*>* QueueIn ) { mQueueIn = QueueIn;};

        ReaderWriterQueue<SpectrumSegment*>* getQueueOut() { return NULL; };
        void setQueueOut (ReaderWriterQueue<SpectrumSegment*>* QueueOut) {};

        enum ConnectionType { TCP=0, TLS};

    private:

        void run();
        void parse_tls_hosts();
        void checkConnection();

        bool mRunning;

        ReaderWriterQueue<SpectrumSegment*>* mQueueIn;

        std::string mStrHosts;

        std::string mHost;
        std::string mPort;
        std::string mCACert;
        std::string mCert;
        std::string mKey;

        ConnectionType mConnection;


        TLS_Connection *tls_con;
        TCP_Connection * tcp_con;

    };

}


#endif //ES_SENSOR_TRANSMISSION_H

////////////////////////////////////////////////////////////////////////////////
/// @brief tasks used to establish connections
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2008-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "ListenTask.h"

#include <errno.h>


#ifdef TRI_HAVE_LINUX_SOCKETS
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif 


#ifdef TRI_HAVE_WINSOCK2_H
#include "BasicsC/win-utils.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include <sys/types.h>

#include "Basics/MutexLocker.h"
#include "Basics/StringUtils.h"
#include "BasicsC/socket-utils.h"
#include "Logger/Logger.h"
#include "Scheduler/Scheduler.h"

using namespace triagens::basics;
using namespace triagens::rest;

// -----------------------------------------------------------------------------
// constructors and destructors
// -----------------------------------------------------------------------------

ListenTask::ListenTask (Endpoint* endpoint, bool reuseAddress)
  : Task("ListenTask"),
    readWatcher(0),
    reuseAddress(reuseAddress),
    _endpoint(endpoint),
    listenSocket(0),
    acceptFailures(0) {
  bindSocket();
}


ListenTask::~ListenTask () {
  if (readWatcher != 0) {
    scheduler->uninstallEvent(readWatcher);
  }
}

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

bool ListenTask::isBound () const {
  MUTEX_LOCKER(changeLock);
  
  return _endpoint != 0 && _endpoint->isConnected();
}



// -----------------------------------------------------------------------------
// Task methods
// -----------------------------------------------------------------------------

bool ListenTask::setup (Scheduler* scheduler, EventLoop loop) {
  if (! isBound()) {
    return true;
  }
  
  this->scheduler = scheduler;
  this->loop = loop;
  
  readWatcher = scheduler->installSocketEvent(loop, EVENT_SOCKET_READ, this, listenSocket);
  
  if (readWatcher == -1) {
    return false;
  }
  return true;
}



void ListenTask::cleanup () {
  scheduler->uninstallEvent(readWatcher);
  readWatcher = 0;
}



bool ListenTask::handleEvent (EventToken token, EventType revents) {
  if (token == readWatcher) {
    if ((revents & EVENT_SOCKET_READ) == 0) {
      return true;
    }
    
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    
    memset(&addr, 0, sizeof(addr));
    
    // accept connection
    socket_t connfd = accept(listenSocket, (sockaddr*) &addr, &len);
    if (connfd == INVALID_SOCKET) {
      ++acceptFailures;
      
      if (acceptFailures < MAX_ACCEPT_ERRORS) {
        LOGGER_WARNING << "accept failed with " << errno << " (" << strerror(errno) << ")";
      }
      else if (acceptFailures == MAX_ACCEPT_ERRORS) {
        LOGGER_ERROR << "too many accept failures, stopping logging";
      }
      
      // TODO GeneralFigures::incCounter<GeneralFigures::GeneralServerStatistics::connectErrorsAccessor>();
      
      return true;
    }
    
    acceptFailures = 0;
    
    struct sockaddr_in addr_out;
    socklen_t len_out = sizeof(addr_out);

    int res = getsockname(connfd, (sockaddr*) &addr_out, &len_out);

    if (res != 0) {
      TRI_CLOSE_SOCKET(connfd);
      
      LOGGER_WARNING << "getsockname failed with " << errno << " (" << strerror(errno) << ")";

      // TODO GeneralFigures::incCounter<GeneralFigures::GeneralServerStatistics::connectErrorsAccessor>();
      
      return true;
    }
 
    // disable nagle's algorithm, set to non-blocking and close-on-exec  
    bool result = _endpoint->initIncoming(connfd); 
    if (!result) {
      TRI_CLOSE_SOCKET(connfd);

      // TODO GeneralFigures::incCounter<GeneralFigures::GeneralServerStatistics::connectErrorsAccessor>();

      return true;
    }

    // set client address and port
    ConnectionInfo info;

    char host[NI_MAXHOST], serv[NI_MAXSERV];
    if (getnameinfo((sockaddr*) &addr, len,
                    host, sizeof(host),
                    serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
      
      info.clientAddress = string(host);
      info.clientPort = addr.sin_port;
    }
    else {
      info.clientAddress = inet_ntoa(addr.sin_addr);
      info.clientPort = addr.sin_port;
    }
    
    info.serverAddress = _endpoint->getHost();
    info.serverPort = _endpoint->getPort();
    
    return handleConnected(connfd, info);
  }
  
  return true;
}

// -----------------------------------------------------------------------------
// private methods
// -----------------------------------------------------------------------------

bool ListenTask::bindSocket () {
  listenSocket = _endpoint->connect(30, 300); // connect timeout in seconds
  if (listenSocket == 0) {
    return false;
  }
  
  return true;
}

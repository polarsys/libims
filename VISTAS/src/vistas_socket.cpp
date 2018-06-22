/*******************************************************************************
 * Copyright (c) 2018 Airbus Operations S.A.S                                  *
 *                                                                             *
 * This program and the accompanying materials are made available under the    *
 * terms of the Eclipse Public License v. 2.0 which is available at            *
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License   *
 * v. 1.0 which is available at                                                *
 * http://www.eclipse.org/org/documents/edl-v10.php.                           *
 *                                                                             *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause                            *
 *******************************************************************************/

//
// Abstract base socket class
//
#include "vistas_socket.hh"
#include <unistd.h>
#ifdef __linux
#include <errno.h>
#endif

namespace vistas
{
/*
   * Constant for setsockopt calls
   */
static const __attribute__((__unused__)) int zero = 0;
static const __attribute__((__unused__)) int one = 1;

//
// Initialize a socket
//
void socket::create(socket_address_ptr address, int socket_type)
throw(ims::exception)
{
    if (_sock != INVALID_SOCKET) {
        THROW_IMS_ERROR(ims_init_failure, "Trying to initialize an already initialied socket!");
    }

    // Open a UDP socket
    _sock = ::socket(PF_INET, socket_type, IPPROTO_IP);
    if ( _sock == INVALID_SOCKET ) {
        THROW_IMS_ERROR(ims_init_failure, "Error creating socket.");
    }

    // Allow to reuse port when socket is dying (restart)
    // On Windows, SO_REUSEADDR mean SO_REUSEPORT + SO_REUSEADDR.
    // With the SO_REUSEPORT option, 2 softwares can bind to the same ip/port, which
    // might have stange behaviors...
    if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &one, sizeof(one)) != 0) {
        close();
        THROW_IMS_ERROR(ims_init_failure, "Failed to set SO_REUSEADDR on socket !");
    }

    // Init address member
    _address = address;
}

void socket::close()
{
    if (_sock != INVALID_SOCKET) {
        LOG_DEBUG("Closing socket " << _address->to_string());
        shutdown(_sock, 2);
#ifdef _WIN32
        closesocket(_sock);
#else
        ::close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
}

socket::~socket()
{
    close();
}

char * socket::getlasterror()
{
#ifdef _WIN32
    static char error_buffer[1024];
    
    DWORD dwError = WSAGetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, 0, error_buffer, 1024, NULL);
    return error_buffer;
#else
    return strerror(errno);
#endif
}

bool socket::wouldblock()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EAGAIN;
#endif
}

}

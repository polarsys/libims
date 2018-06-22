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
#ifndef _VISTAS_SOCKET_HH_
#define _VISTAS_SOCKET_HH_
#include "ims_log.hh"
#include "shared_ptr.hh"
#include "vistas_socket_address.hh"
#include <unistd.h>

#ifdef __linux
#include <arpa/inet.h>
#include <string.h>
#define IMS_SOCKET int
#define INVALID_SOCKET (-1)
#endif
#ifdef _WIN32
#define FD_SETSIZE 2048
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#define IMS_SOCKET SOCKET
#endif

#ifdef ENABLE_INSTRUMENTATION
#include "ims_instrumentation.h"
extern ims_socket_handler_t ims_socket_handler_recv;
extern ims_socket_handler_t ims_socket_handler_send;
extern ims_socket_handler_t ims_socket_handler_reply;
#endif

namespace vistas
{

class socket;
typedef shared_ptr<socket> socket_ptr;

class socket
{
    // Store information on emmiter
public:
    struct client
    {
        inline client();
        inline uint32_t get_port();
        struct sockaddr_in saddr;
    };

public:
    // Address associated with this socket
    inline socket_address_ptr get_address();

    // Receive from the socket
    // @return The size effectively read
    // If client is provided, it will be filled with emmiter information
    virtual uint32_t receive(char* buffer, uint32_t buffer_size, client* client = NULL)
    throw(ims::exception) = 0;

    // Write to the socket
    virtual void send(const char* buffer, uint32_t size)
    throw(ims::exception) = 0;

    // Reply to the emmiter
    virtual void reply(client& client, const char* buffer, uint32_t size)
    throw(ims::exception) = 0;

    // DTor: close the socket
    virtual ~socket();

    // Return a string describing the internal address
    // ** This function is slow and does malloc **
    // ** Don't call it at runtime **
    inline std::string to_string();

    // Return associated file descriptor
    inline IMS_SOCKET get_fd();
    
    // get a string with last socket error
    static char * getlasterror();

    // return true if the error was because it would block
    static bool wouldblock();

protected:
    // Create an unintialized socket @see create()
    inline socket();

    // Create the socket
    // - Initialize the _sock member
    // - Fill _address member
    void create(socket_address_ptr address, int socket_type)
    throw(ims::exception);

    // Deinitialize the socket
    void close();

    IMS_SOCKET         _sock;
    socket_address_ptr _address;
};

//***************************************************************************
// Inlines
//***************************************************************************
socket::socket() :
    _sock(INVALID_SOCKET)
{
}

socket_address_ptr socket::get_address()
{
    return _address;
}

std::string socket::to_string()
{
    if (_address) return _address->to_string();
    return "(no address)";
}

IMS_SOCKET socket::get_fd()
{
    return _sock;
}

socket::client::client()
{
    memset(&saddr, 0, sizeof(sockaddr_in));
}

uint32_t socket::client::get_port()
{
    return htons(saddr.sin_port);
}

}
#endif

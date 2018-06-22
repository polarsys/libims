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
// tcp message socket
// 
#include "vistas_socket_tcp_message.hh"
#include <errno.h>
#include <fcntl.h>

#ifdef __linux
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

namespace vistas 
{
/*
 * Constant for setsockopt calls
 */
static const __attribute__((__unused__)) int zero = 0;
static const __attribute__((__unused__)) int one = 1;

//
// Ctor
//
socket_tcp_message::socket_tcp_message(socket_address_ptr address) throw(ims::exception)
{
    
    socket::create(address, SOCK_STREAM);
    
    // we want to send the packets ASAP
    if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (const char*) &one, sizeof(one))) {
        close();
        THROW_IMS_ERROR(ims_init_failure, to_string() << ": Failed to setup TCP_NODELAY.");
    }
    
    // Connect to the given port
    // the connect is blocking !
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(sockaddr_in));
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(address->get_port());
    saddr.sin_addr.s_addr = inet_addr(address->get_ip().c_str());
    if (connect(_sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) != 0)
    {
        close();
        THROW_IMS_ERROR(ims_init_failure, to_string() << ": Error while connecting socket to port '" << address->get_port() << "'.");
    }
}

//
// Receive a VISTAS packet from the socket
//
uint32_t socket_tcp_message::receive_packet(char* buffer, uint32_t buffer_size)
throw(ims::exception)
{
    uint32_t header[2];
    
    // first, we peek the header to know the size of the full message (payload size in the header)
    ssize_t res = recv(_sock, (char*) &header, sizeof(header) , MSG_PEEK);
    if (res < 0 )
    {
        if ( socket::wouldblock() )
        {
            // not an error, nothing in queue
            return 0;
        }
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to peek from socket. error: " << socket::getlasterror());
    }
    
    if (res < (ssize_t)sizeof(header))
    {
        // not an error, no full message yet
        return 0;
    }
    
    // check that the full message is smaller than the buffer
    uint32_t payload_size = ntohl( header[1] );
    if ( payload_size > buffer_size )
    {
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Payload size is bigger than the buffer");
    }
    
    // check that the full message has arrived
#ifdef _WIN32
    u_long bytes_available = 0;
    ioctlsocket(_sock, FIONREAD, &bytes_available);
#else
    uint32_t bytes_available = 0;
    ioctl(_sock, FIONREAD, &bytes_available);
#endif
    if (payload_size <= bytes_available)
    {
        // should succeed, the payload is available
        res = recv(_sock, buffer, payload_size, 0);

        if (res < 0) {
#ifdef _WIN32
            // Data truncated to buffer size
            if (WSAGetLastError() == WSAEMSGSIZE) return buffer_size;
#endif
            THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to read from socket. error: " << socket::getlasterror());
        }

        return res;

    }
    
    // the full message is not here yet
    return 0;
}

//
// Write to the socket
//
void socket_tcp_message::send(const char* buffer, uint32_t size)
throw(ims::exception)
{
    int32_t sent_size = ::send(_sock, buffer, size, 0);

    if (sent_size < 0 || (unsigned)sent_size != size) {
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to write to socket! errno:" << socket::getlasterror());
    }
}

//
// Receive from the socket
//
uint32_t socket_tcp_message::receive(char* buffer,
                                     uint32_t buffer_size,
                                     __attribute__((__unused__)) client* client)
throw(ims::exception)
{
    ssize_t res = recv(_sock, buffer, buffer_size, 0);

    if (res < 0) {
#ifdef _WIN32
        // Data truncated to buffer size
        if (WSAGetLastError() == WSAEMSGSIZE) return buffer_size;
#endif
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to read from socket. error: " << socket::getlasterror());
    }

    return res;

}

//
// Set the socket to blocking
//
void socket_tcp_message::set_blocking(bool blocking)
throw(ims::exception)
{
#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
    ioctlsocket(_sock, FIONBIO, &mode);
#else
    int flags = fcntl(_sock, F_GETFL, 0);
    flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
    fcntl(_sock, F_SETFL, flags);
#endif
}

//
// Reply to the emmiter
//
void socket_tcp_message::reply(__attribute__((__unused__)) client& client,
                               __attribute__((__unused__)) const char* buffer,
                               __attribute__((__unused__)) uint32_t size)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration, to_string() << ": Cannot reply!");
}

}

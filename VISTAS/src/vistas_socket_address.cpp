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

#include "vistas_socket_address.hh"
#include "ims_log.hh"
#include <sstream>

#ifdef __linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

namespace vistas
{
// Create an address
// The parameter outgoing_port can only be used for output port.
socket_address_t::socket_address_t(ims_direction_t direction,
                                   std::string     target_ip,
                                   uint32_t        target_port,
                                   std::string     interface_ip,
                                   uint32_t        TTL,
                                   uint32_t        outgoing_port) :
    _direction(direction),
    _target_ip(target_ip),
    _target_port(target_port),
    _interface_ip(interface_ip),
    _TTL(TTL),
    _outgoing_port(outgoing_port)
{
    if (_direction == ims_input && outgoing_port != 0)
    {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Input address '" << target_ip << ':' <<
                        target_port << " cannot have an outgoing port! (port " <<
                        outgoing_port << ")");
    }


    std::tr1::hash<std::string> hash_string;
    std::stringstream hash_stream;
    hash_stream << ims_direction_string(_direction) << _target_ip << '_' << _target_port;
    _hash = hash_string(hash_stream.str());
}

// Compare two address
bool socket_address_t::equal_to(const socket_address_t& other) const
{
    if (_direction != other._direction) return false;
    if (_target_ip != other._target_ip) return false;
    if (_target_port != other._target_port) return false;
    return true;
}

// Return a string describing this address
std::string socket_address_t::to_string()
{
    std::stringstream ss;
    ss << ims_direction_string(_direction) << " address " <<
          _target_ip << ':' << _target_port;
    if (_interface_ip.empty() == false) ss << " on interface " << _interface_ip;
    if (_TTL != 1) ss << " with TTL " << _TTL;
    if (_outgoing_port != 0) ss << " ouput port " << _outgoing_port;
    return ss.str();
}

static const uint32_t ip_mask = 0xF0;
static const uint32_t multicast_prefix = 0xE0;

bool socket_address_t::is_multicast() const
{
    // multicast addresses are in the range 224.0.0.0 - 239.255.255.255
    // in hexa, it means 0xEx.xx.xx.xx
    uint32_t  addr_int = inet_addr(_target_ip.c_str());
    
    return (addr_int & ip_mask) == multicast_prefix;
}
}

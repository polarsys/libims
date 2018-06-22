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
// Handle an network socket address.
// The type adress_ptr can be a key for a map (unordered_map).
// 
// WARNING: not all data stored in the address are part of the map key. Only:
//  - direction
//  - target_ip
//  - target_port
//
#ifndef _VISTAS_SOCKET_ADDRESS_HH_
#define _VISTAS_SOCKET_ADDRESS_HH_
#include "ims.h"
#include <string>
#include <stdint.h>
#include <tr1/unordered_map>
#include "shared_ptr.hh"

namespace vistas
{
class socket_address_t;
typedef shared_ptr<socket_address_t> socket_address_ptr;

class socket_address_t
{
public:
    // Create an address
    // the parameter outgoing_port can only be used for output port.
    socket_address_t(ims_direction_t direction,
                     std::string     target_ip,
                     uint32_t        target_port,
                     std::string     interface_ip = std::string(),
                     uint32_t        TTL = 1,
                     uint32_t        outgoing_port = 0);

    // Accessors
    inline ims_direction_t get_direction() const;
    inline std::string get_ip() const;
    inline uint32_t get_port() const;
    inline std::string get_interface_ip() const;
    inline uint32_t get_TTL() const;
    inline uint32_t get_outgoing_port() const;
    bool is_multicast() const;

    // Setters
    inline void set_direction(ims_direction_t direction);
    inline void set_ip(std::string target_ip);
    inline void set_port(uint32_t target_port);
    inline void set_interface_ip(std::string interface_ip);
    inline void set_TTL(uint32_t TTL);
    inline void set_outgoing_port(uint32_t outgoing_port);
    
    // Map key function
    // Only field direction, target_ip, target_port are used.
    inline size_t hash() const;
    bool equal_to(const socket_address_t& other) const;

    // Return a string describing this address
    // ** This function is slow and does malloc **
    // ** Don't call it at runtime **
    std::string to_string();

private:
    ims_direction_t _direction;
    std::string     _target_ip;
    uint32_t        _target_port;
    std::string     _interface_ip;
    uint32_t        _TTL;
    uint32_t        _outgoing_port;
    size_t          _hash;
};
}

// Hash specialization for vistas::socket_address_ptr to create unordered maps
namespace std {
namespace tr1 {
template <>
class hash<vistas::socket_address_ptr> {
public:
    size_t operator()(const vistas::socket_address_ptr& address) const
    {
        return (address)? address->hash() : 0;
    }
};
}
}

// equal_to specialization for vistas::socket_address_ptr to create unordered maps
namespace std {
template <>
struct equal_to<vistas::socket_address_ptr> {
    bool operator() (const vistas::socket_address_ptr& left, const vistas::socket_address_ptr& right) const
    {
        if (!left && !right) return true;
        if (!left || !right) return false;
        return left->equal_to(*right);
    }
};
}

//***************************************************************************
// Inlines
//***************************************************************************
ims_direction_t vistas::socket_address_t::get_direction() const
{
    return _direction;
}

std::string vistas::socket_address_t::get_ip() const
{
    return _target_ip;
}

uint32_t vistas::socket_address_t::get_port() const
{ 
    return _target_port;
}

std::string vistas::socket_address_t::get_interface_ip() const
{
    return _interface_ip;
}

uint32_t vistas::socket_address_t::get_TTL() const
{
    return _TTL;
}

size_t vistas::socket_address_t::hash() const
{
    return _hash;
}

uint32_t vistas::socket_address_t::get_outgoing_port() const
{
    return _outgoing_port;
}

void vistas::socket_address_t::set_direction(ims_direction_t direction)
{
    _direction = direction;
}

void vistas::socket_address_t::set_ip(std::string target_ip)
{
    _target_ip = target_ip;
}

void vistas::socket_address_t::set_port(uint32_t target_port)
{
    _target_port = target_port;
}

void vistas::socket_address_t::set_interface_ip(std::string interface_ip)
{
    _interface_ip = interface_ip;
}

void vistas::socket_address_t::set_TTL(uint32_t TTL)
{
    _TTL = TTL;
}

void vistas::socket_address_t::set_outgoing_port(uint32_t outgoing_port)
{
    _outgoing_port = outgoing_port;
}

#endif

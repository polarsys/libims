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
// Store all vistas socket and their associated ports
// 
#ifndef _VISTAS_SOCKET_POOL_HH_
#define _VISTAS_SOCKET_POOL_HH_
#include "vistas_socket.hh"
#include <tr1/unordered_map>
#include <map>
#include <sstream>
#include <vector>
#include <cstring>
#ifdef __linux
#include <sys/select.h>
#endif

namespace vistas
{
struct channel_ip_t
{
    inline channel_ip_t(ims_direction_t d, std::string i): direction(d), ip(i)
    {
        std::tr1::hash<std::string> hash_string;
        std::stringstream hash_stream;
        hash_stream << ims_direction_string(d) << ip;
        _hash = hash_string(hash_stream.str());
    }
    
    inline size_t hash() const
    {
        return _hash;
    }
    
    inline bool operator==(const channel_ip_t & other) const {
        return ip==other.ip && direction == other.direction;
    }
    
    // Return a string describing this address
    inline std::string to_string()
    {
        std::stringstream ss;
        ss << ims_direction_string(direction) << " address " << ip;
        return ss.str();
    }
    
    ims_direction_t direction;
    std::string  ip;
    size_t     _hash;
};
}

// Hash specialization for vistas::socket_address_ptr to create unordered maps
namespace std {
namespace tr1 {
template <>
class hash<vistas::channel_ip_t> {
public:
    size_t operator()(const vistas::channel_ip_t& ip) const
    {
        return ip._hash;
    }
};
}
}

namespace vistas
{
class port;
typedef shared_ptr<port> port_ptr;

class socket_pool;
typedef shared_ptr<socket_pool> socket_pool_ptr;

//===========================================================================
// Pool interface
//===========================================================================
class socket_pool
{
public:
    // Read available data on the network and fill inputs ports.
    ims_return_code_t import(uint32_t timeout_us);
    
    // Redirect the given channel to the target address.
    // Both must have the same direction.
    // The address_key is the one defined in vistas config file. I.e. If
    // you have already redirected one address, address_key still the original
    // one.
    // Return true on success.
    bool data_exchange_redirect_start(const char * channel_name, socket_address_ptr target);

    // Stop the redirection of the given channel
    bool data_exchange_redirect_stop(const char * channel_name);

    // Stop listening/emmiting on the given channel
    bool data_exchange_stop(const char * channel_name);

    // Stop listening/emmiting on all channels on the same IP than this channel
    bool data_exchange_stop_full(const char * channel_name);

    // (Re)start listening/emmiting all channels on the same IP than this channel
    bool data_exchange_start(const char * channel_name);

    // Stop all instrumentations
    void stop_all_instrumentations();

private:
    bool instrumentation_apply(socket_address_ptr address_key, socket_address_ptr target);
    
    bool start_or_stop_full(channel_ip_t ip, bool start);

    struct pool_element_t
    {
        inline pool_element_t(socket_ptr s, port_ptr p, std::string name): socket(s), port(p), channel_name(name) {}
        socket_ptr socket;
        port_ptr   port;
        std::string channel_name;
    };
    
    struct cmp_str
    {
        bool operator()(const char *a, const char *b)
        {
            return std::strcmp(a, b) < 0;
        }
    };
    
    typedef std::vector<pool_element_t>                             pool_vector_t;
    typedef uint32_t                                                pool_id_t;
    typedef std::pair<uint32_t,pool_id_t>                           port_pool_id_t;
    typedef std::vector<port_pool_id_t>                             port_pool_id_vector_t;
    typedef std::tr1::unordered_map<socket_address_ptr, pool_id_t>  address_key_map_t;
    typedef std::tr1::unordered_map<channel_ip_t, port_pool_id_vector_t> ip_key_map_t;
    
    typedef std::map<const char *, socket_address_ptr, cmp_str> channel_address_map_t;

    pool_vector_t     _input_pool;             // 2 differents pools for import performances reasons.
    pool_vector_t     _output_pool;
    address_key_map_t _address_key_map;        // Map addresses (ip+port+direction) to pool id. Use direction to know wich pool it refers to.
    ip_key_map_t      _ip_key_map;             // Map ip/direction (no port) to a vector of (port, pool id). Use direction to know wich pool it refers to.
    channel_address_map_t _channel_address_map;  // Map channel name of address, used because VISTAS VCC uses channel names...
    fd_set            _select_set;
    int               _select_nfds;

    //=========================================================================
    // Pool Factory
    //=========================================================================
public:
    class factory
    {
    public:
        // Check if the given address is already registered
        bool exists(socket_address_ptr address);

        // Return the associated port. May return NULL.
        port_ptr find(socket_address_ptr address);

        // Add a new element to the pool. Use socket address as address.
        // May throw if this address is already registered. You have
        // to call @see exists() before.
        void add(socket_ptr socket, port_ptr port, std::string channel_name) throw (ims::exception);

        // Generate the pool with the elements registered in the factory.
        // The factory can be safely destroyed after this call.
        socket_pool_ptr create_pool();

    private:
        typedef std::tr1::unordered_map<socket_address_ptr, pool_element_t> address_map_t;
        address_map_t _addresses;
    };
};
}
#endif

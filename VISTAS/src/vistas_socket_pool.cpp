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
#include "vistas_socket_pool.hh"
#include "vistas_port.hh"
#include "ims_time.hh"
#include <errno.h>
#include "vistas_socket_multicast_input.hh"
#include "vistas_socket_multicast_output.hh"
#include "vistas_socket_void.hh"

namespace vistas
{
//===========================================================================
// Factory implementation
//===========================================================================

//
// Check if the given address is already registered
//
bool socket_pool::factory::exists(socket_address_ptr address)
{
    bool res = _addresses.find(address) != _addresses.end();
    return res;
}

//
// Return the associated port. May return NULL.
//
port_ptr socket_pool::factory::find(socket_address_ptr address)
{
    address_map_t::iterator iaddr = _addresses.find(address);
    if (iaddr == _addresses.end()) {
        return port_ptr();
    } else {
        return iaddr->second.port;
    }
}

//
// Add a new element to the pool. Use socket address as address.
// May throw if this address is already registered. You have
// to call @see exists() before.
//
void socket_pool::factory::add(socket_ptr socket,
                               port_ptr   port,
                               std::string channel_name)
throw (ims::exception)
{
    if (socket == NULL) {
        THROW_IMS_ERROR(ims_init_failure, "Cannot add a null socket!");
    }
    
    socket_address_ptr address = socket->get_address();
    
    if (address == NULL) {
        THROW_IMS_ERROR(ims_init_failure, "Socket " << socket->to_string() <<
                        " does not seem to be initialized!");
    }
    
    if (exists(address)) {
        THROW_IMS_ERROR(ims_init_failure, address->to_string() <<
                        " Already exists in the input socket pool!");
    }
    
    _addresses.insert(address_map_t::value_type(address,
                                                pool_element_t(socket, port, channel_name)));
}

// Generate the pool with the elements registered in the factory.
// The factory can be safely destroyed after this call.
socket_pool_ptr socket_pool::factory::create_pool()
{
    socket_pool_ptr pool = socket_pool_ptr(new socket_pool);

    pool->_select_nfds = -1;
    FD_ZERO(&pool->_select_set);

    pool->_input_pool.reserve(_addresses.size());
    pool->_output_pool.reserve(_addresses.size());

    for (address_map_t::iterator iaddr = _addresses.begin();
         iaddr != _addresses.end();
         iaddr++)
    {
        uint32_t pool_id = 0;

        if (iaddr->first->get_direction() == ims_input)
        {
            pool_id = pool->_input_pool.size();
            IMS_SOCKET fd = iaddr->second.socket->get_fd();
            pool->_select_nfds = std::max( (int)(fd + 1), pool->_select_nfds);
            FD_SET(fd, &pool->_select_set);
            pool->_input_pool.push_back(iaddr->second);
        }
        else {
            pool_id = pool->_output_pool.size();
            pool->_output_pool.push_back(iaddr->second);
        }

        // add to the map which uses ip/port/direction as key
        pool->_address_key_map.insert(address_key_map_t::
                                      value_type(iaddr->first, pool_id));

        // add to the map which uses ip/direction as key
        channel_ip_t channel_ip( iaddr->first->get_direction(), iaddr->first->get_ip() );
        ip_key_map_t::iterator it = pool->_ip_key_map.find(channel_ip);
        if (it == pool->_ip_key_map.end() )
        {
            // need to add a new vector
            port_pool_id_vector_t port_id_vector;
            port_id_vector.push_back( std::make_pair( iaddr->first->get_port() , pool_id) );

            pool->_ip_key_map[channel_ip] = port_id_vector;
        }
        else
        {
            // add to existing vector
            it->second.push_back( std::make_pair( iaddr->first->get_port() , pool_id) );
        }

        // add to the map which uses channel_name as key
        pool->_channel_address_map.insert(channel_address_map_t::value_type( iaddr->second.channel_name.c_str() , iaddr->first));
    }

    return pool;
}

//===========================================================================
// Pool implementation
//===========================================================================
ims_return_code_t socket_pool::import(uint32_t timeout_us)
{
    if (_select_nfds == -1)
    {
        // If no consumed data, _select_set is still at -1
        // Avoid calling select, the call will fail
        // Just return without any error

        return ims_no_error;
    }
    
    uint64_t begin = ims_get_real_time();

    // 0 timeout for polling
    struct timeval zero_timeout;
    zero_timeout.tv_sec = 0;
    zero_timeout.tv_usec = 0;

    int select_status = 1;
    fd_set select_set;

    do
    {
        memcpy(&select_set, &_select_set, sizeof(fd_set));
        select_status = select(_select_nfds, &select_set, NULL, NULL, &zero_timeout);

        if (select_status < 0) {
            THROW_IMS_ERROR(ims_implementation_specific_error,
                            "select fail! errno: " << socket::getlasterror());
        }

        if (select_status > 0)
        {
            for(pool_vector_t::iterator ipool = _input_pool.begin();
                ipool != _input_pool.end();
                ipool++) {
                if (ipool->socket->get_fd() != INVALID_SOCKET &&
                        FD_ISSET(ipool->socket->get_fd(), &select_set)) {
                    ipool->port->receive();
                }
            }
        }

    } while ((select_status != 0) &&
             (ims_get_real_time() - begin < timeout_us));

    return ims_no_error;
}

// Apply the given redirection
bool socket_pool::instrumentation_apply(socket_address_ptr address_key, socket_address_ptr target)
{
    address_key_map_t::iterator iaddr = _address_key_map.find(address_key);
    if (iaddr == _address_key_map.end()) {
        LOG_ERROR("Redirect fail: Unknown address " << address_key->to_string());
        return false;
    }

    if (target) {
        target->set_interface_ip(address_key->get_interface_ip());
        target->set_TTL(address_key->get_TTL());
        target->set_outgoing_port(address_key->get_outgoing_port());
    }

    // Create and set the new socket
    if (address_key->get_direction() == ims_input)
    {
        // Remove old socket from fd_set
        if (_input_pool[iaddr->second].socket->get_fd() != INVALID_SOCKET) {
            FD_CLR(_input_pool[iaddr->second].socket->get_fd(), &_select_set);
        }

        // Create the new socket and register it in the FD SET
        socket_ptr socket;
        if (target) {
            socket = socket_ptr(new socket_multicast_input(target));
            _select_nfds = std::max( (int)(socket->get_fd() + 1), _select_nfds);
            FD_SET(socket->get_fd(), &_select_set);
        }
        else  // We don't have an address
        {
            socket = socket_ptr(new socket_void());
        }

        // Insert the new socket in the pool. (the old one will be destroyed with socket object)
        _input_pool[iaddr->second].socket = socket;
        _input_pool[iaddr->second].port->set_socket(socket);
    }
    else // output
    {
        // Create the new socket
        socket_ptr socket;
        if (target) {
            socket = socket_ptr(new socket_multicast_output(target));
        } else {
            socket = socket_ptr(new socket_void());
        }

        // Insert the new socket in the pool. (the old one will be destroyed with socket object)
        _output_pool[iaddr->second].socket = socket;
        _output_pool[iaddr->second].port->set_socket(socket);
    }

    return true;
}

bool socket_pool::start_or_stop_full(channel_ip_t ip, bool start)
{
    ip_key_map_t::iterator iaddr = _ip_key_map.find(ip);
    if (iaddr == _ip_key_map.end()) {
        LOG_ERROR("Start or stop fail: Unknown address " << ip.to_string());
        return false;
    }

    // Create and set the new socket
    if (ip.direction == ims_input)
    {
        for (port_pool_id_vector_t::iterator it = iaddr->second.begin(); it!=iaddr->second.end(); it++ )
        {
            uint32_t port = it->first;
            pool_id_t pool_id = it->second;

            // Remove old socket from fd_set
            if (_input_pool[pool_id].socket->get_fd() != INVALID_SOCKET) {
                FD_CLR(_input_pool[pool_id].socket->get_fd(), &_select_set);
            }

            // Create the new socket and register it in the FD SET
            socket_ptr socket;
            if (start) {
                socket_address_ptr target(new socket_address_t(ip.direction, ip.ip, port));
                socket = socket_ptr(new socket_multicast_input(target));
                _select_nfds = std::max( (int) (socket->get_fd() + 1), _select_nfds);
                FD_SET(socket->get_fd(), &_select_set);
            }
            else  // We don't have an address
            {
                socket = socket_ptr(new socket_void());
            }

            // Insert the new socket in the pool. (the old one will be destroyed with socket object)
            _input_pool[pool_id].socket = socket;
            _input_pool[pool_id].port->set_socket(socket);

        }
    }
    else // output
    {
        for (port_pool_id_vector_t::iterator it = iaddr->second.begin(); it!=iaddr->second.end(); it++ )
        {
            uint32_t port = it->first;
            pool_id_t pool_id = it->second;

            // Create the new socket
            socket_ptr socket;
            if (start) {
                socket_address_ptr target(new socket_address_t(ip.direction, ip.ip, port));
                socket = socket_ptr(new socket_multicast_output(target));
            } else {
                socket = socket_ptr(new socket_void());
            }

            // Insert the new socket in the pool. (the old one will be destroyed with socket object)
            _output_pool[pool_id].socket = socket;
            _output_pool[pool_id].port->set_socket(socket);
        }

    }

    return true;
}

// Redirect the given channel to the target address.
bool socket_pool::data_exchange_redirect_start(const char * channel_name, socket_address_ptr target)
{
    channel_address_map_t::iterator it = _channel_address_map.find( channel_name );
    
    if (it == _channel_address_map.end() )
    {
        LOG_ERROR("Unknown channel name : " << channel_name );
        return false;
    }
    
    socket_address_ptr address_key = it->second;
    
    if (address_key->get_direction() !=
            target->get_direction()) {
        LOG_ERROR("Cannot redirect to another direction! Original: " << address_key->to_string() <<
                  ". Target: " << target->to_string());
        return false;
    }

    // TODO : check if the target socket is already in use

    LOG_INFO("Redirect channel " << channel_name << " to " << target->to_string());
    return instrumentation_apply(address_key, target);
}

// Stop the redirection of the given channel
bool socket_pool::data_exchange_redirect_stop(const char * channel_name)
{
    channel_address_map_t::iterator it = _channel_address_map.find( channel_name );
    
    if (it == _channel_address_map.end() )
    {
        LOG_ERROR("Unknown channel name : " << channel_name );
        return false;
    }
    
    socket_address_ptr address_key = it->second;
    
    LOG_INFO("Stop redirect channel " << channel_name);
    return instrumentation_apply(address_key, address_key);
}

// Stop listening/emmiting on the given channel
bool socket_pool::data_exchange_stop(const char * channel_name)
{
    channel_address_map_t::iterator it = _channel_address_map.find( channel_name );
    
    if (it == _channel_address_map.end() )
    {
        LOG_ERROR("Unknown channel name : " << channel_name );
        return false;
    }
    
    socket_address_ptr address_key = it->second;

    LOG_INFO("Stop channel " << channel_name);
    return instrumentation_apply(address_key, socket_address_ptr((socket_address_t*)NULL));
}

// Stop listening/emmiting on all channels on the same IP than this channel
bool socket_pool::data_exchange_stop_full(const char * channel_name)
{
    channel_address_map_t::iterator it = _channel_address_map.find( channel_name );
    
    if (it == _channel_address_map.end() )
    {
        LOG_ERROR("Unknown channel name : " << channel_name );
        return false;
    }
    
    socket_address_ptr address_key = it->second;
    
    channel_ip_t channel_ip( address_key->get_direction(), address_key->get_ip() );
    LOG_INFO("Stop full IP " << channel_ip.to_string());
    return start_or_stop_full(channel_ip, false);
}

// (Re)start listening/emmiting all channels on the same IP than this channel
bool socket_pool::data_exchange_start(const char * channel_name)
{
    channel_address_map_t::iterator it = _channel_address_map.find( channel_name );
    
    if (it == _channel_address_map.end() )
    {
        LOG_ERROR("Unknown channel name : " << channel_name );
        return false;
    }
    
    socket_address_ptr address_key = it->second;

    channel_ip_t channel_ip( address_key->get_direction(), address_key->get_ip() );
    LOG_INFO("Restart IP " << channel_ip.to_string());
    return start_or_stop_full(channel_ip, true);
}

// Stop all redirection
void socket_pool::stop_all_instrumentations()
{
    // TODO: this code is really slow. We might store somewhere all redirections
    for(address_key_map_t::iterator iaddr = _address_key_map.begin();
        iaddr != _address_key_map.end();
        iaddr++)
    {
        pool_element_t& element =
                (iaddr->first->get_direction() == ims_input)? _input_pool[iaddr->second] : _output_pool[iaddr->second];

        if (element.socket->get_address() != iaddr->first) {
            instrumentation_apply(iaddr->first, iaddr->first);
        }
    }
}

}


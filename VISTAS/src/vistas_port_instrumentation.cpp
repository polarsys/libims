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

/*
 * Instrumentation port
 */
#include "vistas_port_instrumentation.hh"
#include "vistas_context.hh"
#include <limits.h>
#include <sstream>

#define INSTRUMENTATION_MESSAGE_MAX_SIZE 200
#define REQUEST_MAX_STRING_SIZE          100

#define UNREGISTER 0
#define REGISTER 1

#pragma pack (push, 1)

struct vistas_vcc_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t acknowledge_status;
};

struct vistas_r_config_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t ack_id;
    uint32_t ack_value;
};

struct vistas_r_state_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t ack_id;
    uint32_t ack_value;
    uint32_t end_status;
};

struct vistas_f_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t register_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

struct vistas_f_mode_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t register_command;
    uint32_t step_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

struct vistas_f_sync_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t application_period_ms;
    uint32_t register_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

#pragma pack (pop)

namespace vistas
{
// Main request codes
enum command_id_t {
    command_f_instrum_register = 1,
    command_r_instrum_register = 2,
    command_f_failure          = 3,
    command_r_failure          = 4,
    command_f_mode_register    = 5,
    command_r_mode_register    = 6,
    command_f_state            = 7,
    command_r_state            = 8,
    command_f_power_register   = 9,
    command_r_power_register  = 10,
    command_f_powersupply     = 11,
    command_r_powersupply     = 12,
    command_f_sync_register   = 13,
    command_r_sync_register   = 14,
    command_f_synchro         = 15,
    command_r_synchro         = 16,
    command_f_config          = 17,
    command_r_config          = 18,

    command_invalid = UINT_MAX
};

// ==========================================================================
// Internal request parser
// ==========================================================================
class port_instrumentation::request_parser
{
public:

    request_parser(context_weak_ptr context, socket_tcp_message_ptr socket, port_instrumentation * port_instrumentation);
    void parse_message(const char* message, uint32_t message_size);

private:
    typedef std::exception error;
    
    // Data exchange secondary request codes
    enum instrum_register_sub_id_t
    {
        instrum_register_stop_emission           = 0,
        instrum_register_stop_reception          = 1,
        instrum_register_stop_full_emission      = 2,
        instrum_register_stop_full_reception     = 3,
        instrum_register_start_emission          = 4,
        instrum_register_start_reception         = 5,
        instrum_register_override_emission       = 6,
        instrum_register_override_reception      = 7,
        instrum_register_stop_override_emission  = 8,
        instrum_register_stop_override_reception = 9,

        instrum_register_end_id,
    };

    enum config_sub_id_t
    {
        config_load                              = 0,
        config_apply_config                      = 1,
    };

    enum state_sub_id_t
    {
        state_reset                              = 0,
        state_run                                = 1,
        state_step                               = 2,
        state_hold                               = 3,
    };

    // direction of secondary data_exchange request codes
    ims_direction_t instrum_register_direction[instrum_register_end_id];

    // Current request
    command_id_t _command_id;

    // Parsing methods
    void               can_read_size(uint32_t size);
    uint32_t           read_word();
    float              read_float();
    std::string        read_string();
    const char*        read_string_pointer();
    socket_address_ptr read_address(ims_direction_t direction);
    
    void               parse_failure();
    void               parse_config();
    void               parse_state();
    void               parse_synchro();
    void               parse_powersupply();

    const char* _parser_pos;
    const char* _parser_end;

    context_weak_ptr _context;
    socket_tcp_message_ptr _socket;
    port_instrumentation * _port_instrumentation;
};

//
// Ctor
//
port_instrumentation::request_parser::request_parser(context_weak_ptr context, socket_tcp_message_ptr socket, port_instrumentation * port_instrumentation) :
    _context(context), _socket(socket), _port_instrumentation(port_instrumentation)
{
    instrum_register_direction[instrum_register_stop_emission]           = ims_output;
    instrum_register_direction[instrum_register_stop_reception]          = ims_input;
    instrum_register_direction[instrum_register_stop_full_emission]      = ims_output;
    instrum_register_direction[instrum_register_stop_full_reception]     = ims_input;
    instrum_register_direction[instrum_register_start_emission]          = ims_output;
    instrum_register_direction[instrum_register_start_reception]         = ims_input;
    instrum_register_direction[instrum_register_override_emission]       = ims_output;
    instrum_register_direction[instrum_register_override_reception]      = ims_input;
    instrum_register_direction[instrum_register_stop_override_emission]  = ims_output;
    instrum_register_direction[instrum_register_stop_override_reception] = ims_input;
}

//
// Main parser entry
//
void port_instrumentation::request_parser::parse_message(const char* message, uint32_t message_size)
{
    // Initialize the parser
    _parser_pos = message;
    _parser_end = message + message_size;
    _command_id = command_invalid;

    // Parse the header
    _command_id = (command_id_t)read_word();
    uint32_t payload_size = read_word();

    LOG_DEBUG("Received instrum command id " << _command_id << " payload_size " << payload_size);

    // Execute the request
    switch(_command_id) {

    case command_f_failure:
        parse_failure();
        break;

    case command_f_config:
        parse_config();
        break;

    case command_f_state:
        parse_state();
        break;

    case command_f_synchro:
        parse_synchro();
        break;

    case command_f_powersupply:
        parse_powersupply();
        break;

    default:
        LOG_ERROR("Unknown instrumentation command id (" << _command_id << ")");
        throw error();
    }

}

void port_instrumentation::request_parser::parse_failure()
{
    bool is_valid = true;
    
    try {
        const char * channel_name = read_string_pointer();
        instrum_register_sub_id_t failure_command_id = (instrum_register_sub_id_t)read_word();

        LOG_DEBUG("failure command " << failure_command_id << " on channel : " << channel_name);


        switch(failure_command_id)
        {
        case instrum_register_stop_emission:
        case instrum_register_stop_reception:
            if (_context->get_socket_pool()->data_exchange_stop(channel_name) == false) {
                throw error();
            }
            break;

        case instrum_register_stop_full_emission:
        case instrum_register_stop_full_reception:
            if (_context->get_socket_pool()->data_exchange_stop_full(channel_name) == false) {
                throw error();
            }
            break;

        case instrum_register_start_emission:
        case instrum_register_start_reception:
            if (_context->get_socket_pool()->data_exchange_start(channel_name) == false) {
                throw error();
            }
            break;

        case instrum_register_override_emission:
        case instrum_register_override_reception:
        {
            socket_address_ptr address_target = read_address(instrum_register_direction[failure_command_id]);
            LOG_DEBUG("reroute begin command to " << address_target->to_string());
            if (_context->get_socket_pool()->data_exchange_redirect_start(channel_name, address_target) == false) {
                throw error();
            }
            LOG_DEBUG("data exchange command success.");
        }
            break;

        case instrum_register_stop_override_emission:
        case instrum_register_stop_override_reception:
            LOG_DEBUG("reroute end command.");
            if (_context->get_socket_pool()->data_exchange_redirect_stop(channel_name) == false) {
                throw error();
            }
            break;

        default:
            LOG_ERROR("Unkwown failure command id " << failure_command_id);
            throw error();
        }
    } catch (const error & e) {
        is_valid = false;
    }

    vistas_vcc_packet response;
    
    response.command_id = htonl(command_r_failure);
    response.payload_size = htonl(sizeof(vistas_vcc_packet) );
    response.acknowledge_status = htonl( is_valid ? 0 : 1 );
    
    _socket->send((char*)&response, sizeof(vistas_vcc_packet));
}

void port_instrumentation::request_parser::parse_config()
{
    bool is_valid = true;
    
    uint32_t config_command_id = 0;
    std::string configuration;
    
    try {
        config_command_id = read_word();
        configuration = read_string();

        switch(config_command_id)
        {
        case config_load:
            LOG_DEBUG("config : LOAD " << configuration);
            break;

        case config_apply_config:
            LOG_DEBUG("config : APPLY_CONFIG " << configuration);
            break;

        default:
            LOG_ERROR("Unkwown config command id " << config_command_id);
            throw error();
        }
    } catch (const error & e) {
        is_valid = false;
    }
    
    vistas_r_config_packet response;
    
    response.command_id = htonl(command_r_config);
    response.payload_size = htonl(sizeof(vistas_r_config_packet) );
    response.ack_id = htonl( config_command_id );
    response.ack_value = htonl( is_valid ? 0 : 1 );
    
    _socket->send((char*)&response, sizeof(vistas_r_config_packet));
    
    // if APPLY CONFIG, we need to send a second ACK after the first one....
    if (is_valid && config_command_id == config_apply_config)
    {
        response.command_id = htonl(command_r_config);
        response.payload_size = htonl(sizeof(vistas_r_config_packet) );
        response.ack_id = htonl( config_command_id );
        response.ack_value = htonl( 2 ); // 2 = Virtual component configured

        _socket->send((char*)&response, sizeof(vistas_r_config_packet));
    }
}

void port_instrumentation::request_parser::parse_synchro()
{
    bool is_valid = true;
    
    try {

        if ( !_context->is_step_by_step_enabled() )
        {
            LOG_ERROR("F_SYNCHRO : step by step not supported");
            throw error();
        }

        uint32_t number_of_cycles = read_word();

        LOG_DEBUG("F_SYNCHRO number_of_cycles : " << number_of_cycles);
        _context->set_synchro_steps_request(number_of_cycles, _port_instrumentation);

    } catch (const error & e) {
        is_valid = false;
    }
    
    if (!is_valid)
    {
        vistas_vcc_packet response;

        response.command_id = htonl(command_r_synchro);
        response.payload_size = htonl(sizeof(vistas_vcc_packet) );
        response.acknowledge_status = htonl( 1 );

        _socket->send((char*)&response, sizeof(vistas_vcc_packet));
    }
    
}

void port_instrumentation::acknowledge_synchro()
{
    vistas_vcc_packet response;

    response.command_id = htonl(command_r_synchro);
    response.payload_size = htonl(sizeof(vistas_vcc_packet) );
    response.acknowledge_status = htonl( 0 );

    _socket->send((char*)&response, sizeof(vistas_vcc_packet));
}

void port_instrumentation::request_parser::parse_powersupply()
{
    bool is_valid = true;
    
    try {
        uint32_t powersupply_command_id = read_word();

        switch(powersupply_command_id)
        {
        case 0:
            LOG_DEBUG("power : OFF");
            _context->set_powersupply_on(false);
            break;

        case 1:
            LOG_DEBUG("power : ON");
            _context->set_powersupply_on(true);
            break;


        default:
            LOG_ERROR("Unkwown state command id " << powersupply_command_id);
            throw error();
        }


    } catch (const error & e) {
        is_valid = false;
    }
    
    vistas_vcc_packet response;
    
    response.command_id = htonl(command_r_powersupply);
    response.payload_size = htonl(sizeof(vistas_vcc_packet) );
    response.acknowledge_status = htonl( is_valid ? 0 : 1 );
    
    _socket->send((char*)&response, sizeof(vistas_vcc_packet));
}

void port_instrumentation::request_parser::parse_state()
{
    bool is_valid = true;
    
    uint32_t state_command_id = 0;
    
    // 1 is generic error, 2 means step by step not supported
    uint32_t error_code = 1;
    
    try {
        state_command_id = read_word();
        /*uint32_t synchro_mode =*/  read_word();
        float time_ratio = read_float();
        if (time_ratio <= 0.0)
        {
            LOG_WARN("Invalid time ratio : " << time_ratio << ", using 1.0 instead");
            time_ratio = 1.0;
        }

        switch(state_command_id)
        {
        case state_reset:
            LOG_DEBUG("state : RESET");
            _context->set_running_state(ims_running_state_reset);
            break;

        case state_run:
            LOG_DEBUG("state : RUN");
            _context->set_running_state(ims_running_state_run);
            break;

        case state_step:
            LOG_DEBUG("state : STEP");
            _context->set_running_state(ims_running_state_step);
            break;

        case state_hold:
            LOG_DEBUG("state : HOLD");
            _context->set_running_state(ims_running_state_hold);
            break;

        default:
            LOG_ERROR("Unkwown state command id " << state_command_id);
            throw error();
        }

        // for now, synchro_mode is completely ignored, but it may change in the future
        // instead, we check if the state is "STEP"
        /*
      switch (synchro_mode)
      {
        case 0:
          LOG_DEBUG("autonomous real time");
          _context->set_autonomous_realtime(true);
          break;
        case 1:
          if ( _context->is_step_by_step_enabled() )
          {
            LOG_DEBUG("time controlled through F_SYNCHRO");
            _context->set_autonomous_realtime(false);
          }
          else
          {
            LOG_ERROR("F_STATE : step by step mode not supported");
            error_code = 2;
            throw error();
          }
          break;
        default:
          LOG_ERROR("Unkwown synchro mode " << synchro_mode);
          throw error();
      }
      */

        if ( _context->get_running_state() == ims_running_state_step )
        {
            if ( _context->is_step_by_step_enabled() )
            {
                LOG_DEBUG("time controlled through F_SYNCHRO");
                _context->set_autonomous_realtime(false);
            }
            else
            {
                LOG_ERROR("F_STATE : step by step mode not supported");
                error_code = 2;
                throw error();
            }
        }
        else
        {
            _context->set_autonomous_realtime(true);
        }

        LOG_DEBUG("time ratio : " << time_ratio);
        _context->set_time_ratio(time_ratio);

    } catch (const error & e) {
        is_valid = false;
    }

    vistas_r_state_packet response;
    
    response.command_id = htonl(command_r_state);
    response.payload_size = htonl(sizeof(vistas_r_state_packet) );
    response.ack_id = htonl( state_command_id );
    response.ack_value = htonl( is_valid ? 0 : error_code );
    response.end_status = htonl( state_command_id == state_reset ? 1 : 0 );;
    
    _socket->send((char*)&response, sizeof(vistas_r_state_packet));
}

socket_address_ptr port_instrumentation::request_parser::read_address(ims_direction_t direction)
{
    std::string ip_port = read_string();
    
    std::size_t index = ip_port.find(':');
    if (index!=std::string::npos)
    {
        ip_port[index]=' ';
    }
    
    std::istringstream iss(ip_port);
    
    std::string ip;
    uint32_t port = 0;
    
    iss >> ip;
    iss >> port;
    
    return socket_address_ptr(new socket_address_t(direction, ip, port));
}


std::string port_instrumentation::request_parser::read_string()
{
    char string[REQUEST_MAX_STRING_SIZE];

    uint32_t size = read_word();
    can_read_size(size);
    memcpy(string, _parser_pos, size);
    string[size] = 0;
    _parser_pos += size;

    return string;
}

const char * port_instrumentation::request_parser::read_string_pointer()
{
    static char string[REQUEST_MAX_STRING_SIZE];

    uint32_t size = read_word();
    can_read_size(size);
    memcpy(string, _parser_pos, size);
    string[size] = 0;
    _parser_pos += size;

    return string;
}

uint32_t port_instrumentation::request_parser::read_word()
{
    can_read_size(sizeof(uint32_t));
    uint32_t value =  ntohl(*(uint32_t*)_parser_pos);
    _parser_pos += sizeof(uint32_t);
    return value;
}

// we need to use an union, else we break aliasing rules of GCC
union union_uint32_float
{
    uint32_t as_uint;
    float as_float;
};

float port_instrumentation::request_parser::read_float()
{
    union_uint32_float my_union;
    my_union.as_uint = read_word();
    return my_union.as_float;
}

void port_instrumentation::request_parser::can_read_size(uint32_t size)
{
    if (_parser_pos + size > _parser_end) {
        LOG_ERROR("Signal overriding " << _command_id << " is too small !");
        throw error();
    }
}

// ==========================================================================
// The port itself
// ==========================================================================

//
// Ctor
//
port_instrumentation::port_instrumentation(context_weak_ptr context, socket_tcp_message_ptr socket) :
    port(context, socket),
    _message(new char[INSTRUMENTATION_MESSAGE_MAX_SIZE]),
    _parser(new request_parser(context, socket, this)),
    _registered_instrum(false),
    _registered_mode(false),
    _registered_power(false),
    _registered_sync(false)
{
    memset(_message, 0, INSTRUMENTATION_MESSAGE_MAX_SIZE);
}

port_instrumentation::~port_instrumentation()
{
    if (_registered_instrum)
    {
        vistas_f_register_packet packet;

        packet.command_id = htonl(command_f_instrum_register);
        packet.payload_size = htonl(sizeof(vistas_f_register_packet));
        packet.register_command = htonl( UNREGISTER );

        fill_vc_name(packet.string_size, packet.vc_name);

        _socket->send( (char*) &packet,  sizeof(packet) );
    }
    
    if (_registered_mode)
    {
        vistas_f_mode_register_packet packet;

        packet.command_id = htonl(command_f_mode_register);
        packet.payload_size = htonl(sizeof(vistas_f_mode_register_packet));
        packet.register_command = htonl( UNREGISTER );
        packet.step_command = htonl( _context->is_step_by_step_enabled() ? 1 : 0 );

        fill_vc_name(packet.string_size, packet.vc_name);

        _socket->send( (char*) &packet,  sizeof(packet) );
    }
    
    if (_registered_power)
    {
        vistas_f_register_packet packet;

        packet.command_id = htonl(command_f_power_register);
        packet.payload_size = htonl(sizeof(vistas_f_register_packet));
        packet.register_command = htonl( UNREGISTER );

        fill_vc_name(packet.string_size, packet.vc_name);

        _socket->send( (char*) &packet,  sizeof(packet) );
    }
    
    if (_registered_sync)
    {
        vistas_f_sync_register_packet packet;

        packet.command_id = htonl(command_f_sync_register);
        packet.payload_size = htonl(sizeof(vistas_f_sync_register_packet));
        packet.application_period_ms = htonl( _context->get_period_us() / 1000 );
        packet.register_command = htonl( UNREGISTER );

        fill_vc_name(packet.string_size, packet.vc_name);

        _socket->send( (char*) &packet,  sizeof(packet) );
    }
    
    delete[] _message;
    delete _parser;
}

//
// Send (or not.)
//
void port_instrumentation::send()
{
    THROW_IMS_ERROR(ims_implementation_specific_error, "Implementation error: Cannot send to instrumentation port!");
}

void port_instrumentation::fill_vc_name(uint32_t & string_size, char * vc_name)
{
    std::string vcc_name = _context->get_vc_name();
    if (vcc_name.size() >= REQUEST_MAX_STRING_SIZE )
    {
        vcc_name.resize( REQUEST_MAX_STRING_SIZE - 1 );
    }
    
    string_size = htonl(vcc_name.size() + 1);
    strcpy( vc_name, vcc_name.c_str() );
}

//
// Receive and handle one instrumentation message.
//
void port_instrumentation::receive()
{
    // Read the message and initialize the parser
    uint32_t message_size;
    
    socket_tcp_message * socket_tcp = static_cast<socket_tcp_message *>( _socket.get() );
    
    while ( (message_size = socket_tcp->receive_packet(_message, INSTRUMENTATION_MESSAGE_MAX_SIZE)) > 0)
    {
        _parser->parse_message(_message, message_size);
    }
}

void port_instrumentation::register_instrum()
{
    vistas_f_register_packet packet;
    
    packet.command_id = htonl(command_f_instrum_register);
    packet.payload_size = htonl(sizeof(vistas_f_register_packet));
    packet.register_command = htonl( REGISTER );
    
    fill_vc_name(packet.string_size, packet.vc_name);
    
    _registered_instrum = true;
    
    register_generic(&packet, sizeof(packet) , command_r_instrum_register);
}

void port_instrumentation::register_mode()
{
    vistas_f_mode_register_packet packet;
    
    packet.command_id = htonl(command_f_mode_register);
    packet.payload_size = htonl(sizeof(vistas_f_mode_register_packet));
    packet.register_command = htonl( REGISTER );
    packet.step_command = htonl( _context->is_step_by_step_enabled() ? 1 : 0 );
    
    fill_vc_name(packet.string_size, packet.vc_name);
    
    _registered_mode = true;
    
    register_generic(&packet, sizeof(packet), command_r_mode_register);
}

void port_instrumentation::register_power()
{
    vistas_f_register_packet packet;
    
    packet.command_id = htonl(command_f_power_register);
    packet.payload_size = htonl(sizeof(vistas_f_register_packet));
    packet.register_command = htonl( REGISTER );
    
    fill_vc_name(packet.string_size, packet.vc_name);
    
    _registered_power = true;

    register_generic(&packet, sizeof(packet), command_r_power_register);
}

void port_instrumentation::register_sync()
{
    vistas_f_sync_register_packet packet;
    
    packet.command_id = htonl(command_f_sync_register);
    packet.payload_size = htonl(sizeof(vistas_f_sync_register_packet));
    packet.application_period_ms = htonl( _context->get_period_us() / 1000 );
    packet.register_command = htonl( REGISTER );
    
    fill_vc_name(packet.string_size, packet.vc_name);
    
    _registered_sync = true;
    
    register_generic(&packet, sizeof(packet), command_r_sync_register);
}

void port_instrumentation::register_generic(void * register_packet, uint32_t register_packet_size, uint32_t expected_response_id)
{
    socket_tcp_message * socket_tcp = static_cast<socket_tcp_message *>( _socket.get() );
    
    socket_tcp->set_blocking(true);
    
    socket_tcp->send( (char*) register_packet, register_packet_size);
    
    vistas_vcc_packet packet;
    
    uint32_t received = socket_tcp->receive((char*)&packet, sizeof(vistas_vcc_packet));
    
    socket_tcp->set_blocking(false);
    
    if (received != sizeof(vistas_vcc_packet))
    {
        THROW_IMS_ERROR(ims_implementation_specific_error, "register ACK : invalid size");
    }
    
    if (packet.command_id != htonl(expected_response_id))
    {
        THROW_IMS_ERROR(ims_implementation_specific_error, "register ACK : invalid response ID");
    }
    
    if (packet.acknowledge_status != htonl(0))
    {
        THROW_IMS_ERROR(ims_implementation_specific_error, "register ACK : error in command treatment");
    }
}
}

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
 * Fast logging tool
 * This log API only cost one call and one 'if' when logging is disabled.
 */
#ifndef _IMS_LOG_HH_
#define _IMS_LOG_HH_
#include "ims.h"
#include <exception>
#include <ostream>

namespace ims {

//
// Basic exception witch just store and ims_return_code_t
//
class exception : public std::exception
{
public:
    inline exception(ims_return_code_t ims_return_code) :
        _ims_return_code(ims_return_code) {}
    
    virtual const char* what() const throw();
    
    virtual ~exception() throw () {}
    
    ims_return_code_t get_ims_return_code() const { return _ims_return_code; }
    
private:
    ims_return_code_t    _ims_return_code;
    mutable std::string  _what;
};

namespace log {

//
// Shortcuts for each log level
//
#define LOG_DEBUG(message)   LOG_PROCESS(ims::log::debug, __FILE__ << ':' << __LINE__ << ' ' << message)
#define LOG_INFO(message)    LOG_PROCESS(ims::log::info , __FILE__ << ':' << __LINE__ << ' ' << message)
#define LOG_WARN(message)    LOG_PROCESS(ims::log::warn , __FILE__ << ':' << __LINE__ << ' ' << message)
#define LOG_ERROR(message)   LOG_PROCESS(ims::log::error, __FILE__ << ':' << __LINE__ << ' ' << message)

//
// Helpers to check level
//
#define IS_LOG_DEBUG()   (ims::log::debug <= ims::log::max_level)
#define IS_LOG_INFO()    (ims::log::info  <= ims::log::max_level)
#define IS_LOG_WARN()    (ims::log::warn  <= ims::log::max_level)
#define IS_LOG_ERROR()   (ims::log::error <= ims::log::max_level)

//
// Log a string without file/line and without checking log level
//
#define LOG_SAY(message) LOG_PROCESS(ims::log::disabled, message)

//
// Helper to throw an exception
//
#define THROW_IMS_ERROR(ims_return_code, message)                                    \
    do {                                                                             \
    LOG_PROCESS(ims::log::error, __FILE__ << ':' << __LINE__ << ' ' << message);   \
    throw ims::exception(ims_return_code);                                         \
} while (0)

// Helper class to "streamize" a payload
// std::cout << log::hex(&my_int, 4) << std::endl;
// LOG_INFO("PAYLOAD AFDX: " << log::hex(afdx_payload, 100));
class hex
{
public:
    hex(const void* payload, uint32_t len) : _payload((const uint8_t*)payload), _len(len) {}
    friend std::ostream& operator<<(std::ostream& stream, const hex& ps);
    
private:
    const uint8_t* _payload;
    uint32_t       _len;
};

//
// Log levels
//
enum level_t {
    disabled = 0,
    error    = 1,
    warn     = 2,
    info     = 3,
    debug    = 4,
};

//
// Access to the backend stream
//
std::ostream& backend_stream();

//
// Log the current date
//
void backend_log_date();

//
// Close the current line for the current backend
//
void backend_endl();

//
// Internal log level
//
extern level_t max_level;

//
// internal LOG macro
//
#define LOG_PROCESS(level, message)             \
    do {                                        \
    if (level <= ims::log::max_level) {       \
    ims::log::backend_log_date();            \
    ims::log::backend_stream() << message;  \
    ims::log::backend_endl();               \
}                                         \
} while (0)

}
}

#endif


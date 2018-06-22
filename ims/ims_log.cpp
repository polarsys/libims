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

#include "ims_log.hh"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>

#include <sys/time.h>

namespace ims
{

//
// ims::exception
//
const char* exception::what() const throw()
{
    //@TODO: better error message
    std::stringstream ss;
    ss << "IMS return code " << _ims_return_code << '.';
    _what = ss.str();

    return _what.c_str();
}

namespace log
{

//
// Internal log level
//
level_t max_level = disabled;

//
// Backend. std::cerr by default
//
static std::ostream* backend = &std::cerr;

//
// Initialize the log API
//
__attribute__((constructor)) void load()
{
    char* log_level = getenv("IMS_LOG_LEVEL");
    if (log_level) {
        max_level = (level_t)atoi(log_level);
    }

    char* log_file = getenv("IMS_LOG_FILE");
    if (log_file && log_file[0]) {
        backend = new std::ofstream(log_file, std::ios_base::trunc);
        if (backend->fail()) {
            backend = &std::cerr;
            LOG_ERROR("Failed to open log file '" << log_file << "'!");
            exit(1);
        }
    }
}

//
// Return the back-end
//
std::ostream& backend_stream()
{
    return *backend;
}

//
// Log the current date
//
void backend_log_date()
{
  struct timeval tv;
  struct tm s_tm;
  gettimeofday(&tv, NULL);
  // with mingw, we need to convert tv_sec (a long) to localtime_r (a long long)
  time_t converted_time = tv.tv_sec;
  localtime_r(&converted_time, &s_tm);
  
  *backend << std::setfill('0') << std::setw(2) << s_tm.tm_hour << ":" << std::setfill('0') << std::setw(2) << s_tm.tm_min << ":" << std::setfill('0') << std::setw(2) << s_tm.tm_sec << "," << std::setfill('0') << std::setw(6) << tv.tv_usec << " : " ;
}

//
// Close current line in the current back end
//
void backend_endl()
{
    *backend << std::endl;
}

//
// hex stream
//
std::ostream& operator<<(std::ostream &stream, const hex& ps)
{
    std::ios::fmtflags f = stream.flags();
    stream.setf(std::ios::hex, std::ios::basefield);
    stream.setf(std::ios::uppercase);
    char fill = stream.fill('0');

    for (uint32_t pos = 0; pos < ps._len; pos++) {
        stream << std::setw(2) << (int)ps._payload[pos] << " ";
    }

    stream.fill(fill);
    stream.flags(f);
    return stream;
}

}
}

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

#ifndef IMS_INSTRUMENTATION_H
#define IMS_INSTRUMENTATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup group_instrumentation Instrumentation
 * @brief Instrumentation API.
 * - Define internally used handlers on socket data reception and emission
 *   - @ref ims_instrumentation_set_socket_handler_recv()
 *   - @ref ims_instrumentation_set_socket_handler_send()
 *   - @ref ims_instrumentation_set_socket_handler_reply()
 * - Retrieve internally set / received data
 *   - @ref ims_instrumentation_message_get_sampling_data()
 *   - @ref ims_instrumentation_message_get_queued_data()
 * .
 */

/************
 * Includes *
 ************/

#include "ims.h"

/********************
 * Types definition *
 ********************/

/**
 * @ingroup group_instrumentation
 * Instrumentation handler.@n
 * Arguments:
 * - <b>const char * buffer</b>@n
 *   Socket buffered data
 * - <b>int buffer_size</b>@n
 *   Socket buffered data size
 * - <b>const char * ip_address</b>@n
 *   Ip address of the emitter/receiver
 * - <b>unsigned short ip_port</b>@n
 *   Emission/reception port
 * .
 */
typedef void (*ims_socket_handler_t)(const char * buffer, int buffer_size, const char *ip_address, unsigned short ip_port);

/**************
 * Prototypes *
 **************/

/**
 * @ingroup group_instrumentation
 * @brief Set the handler called just after socket recvfrom() internal function
 * @param handler [in] The handler to be called.
 * @return @ref ims_return_code_t::ims_no_error
 */
extern LIBIMS_EXPORT ims_return_code_t ims_instrumentation_set_socket_handler_recv (ims_socket_handler_t handler);

/**
 * @ingroup group_instrumentation
 * @brief Set the handler called just before socket sendto() internal function
 * @param handler [in] The handler to be called.
 * @return @ref ims_return_code_t
 */
extern LIBIMS_EXPORT ims_return_code_t ims_instrumentation_set_socket_handler_send (ims_socket_handler_t handler);

/**
 * @ingroup group_instrumentation
 * @brief Set the handler called just before the socket sento() internal function on reply
 * @param handler [in] The handler to be called. @see ims_instrumentation_set_handler_send()
 * @return @ref ims_return_code_t
 */
extern LIBIMS_EXPORT ims_return_code_t ims_instrumentation_set_socket_handler_reply(ims_socket_handler_t handler);

/**
 * @ingroup group_instrumentation
 * @brief Get a copy of the current data in the message buffer.
 * @param message [in] The message considered.@see ims_get_message()
 * @param data [out] A preallocated buffer of a size equal to max_size
 * @param data_size [out] Size of effective data filled in data pointer
 * @param max_size [in] The size of the preallocated buffer
 * @return Size of valid data available in data
 */
extern LIBIMS_EXPORT ims_return_code_t ims_instrumentation_message_get_sampling_data(ims_message_t   message,
                                                                                     char            *data,
                                                                                     uint32_t        *data_size,
                                                                                     uint32_t        max_size);

/**
 * @ingroup group_instrumentation
 * @brief Get a copy of the current queue data in the message buffer.
 * @param message [in] Provided by @see ims_get_message()
 * @param data [out] A preallocated buffer of a size equal to max_size
 * @param data_size [out] Size of effective data filled in data pointer
 * @param max_size [in] The size of the preallocated buffer
 * @param queue_index [in] The data index in the data queue
 * @return Size of valid data available in data
 */
extern LIBIMS_EXPORT ims_return_code_t ims_instrumentation_message_get_queued_data(ims_message_t message,
                                                                                   char          *data,
                                                                                   uint32_t      *data_size,
                                                                                   uint32_t      max_size,
                                                                                   uint32_t      queue_index);

#ifdef __cplusplus
};
#endif

#endif // IMS_INSTRUMENTATION_H

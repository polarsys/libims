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

#ifndef _IMS_H_
#define _IMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/************
 * Includes *
 ************/

#ifdef __linux__
#define LIBIMS_EXPORT __attribute__ ((visibility ("default")))
#elif _WIN32
#ifdef LIBIMS_EXPORTS
#define  LIBIMS_EXPORT __declspec(dllexport)
#else
#define  LIBIMS_EXPORT __declspec(dllimport)
#endif
#endif

#include <stdint.h>
#include <stdbool.h>

/*********************
 *  Types definition *
 *********************/

/**
 * Function return codes
 * @ingroup group_init_close
 */
typedef enum {
    ims_no_error = 0,                           ///< No error
    ims_init_failure,                           ///< Failure during initialization
    ims_invalid_configuration,                  ///< Invalid configuration
    ims_message_not_found,                      ///< Message not found
    ims_message_invalid_size,                   ///< Invalid message size
    ims_message_queue_full,                     ///< Message queue is full
    ims_implementation_specific_error = 1000,   ///< First implementation protocol specific error code
} ims_return_code_t;

/**
 * @ingroup group_message_content
 * Sampling message validity
 */
typedef enum {
    ims_valid = 0,                              ///< Message is valid
    ims_invalid = 1,                            ///< Message is not valid
    ims_never_received = 2,                     ///< Message has never been received
    ims_never_received_but_initialized = 3,     ///< Message has never been received but has been initialized
} ims_validity_t;

/**
 * @ingroup group_message_functions
 * Messages direction
 */
typedef enum {
    ims_input = 0,  ///< Input
    ims_output = 1, ///< Output
} ims_direction_t;

/**
 * @ingroup group_message_functions
 * Message protocol
 */
typedef enum {
    ims_afdx,       ///< AFDX
    ims_a429,       ///< A429
    ims_can,        ///< CAN
    ims_analogue,   ///< Analogue
    ims_discrete,   ///< Discrete
    ims_nad,        ///< NAD
} ims_protocol_t;

/**
 * @ingroup group_message_functions
 * Message mode
 */
typedef enum {
    ims_sampling,   ///< Sampling
    ims_queuing     ///< Queuing
} ims_mode_t;

/**
 * @ingroup group_protocol_state
 * Protocol running states
 */
typedef enum {
    ims_running_state_run,  ///< State Run
    ims_running_state_step, ///< State Step
    ims_running_state_hold, ///< State Hold
    ims_running_state_reset ///< State Reset
} ims_running_state_t;

/**
 * @ingroup group_message_functions
 * @brief The type of the NAD data.
 */
typedef enum {
    ims_nad_undefined,  ///< Undefined NAD type
    ims_nad_boolean,    ///< Boolean NAD type
    ims_nad_c08,        ///< Character NAD type
    ims_nad_f32,        ///< Float NAD type
    ims_nad_f64,        ///< Double NAD type
    ims_nad_i16,        ///< Short NAD type
    ims_nad_i32,        ///< Integer NAD type
    ims_nad_i64         ///< Long NAD type
} ims_nad_type_t;

/**
 * @ingroup group_message_functions
 * @brief LIBIMS node type structure.
 */
typedef struct ims_internal_node_t*          ims_node_t;


/**
 * @ingroup group_message_functions
 * @brief LIBIMS message type structure.
 */
typedef struct ims_internal_message_t*       ims_message_t;

/**
 * @ingroup group_message_functions
 * @brief LIBIMS node list type structure.
 */
typedef struct ims_internal_node_list_t*     ims_node_list_t;

/**
 * @ingroup group_message_functions
 * @brief LIBIMS message list type structure.
 */
typedef struct ims_internal_messages_list_t* ims_messages_list_t;

#pragma pack (push, 1)

/**
 * @ingroup group_init_close
 * @brief The context creation parameters
 * @see IMS_CREATE_CONTEXT_INITIALIZER
 */
typedef struct {
    uint32_t struct_size;       ///< The size of this version of the structure, later versions may become bigger.
    uint32_t period_us;         ///< The period in micro seconds of the Virtual Component, used for step by step mode, and VISTAS VCC registering. Default value : 0 (undefined period)
    int step_by_step_supported; ///< If the step by step mode is supported, must be non null, and "period_us" must have a valid period. Default value : 0 (step by step not supported)
    const char * init_file_path;///< If not null, contains the path of a XML file with init values for never received messages.
} ims_create_context_parameter_t;

/**
 * @ingroup group_init_close
 * @def IMS_CREATE_CONTEXT_INITIALIZER
 * @brief The default value for the context creation structure @ref ims_create_context_parameter_t.
 */
#define IMS_CREATE_CONTEXT_INITIALIZER  { sizeof(ims_create_context_parameter_t), 0, 0, 0 }

#pragma pack (pop)

/**************************************
 * Initialization an close of the API *
 **************************************/

/**
 * @defgroup group_init_close Initialization & Close
 * @brief Common functions to create and close the LIBIMS context.
 */

/**
 * @ingroup group_init_close
 * @brief LIBIMS Context creation
 * @param ims_config_file_path Path to LIBIMS configuration file.
 * @param implementation_config_file_path Path to LIBIMS implementation specific configuration file (i.e. VISTAS: Network.xml)
 * @param create_context_parameter Context parameters
 * @param ims_context
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_create_context(const char*                     ims_config_file_path,
                                                          const char*                     implementation_config_file_path,
                                                          ims_create_context_parameter_t* create_context_parameter,
                                                          ims_node_t*                     ims_context);

/**
 * @ingroup group_init_close
 * @brief Free a context previously allocated by @ref ims_create_context().
 * @param ims_context [in] The LIBIMS context previously created.
 */
extern LIBIMS_EXPORT void ims_free_context(ims_node_t ims_context);

/**********************************************************
 *  Get the current values of state asked by the protocol *
 **********************************************************/

/**
 * @defgroup group_protocol_state Protocol State
 * @brief Common function to get the state values asked by the protocol.
 */

/**
 * @ingroup group_protocol_state
 * @brief Provide the running state. It is changed only through the ims_import() function.
 * This state is entirely ignored by the rest of LIBIMS library. The library only does the glue between the protocol and this function.
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The @ref ims_running_state_t return code.
 */
extern LIBIMS_EXPORT ims_running_state_t ims_get_running_state(ims_node_t ims_context);

/**
 * @ingroup group_protocol_state
 * @brief Provide the current time ratio. A ratio of 2.0 means twice the speed !
 * This ratio might change only by the call of ims_import().
 * This ratio is entirely ignored by the rest of the LIBIMS library. The library
 * only does the glue between the protocol and this function.
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The current time ratio.
 */
extern LIBIMS_EXPORT float ims_get_time_ratio(ims_node_t ims_context);

/**
 * @ingroup group_protocol_state
 * @brief Provide the powersupply setting.
 * This setting might change only by the call of ims_import().
 * This setting is entirely ignored by the rest of the LIBIMS library. The library
 * only does the glue between the protocol and this function.
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The powersupply state status.
 */
extern LIBIMS_EXPORT bool ims_is_powersupply_on(ims_node_t ims_context);

/*****************************************************************
 *  Synchro functions, only used if step by step mode is enabled *
 *****************************************************************/

/**
 * @defgroup group_step_by_step Step by step mode
 * @brief Synchronization functions available in step by step mode
 */

/**
 * @ingroup group_step_by_step
 * @brief Return the autonomous realtime setting status.
 * This ratio might change only by the call of ims_import().
 * This ratio is entirely ignored by the rest of the LIBIMS library. The library
 * only does the glue between the protocol and this function.
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The realtime settings status.
 */
extern LIBIMS_EXPORT bool ims_is_autonomous_realtime(ims_node_t ims_context);

/**
 * @ingroup group_step_by_step
 * @remark <b>IMPORTANT</b> After this method has been called, this method will then return 0 until a new request is received.
 * @brief When the expected number of steps is executed (i.e. when ims_progress() has progressed the simulation time of at least steps*periods_us micro seconds ), LIBIMS will acknowledge automatically the request.
 * Return 0 if no request received, or return the number of steps ( > 0) if received a request.
 * This setting might change only by the call of ims_import().
 * This setting is entirely ignored by the rest of the LIBIMS Library. The library
 * only does the glue between the protocol and this function.
 * @param ims_context [in] The LIBIMS context previously created.
 * @return Number of request received.
 */
extern LIBIMS_EXPORT uint32_t ims_get_synchro_steps_request(ims_node_t ims_context);

/********************************
 * Generic infomation functions *
 ********************************/

/**
 * @defgroup group_generic_information Generic informations
 * @brief Common general functions.
 */

/**
 * @ingroup group_generic_information
 * @brief Return the name of the implementation.
 * @return The name of the implementation.
 */
extern LIBIMS_EXPORT const char* ims_implementation_name();

/**
 * @ingroup group_generic_information
 * @brief Return the version of the implementation.
 * @return The version of the implementation.
 */
extern LIBIMS_EXPORT const char* ims_implementation_version();

/**
 * @ingroup group_init_close
 * @brief Return the name of the current virtual component name.
 * @param ims_context [in] The LIBIMS context previously created.
 * @return Virtual component name as a string
 */
extern LIBIMS_EXPORT const char* ims_virtual_component_name(ims_node_t ims_context);

/**
 * @ingroup group_generic_information
 * @brief Convert an ims_direction_t to a string.
 * @param direction [in] The ims_direction_t direction.
 * @return The direction as a string.
 */
extern LIBIMS_EXPORT const char* ims_direction_string(ims_direction_t direction);

/**
 * @ingroup group_generic_information
 * @brief Convert an ims_mode_t to a string.
 * @warning Don't free the returned string!
 * @param mode [in] The ims_mode_t mode.
 * @return The mode as a string.
 */
extern LIBIMS_EXPORT const char* ims_mode_string(ims_mode_t mode);

/**
 * @ingroup group_generic_information
 * @brief Convert an ims_protocol_t to a string.
 * @warning Don't free the returned string!
 * @param protocol [in] The ims_protocol_t protocol.
 * @return The protocol as a string.
 */
extern LIBIMS_EXPORT const char* ims_protocol_string(ims_protocol_t protocol);

/***************************************************************
 * Message configuration functions                             *
 * These function are not designed to be called during runtime *
 * Some may perform memory allocation.                         *
 ***************************************************************/

/**
 * @defgroup group_message_functions Messages configuration
 * @warning These function are not designed to be called during runtime as the may perform internal memory allocation.
 * @brief Functions designed to get message information once configuration files are loaded.
 */

/**
 * @ingroup group_message_functions
 * @brief Get an equipment node according to its name.
 * @param ims_context [in] The LIBIMS context previously created.
 * @param name [in] The name of the desired equipement.
 * @param equipment [out] The filled equipement data pointer.
 * @return The @ref ims_return_code_t return code.
 * @see ims_node_get_child().
 */
extern LIBIMS_EXPORT ims_return_code_t ims_get_equipment(ims_node_t  ims_context,
                                                         const char* name,
                                                         ims_node_t* equipment);

/**
 * @ingroup group_message_functions
 * @brief Get an application node according to its name.
 * @param equipment [in] The equipment node.
 * @param name [in] The name of the desired equipement.
 * @param application [out] The filled application data pointer.
 * @return The @ref ims_return_code_t return code.
 * @see ims_node_get_child().
 */
extern LIBIMS_EXPORT ims_return_code_t ims_get_application(ims_node_t  equipment,
                                                           const char* name,
                                                           ims_node_t* application);

/**
 * @ingroup group_message_functions
 * @brief Get a child node according to its name in a given ims_node_t node element.
 * @param parent [in] The parent node.
 * @param name [in] Name of the child defined in the LIBIMS configuration file.
 * @param child [out] Will be filled with and implementation-dependant node identification.
 * @warning Child pointer must not be NULL.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_node_get_child(ims_node_t  parent,
                                                          const char* name,
                                                          ims_node_t* child);

/**
 * @ingroup group_message_functions
 * @brief Get the list of children of a given ims_node_t node element.
 * @param parent_node [in] The parent node.
 * @param node_list [out] Will be initialized with an abstract list of nodes.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_node_get_children(ims_node_t       parent_node,
                                                             ims_node_list_t* node_list);

/**
 * @ingroup group_message_functions
 * @brief Get the next node in the given list.
 * @param node_list [in] Node list initialized by ims_node_get_children().
 * @param next_node [out] The next node in the list, NULL if none.
 * @return The @ref ims_return_code_t return code.
 * @remark This function <i>loops</i>, meaning that if you call again this function after
 * it return a NULL node, it will return again the first node of the list.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_node_list_next(ims_node_list_t node_list,
                                                          ims_node_t*     next_node);

/**
 * @ingroup group_message_functions
 * @brief Free the list initailized by ims_node_get_children().
 * @param node_list [in] The node list to be free.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_node_list_free(ims_node_list_t node_list);

/**
 * @ingroup group_message_functions
 * @brief Get the name of a node element.
 * @param node [in] The considered node element.
 * @return The name of the node as a string.
 */
extern LIBIMS_EXPORT const char* ims_node_get_name(ims_node_t node);

/**
 * @ingroup group_message_functions
 * @brief Get a message and check its consistency.
 * @param node [in] The node containing the message.
 * @param message_protocol [in] The message protocol name. Used to check consistency.
 * @param message_key [in] String identifying the message. Refer to LocalName in the LIBIMS configuration file.
 * @param message_max_size [in] Max size of the message. Used only for consistency checks.@n
 * This value must be the size of the payload provided to read/write function.
 * <table>
 * <tr><th>Protocol<th>Max size
 * <tr><td rowspan="2">AFDX<td><b>Sampling</b> Size of the message.
 * <tr><td><b>Queuing</b> Max size of the message.
 * <tr><td rowspan="2">A429<td><b>Sampling</b> Always 4.
 * <tr><td><b>Queuing</b> Max number of messages to be wrote per cycle * 4.
 * <tr><td rowspan="2">CAN<td><b>Sampling</b> Fixed size of this can message.
 * <tr><td><b>Queuing</b> Max number of messages to be wrote per cycle * message size.
 * <tr><td>Analogue<td>Always 4.
 * <tr><td>Discrete<td>1 or 4. (byte or integer can be used to store discrete, API will handle it correctly)
 * </table>
 * @param message_depth [in] Max number of AFDX queuing messages than can be wrote per cycle. Should be 1 for all other protocols.
 * Used only for consistency checking.
 * @param message_direction [in] Direction of this message. Used only for consistency checking.
 * @param message [out] Will be filled with and implementation-dependant message identification. Must not be NULL.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_get_message(ims_node_t        node,
                                                       ims_protocol_t    message_protocol,
                                                       const char*       message_key,
                                                       uint32_t          message_max_size,
                                                       uint32_t          message_depth,
                                                       ims_direction_t   message_direction,
                                                       ims_message_t*    message);

/**
 * @ingroup group_message_functions
 * @brief Get the message list of a node element.
 * @param parent_node [in] The parent node element.
 * @param messages_list [out] Will be initialized with an abstract list of messages.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_node_get_messages(ims_node_t           parent_node,
                                                             ims_messages_list_t* messages_list);

/**
 * @ingroup group_message_functions
 * @brief Get the next message in the given list.
 * @param messages_list [in] The messages list initialized by ims_node_get_messages().
 * @param next_message [out] The next message in the list, NULL if none.
 * @remark This function <i>loops</i>, meaning that if you call again this function after
 * it return a NULL message, it will return again the first message of the list.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_messages_list_next(ims_messages_list_t messages_list,
                                                              ims_message_t*      next_message);

/**
 * @ingroup group_message_functions
 * @brief Free the list initialized by ims_node_get_messages().
 * @param messages_list [in] The message list to be free.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_messages_list_free(ims_messages_list_t messages_list);

/**
 * @ingroup group_message_functions
 * @brief Get the message name.
 * @param message [in] The message element.
 * @return The message name as a string.
 */
extern LIBIMS_EXPORT const char*       ims_message_get_name(                 ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Get the message protocol.
 * @param message [in] The message element.
 * @return The message protocol as a ims_protocol_t value.
 */
extern LIBIMS_EXPORT ims_protocol_t    ims_message_get_protocol(             ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Get the message direction.
 * @param message [in] The message element.
 * @return The message direction as a ims_direction_t value.
 */
extern LIBIMS_EXPORT ims_direction_t   ims_message_get_direction(            ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Get the message mode.
 * @param message [in] The message element.
 * @return The message mode as a ims_mode_t value.
 */
extern LIBIMS_EXPORT ims_mode_t        ims_message_get_mode(                 ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Get the message validity duration in micro seconds.
 * @param message [in] The message element.
 * @return The message validity duration in micro seconds.
 */
extern LIBIMS_EXPORT uint32_t          ims_message_get_validity_duration_us( ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Get the message maximum size.
 * @param message [in] The message element.
 * @return The message maximum size.
 */
extern LIBIMS_EXPORT uint32_t          ims_message_get_max_size(             ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief The the message depth.
 * @param message [in] The message element.
 * @return The message depth value.
 */
extern LIBIMS_EXPORT uint32_t          ims_message_get_depth(                ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief The message local name.
 * @param message [in] The message element.
 * @return The message local name.
 */
extern LIBIMS_EXPORT const char*     ims_message_get_local_name(				ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Return an ID to gather messages which need to be sent at the same time.@n
 * - For A429 and CAN messages : return the name of the bus
 * - For discrete/analog/NAD messages : return the name of the group
 * - For AFDX : return the message name (each AFDX message is sent independently)
 * .
 * @param message [in] The message element.
 * @return The bus name as a string.
 */
extern LIBIMS_EXPORT const char*     ims_message_get_bus_name(				ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief The refresh period associated to the message. If not defined or for queuing, it returns 0.
 * @param message [in] The message element.
 * @return The message period in microseconds.
 */
extern LIBIMS_EXPORT uint32_t        ims_message_get_period_us(				ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief The data type for NAD messages. If the message is not a NAD one, it returns ims_nad_undefined.
 * @param message [in] The message element.
 * @return The ims_nad_type_t data type.
 */
extern LIBIMS_EXPORT ims_nad_type_t  ims_message_get_nad_type(				ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief The first data size of the NAD messages. If the message is not a NAD one, it returns 0.
 * @param message  [in] The message element.
 * @return The first data size.
 */
extern LIBIMS_EXPORT uint32_t        ims_message_get_nad_dim1(				ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief The second data size of the NAD messages.
 * @param message  [in] The message element. If the message is not a NAD one, it returns 0.
 * @return The second data size.
 */
extern LIBIMS_EXPORT uint32_t        ims_message_get_nad_dim2(				ims_message_t message);

/**
 * @ingroup group_message_functions
 * @brief Update the id parameter of a message.@n
 * @param message [in] The message element.
 * @param message_addr [in] The new message id.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_set_id(ims_message_t		message,
											      uint32_t			message_id);

/**
 * @ingroup group_message_functions
 * @brief Set the validity duration for a sampling message.@n
 * The implementation should have a default value such as 3 times the emmission period.@n
 * This function can be used to overwrite this value.@n
 * Use a null value to disable validity checking.
 * @warning This function should not be called on wired nor queuings messages.
 * @param message [in] The message element.
 * @param validity_duration_us [in] The validity duration since the implementation has receive the message.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_message_set_sampling_timeout(ims_message_t message,
                                                                        uint32_t      validity_duration_us);

/*************************
 * Model cycle functions *
 *************************/

/**
 * @defgroup group_cycle Cycle
 * @brief Functions mainly used and oprimized for during runtime cycles.
 */

/**
 * @ingroup group_cycle
 * @brief Make time progress for the given microseconds.@n
 * The time is needed, depending of the implementation, for:@n
 * - Send wired periodically (ims_send_all()).
 * - Compute input sampling validity (ims_read_sampling_message()).
 * - Mark output messages as invalid when they are.
 * .
 * @param ims_context [in] The LIBIMS context previously created.
 * @param us [in] The number of microsecond for time progression.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_progress(ims_node_t ims_context, uint32_t us);

/**
 * @ingroup group_cycle
 * @brief Make incomming messages available to read/pop functions.@n
 * This method also date the messages to compute sampling validity.@n
 * This function will return immediatelly when no more messages available or
 * timeout_us reached.
 * @param ims_context [in] The LIBIMS context previously created.
 * @param timeout_us [in] Maximum time available for this method.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_import(ims_node_t ims_context, uint32_t timeout_us);

/**
 * @ingroup group_cycle
 * @brief Send all messages.
 * Depending on the implementation, this method will:@n
 * - Send messages writed by methods ims_write_sampling_message() and ims_queuing_message_pending().
 * - Send wired periodically.
 * - Mark output messages as invalid when they are.
 * .
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_send_all(ims_node_t ims_context);

/***********************
 * Stop/Hold functions *
 ***********************/

/**
 * @defgroup group_stop_hold Stop & Hold
 * @brief Main functions used for stop and hold purposes.
 */

/**
 * @ingroup group_stop_hold
 * @brief Reset all received and prepared data.@n
 * For input messages:@n
 *  - Sampling protocoled messages become empty and invalid.
 *  - Sampling wired return to their default value.
 *  - Queuing become empty.
 *  - NAD become "never received".
 * .
 * For output messages:@n
 *  - protocoled messages: Nothing will be send at the next ims_send_all().
 *  - Wired messages: If messages has to be send, they will be with their default value.
 *  - NAD: payload filed with 0.
 * .
 * @param message [in] The message element.
 * @return The ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_reset_message(ims_message_t message);

/**
 * @ingroup group_stop_hold
 * @brief Reset all messages for the given context, and also reset the simulation time and
 * protocol counters if any (for instance VISTAS sequence number).
 * @see ims_reset_message()
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_reset_all(ims_node_t ims_context);

/**
 * @ingroup group_stop_hold
 * @brief Invalidate a sampling message.@n
 * For input messages, mark them as invalid.@n
 * for output messages, notify they are invalid, if the implementation support this functionality.
 * @param message [in] The message element.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_invalidate_sampling_message(ims_message_t message);

/**
 * @ingroup group_stop_hold
 * @brief Invalidate all messages for the given context.
 * @see ims_invalidate_sampling_message()
 * @param ims_context [in] The LIBIMS context previously created.
 * @return The ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_invalidate_all_sampling_messages(ims_node_t ims_context);

/******************************
 * Access to messages content *
 ******************************/

/**
 * @defgroup group_message_content Message content
 * @brief Access to the content of messages. Read / write data to them.
 */

/**
 * @ingroup group_message_content
 * @brief Write to a sampling message.@n
 * This function doesn't effectively send the message.@n
 * If this function is called several times before the call to ims_send_all(), only the last write will be effectively send.
 * <table>
 * <tr><th>Protocol<th>Size
 * <tr><td>AFDX<td>The full payload must be writed, the API will not modify FS.
 * <tr><td>A429<td>The message_size will always be 4. The implementation will update number, SDI and the parity bit.
 * <tr><td>CAN<td>The message_size is the CAN length.
 * <tr><td>Discrete<td>Engineering value: The logic is handled by the implementation. The size can be 1 or 4.
 * <tr><td>Analogue<td>Engineering float IEEE 754 value. The size must be 4.
 * </table>
 * @param message [in] The message element.
 * @param message_addr [in] Pointer to the message payload. (We can't write several samplings at one time)
 * @param message_size [in] Size of the message payload. Should be the same as provided to ims_get_message().
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_write_sampling_message(ims_message_t message,
                                                                  const char*   message_addr,
                                                                  uint32_t      message_size);

/**
 * @ingroup group_message_content
 * @brief Read a sampling message.@n
 * Once received, the message will never be empty, but the validity flag might become invalid.@n
 * Validity will be always valid for wired messages, even if message has never been received.@n
 * In this last case, a default valude will be filled in message_addr & message_size.@n
 * For AFDX, A429 and CAN, message_validity will be set to invalid if the message has not been received during its validity period.@n
 * If the message has never been received, addr will not be modified and size will be set to 0.@n
 * message_validity will be set to ims_never_received.@n
 * The discrete value will not necessarily be 0 and 1. So, never compare to 1!
 * @see ims_set_sampling_message_timeout()
 * @see ims_write_sampling_message()
 * @param message [in] The message element.
 * @param message_addr [out] Will be filled with the message payload. Its size must be at least the one provided to @see ims_get_message().
 * @param message_size [out] Will be filled with the message size.
 * @param message_validity [out] Will be filled with the message validity.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_read_sampling_message(ims_message_t   message,
                                                                 char*           message_addr,
                                                                 uint32_t*       message_size,
                                                                 ims_validity_t* message_validity);

/**
 * @ingroup group_message_content
 * @brief Push a queuing message.@n
 * Add a message to the queue which will be sent with ims_send_all().
 * What happens if you get the @ref ims_return_code_t::ims_message_queue_full ?
 * - A429, CAN : Total message_size of all ims_push_queuing_message() calls since last send() is bigger than message_depth * message_size.@n
 * - AFDX : Number of ims_push_queuing_message() calls since last send() is bigger than message_depth.
 * .
 * @param message [in] The message element.
 * @param message_addr [in] Pointer to the message(s) payload. This payload may contain severals messages for A429 and CAN but only one for AFDX.
 * @param message_size [in] Size of the payload, in bytes. (i.e. Number of messages * message size)
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_push_queuing_message(ims_message_t  message,
                                                                const char*    message_addr,
                                                                uint32_t       message_size);

/**
 * @ingroup group_message_content
 * @brief Receive a queuing message.@n
 * If the implementation has more data than the application can read (i.e. more than max_message_size provided to ims_get_message()) :
 * AFDX: The rest of the message is dropped.
 * A429, CAN: The rest of data will be available for the next cycle.
 * @param message [in] The message element.
 * @param message_addr [out]  Will be filled with the message(s) payload. Its size must be at least the one provided to ims_get_message().
 * @param message_size [out] Will be filled with the payload size, in bytes. (i.e. Number of messages * message size)
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_pop_queuing_message(ims_message_t message,
                                                               char*         message_addr,
                                                               uint32_t*     message_size);

/**
 * @ingroup group_message_content
 * @brief Return the number of queuing message available.@n
 * For input parameter, it's the number of messages received by the implementation but not readed by the application.@n
 * For output parameter, it's the number of messages pushed but not actually send.
 * @param message [in] The message element.
 * @param messages_count [out] Will be filled with the number of messages.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_queuing_message_pending(ims_message_t message,
                                                                   uint32_t*     messages_count);

/**
 * @ingroup group_message_content
 * @brief Write to a NAD message.@n
 * This function doesn't effectively send the message.@n
 * If this function is called several times before the call to ims_send_all(),
 * only the last write will be effectively send.
 * @param message [in] The message element.
 * @param message_addr [in] Pointer to the message payload. (We can't write several NADs at one time).
 * @param message_size [in] Size of the message payload. Should be the same as provided to ims_get_message().
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_write_nad_message(ims_message_t message,
                                                             const char*   message_addr,
                                                             uint32_t      message_size);

/**
 * @ingroup group_message_content
 * @brief Read a NAD message.@n
 * Once received, the message will never be empty.@n
 * If the message has never been received, validity will be set to  ims_never_received and the
 * payload will not be modified.@n
 * If the message has been received, validity will always be ims_valid and the payload filled with
 * the last received value.
 * @param message [in] The message element.
 * @param message_addr [out] Will be filled with the message payload. Its size must be at least the one provided to @see ims_get_message().
 * @param message_size [out] Will be filled with the message size.
 * @param message_validity [out] Will be filled with the message validity.
 * @return The @ref ims_return_code_t return code.
 */
extern LIBIMS_EXPORT ims_return_code_t ims_read_nad_message(ims_message_t   message,
                                                            char*           message_addr,
                                                            uint32_t*       message_size,
                                                            ims_validity_t* message_validity);

#ifdef __cplusplus
};
#endif


#endif

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
 * Helper to provide list to the C interface
 */
#include "ims_node.hh"

// C API interface 
// Beacause of the use of namesapces, we can't directly use this type in C++ code
struct ims_internal_node_list_t {};
struct ims_internal_messages_list_t {};

namespace ims
{
// Node children list
class node_children_list : public ims_internal_node_list_t
{
public:
    inline node_children_list(weak_node_ptr parent);
    weak_node_ptr next();
    
private:
    weak_node_ptr        _parent;
    node_map_t::iterator _node_iterator;
};

// Node messages list
class node_messages_list : public ims_internal_messages_list_t
{
public:
    inline node_messages_list(weak_node_ptr parent);
    weak_message_ptr next();
    
private:
    weak_node_ptr           _parent;
    message_map_t::iterator _messages_iterator;
};

// ==========================================================================
// Inlines
// ==========================================================================
node_children_list::node_children_list(weak_node_ptr parent) :
    _parent(parent),
    _node_iterator(parent->children_end())
{
}

node_messages_list::node_messages_list(weak_node_ptr parent) :
    _parent(parent),
    _messages_iterator(parent->messages_end())
{
}

}

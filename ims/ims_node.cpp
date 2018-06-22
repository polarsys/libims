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
 * Base class for all IMS nodes
 */
#include "ims_node.hh"

namespace ims
{

//
// Add a new child
//
void node::add_child(node_ptr child) throw (ims::exception)
{
    if (_children.find(child->get_name()) != _children.end()) {
        THROW_IMS_ERROR(ims_init_failure, "Node '" << child->get_name() <<
                        "' already exists in node '" << get_path() << "'!");
    }

    _children.insert(node_map_t::value_type(child->get_name(), child));
}

//
// Look for a child
//
node_ptr node::find_child(std::string name) throw (ims::exception)
{
    node_map_t::iterator it = _children.find(name);
    if (it == _children.end()) {
        THROW_IMS_ERROR(ims_invalid_configuration, "Node '" << name <<
                        "' doesn't exists in node '" << get_path() << "'!");
    }

    return it->second;
}

//
// Add a new message
//
void node::add_message(std::string name, message_ptr message)
throw (ims::exception)
{
    if (_messages.find(name) != _messages.end()) {
        THROW_IMS_ERROR(ims_init_failure, "Message '" << name <<
                        "' already exists in node '" << _name << "'!");
    }

    _messages.insert(message_map_t::value_type(name, message));
}

//
// Look for a message
//
message_ptr node::find_message(std::string name)
throw (ims::exception)
{
    message_map_t::iterator it = _messages.find(name);
    if (it == _messages.end()) {
        THROW_IMS_ERROR(ims_invalid_configuration, "Message '" << name <<
                        "' doesn't exists in node '" << _name << "'!");
    }

    return it->second;
}

//Debug stuff
void node::dump(std::string prefix)
{
    LOG_SAY(prefix << "Node '" << _name << "'");

    if (prefix.empty() == true) {
        prefix = "  + ";
    } else {
        prefix = "  " + prefix;
    }

    for (node_map_t::iterator it = _children.begin();
         it != _children.end();
         it++) {
        it->second->dump(prefix);
    }

    for (message_map_t::iterator it = _messages.begin();
         it != _messages.end();
         it++) {
        it->second->dump(prefix);
    }
}
}

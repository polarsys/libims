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
#ifndef _IMS_NODE_HH_
#define _IMS_NODE_HH_
#include "ims_log.hh"
#include "shared_ptr.hh"
#include "ims_message.hh"

// C API interface 
// Beacause of the use of namesapces, we can't directly use this type in C++ code
struct ims_internal_node_t {};

namespace ims
{

// Pointer to node
class node;
typedef shared_ptr<node> node_ptr;
typedef node*            weak_node_ptr;

typedef std::tr1::unordered_map<std::string, node_ptr>  node_map_t;

class node : public ims_internal_node_t
{
public:

    // Ctor
    inline static node_ptr create(std::string name, weak_node_ptr parent = NULL);
    inline node(std::string name, weak_node_ptr parent = NULL);
    inline virtual ~node() {}

    // Accessor
    inline std::string   get_name();
    inline weak_node_ptr get_parent();
    inline std::string   get_path();    // Will alloc for all non-root nodes

    // Children
    void add_child(node_ptr child) throw (ims::exception);
    node_ptr find_child(std::string name) throw (ims::exception);
    inline node_map_t::iterator children_begin();
    inline node_map_t::iterator children_end();

    // Safe cast node to derived classes
    template <typename node_derived>
    static inline shared_ptr<node_derived> cast(node_ptr node);

    template <typename node_derived>
    static inline node_derived* cast(weak_node_ptr node);

    // Messages
    void add_message(std::string name, message_ptr message) throw (ims::exception);
    message_ptr find_message(std::string name) throw (ims::exception);
    inline message_map_t::iterator messages_begin();
    inline message_map_t::iterator messages_end();

    //Debug stuff
    virtual void dump(std::string prefix = std::string());

protected:

    std::string   _name;
    weak_node_ptr _parent;
    node_map_t    _children;
    message_map_t _messages;
};

// ==========================================================================
// Inlines/templates
// ==========================================================================
node_ptr node::create(std::string name, weak_node_ptr parent)
{
    return node_ptr(new node(name, parent));
}

node::node(std::string name, weak_node_ptr parent) :
    _name(name),
    _parent(parent)
{
}

std::string   node::get_name()   { return _name;                                                }
weak_node_ptr node::get_parent() { return _parent;                                              }
std::string   node::get_path()   { return (_parent)? _parent->get_path() + '/' + _name : _name; }

node_map_t::iterator node::children_begin()    { return _children.begin(); }
node_map_t::iterator node::children_end()      { return _children.end();   }
message_map_t::iterator node::messages_begin() { return _messages.begin(); }
message_map_t::iterator node::messages_end()   { return _messages.end();   }

template <typename node_derived>
shared_ptr<node_derived> node::cast(node_ptr node)
{
    shared_ptr<node_derived> typed_node = dynamic_pointer_cast<node_derived>(node);
    if (typed_node == NULL) {
        THROW_IMS_ERROR(ims_invalid_configuration, "IMS node " << node->get_path() << " is not a " << typeid(node_derived).name());
    }
    return typed_node;
}

template <typename node_derived>
node_derived* node::cast(weak_node_ptr node)
{
    node_derived* typed_node = dynamic_cast<node_derived*>(node);
    if (typed_node == NULL) {
        THROW_IMS_ERROR(ims_invalid_configuration, "IMS node " << node->get_path() << " is not a " << typeid(node_derived).name());
    }
    return typed_node;
}

}

#endif

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
#include "ims_list.hh"

namespace ims
{
  //
  // Traverse node list
  //
  weak_node_ptr node_children_list::next()
  {
    if (_node_iterator == _parent->children_end()) {
      _node_iterator = _parent->children_begin();
    } else {
      _node_iterator++;
    }
   
    if (_node_iterator == _parent->children_end()) {
      return NULL;
    } else {
      return _node_iterator->second.get();
    }
  }

  //
  // Traverse messages list
  //
  weak_message_ptr node_messages_list::next()
  {
    if (_messages_iterator == _parent->messages_end()) {
      _messages_iterator = _parent->messages_begin();
    } else {
      _messages_iterator++;
    }
   
    if (_messages_iterator == _parent->messages_end()) {
      return NULL;
    } else {
      return _messages_iterator->second.get();
    }
  }
}

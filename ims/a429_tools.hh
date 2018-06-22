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
// Helpers for A429
//
#ifndef __A429_TOOLS_H__
#define __A429_TOOLS_H__
#include <stdint.h>
#include <string>

namespace a429
{

#define A429_LABEL_SIZE  4


// full label (4 bytes)
typedef uint8_t label_t[A429_LABEL_SIZE];

// Label number
typedef uint8_t label_number_t;

// Label SDI
enum sdi_t {
    SDI_00 = 0,
    SDI_01 = 1,
    SDI_10 = 2,
    SDI_11 = 3,
    SDI_XX = 4,        // SDI is used as payload extension
	SDI_DD = 5,
    SDI_NOT_USED,
};


//
// Encode a string representing an octal number in an A429 label number
// Throw an std::exception on error.
//
extern label_number_t label_number_encode(std::string octal_number);

//
// Decode a label number into a string representing an octal number
// Never free the returned buffer!
//
extern const char* label_number_decode(uint8_t number);

//
// Return the label number of a label
//
inline label_number_t label_number_get(label_t label) { return label[3]; }

//
// Convert a string to SDI
// If the string is invalid SDI_DD will be returned.
//
extern sdi_t sdi_encode(std::string sdi);

//
// Return the sdi of a label
//
inline sdi_t sdi_get(label_t label) { return sdi_t(label[2] & 0x03); }


//
// Update label with label number, sdi and fix its parity
// SDI payload is not modified if sdi is DD or NOT_USED.
//
extern void update_label(label_t label, label_number_t label_number, sdi_t sdi);

//
// Fix the parity bit in the given label.
//
extern void update_parity(label_t label);

}

#endif

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
// A429 tools
//
#ifndef _A429_TOOLS_H_
#define _A429_TOOLS_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

# define SDI_IS_PAYLOAD  5

//
// "exploded" label
//
typedef struct {
    uint8_t  parity;
    uint8_t  ssm;
    uint32_t payload;     // Max 1FFFFF if SDI is payload extension, else max 7FFFF.
    uint8_t  sdi;         // May be filed with SDI_IS_PAYLOAD
    uint8_t  number;
} ims_test_a429_exploded_label;

//
// Fill the provided label (4 bytes) with given data
// if sdi == SDI_IS_PAYLOAD, sdi will not be set and be used to fill payload.
// payload max is 1FFFFF if SDI is payload extension, else 7FFFF.
// THERE IS NO ERROR MANAGMENT! (payload too big, invalid ssm, ...)
//
extern void ims_test_a429_fill_label(char*    label,
                                     uint32_t number,
                                     uint32_t sdi,
                                     uint32_t payload,
                                     uint32_t ssm,
                                     uint32_t parity);

//
// Extract label in exploded.
// if sdi_is_payload, exploded->sdi will be set to SDI_IS_PAYLOAD and
// sdi bits in label will be used for exploded->payload.
//
extern void ims_test_a429_explode_label(const char*                   label,
                                        ims_test_a429_exploded_label* exploded,
                                        int                           sdi_is_payload);

//
// Return non-0 if a and b are the same
//
extern int ims_test_a429_explode_cmp(ims_test_a429_exploded_label* a,
                                     ims_test_a429_exploded_label* b);

//
// Encode a string representing an octal number in an A429 label number
//
extern uint8_t ims_test_a429_label_number_encode(const char* octal_number);

//
// Decode a label number into a string representing an octal number
// Never free the returned buffer!
//
extern const char* ims_test_a429_label_number_decode(uint8_t number);

#ifdef __cplusplus
}
#endif
#endif

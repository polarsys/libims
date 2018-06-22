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
#include "a429_tools.h"
#include "htonl.h"
#include <string.h>

#define LABEL_NUMBER_BYTE        3
#define LABEL_NUMBER_MASK        0xFF

#define LABEL_SDI_MASK           3

#define LABEL_PAYLOAD_SDI_SHIFT  2
#define LABEL_PAYLOAD_SHIFT      8
#define LABEL_PAYLOAD_SDI_MASK   0x1FFFFF00

#define LABEL_SSM_SHIFT          5
#define LABEL_SSM_MASK           (3 << LABEL_SSM_SHIFT)

#define LABEL_PARITY_SHIFT       7
#define LABEL_PARITY_MASK        (1 << LABEL_PARITY_SHIFT)

#define CHAR_0  '0'

static const char* number_2_octal_string[256] = {
    "000", "200", "100", "300", "040", "240", "140", "340", // labels 000 - 007
    "020", "220", "120", "320", "060", "260", "160", "360", // labels 010 - 017
    "010", "210", "110", "310", "050", "250", "150", "350", // labels 020 - 027
    "030", "230", "130", "330", "070", "270", "170", "370", // labels 030 - 037
    "004", "204", "104", "304", "044", "244", "144", "344", // labels 040 - 047
    "024", "224", "124", "324", "064", "264", "164", "364", // labels 050 - 057
    "014", "214", "114", "314", "054", "254", "154", "354", // labels 060 - 067
    "034", "234", "134", "334", "074", "274", "174", "374", // labels 070 - 077
    "002", "202", "102", "302", "042", "242", "142", "342", // labels 100 - 107
    "022", "222", "122", "322", "062", "262", "162", "362", // labels 110 - 117
    "012", "212", "112", "312", "052", "252", "152", "352", // labels 120 - 127
    "032", "232", "132", "332", "072", "272", "172", "372", // labels 130 - 137
    "006", "206", "106", "306", "046", "246", "146", "346", // labels 140 - 147
    "026", "226", "126", "326", "066", "266", "166", "366", // labels 150 - 157
    "016", "216", "116", "316", "056", "256", "156", "356", // labels 160 - 167
    "036", "236", "136", "336", "076", "276", "176", "376", // labels 170 - 177
    "001", "201", "101", "301", "041", "241", "141", "341", // labels 200 - 207
    "021", "221", "121", "321", "061", "261", "161", "361", // labels 210 - 217
    "011", "211", "111", "311", "051", "251", "151", "351", // labels 220 - 227
    "031", "231", "131", "331", "071", "271", "171", "371", // labels 230 - 237
    "005", "205", "105", "305", "045", "245", "145", "345", // labels 240 - 247
    "025", "225", "125", "325", "065", "265", "165", "365", // labels 250 - 257
    "015", "215", "115", "315", "055", "255", "155", "355", // labels 260 - 267
    "035", "235", "135", "335", "075", "275", "175", "375", // labels 270 - 277
    "003", "203", "103", "303", "043", "243", "143", "343", // labels 300 - 307
    "023", "223", "123", "323", "063", "263", "163", "363", // labels 310 - 317
    "013", "213", "113", "313", "053", "253", "153", "353", // labels 320 - 327
    "033", "233", "133", "333", "073", "273", "173", "373", // labels 330 - 337
    "007", "207", "107", "307", "047", "247", "147", "347", // labels 340 - 347
    "027", "227", "127", "327", "067", "267", "167", "367", // labels 350 - 357
    "017", "217", "117", "317", "057", "257", "157", "357", // labels 360 - 367
    "037", "237", "137", "337", "077", "277", "177", "377"  // labels 370 - 377
};

//
// Fill the provided label (4 bytes) with given data
//
void ims_test_a429_fill_label(char*    label, 
                              uint32_t number,
                              uint32_t sdi,
                              uint32_t payload,
                              uint32_t ssm,
                              uint32_t parity)
{
    // Reset label
    memset(label, 0, 4);

    // Set number
    *(label + LABEL_NUMBER_BYTE) = number & LABEL_NUMBER_MASK;

    // Add SDI to payload if needed
    if (sdi < SDI_IS_PAYLOAD) {
        payload = (payload << LABEL_PAYLOAD_SDI_SHIFT);
        payload += sdi;
    }

    // Set payload and SDI
    payload = (payload << LABEL_PAYLOAD_SHIFT);
    payload = htonl(payload);
    *(uint32_t*)label |= payload;

    // Be sure payload has not overwire SSM + parity
    *label = *label & ((LABEL_SSM_MASK | LABEL_PARITY_MASK) ^ 255);

    // Set SSM
    *label += (ssm << LABEL_SSM_SHIFT) & LABEL_SSM_MASK;

    // Set parity bit
    if (parity) *label |= LABEL_PARITY_MASK;
}

//
// Extract label in exploded.
//
void ims_test_a429_explode_label(const char*                   label, 
                                 ims_test_a429_exploded_label* exploded,
                                 int                           sdi_is_payload)
{
    exploded->parity = (*label & LABEL_PARITY_MASK)? 1 : 0;
    exploded->ssm = (*label & LABEL_SSM_MASK) >> LABEL_SSM_SHIFT;

    exploded->payload = *(uint32_t*)label;
    exploded->payload = (htonl(exploded->payload) & LABEL_PAYLOAD_SDI_MASK) >> LABEL_PAYLOAD_SHIFT;

    if (sdi_is_payload == 0) {
        exploded->sdi = exploded->payload & LABEL_SDI_MASK;
        exploded->payload = exploded->payload >> LABEL_PAYLOAD_SDI_SHIFT;
    } else {
        exploded->sdi = SDI_IS_PAYLOAD;
    }

    exploded->number = *(label + LABEL_NUMBER_BYTE);
}

//
// Return non-0 if a and b are the same
//
int ims_test_a429_explode_cmp(ims_test_a429_exploded_label* a, 
                              ims_test_a429_exploded_label* b)
{
    return (memcmp(a, b, sizeof(ims_test_a429_exploded_label)))? 0 : 1;
}

//
// Encode a string representing an octal number in an A429 label number
//
uint8_t ims_test_a429_label_number_encode(const char* octal_number)
{
    uint8_t number = 0;

    // Get and check string length
    // Remove first 0 if string use C octal notation
    int len = strlen(octal_number);
    if (len == 4 && octal_number[0] == CHAR_0) {
        len = 3;
        octal_number = octal_number + 1;
    }
    if (len == 0 || len > 3) return 0;

    // Extract digits and check them
    int digits[3] = { 0, 0, 0 };
    for (int i = 0; i < len; i++) {
        if (octal_number[i] < CHAR_0 || octal_number[i] > CHAR_0 + 7) return 0;
        digits[len - i - 1] = octal_number[i] - CHAR_0;
    }

    // The major digit cannot be greather than 3 (377 is the max octal 8-bits value)
    if (digits[2] > 3) return 0;

    // Convert digits to their decimal representation according to A429 protocol
    uint8_t inverted_number = (digits[2] << 6) + (digits[1] << 3) + digits[0];
    number = 0
            | ((inverted_number & 0x01) << 7)
            | ((inverted_number & 0x02) << 5)
            | ((inverted_number & 0x04) << 3)
            | ((inverted_number & 0x08) << 1)
            | ((inverted_number & 0x10) >> 1)
            | ((inverted_number & 0x20) >> 3)
            | ((inverted_number & 0x40) >> 5)
            | ((inverted_number & 0x80) >> 7);

    return number;
}

//
// Decode a label number into a string representing an octal number
//
const char* ims_test_a429_label_number_decode(uint8_t number)
{
    return number_2_octal_string[number];
}

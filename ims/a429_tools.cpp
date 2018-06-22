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
#include "a429_tools.hh"

namespace a429
{

//
// Basic exception who hold a message
//
class exception: public std::exception
{
public:
    exception(std::string message = std::string()) : _message(message) {}
    
    virtual const char* what() const throw() { return _message.c_str(); }
    
    virtual ~exception() throw () {}
    
protected:
    std::string _message;
};

//
// Convert an octal number to a string
//
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
// Encode a string representing an octal number in an A429 label number
//
#define CHAR_0  '0'
label_number_t label_number_encode(std::string octal_number)
{
    label_number_t number = 0;

    // Get and check string length
    if (octal_number.size() == 0) {
        throw exception("Invalid empty label number !");
    }

    // Remove first 0 if string use C octal notation
    uint32_t first_digit = 0;
    if (octal_number.size() == 4 && octal_number[0] == CHAR_0) {
        first_digit = 1;
    }
    if (octal_number.size() - first_digit > 3) {
        throw exception("Invalid label number '" + octal_number + "' !");
    }


    // Extract digits and check them
    uint32_t digits[3] = { 0, 0, 0 };
    uint32_t digit_base_id = octal_number.size() - first_digit - 1;
    for (uint32_t i = first_digit; i < octal_number.size(); i++) {
        if (octal_number[i] < CHAR_0 || octal_number[i] > CHAR_0 + 7) {
            throw exception("Invalid label number '" + octal_number + "' !");
        }
        digits[digit_base_id - i] = octal_number[i] - CHAR_0;
    }

    // The major digit cannot be greather than 3 (377 is the max octal 8-bits value)
    if (digits[2] > 3) {
        throw exception("Invalid label number '" + octal_number + "' !");
    }

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
// Never free the returned buffer!
//
const char* label_number_decode(uint8_t number)
{
    return number_2_octal_string[number];
}

//
// Convert a string to SDI
// If the string is invalid SDI_DD will be returned.
//
extern sdi_t sdi_encode(std::string sdi)
{
    uint32_t result = 0;

    if (sdi.size() < 1 || sdi.size() > 2) return SDI_DD;

    if (sdi[0] == '1') result = 1;
	else if (sdi[0] == 'X') result = 1; 
    else if (sdi[0] != '0') return SDI_DD;

    if (sdi.size() == 1) return sdi_t(result);

    result <<= 1;
    if (sdi[1] == '1') result += 1;
	else if (sdi[1] == 'X') result <<= 1;
    else if (sdi[1] != '0') return SDI_DD;

    return sdi_t(result);
}

//
// Update label with label number, sdi and fix its parity
//
void update_label(label_t label, label_number_t label_number, sdi_t sdi)
{
    label[3] = label_number;
    if (sdi <= 3) {
        label[2] &= 0xFC;
        label[2] |= sdi;
    }
    update_parity(label);
}

//
// Fix the parity bit in the given label.
//
static const bool ParityTable256[256] =
{
    #   define P2(n) n, n^1, n^1, n
    #   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
    #   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
    P6(0), P6(1), P6(1), P6(0)
};

void update_parity(label_t label)
{
    label[0] &= 0x7F;  // Don't use parity bit in parity compuatation.

    if (ParityTable256[label[0] ^ label[1] ^ label[2] ^ label[3]] == 0)
    {
        label[0] |= 0x80;
    }
}

}

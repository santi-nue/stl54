/*
 * Copyright 2005-2020 Olivier Aveline <wsgd@free.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//*****************************************************************************
// Includes.
//*****************************************************************************

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"
#include "unitary_tests_ut_interpret_bytes.h"

#include "T_interpret_data.h"


//*****************************************************************************
// test_interpret_simple_position
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_position)
{
    T_type_definitions    type_definitions;
    ut_interpret_bytes_init(type_definitions);

    T_interpret_data      interpret_data;


    M_TEST_SIMPLE("0102030405060708090A", "uint8  var_a; hide raw(9) ignore;", "var_a = 1" K_eol);

    // Move position forward
    M_TEST_SIMPLE("0102030405060708090A", "move_position_bytes 2;                 uint8  var_a; hide raw(7) ignore;", "var_a = 3" K_eol);
    M_TEST_SIMPLE("0102030405060708090A", "move_position_bits 16;                 uint8  var_a; hide raw(7) ignore;", "var_a = 3" K_eol);
    M_TEST_SIMPLE("0102030405060708090A", "move_position_bits  7; hide uint9 var; uint8  var_a; hide raw(7) ignore;", "var_a = 3" K_eol);

    // Move position backward
    M_TEST_SIMPLE("0102030405060708090A", "hide raw(7) ignore; move_position_bytes -2;                 uint8  var_a; hide raw(4) ignore;", "var_a = 6" K_eol);
    M_TEST_SIMPLE("0102030405060708090A", "hide raw(7) ignore; move_position_bits -16;                 uint8  var_a; hide raw(4) ignore;", "var_a = 6" K_eol);
    M_TEST_SIMPLE("0102030405060708090A", "hide raw(7) ignore; move_position_bits -25; hide uint9 var; uint8  var_a; hide raw(4) ignore;", "var_a = 6" K_eol);

    // save_position, goto_position
    M_TEST_SIMPLE("0102030405060708090A",
                  "hide raw(7) ignore; save_position pos; hide raw(2) ignore; goto_position pos; uint8  var_a; hide raw(2) ignore;",
                  "var_a = 8" K_eol);

    // save_position, goto_position on bit position
    M_TEST_SIMPLE("0102030405060708090A",
        "hide uint17 var; save_position pos; hide uint16 var; goto_position pos; uint8  var_a; hide uint7 var; hide raw(6) ignore;",
        "var_a = 6" K_eol);  // 6 = 3 << 1
}


//*****************************************************************************
// test_interpret_simple_position_decode
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_position_decode)
{
    T_type_definitions    type_definitions;
    ut_interpret_bytes_init(type_definitions);
    istringstream         iss(
        "function void  decode_remove_1_byte (in frame  frame, in uint32   nb_of_bits_needed) "
        "{ "
        "    while (nb_of_bits_needed > 0) "
        "    { "
        "        hide uint8  byte1; "
        "        hide uint8  byte2; "
        "        call frame_append_data (frame, byte2); "
        "        set nb_of_bits_needed = nb_of_bits_needed - 8; "
        "    } "
        "} "
    );
    build_types(iss, type_definitions);

    T_interpret_data      interpret_data;
    interpret_data.set_big_endian();

    // Test du décodeur
    M_TEST_SIMPLE("0102030405060708090A", "decoder decode_remove_1_byte; uint8[5]  var_a;",
        "var_a[0] = 2" K_eol
        "var_a[1] = 4" K_eol
        "var_a[2] = 6" K_eol
        "var_a[3] = 8" K_eol
        "var_a[4] = 10" K_eol
    );

    M_TEST_SIMPLE("0102030405060708090A", "decoder decode_remove_1_byte; uint16 var_a; uint8[3] var_b;",
        "var_a = 516" K_eol  // 516 = 2*256 + 4
        "var_b[0] = 6" K_eol
        "var_b[1] = 8" K_eol
        "var_b[2] = 10" K_eol
    );


    // move_position_..., save_position, goto_position "fails" with decoder
    // Ie:
    // - ..._position... do not take care of decoder
    // - decoder is not aware of ..._position...
    M_TEST_ERROR_ALREADY_KNOWN__OPEN(0, "..._position... not compatible with decoder")
        // Here "move_position_bytes 4" moves of 4 physical bytes (instead of 8 using decoder)
        M_TEST_SIMPLE("0102030405060708090A", "decoder decode_remove_1_byte; move_position_bytes 4; uint8  var_a;", "var_a = 10" K_eol);  // gives 6
    }
}

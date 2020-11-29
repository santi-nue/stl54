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
// test_interpret_simple_trailer
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_trailer)
{
    T_type_definitions    type_definitions;
    ut_interpret_bytes_init(type_definitions);

    T_interpret_data      interpret_data;

	type_definitions.trailer_sizeof_bits = 24;
#define K_TRAILER_INPUT   "767778"
#define K_TRAILER_READ    "uint8[3]  trailer ;"
#define K_TRAILER_OUTPUT  "trailer[0] = 118" K_eol "trailer[1] = 119" K_eol "trailer[2] = 120" K_eol

    {
        M_TEST_SIMPLE("4249472d5245515545535453" K_TRAILER_INPUT,
                      "raw(*)  val ;" K_TRAILER_READ,
                      "val = " K_eol
                      "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    " K_eol
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("" K_TRAILER_INPUT,
                      "raw(*)  val ;" K_TRAILER_READ,
                      ""
                      K_TRAILER_OUTPUT);
    }

    {
        C_ut_interpret_bytes_no_decode_guard  uibndg;    // [*] & [+] rejected with decode
        M_TEST_SIMPLE("4249472d5245515545535453" K_TRAILER_INPUT,
                      "uint8[*]  val ;" K_TRAILER_READ,
                      "val[0] = 66" K_eol
                      "val[1] = 73" K_eol
                      "val[2] = 71" K_eol
                      "val[3] = 45" K_eol
                      "val[4] = 82" K_eol
                      "val[5] = 69" K_eol
                      "val[6] = 81" K_eol
                      "val[7] = 85" K_eol
                      "val[8] = 69" K_eol
                      "val[9] = 83" K_eol
                      "val[10] = 84" K_eol
                      "val[11] = 83" K_eol
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("4249472d5245515545535453" K_TRAILER_INPUT,
                      "uint8[+]  val ;" K_TRAILER_READ,
                      "val[0] = 66" K_eol
                      "val[1] = 73" K_eol
                      "val[2] = 71" K_eol
                      "val[3] = 45" K_eol
                      "val[4] = 82" K_eol
                      "val[5] = 69" K_eol
                      "val[6] = 81" K_eol
                      "val[7] = 85" K_eol
                      "val[8] = 69" K_eol
                      "val[9] = 83" K_eol
                      "val[10] = 84" K_eol
                      "val[11] = 83" K_eol
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("" K_TRAILER_INPUT,
                      "uint8[*]  val ;" K_TRAILER_READ,
                      ""
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("42" K_TRAILER_INPUT,
                      "uint8[+]  val ;" K_TRAILER_READ,
                      "val[0] = 66" K_eol
                      K_TRAILER_OUTPUT);
    }
}

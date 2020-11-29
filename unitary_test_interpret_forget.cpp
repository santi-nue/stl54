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
#include "byte_interpret.h"


//*****************************************************************************
// test_interpret_forget
//*****************************************************************************

M_TEST_FCT(test_interpret_forget)
{
    T_type_definitions    type_definitions;
    ut_interpret_bytes_init(type_definitions);
    istringstream         iss(
        "struct T_forget "
        "{ "
        "    var int32  struct_counter = struct_counter + 1; "
        "} "
    );
    build_types(iss, type_definitions);

    T_interpret_data      interpret_data;

    // Test with simple variable
    M_TEST_EQ(interpret_data.is_read_variable("value"), false);
    M_TEST_SIMPLE("", " forget var uint32  value = 3457 ;", "value = 3457" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), false);            // value is forgotten
    M_TEST_SIMPLE("", "        var uint32  value = 7 ;", "value = 7" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), true);
    M_TEST_SIMPLE("", "        print (\"%d\", value);", "7" K_eol);
    M_TEST_SIMPLE("", " forget var uint32  value = 3457 ;", "value = 3457" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), true);
    M_TEST_SIMPLE("", "        print (\"%d\", value);", "7" K_eol);            // value 3457 is forgotten
    M_TEST_SIMPLE("", "        var uint32  value = 3 ;", "value = 3" K_eol);
    M_TEST_SIMPLE("", "        print (\"%d\", value);", "3" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), true);

    // Test with struct
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), false);
    M_TEST_SIMPLE("", "    var int32  struct_counter = 1000 ;", "struct_counter = 1000" K_eol);
    M_TEST_SIMPLE("", "        T_forget      st1 ;", "st1.struct_counter = 1001" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), true);
    M_TEST_SIMPLE("", " forget T_forget      st2 ;", "st2.struct_counter = 1002" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), true);
    M_TEST_SIMPLE("", "        T_forget      st3 ;", "st3.struct_counter = 1002" K_eol);   // st2 (and its struct_counter) is forgotten
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), true);

    // Test with array : each item of the array is forgot (one by one, not at the end of the array)
    M_TEST_SIMPLE("", " forget T_forget[3]   sta ;", "sta[0].struct_counter = 1003" K_eol
                                                     "sta[1].struct_counter = 1003" K_eol
                                                     "sta[2].struct_counter = 1003" K_eol);
    M_TEST_SIMPLE("", "        T_forget      st7 ;", "st7.struct_counter = 1003" K_eol);
}

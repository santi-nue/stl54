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
// test_is_existing_field_or_variable
//*****************************************************************************

M_TEST_FCT(test_is_existing_field_or_variable)
{
    T_type_definitions    type_definitions;
    build_types("unitary_tests_basic.fdesc",
                type_definitions);

    T_interpret_data      interpret_data;

    M_TEST_SIMPLE("", "T_is_existing_field_or_variable    st;", "st.in_main_struct = 1" K_eol);

    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_none_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_not_exist_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_main_struct_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_if_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_else_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_while_begin")->get_bool(), false);

    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_none_end")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_not_exist_end")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_main_struct_end")->get_bool(), true);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_if_end")->get_bool(), true);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_else_end")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_while_end")->get_bool(), true);
}

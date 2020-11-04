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

using namespace std;

#include "unitary_tests_tools.h"
#include "T_interpret_read_values.h"


//*****************************************************************************
// test_read_values
//*****************************************************************************

M_TEST_FCT(test_read_values)
{
    T_interpret_read_values    read_values;

    // Tests d'existence et de valeurs

    M_TEST_EQ(read_values.is_read_variable("toto"), false);
    M_TEST_EQ(read_values.is_read_variable("tata"), false);
    M_TEST_EQ(read_values.is_read_variable("titi"), false);
    M_TEST_EQ(read_values.is_read_variable("tutu"), false);
    M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

    read_values.add_read_variable("toto", 11);

    M_TEST_EQ(read_values.is_read_variable("toto"), true);
    M_TEST_EQ(read_values.is_read_variable("tata"), false);
    M_TEST_EQ(read_values.is_read_variable("titi"), false);
    M_TEST_EQ(read_values.is_read_variable("tutu"), false);
    M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

    read_values.add_read_variable("tata", 22);

    M_TEST_EQ(read_values.is_read_variable("toto"), true);
    M_TEST_EQ(read_values.is_read_variable("tata"), true);
    M_TEST_EQ(read_values.is_read_variable("titi"), false);
    M_TEST_EQ(read_values.is_read_variable("tutu"), false);
    M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

    read_values.add_read_variable("titi", 33);
    read_values.add_read_variable("tutu", 44);

    M_TEST_EQ(read_values.is_read_variable("toto"), true);
    M_TEST_EQ(read_values.is_read_variable("tata"), true);
    M_TEST_EQ(read_values.is_read_variable("titi"), true);
    M_TEST_EQ(read_values.is_read_variable("tutu"), true);
    M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

    M_TEST_EQ(read_values.get_P_value_of_read_variable("toto")->get_int(), 11);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("tata")->get_int(), 22);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("titi")->get_int(), 33);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("tutu")->get_int(), 44);

    read_values.read_variable_group_begin("titi");
    read_values.add_read_variable("tutu", 55);
    read_values.add_read_variable("allo", 111);
    read_values.add_read_variable("bonjour", 222);
    read_values.add_read_variable("c_est_moi", 333);
    read_values.add_read_variable("comment_ca_va", 444);
    read_values.add_read_variable("bien_et_toi", 444);
    read_values.add_read_variable("ping", "pong");

    M_TEST_EQ(read_values.is_read_variable("titi.tutu"), true);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("tutu")->get_int(), 55);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("titi.tutu")->get_int(), 55);

    read_values.read_variable_group_end();

    M_TEST_EQ(read_values.get_P_value_of_read_variable("tutu")->get_int(), 55);     // NAMERULE
    M_TEST_EQ(read_values.get_P_value_of_read_variable("titi.tutu")->get_int(), 55);

    // Test mulitple values
    T_interpret_read_values::T_var_name_P_values    name_values;

    name_values.clear();
    read_values.get_multiple_P_attribute_value_of_read_variable("titi.*", name_values);
    M_TEST_EQ(name_values.size(), 7);

    name_values.clear();
    read_values.get_multiple_P_attribute_value_of_read_variable("titi.a*", name_values);
    M_TEST_EQ(name_values.size(), 1);

    name_values.clear();
    read_values.get_multiple_P_attribute_value_of_read_variable("titi.c*", name_values);
    M_TEST_EQ(name_values.size(), 2);

    // Test array
    read_values.add_read_variable("an_array[0]", 1000);
    read_values.add_read_variable("an_array[1]", 1001);
    read_values.add_read_variable("an_array[2]", 1002);
    read_values.add_read_variable("an_array[3]", 1003);

    M_TEST_EQ(read_values.is_read_variable("an_array[0]"), true);
    M_TEST_EQ(read_values.is_read_variable("an_array[1]"), true);
    M_TEST_EQ(read_values.is_read_variable("an_array[2]"), true);
    M_TEST_EQ(read_values.is_read_variable("an_array[3]"), true);
    M_TEST_EQ(read_values.is_read_variable("an_array[4]"), false);

    M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[0]")->get_int(), 1000);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[1]")->get_int(), 1001);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[2]")->get_int(), 1002);
    M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[3]")->get_int(), 1003);
}

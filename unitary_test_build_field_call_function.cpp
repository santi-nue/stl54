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
#include "byte_interpret_parse.h"
#include "byte_interpret.h"

#define M_TEST_EXPR_STR_EQ(VAL1,VAL2)            M_TEST_EQ(VAL1,VAL2)


//*****************************************************************************
// test_build_field_call_function
//*****************************************************************************

M_TEST_FCT(test_build_field_call_function)
{
    T_type_definitions    type_definitions;

    // Add some types
    istringstream         iss(
        "function void  return_nothing () "
        "{ "
        "} "
        "function void  return_nothing_2 (in int64  value, out bool8  value_modified) "
        "{ "
        "} "
    );
    build_types(iss, type_definitions);

    // call function
    {
        string         first_word = "call";
        istringstream  iss(" "
            " return_nothing ();");      // space is currently mandatory

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "call");                            // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EXPR_STR_EQ(field_type_name.name, "return_nothing");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_NULL(field_type_name.P_sub_struct);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_NULL(field_type_name.P_bitfield_inline);
        M_TEST_NULL(field_type_name.P_switch_inline);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_NULL(field_type_name.pf_frame_to_any);
        M_TEST_NULL(field_type_name.pf_frame_to_field);
        M_TEST_NULL(field_type_name.P_type_enum_def);
        M_TEST_NULL(field_type_name.P_type_struct_def);
        M_TEST_NULL(field_type_name.P_type_switch_def);
        M_TEST_NULL(field_type_name.P_type_bitfield_def);
    }

    // call function
    {
        string         first_word = "call";
        istringstream  iss(" "
            " return_nothing_2 (12+7, 1 - 1 );");      // space is currently mandatory

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "call");                            // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EXPR_STR_EQ(field_type_name.name, "return_nothing_2");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 2);
        M_TEST_EQ(field_type_name.fct_parameters[0].get_original_string_expression(), "12+7");
        M_TEST_EQ(field_type_name.fct_parameters[1].get_original_string_expression(), "1 - 1");
        M_TEST_NULL(field_type_name.P_sub_struct);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_NULL(field_type_name.P_bitfield_inline);
        M_TEST_NULL(field_type_name.P_switch_inline);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_NULL(field_type_name.pf_frame_to_any);
        M_TEST_NULL(field_type_name.pf_frame_to_field);
        M_TEST_NULL(field_type_name.P_type_enum_def);
        M_TEST_NULL(field_type_name.P_type_struct_def);
        M_TEST_NULL(field_type_name.P_type_switch_def);
        M_TEST_NULL(field_type_name.P_type_bitfield_def);
    }

    // call inner function (...) 
    {
        string         first_word = "chat";
        istringstream  iss("(12+7, 1 - 1 );");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "chat");                            // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12+7, 1 - 1 )");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_NULL(field_type_name.P_sub_struct);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_NULL(field_type_name.P_bitfield_inline);
        M_TEST_NULL(field_type_name.P_switch_inline);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_NULL(field_type_name.pf_frame_to_any);
        M_TEST_NULL(field_type_name.pf_frame_to_field);
        M_TEST_NULL(field_type_name.P_type_enum_def);
        M_TEST_NULL(field_type_name.P_type_struct_def);
        M_TEST_NULL(field_type_name.P_type_switch_def);
        M_TEST_NULL(field_type_name.P_type_bitfield_def);
    }

    // call inner function "..."
    {
        string         first_word = "print";
        istringstream  iss(" \"Yo les potos\" ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "print");                           // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EXPR_STR_EQ(field_type_name.name, "\"Yo les potos\"");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_NULL(field_type_name.P_sub_struct);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_NULL(field_type_name.P_bitfield_inline);
        M_TEST_NULL(field_type_name.P_switch_inline);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_NULL(field_type_name.pf_frame_to_any);
        M_TEST_NULL(field_type_name.pf_frame_to_field);
        M_TEST_NULL(field_type_name.P_type_enum_def);
        M_TEST_NULL(field_type_name.P_type_struct_def);
        M_TEST_NULL(field_type_name.P_type_switch_def);
        M_TEST_NULL(field_type_name.P_type_bitfield_def);
    }

    // call inner function name.*
    {
        string         first_word = "debug_print";
        istringstream  iss(" name.* ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "debug_print");                     // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EXPR_STR_EQ(field_type_name.name, "name.*");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_NULL(field_type_name.P_sub_struct);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_NULL(field_type_name.P_bitfield_inline);
        M_TEST_NULL(field_type_name.P_switch_inline);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_NULL(field_type_name.pf_frame_to_any);
        M_TEST_NULL(field_type_name.pf_frame_to_field);
        M_TEST_NULL(field_type_name.P_type_enum_def);
        M_TEST_NULL(field_type_name.P_type_struct_def);
        M_TEST_NULL(field_type_name.P_type_switch_def);
        M_TEST_NULL(field_type_name.P_type_bitfield_def);
    }
}

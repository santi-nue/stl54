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
// test_build_field_misc
//*****************************************************************************

M_TEST_FCT(test_build_field_misc)
{
    T_type_definitions    type_definitions;

    // set
    {
        string         first_word = "set";
        istringstream  iss(" name = 22 +variable[7][2].toto ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "set");                             // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EXPR_STR_EQ(field_type_name.name, "name");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.new_expression.get_original_string_expression(), "22 +variable[7][2].toto");

        auto expressions = field_type_name.new_expression.get_expressions_for_UT();
        M_TEST_EQ(expressions.size(), 2);
        M_TEST_EQ(expressions[1].is_a_variable(), true);

        auto array_expressions = expressions[1].get_expressions_for_UT();
        M_TEST_EQ(array_expressions.size(), 2);
        M_TEST_EQ(array_expressions[0].get_original_string_expression(), "7");
        M_TEST_EQ(array_expressions[1].get_original_string_expression(), "2");

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

    // return
    {
        string         first_word = "return";
        istringstream  iss(" ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "return");                          // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "");
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

    // output
    {
        string         first_word = "output";
        istringstream  iss(" ++ ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "output");                          // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "++");
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

    // byte_order
    {
        string         first_word = "byte_order";
        istringstream  iss(" little_endian ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "byte_order");                      // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "little_endian");
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

    // break
    {
        string         first_word = "break";
        istringstream  iss("  ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "break");                           // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "");
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

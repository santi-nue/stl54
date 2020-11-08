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
// test_build_field_loop_condition
//*****************************************************************************

M_TEST_FCT(test_build_field_loop_condition)
{
    T_type_definitions    type_definitions;

    // loop_size_bytes
    {
        string         first_word = "show";
        istringstream  iss(" "
            " loop_size_bytes  12"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), true);                       // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "loop_size_bytes");                 // type
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
        M_TEST_EQ(field_type_name.name, "12");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "12");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // loop_size_bits
    {
        string         first_word = "show";
        istringstream  iss(" "
            " loop_size_bits  (12 + 7)"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), true);                       // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "loop_size_bits");                  // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7)");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7)");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // if
    {
        string         first_word = "forget";   // currently accepted, but ...
        istringstream  iss(" "
            " if  (12 + 7 > 18)"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "if");                              // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // if else
    {
        string         first_word = "forget";   // currently accepted, but ...
        istringstream  iss(" "
            " if  (12 + 7 > 18)"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " }"
            " else"
            " {"
            "   string    c2;"
            "   int13[2]  c3;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "if");                              // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 2);
        M_TEST_EQ(field_type_name.sub_struct_2[0].type, "string");
        M_TEST_EQ(field_type_name.sub_struct_2[0].name, "c2");
        M_TEST_EQ(field_type_name.sub_struct_2[1].type, "int13");
        M_TEST_EQ(field_type_name.sub_struct_2[1].name, "c3");
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // while
    {
        string         first_word = "hide";   // currently accepted, but ...
        istringstream  iss(" "
            " while  (12 + 7 > 18)"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "while");                           // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // do while
    {
        string         first_word = "hide";   // currently accepted, but ...
        istringstream  iss(" "
            " do"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " }"
            " while  (12 + 7 > 18) ;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "do_while");                        // type
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
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_display_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_filter_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_extended_name(), field_type_name.name);
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }
}

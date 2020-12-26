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
// test_build_field
//*****************************************************************************

M_TEST_FCT(test_build_field)
{
    T_type_definitions    type_definitions;

    // Field 
    {
        string         first_word = "uint3";
        istringstream  iss("toto;follow");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "follow");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false);                  // var
        M_TEST_EQ(field_type_name.orig_type, "uint3");
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.basic_type_bit_size, 3);
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
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_display_name(), "toto");
        M_TEST_EQ(field_type_name.get_filter_name(), "toto");
        M_TEST_EQ(field_type_name.get_extended_name(), "toto");
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

    // Field var float
    {
        string         first_word = "var";
        istringstream  iss("float32  val = 12;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.orig_type, "float32");
        M_TEST_EQ(field_type_name.type, "float32");                         // type
        M_TEST_EQ(field_type_name.basic_type_bit_size, 32);
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "val");
        M_TEST_EQ(field_type_name.get_display_name(), "val");
        M_TEST_EQ(field_type_name.get_filter_name(), "val");
        M_TEST_EQ(field_type_name.get_extended_name(), "val");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12");
        M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_int(), 12);
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

    // Field var float
    {
        string         first_word = "var";
        istringstream  iss("float32  val = 12.3;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.orig_type, "float32");
        M_TEST_EQ(field_type_name.type, "float32");                         // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "val");
        M_TEST_EQ(field_type_name.get_display_name(), "val");
        M_TEST_EQ(field_type_name.get_filter_name(), "val");
        M_TEST_EQ(field_type_name.get_extended_name(), "val");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12.3");
        M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_flt(), 12.3);
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

    // Field var float
    {
        string         first_word = "var";
        istringstream  iss("float32  val = 12.3e2;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.orig_type, "float32");
        M_TEST_EQ(field_type_name.type, "float32");                         // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "val");
        M_TEST_EQ(field_type_name.get_display_name(), "val");
        M_TEST_EQ(field_type_name.get_filter_name(), "val");
        M_TEST_EQ(field_type_name.get_extended_name(), "val");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12.3e2");
        M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_flt(), 1230.0);
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

    // Field with {...} and spaces
    {
        string         first_word = "hide";
        istringstream  iss(" var"
            " uint3{ns=0}"
            " toto = 4;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
    }
    {
        string         first_word = "hide";
        istringstream  iss(" var"
            " uint3{ns=0}{q=.2:o=-.2}"
            " toto = 4;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
    }
    {
        string         first_word = "hide";
        istringstream  iss(" var"
            " uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=\"hello world\"}[ val + 2 ]"
            " toto = 4;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.orig_type, "uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=\"hello world\"}[ val + 2 ]");
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.get_flt(), 0.2);        // q
        M_TEST_EQ(field_type_name.transform_offset.get_flt(), -0.2);        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_flt(), -3.2);      // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_flt(), 2.3);       // max
        M_TEST_EQ(field_type_name.str_display_expression, "\"hello world\"");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_display_name(), "toto");
        M_TEST_EQ(field_type_name.get_filter_name(), "toto");
        M_TEST_EQ(field_type_name.get_extended_name(), "toto");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "4");
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

    // Field 
    {
        string         first_word = "uint3{de='hello' + ' world'}";
        istringstream  iss("toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.str_display_expression, "'hello' + ' world'");
        M_TEST_EQ(field_type_name.name, "toto");
    }

    // Field with transform expression
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " uint3{ns=0}{tef=.2*this-.2}{min=-3.2:max=2.3}[ val + 2 ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "uint3{ns=0}{tef=.2*this-.2}{min=-3.2:max=2.3}[ val + 2 ]");
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.get_original_string_expression(), ".2*this-.2");
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_float);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_flt(), -3.2);      // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_flt(), 2.3);       // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
        M_TEST_EQ(field_type_name.str_size_or_parameter, "");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_display_name(), "toto");
        M_TEST_EQ(field_type_name.get_filter_name(), "toto");
        M_TEST_EQ(field_type_name.get_extended_name(), "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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

    // Field with size and specifications before and after
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " string{decoder=decode_utf8}(50){d=string is %s}[ val + 2 ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "string{decoder=decode_utf8}(50){d=string is %s}[ val + 2 ]");
        M_TEST_EQ(field_type_name.type, "string");                          // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_decoder_function, "decode_utf8");
        M_TEST_EQ(field_type_name.str_size_or_parameter, "50");
        M_TEST_EQ(field_type_name.str_display, "string is %s");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
        M_TEST_EQ(field_type_name.str_size_or_parameter, "50");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "decode_utf8");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_display_name(), "toto");
        M_TEST_EQ(field_type_name.get_filter_name(), "toto");
        M_TEST_EQ(field_type_name.get_extended_name(), "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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

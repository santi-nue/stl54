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
// test_build_field_ternary_condition
//*****************************************************************************

M_TEST_FCT(test_build_field_ternary_condition)
{
    T_type_definitions    type_definitions;

    // Add some types
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    // Use condition ? xxx : yyy in many places
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " uint3"
            "{ns=T_enum3::value0 == 0 ? 1 : 2}"
            "{q=T_enum4::Val13 != 13 ? konst::int_enum : 12:o=(konst::int_enum != 0 ? T_enum3::value0 : T_enum4::Val13)}"
            "{min=T_enum4::Val13 == 13 ? konst::int_enum : 12:max=(konst::int_enum == 0 ? T_enum4::Val13 : T_enum3::value0)}"
            "[ val + (T_enum3::value0 != 0 ? 1 : 2) ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "uint3"
                                             "{ns=T_enum3::value0 == 0 ? 1 : 2}"
                                             "{q=T_enum4::Val13 != 13 ? konst::int_enum : 12:o=(konst::int_enum != 0 ? T_enum3::value0 : T_enum4::Val13)}"
                                             "{min=T_enum4::Val13 == 13 ? konst::int_enum : 12:max=(konst::int_enum == 0 ? T_enum4::Val13 : T_enum3::value0)}"
                                             "[ val + (T_enum3::value0 != 0 ? 1 : 2) ]");
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 1);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.get_int(), 12);         // q
        M_TEST_EQ(field_type_name.transform_offset.get_int(), 13);          // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_msg);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + (T_enum3::value0 != 0 ? 1 : 2) ");
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
        M_TEST_EQ(field_type_name.P_sub_struct, NULL);
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
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " uint3"
            "{ns=T_enum3::value0 == 0 ? 1 : 2}"
            "{tef=.2*this-.2+(konst::int_enum == 0 ? T_enum3::value0 : T_enum4::Val13)}"
            "{min=T_enum4::Val13 == 13 ? konst::int_enum : 12:max=(konst::int_enum == 0 ? T_enum4::Val13 : T_enum3::value0)}"
            "[ val + (T_enum3::value0 != 0 ? 1 : 2) ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "uint3"
                                             "{ns=T_enum3::value0 == 0 ? 1 : 2}"
                                             "{tef=.2*this-.2+(konst::int_enum == 0 ? T_enum3::value0 : T_enum4::Val13)}"
                                             "{min=T_enum4::Val13 == 13 ? konst::int_enum : 12:max=(konst::int_enum == 0 ? T_enum4::Val13 : T_enum3::value0)}"
                                             "[ val + (T_enum3::value0 != 0 ? 1 : 2) ]");
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 1);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.get_original_string_expression(), ".2*this-.2+(konst::int_enum == 0 ? T_enum3::value0 : T_enum4::Val13)");
        M_TEST_EQ(field_type_name.transform_expression_type, C_value::E_type_float);  // not set
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + (T_enum3::value0 != 0 ? 1 : 2) ");
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
        M_TEST_EQ(field_type_name.P_sub_struct, NULL);
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
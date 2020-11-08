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
// test_build_field_switch_struct
//*****************************************************************************

M_TEST_FCT(test_build_field_switch_struct)
{
    T_type_definitions    type_definitions;

     // switch inline
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " switch"
            " (1)"
            " {"
            "   case 0 : uint8  c0;"
            "   case 1 : uint8  c1; uint8  c1bis;"
//            "   case 2:  uint8  c2;"                // space is mandatory before :
            "   default : uint8  df;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "switch");                          // type
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
        M_TEST_EQ(field_type_name.str_size_or_parameter, "1");
        M_TEST_EQ(field_type_name.str_byte_order, "");
        M_TEST_EQ(field_type_name.str_decoder_function, "");
        M_TEST_EQ(field_type_name.str_dissector, "");
        M_TEST_EQ(field_type_name.name, "");
        M_TEST_EQ(field_type_name.get_display_name(), "");
        M_TEST_EQ(field_type_name.get_filter_name(), "");
        M_TEST_EQ(field_type_name.get_extended_name(), "");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct, NULL);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline->is_switch_expr, false);
        M_TEST_EQ(field_type_name.P_switch_inline->case_type, "");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases.size(), 3);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].is_default_case, false);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].case_value.get_int(), 0);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].fields.size(), 1);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].is_default_case, false);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].case_value.get_int(), 1);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].fields.size(), 2);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].fields[0].name, "c1");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].fields[1].name, "c1bis");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[2].is_default_case, true);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[2].fields.size(), 1);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[2].fields[0].name, "df");
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // switch_expr inline
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " switch_expr"
            " "
            " {"
            "   case (0 == 0)     : uint8  c0;"
            "   case (1 == a_var) : uint8  c1; uint8  c1bis;"
//            "   case 2:  uint8  c2;"                // space is mandatory before :
            "   default : uint8  df;"
            " }");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "switch");                          // type
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
        M_TEST_EQ(field_type_name.name, "");
        M_TEST_EQ(field_type_name.get_display_name(), "");
        M_TEST_EQ(field_type_name.get_filter_name(), "");
        M_TEST_EQ(field_type_name.get_extended_name(), "");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.return_expression.get_original_string_expression(), "");
        M_TEST_EQ(field_type_name.fct_parameters.size(), 0);
        M_TEST_EQ(field_type_name.P_sub_struct, NULL);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline, NULL);
        M_TEST_EQ(field_type_name.P_switch_inline->is_switch_expr, true);
        M_TEST_EQ(field_type_name.P_switch_inline->case_type, "");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases.size(), 3);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].is_default_case, false);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].case_expr.get_original_string_expression(), "(0 == 0)");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].fields.size(), 1);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[0].fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].is_default_case, false);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].case_expr.get_original_string_expression(), "(1 == a_var)");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].fields.size(), 2);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].fields[0].name, "c1");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[1].fields[1].name, "c1bis");
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[2].is_default_case, true);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[2].fields.size(), 1);
        M_TEST_EQ(field_type_name.P_switch_inline->switch_cases[2].fields[0].name, "df");
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // struct inline
    {
        string         first_word = "forget";
        istringstream  iss(" "
            " struct"
            " {"
            "   uint8  c0;"
            "    int8  c1;"
            " } toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "struct");                          // type
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
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "");
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

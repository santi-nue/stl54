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
// test_build_field_bitfield_bitstream
//*****************************************************************************

M_TEST_FCT(test_build_field_bitfield_bitstream)
{
    T_type_definitions    type_definitions;

    // bitfield inline
    {
        string         first_word = "forget";
        istringstream  iss(" "
            " bitfield16"
            " {"
            "   uint1  c0;"
            "   uint3  c1;"
            " } toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "bitfield");                        // type
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
        M_TEST_EQ(field_type_name.P_sub_struct, NULL);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline->is_a_bitstream, false);
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.type, "uint16");
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.name, "last_bitfield_value");
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.output_directive, T_field_type_name::E_output_directive_hide);
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.str_display, "hex");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition.size(), 2);
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[0].type, "uint32");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[0].name, "c0");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[0].is_a_variable(), true);
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[0].get_var_expression().get_original_string_expression(),
                                                                          "((last_bitfield_value >> 0) & 1)");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].type, "uint32");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].name, "c1");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].is_a_variable(), true);
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].get_var_expression().get_original_string_expression(),
                                                                          "((last_bitfield_value >> 1) & 7)");
        M_TEST_EQ(field_type_name.P_switch_inline, NULL);
        M_TEST_EQ(field_type_name.wsgd_field_idx, -1);
        M_TEST_EQ(field_type_name.pf_frame_to_any, NULL);
        M_TEST_EQ(field_type_name.pf_frame_to_field, NULL);
        M_TEST_EQ(field_type_name.P_type_enum_def, NULL);
        M_TEST_EQ(field_type_name.P_type_struct_def, NULL);
        M_TEST_EQ(field_type_name.P_type_switch_def, NULL);
        M_TEST_EQ(field_type_name.P_type_bitfield_def, NULL);
    }

    // bitstream inline
    {
        string         first_word = "forget";
        istringstream  iss(" "
            " bitstream24"
            " {"
            "   var int64  hc = 123456789;"
            "   uint1  c0;"
            "   set c0 = 1;"
            "   uint3  c1;"
            " } toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.orig_type, "");
        M_TEST_EQ(field_type_name.type, "bitfield");                        // type
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
        M_TEST_EQ(field_type_name.P_sub_struct, NULL);
        M_TEST_EQ(field_type_name.sub_struct_2.size(), 0);
        M_TEST_EQ(field_type_name.P_bitfield_inline->is_a_bitstream, true);
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.type, "uint24");
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.name, "last_bitfield_value");
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.output_directive, T_field_type_name::E_output_directive_hide);
        M_TEST_EQ(field_type_name.P_bitfield_inline->master_field.str_display, "hex");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition.size(), 4);
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[0].type, "int64");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[0].name, "hc");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].type, "uint32");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].name, "c0");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].is_a_variable(), true);
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[1].get_var_expression().get_original_string_expression(),
                                                                          "((last_bitfield_value >> 23) & 1)");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[2].type, "set");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[2].name, "c0");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[3].type, "uint32");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[3].name, "c1");
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[3].is_a_variable(), true);
        M_TEST_EQ(field_type_name.P_bitfield_inline->fields_definition[3].get_var_expression().get_original_string_expression(),
                                                                          "((last_bitfield_value >> 20) & 7)");
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

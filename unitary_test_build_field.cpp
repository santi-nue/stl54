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

        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), false);                  // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
    }

    // Field var float
    {
        string         first_word = "var";
        istringstream  iss("float32  val = 12;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.type, "float32");                         // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "val");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12");
        M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_int(), 12);
    }

    // Field var float
    {
        string         first_word = "var";
        istringstream  iss("float32  val = 12.3;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.type, "float32");                         // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "val");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12.3");
        M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_flt(), 12.3);
    }

    // Field var float
    {
        string         first_word = "var";
        istringstream  iss("float32  val = 12.3e2;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.type, "float32");                         // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "val");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12.3e2");
        M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_flt(), 1230.0);
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

        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.get_flt(), 0.2);        // q
        M_TEST_EQ(field_type_name.transform_offset.get_flt(), -0.2);        // o
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_flt(), -3.2);      // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_flt(), 2.3);       // max
        M_TEST_EQ(field_type_name.str_display_expression, "\"hello world\"");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "4");
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

        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_flt(), -3.2);      // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_flt(), 2.3);       // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
    }

    // Field with size and specifications before and after
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " string{decoder=decode_utf8}(50){d=string is %s}[ val + 2 ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "string");                          // type
        M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_decoder_function, "decode_utf8");
        M_TEST_EQ(field_type_name.str_size_or_parameter, "50");
        M_TEST_EQ(field_type_name.str_display, "string is %s");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
    }

    // Invalid syntax field name []
    {
        string         first_word = "float32";
        istringstream  iss(" x[6];");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }

    // Invalid syntax field name {}
    {
        string         first_word = "uint32";
        istringstream  iss(" x{ns=0};");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }

    // Invalid syntax field name ()
    {
        string         first_word = "string";
        istringstream  iss(" x(50);");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }

    // Invalid syntax field name ::
    {
        string         first_word = "uint32";
        istringstream  iss(" konst::one_hundred;");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }

    // Invalid syntax field name ::
    {
        string         first_word = "var";
        istringstream  iss("uint32 konst::one_hundred = 100;");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }

    // Valid const syntax field name
    {
        string         first_word = "konst";
        istringstream  iss("uint32 konst::one_hundred = 100;");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }


    // Add some types
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    // Use const and enum (::) in many places
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " uint3"
            "{ns=T_enum3::value0}"
            "{q=konst::int_enum:o=T_enum4::Val13}"
            "{min=konst::int_enum:max=T_enum4::Val13}"
            "[ val + T_enum2::enu2 ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
        M_TEST_EQ(field_type_name.transform_offset.get_int(), 13);          // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + T_enum2::enu2 ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
    }
    {
        string         first_word = "hide";
        istringstream  iss(" "
            " uint3"
            "{ns=T_enum3::value0}"
            "{tef=.2*this-.2+T_enum3::value0}"
            "{min=konst::int_enum:max=T_enum4::Val13}"
            "[ val + T_enum2::enu2 ]"
            " toto;");

        T_field_type_name    field_type_name;

        M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + T_enum2::enu2 ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
    }

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
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 1);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.get_int(), 12);         // q
        M_TEST_EQ(field_type_name.transform_offset.get_int(), 13);          // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + (T_enum3::value0 != 0 ? 1 : 2) ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "uint3");                           // type
        M_TEST_EQ(field_type_name.no_statement.get_int(), 1);               // ns
        M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
        M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), true);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
        M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
        M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
        M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 1);
        M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
        M_TEST_TRIM_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + (T_enum3::value0 != 0 ? 1 : 2) ");
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
    }

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
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "switch");                          // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "switch");                          // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "struct");                          // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
    }

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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "bitfield");                        // type
//        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//        M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//        M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "bitfield");                        // type
//        M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//        M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//        M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "toto");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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
    }

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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), true);                       // show
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "loop_size_bytes");                         // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EQ(field_type_name.name, "12");
        M_TEST_EQ(field_type_name.condition_expression.get_original_string_expression(), "12");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), true);                       // show
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "loop_size_bits");                          // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7)");
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7)");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "if");                              // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
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
        M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.must_forget, true);                       // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "if");                              // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
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
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "while");                           // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
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
        M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
        M_TEST_EQ(field_type_name.must_show(), false);                      // show
        M_TEST_EQ(field_type_name.must_forget, false);                      // forget
        M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
        M_TEST_EQ(field_type_name.type, "do_while");                        // type
//		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
//		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
//		M_TEST_EQ(field_type_name.transform_offset.get_int(), 0);           // o
        M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
        M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
        M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
        M_TEST_EQ(field_type_name.str_display_expression, "");
        M_TEST_EQ(field_type_name.str_arrays.size(), 0);
        M_TEST_EXPR_STR_EQ(field_type_name.name, "(12 + 7 > 18)");
        M_TEST_EXPR_STR_EQ(field_type_name.condition_expression.get_original_string_expression(), "(12 + 7 > 18)");
        M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
        M_TEST_EQ(field_type_name.P_sub_struct->fields.size(), 2);
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].type, "uint8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[0].name, "c0");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].type, "int8");
        M_TEST_EQ(field_type_name.P_sub_struct->fields[1].name, "c1");
    }
}

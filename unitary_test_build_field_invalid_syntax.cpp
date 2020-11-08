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
// test_build_field_invalid_syntax
//*****************************************************************************

M_TEST_FCT(test_build_field_invalid_syntax)
{
    T_type_definitions    type_definitions;

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

    // Invalid const syntax field name
    {
        string         first_word = "konst";
        istringstream  iss("uint32 konst::one_hundred = 100;");

        T_field_type_name    field_type_name;
        M_TEST_CATCH_EXCEPTION(
            build_field(iss, type_definitions, first_word, field_type_name),
            C_byte_interpret_exception);
    }
}

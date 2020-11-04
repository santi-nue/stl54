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
#include "byte_interpret_build_types_read_token.h"

//*****************************************************************************
// test_read_token
//*****************************************************************************

M_TEST_FCT(test_read_token)
{
    string    str_result;

    // Field with (...)
    {
        istringstream  iss(""
            "  raw(val-3)[val+2]"
            "  toto ; ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "raw(val-3)[val+2]");

        str_result = "";
        M_TEST_EQ(read_token_field_name(iss, str_result), true);
        M_TEST_EQ(str_result, "toto");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // Field with (...) and spaces
    {
        istringstream  iss(""
            "  raw( val - 3 )[ val + 2 ]"
            "  \"\" ; ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "raw( val - 3 )[ val + 2 ]");

        str_result = "";
        M_TEST_EQ(read_token_field_name(iss, str_result), true);
        M_TEST_EQ(str_result, "");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // Field with {...}
    {
        istringstream  iss(" hide  var"
            "  uint13{ns=0}{q=12.2:o=-47.2}{min=-43.2:max=12.3}{d=bin}[val+2]"
            "  toto; ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "hide");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "var");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "uint13{ns=0}{q=12.2:o=-47.2}{min=-43.2:max=12.3}{d=bin}[val+2]");

        str_result = "";
        M_TEST_EQ(read_token_field_name(iss, str_result), true);
        M_TEST_EQ(str_result, "toto");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // Field with {...} and spaces
    {
        istringstream  iss(" hide  var"
            "  uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=hello world}[ val + 2 ]"
            "  toto; ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "hide");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "var");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=hello world}[ val + 2 ]");

        str_result = "";
        M_TEST_EQ(read_token_field_name(iss, str_result), true);
        M_TEST_EQ(str_result, "toto");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // return  expression
    {
        istringstream  iss("	return  var_str_array[var_idx+1] + \" et quelques ...\"; ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "return");

        str_result = "";
        M_TEST_EQ(read_token_expression_any(iss, str_result), true);
        M_TEST_EQ(str_result, "var_str_array[var_idx+1] + \" et quelques ...\"");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // return  expression function
    {
        istringstream  iss(" return  print (var[var_idx+1] + \" et quelques ...\", 2); ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "return");

        str_result = "";
        M_TEST_EQ(read_token_expression_any(iss, str_result), true);
        M_TEST_EQ(str_result, "print (var[var_idx+1] + \" et quelques ...\", 2)");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // return  expression function
    {
        istringstream  iss(" print (\"int24[0] = %s\", int24[0]); ");

        str_result = "";
        M_TEST_EQ(read_token_left_any(iss, str_result), true);
        M_TEST_EQ(str_result, "print");

        str_result = "";
        M_TEST_EQ(read_token_expression_any(iss, str_result), true);
        M_TEST_EQ(str_result, "(\"int24[0] = %s\", int24[0])");

        read_token_end_of_statement(iss);

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }
}

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

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"

#include "byte_interpret_parse.h"
#include "byte_interpret.h"
#include "T_expression.h"


//*****************************************************************************
// 
//*****************************************************************************
T_type_definitions  * SP_type_definitions = nullptr;
T_interpret_data    * SP_interpret_data = nullptr;

long long  compute_expression_static_int(
                const string              & expression_str)
{
    T_expression    expression;
    expression.build_expression(*SP_type_definitions, expression_str);

    C_value    value = expression.compute_expression_static(*SP_type_definitions);

    return  value.get_int();
}
long long  compute_expression_no_io_int(
                const string              & expression_str)
{
    T_expression    expression;
    expression.build_expression(*SP_type_definitions, expression_str);

    C_value    value = expression.compute_expression_no_io(*SP_type_definitions, *SP_interpret_data);

    return  value.get_int();
}

double  compute_expression_static_flt(
                const string              & expression_str)
{
    T_expression    expression;
    expression.build_expression(*SP_type_definitions, expression_str);

    C_value    value = expression.compute_expression_static(*SP_type_definitions);

    return  value.get_flt();
}
double  compute_expression_no_io_flt(
                const string              & expression_str)
{
    T_expression    expression;
    expression.build_expression(*SP_type_definitions, expression_str);

    C_value    value = expression.compute_expression_no_io(*SP_type_definitions, *SP_interpret_data);

    return  value.get_flt();
}

string  compute_expression_static_str(
                const string              & expression_str)
{
    T_expression    expression;
    expression.build_expression(*SP_type_definitions, expression_str);

    C_value    value = expression.compute_expression_static(*SP_type_definitions);

    return  value.get_str();
}
string  compute_expression_no_io_str(
                const string              & expression_str)
{
    T_expression    expression;
    expression.build_expression(*SP_type_definitions, expression_str);

    C_value    value = expression.compute_expression_no_io(*SP_type_definitions, *SP_interpret_data);

    return  value.get_str();
}

//*****************************************************************************
// test_function_call
//*****************************************************************************

M_TEST_FCT(test_function_call)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);
    SP_type_definitions = &type_definitions;

    T_interpret_data      interpret_data;
    SP_interpret_data = &interpret_data;

    M_TEST_EQ(compute_expression_static_int("call_me_maybe_u8 (255)"), 254);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_u16(65535)"), 65534);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_u24(16777215)"), 16777214);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_u32(4294967295)"), 4294967294);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_u40(1099511627775)"), 1099511627774);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_u48(281474976710655)"), 281474976710654);

    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s8 (-127)"), -126);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s16(-32767)"), -32766);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s24(-8388607)"), -8388606);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s32(-2122317823)"), -2122317822);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s40(-543313362943)"), -543313362942);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s48(-139088220913663)"), -139088220913662);
    M_TEST_EQ(compute_expression_static_int("call_me_maybe_s64(-35606584553897983)"), -35606584553897982);

    // 1234 gives u8 overflow
    M_TEST_CATCH_EXCEPTION(compute_expression_static_int("call_me_maybe_u8 (1234)"), C_byte_interpret_exception);

    // Currently, parameter must be an entire byte size
    M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u30(1234)"));
    M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u20(1234)"));
    M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u10( 934)"));
    M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u2 (   3)"));
    M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u1 (   1)"));
}

//*****************************************************************************
// test_function_in_out
//*****************************************************************************

M_TEST_FCT(test_function_in_out)
{
    T_type_definitions    type_definitions;
    build_types("unitary_tests_basic.fdesc",
                type_definitions);
    SP_type_definitions = &type_definitions;

    T_interpret_data      interpret_data;
    SP_interpret_data = &interpret_data;
    interpret_data.add_read_variable("is_value_modified", 1);
    interpret_data.add_read_variable("sum", -7);

    M_TEST_EQ(compute_expression_no_io_int("test_out_normalize (5, is_value_modified)"), 5);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_value_modified")->get_bool(), false);

    M_TEST_EQ(compute_expression_no_io_int("test_out_normalize (15, is_value_modified)"), 10);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_value_modified")->get_bool(), true);

    M_TEST_EQ(compute_expression_no_io_int("test_out_normalize (-15, is_value_modified)"), -15);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_value_modified")->get_bool(), false);


    M_TEST_EQ(compute_expression_no_io_int("test_in_out_normalize_accumulate (5, is_value_modified, sum)"), 5);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_value_modified")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("sum")->get_int(), -2);

    M_TEST_EQ(compute_expression_no_io_int("test_in_out_normalize_accumulate (15, is_value_modified, sum)"), 10);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_value_modified")->get_bool(), true);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("sum")->get_int(), 13);

    M_TEST_EQ(compute_expression_no_io_int("test_in_out_normalize_accumulate (-15, is_value_modified, sum)"), -15);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_value_modified")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("sum")->get_int(), -2);
}

//*****************************************************************************
// test_builtin_functions
//*****************************************************************************

M_TEST_FCT(test_builtin_functions)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);
    SP_type_definitions = &type_definitions;

    T_interpret_data      interpret_data;
    SP_interpret_data = &interpret_data;
    interpret_data.add_read_variable("int", 0);
    interpret_data.add_read_variable("flt", 0.0);
    interpret_data.add_read_variable("str", "");

    // to_string
    M_TEST_EQ(compute_expression_static_str("to_string(1234)"), "1234");
    M_TEST_EQ(compute_expression_static_str("to_string (1234) + \"5\""), "12345");
    M_TEST_EQ(compute_expression_static_str("to_string (12.34)"), "12.34");
    interpret_data.set_read_variable("int", -136);
    interpret_data.set_read_variable("flt", -136.2);
    M_TEST_EQ(compute_expression_no_io_str("to_string (int+5) + \"5\""), "-1315");
    M_TEST_EQ(compute_expression_no_io_str("to_string(flt+5)"), "-131.2");
  
    // to_numeric
    M_TEST_EQ(compute_expression_static_int("to_numeric (\"12\" + \"34\")"), 1234);
    M_TEST_EQ(compute_expression_static_int("to_numeric (\"12\" + \"3\") * 3"), 369);
    interpret_data.set_read_variable("str", "123");
    M_TEST_EQ(compute_expression_no_io_int("to_numeric (str + \"3\")"), 1233);

    M_TEST_EQ(compute_expression_static_flt("to_numeric (\"1.2\" + \"34\")"), 1.234);
    M_TEST_EQ(compute_expression_static_flt("to_numeric (\"1.2\" + \"3\") / 2"), 0.615);
    interpret_data.set_read_variable("str", "123");
    M_TEST_EQ(compute_expression_no_io_flt("to_numeric (str + \".2\" + \"3\")"), 123.23);

    // to_integer
    M_TEST_EQ(compute_expression_static_int("to_integer (\"12\" + \"34\")"), 1234);
    M_TEST_EQ(compute_expression_static_int("to_integer (\"12\" + \"3\") * 3"), 369);
    interpret_data.set_read_variable("str", "123");
    M_TEST_EQ(compute_expression_no_io_int("to_integer (str + \"3\")"), 1233);
    // to_integer from float (can loose data) inside string or not
    M_TEST_EQ(compute_expression_static_int("to_integer (\"1.2\" + \"34\")"), 1);   //crash
    M_TEST_EQ(compute_expression_static_int("to_integer (1.2 + 34)"), 35);

    // to_float
    M_TEST_EQ(compute_expression_static_flt("to_float (\"1.2\" + \"34\")"), 1.234);
    M_TEST_EQ(compute_expression_static_flt("to_float (\"1.2\" + \"3\") / 2"), 0.615);
    interpret_data.set_read_variable("str", "123");
    M_TEST_EQ(compute_expression_no_io_flt("to_float (str + \".2\" + \"3\")"), 123.23);
    // to_float from int (can loose precision) inside string or not
    M_TEST_EQ(compute_expression_static_flt("to_float (\"12\" + \"34\")"), 1234.0);
    M_TEST_EQ(compute_expression_static_flt("to_float (12 + 34)"), 46.0);

    // getenv
    compute_expression_static_str("getenv (\"USERNAME\")");

    // string.length
    M_TEST_EQ(compute_expression_static_int("string.length (\"1234\")"), 4);
    M_TEST_EQ(compute_expression_static_int("string.length (\"abcde\")"), 5);
    interpret_data.set_read_variable("str", "123");
    M_TEST_EQ(compute_expression_no_io_int("string.length (str)"), 3);

    // string.substr
    M_TEST_EQ(compute_expression_static_str("string.substr (\"12345678\", 2)"), "345678");
    M_TEST_EQ(compute_expression_static_str("string.substr (\"abcdefgh\", 1, 3)"), "bcd");
    interpret_data.set_read_variable("str", "abcdefgh");
    interpret_data.set_read_variable("int", 1);
    M_TEST_EQ(compute_expression_no_io_str("string.substr (str, int, 3)"), "bcd");

    // string.erase
    M_TEST_EQ(compute_expression_static_str("string.erase (\"12345678\", 2)"), "12");
    M_TEST_EQ(compute_expression_static_str("string.erase (\"abcdefgh\", 1, 3)"), "aefgh");
    interpret_data.set_read_variable("str", "abcdefgh");
    interpret_data.set_read_variable("int", 1);
    M_TEST_EQ(compute_expression_no_io_str("string.erase (str, int, 3)"), "aefgh");

    // string.insert
    M_TEST_EQ(compute_expression_static_str("string.insert (\"12345678\", 2, \"yo\")"), "12yo345678");
    interpret_data.set_read_variable("str", "yo");
    interpret_data.set_read_variable("int", 2);
    M_TEST_EQ(compute_expression_no_io_str("string.insert (\"12345678\", int, str)"), "12yo345678");

    // string.replace
    M_TEST_EQ(compute_expression_static_str("string.replace (\"abcdefgh\", 1, 3, \"123\")"), "a123efgh");
    interpret_data.set_read_variable("str", "123");
    M_TEST_EQ(compute_expression_no_io_str("string.replace (\"abcdefgh\", 1, 3, str)"), "a123efgh");

    // string.replace_all
    M_TEST_EQ(compute_expression_static_str("string.replace_all (\"abcdefgh - abcdefgh\", \"bc\", \"xyz\")"), "axyzdefgh - axyzdefgh");
    interpret_data.set_read_variable("str", "abcdefgh - abcdefgh");
    M_TEST_EQ(compute_expression_no_io_str("string.replace_all (str, \" \", \"\")"), "abcdefgh-abcdefgh");

    // string.find
    M_TEST_EQ(compute_expression_static_int("string.find (\"12345678\", \"2\")"), 1);
    M_TEST_EQ(compute_expression_static_int("string.find (\"abcdefgh\", \"ef\")"), 4);
    M_TEST_EQ(compute_expression_static_int("string_find (\"abcdefgh\", \"ef\")"), 4);
    M_TEST_EQ(compute_expression_static_int("string_found(\"abcdefgh\", \"ef\")"), true);
    M_TEST_EQ(compute_expression_static_int("string.find (\"abcdefgh\", \"fe\")"), compute_expression_static_int("string::npos"));
    M_TEST_EQ(compute_expression_static_int("string_find (\"abcdefgh\", \"fe\")"), compute_expression_static_int("string::npos"));
    M_TEST_EQ(compute_expression_static_int("string_found(\"abcdefgh\", \"fe\")"), false);
    interpret_data.set_read_variable("str", "abcdefgh");
    M_TEST_EQ(compute_expression_no_io_int("string.find (str, \"ef\")"), 4);


    // date.get_string_from_days
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,   0)"), "2012/01/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,   1)"), "2012/01/02");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  31)"), "2012/02/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  59)"), "2012/02/29");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  60)"), "2012/03/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  91)"), "2012/04/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 121)"), "2012/05/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 152)"), "2012/06/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 182)"), "2012/07/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 213)"), "2012/08/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 244)"), "2012/09/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 274)"), "2012/10/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 305)"), "2012/11/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 335)"), "2012/12/01");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 366)"), "2013/01/01");

    // date.get_string_from_seconds
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 0)"), "1970/01/01 00:00:00");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 4133980800)"), "2101/01/01 00:00:00");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 4134067199)"), "2101/01/01 23:59:59");
    M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 13601174399)"), "2401/01/01 23:59:59");
}

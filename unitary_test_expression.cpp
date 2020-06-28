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
#include "C_perf_chrono.h"

#include "byte_interpret_parse.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret.h"
#include "T_expression.h"

//*****************************************************************************
// test_expression_perf_operation
//*****************************************************************************

void    test_expression_perf_operation(
                const T_type_definitions  & type_definitions)
{
    T_interpret_data                  interpret_data;

    //-------------------------------------------------------------------------
    // perf operation expression
    //-------------------------------------------------------------------------
    const string     perf_expression =
        "((31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
#if 0		// Too long for win64 & VS 2019 : 0xC00000FD: Stack overflow
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
#if 0		// Too long for win64 (ok for win32 or linux)
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
        " (31*(2**27)+24*3600*1000) -"
        " (31*(2**27)+24*3600*1000) +"
#endif
#endif
        " (31*(2**27)+24*3600*1000))";
    const long long  perf_expression_result =  4247149568LL;

    {
        C_value    value;
        {
            C_perf_chrono  perf_chrono(interpret_data, "perf integer expression operation", 0);
            value = compute_expression_no_io (type_definitions, interpret_data, perf_expression);
        }
        M_TEST_EQ(value.get_int(), perf_expression_result);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, perf_expression);
        C_value    value;
        {
            value = expression.compute_expression_no_io (type_definitions, interpret_data);
            // x  1 =  0 ms
            // x 10 = 62 ms !!!
            C_perf_chrono  perf_chrono(interpret_data, "perf integer expression operation T_expression x 10", 0);
            for (int idx = 0; idx < 10; ++idx)
            {
                expression.compute_expression_no_io (type_definitions, interpret_data);
            }
        }
        M_TEST_EQ(value.get_int(), perf_expression_result);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, perf_expression);
        expression.pre_compute_expression(type_definitions);
        C_value    value;
        {
            value = expression.compute_expression_no_io (type_definitions, interpret_data);
            // x    10 =  0 ms
            // x 40000 = 66 ms
            C_perf_chrono  perf_chrono(interpret_data, "perf integer expression operation T_expression x 40000 pre_compute", 0);
            for (int idx = 0; idx < 40000; ++idx)
            {
                expression.compute_expression_no_io (type_definitions, interpret_data);
            }
        }
        M_TEST_EQ(value.get_int(), perf_expression_result);
        value.as_string();
    }

    //-------------------------------------------------------------------------
    // operation expression with unexistent_variable
    //-------------------------------------------------------------------------
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "2 + unexistent_variable");
        expression.pre_compute_expression(type_definitions);

        M_TEST_CATCH_EXCEPTION(
            C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data),
            C_byte_interpret_exception);
    }
}

//*****************************************************************************
// test_expression_perf_function
//*****************************************************************************

void    test_expression_perf_function(
                const T_type_definitions  & type_definitions)
{
    T_interpret_data                  interpret_data;

    //-------------------------------------------------------------------------
    // perf expression
    //-------------------------------------------------------------------------
    const string     perf_expression = "perf(1,4201)";
    const long long  perf_expression_result =  4201LL;

    {
        C_value    value;
        {
            C_perf_chrono  perf_chrono(interpret_data, "perf integer expression function", 0);
            value = compute_expression_no_io (type_definitions, interpret_data, perf_expression);
        }
        M_TEST_EQ(value.get_int(), perf_expression_result);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, perf_expression);
        C_value    value;
        {
            value = expression.compute_expression_no_io (type_definitions, interpret_data);
            C_perf_chrono  perf_chrono(interpret_data, "perf integer expression function T_expression x 1", 0);
            for (int idx = 0; idx < 1; ++idx)
            {
                expression.compute_expression_no_io (type_definitions, interpret_data);
            }
        }
        M_TEST_EQ(value.get_int(), perf_expression_result);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, perf_expression);
        expression.pre_compute_expression(type_definitions);
        C_value    value;
        {
            value = expression.compute_expression_no_io (type_definitions, interpret_data);
            // x    10 =  0 ms
            // x 40000 = 66 ms
            C_perf_chrono  perf_chrono(interpret_data, "perf integer expression function T_expression x 1 pre_compute", 0);
            for (int idx = 0; idx < 1; ++idx)
            {
                expression.compute_expression_no_io (type_definitions, interpret_data);
            }
        }
        M_TEST_EQ(value.get_int(), perf_expression_result);
        value.as_string();
    }

    //-------------------------------------------------------------------------
    // function expression with unexistent_variable
    //-------------------------------------------------------------------------
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "perf_value(1,4201)");
        expression.pre_compute_expression(type_definitions);
#if 0
        M_TEST_CATCH_EXCEPTION(
            C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data),
            C_byte_interpret_exception);
#endif
    }
}

//*****************************************************************************
// get_int_value_bit_pos_size
//*****************************************************************************

C_value  get_int_value_bit_pos_size(long long  basic_value,
                                    int        bit_offset,
                                    int        bit_size)
{
    C_value  value(basic_value);
    value.set_bit_position_offset_size(bit_offset, bit_size);
    return  value;
}

//*****************************************************************************
// get_flt_value_bit_pos_size
//*****************************************************************************

C_value  get_flt_value_bit_pos_size(double     basic_value,
                                    int        bit_offset,
                                    int        bit_size)
{
    C_value  value(basic_value);
    value.set_bit_position_offset_size(bit_offset, bit_size);
    return  value;
}

//*****************************************************************************
// test_expression
//*****************************************************************************

M_TEST_FCT(test_expression)
{
    T_type_definitions    type_definitions;
    {
        istringstream  iss(
            "enum16 T_Operation {  CREATION 0   MODIFICATION 1   DELETION 2 }" "\n"

            "function any  addition (in any lhs, in any rhs)" "\n"
            "{" "\n"
            "  return  (lhs + rhs) ;" "\n"
            "}" "\n"

            "function int64  perf (in int64 incr, in int64 limit)" "\n"
            "{" "\n"
            "  hide var int64  value = 0;" "\n"
            "  while (value < limit)" "\n"
            "  {" "\n"
            "    set value = value + incr;" "\n"
            "  }" "\n"
            "  return  value ;" "\n"
            "}" "\n"

            "function int64  perf_value (in int64 incr, in int64 limit)" "\n"
            "{" "\n"
            "  while (value < limit)" "\n"
            "  {" "\n"
            "    set value = value + incr;" "\n"
            "  }" "\n"
            "  return  value ;" "\n"
            "}" "\n"

            "" "\n");

        string    last_word_not_treated = build_types (iss, type_definitions);
        M_FATAL_IF_NE(last_word_not_treated, "");
    }
    {
        istringstream  iss(
            "const int32    k::int = -17;" "\n"
            "const float32  k::flt = -234.9;" "\n"
            "const string   k::str = \"hello world\";" "\n"

            "" "\n");

        string    last_word_not_treated = build_types (iss, type_definitions);
        M_FATAL_IF_NE(last_word_not_treated, "");
    }

    //-------------------------------------------------------------------------
    // integer static expression
    //-------------------------------------------------------------------------
    {
        struct T_test_expr_int_result
        {
            const char  * expr;
            long long     expected_result;
        };

        const T_test_expr_int_result    expressions[] = {
            { "27", 27 },
            { "-234", -234 },
            { "23+4", 27 },
            { "-1345-65", -1410 },
            { "234 * 2 - 5 + 1", 464 },
            { "(234 * 2) - 5 + 1", 464 },
            { "(234 * 2) - (5 + 1)", 462 },
            { "234 - 2 * 5 + 1", 225 },
            { "-(2**31)", -2147483648LL },
            { "-2**31", -2147483648LL },
            { "-2**2", 4 },
            { "(2**31)-1", 2147483647LL },
            { "2**31-1", 2147483647LL },
            { "31*(2**27)+24*3600*1000", 4247149568LL },
            { "31*2**27+24*3600*1000", 4247149568LL },
            { "2**32-1", 4294967295LL },
            { "3*2**2", 12 },
            { "36/2**2", 9 },
            { "7%2**2", 3 },
            { "0x045", 69 },
            { "-0Xdf", -223 },
            { "0x045 * 1", 69 },
            { "-0Xdf / 1", -223 },
            { "-0Xdf / -1", 223 },
            { "-0Xdf * -1", 223 },
            { "-0Xdf + -(-1+2)", -0Xe0 },
            { "007", 7 },
            { "03456", 1838 },
            { "007 / 0x7", 1 },
            { "03456/1838", 1 },
            { "007 % 0x5", 2 },
            // const
            { "136 + k::int", 119 },
            // bit operations
            { "007 & 0x5", 5 },
            { "007 & 0x9", 1 },
            { "007 | 0x5", 7 },
            { "007 | 0x9", 15 },
            { "007 ^ 0x5", 2 },
            { "007 ^ 0x9", 14 },
            // condition
            { "7 ? 2 : 0x5", 2 },
            { "0 ? 2 : 0x5", 5 },
            // complex
            { "(007 | 0x5) > (27 - 4 * 5) ? 2 - 2 : 31*(2**27)+24*3600*1000", 4247149568LL },
            // function
            { " addition (3, 7) ", 10 }
        };
        for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
        {
            const char       * expr            = expressions[idx].expr;
            const long long    expected_result = expressions[idx].expected_result;

            cout << "int expression : ";
            cout.width(25);
            cout << expr;
#if 1
            C_value    value = compute_expression_static (type_definitions, expr);
#else
            C_value    value;
            {
                T_interpret_data  interpret_data;
                C_perf_chrono  perf_chrono(interpret_data, expr, 0);
                value = compute_expression_static (type_definitions, expr);
            }
#endif
            cout << "    gives " << value.as_string();
            M_TEST_EQ(value.get_int(), expected_result);
            M_TEST_EQ(value.get_bit_position_offset(), -1);
            M_TEST_EQ(value.get_bit_position_size(), -1);

            T_expression    expression;
            expression.build_expression(type_definitions, expr);
            C_value    value_new = expression.compute_expression_static(type_definitions);
            if (value_new.get_int() != expected_result)
            {
                cout << "    and " << value_new.as_string();
            }
            M_TEST_EQ(value_new.get_int(), expected_result);
            M_TEST_EQ(value_new.get_bit_position_offset(), -1);
            M_TEST_EQ(value_new.get_bit_position_size(), -1);

            cout << endl;
        }
    }

    //-------------------------------------------------------------------------
    // bool static expression
    //-------------------------------------------------------------------------
    {
        struct T_test_expr_int_result
        {
            const char  * expr;
            long long     expected_result;
        };

        const T_test_expr_int_result    expressions[] = {
            { "007 <  0x7", 0 },
            { "007 <  0x8", 1 },
            { "007 <= 0x6", 0 },
            { "007 <= 0x7", 1 },
            { "007 >  0x7", 0 },
            { "007 >  0x6", 1 },
            { "007 >= 0x8", 0 },
            { "007 >= 0x7", 1 },
            { "007 == 0x5", 0 },
            { "007 == 0x7", 1 },
            { "007 != 0x7", 0 },
            { "007 != 0x5", 1 },
            { "007 && 0x5", 1 },
            { "(007 != 0x5) && (007 != 0x7)", 0 },
            { "007 != 0x5 && 007 != 0x7", 0 },
            { "007 != 0x5 && 007 == 0x7", 1 },
            { "007 == 0x5 || 007 != 0x7", 0 },
            { "007 == 0x5 || 007 == 0x7", 1 },
            { "1   == 0x1", 1 }
        };
        for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
        {
            const char       * expr            = expressions[idx].expr;
            const long long    expected_result = expressions[idx].expected_result;

            cout << "int expression : ";
            cout.width(25);
            cout << expr;
            C_value    value = compute_expression_static (type_definitions, expr);
            cout << "    gives " << value.as_string();
            M_TEST_EQ(value.get_bool(), expected_result);
            M_TEST_EQ(value.get_bit_position_offset(), -1);
            M_TEST_EQ(value.get_bit_position_size(), -1);

            T_expression    expression;
            expression.build_expression(type_definitions, expr);
            C_value    value_new = expression.compute_expression_static(type_definitions);
            if (value_new.get_int() != expected_result)
            {
                cout << "    and " << value_new.as_string();
            }
            M_TEST_EQ(value_new.get_bool(), expected_result);
            M_TEST_EQ(value_new.get_bit_position_offset(), -1);
            M_TEST_EQ(value_new.get_bit_position_size(), -1);

            cout << endl;
        }
    }

    //-------------------------------------------------------------------------
    // float static expression
    //-------------------------------------------------------------------------
    {
        struct T_test_expr_flt_result
        {
            const char  * expr;
            double        expected_result;
        };

        const T_test_expr_flt_result    expressions[] = {
            { "27.1", 27.1 },
            { "-234.9", -234.9 },
            { "-234.9E3", -234900.0 },
            { "-234.9E-1", -23.49 },     // exception
            { "-234.9E+1", -2349.0 },    // exception
            { "23+4.0", 27.0 },
            { "-1345.0-65", -1410.0 },
            { "234 * 2.003 - 5 + 1", 464.702 },
            { "0x045 + 1.0", 70.0 },
            { "-0Xdf / 1.0", -223.0 },
            { "-0Xdf // 1", -223.0 },      // // always gives float
            { "-0Xde / 2.0", -111.0 },
            { "-0Xde // 2", -111.0 },      // // always gives float
            { "(7 / 3) // 1", 2.0 },       // // always gives float
            { "007 * 1.0", 7.0 },
            { "03456 - 1.0", 1837.0 },
            { "235.9 + k::flt", 1 },			// const
            { " addition (3.0, 7) ", 10}
        };
        for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
        {
            const char   * expr            = expressions[idx].expr;
            const double   expected_result = expressions[idx].expected_result;

            cout << "flt expression : ";
            cout.width(25);
            cout << expr;
            if ((idx == 3) || (idx == 4)) 
            {
                M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static (type_definitions, expr));
                continue;
            }
            C_value    value = compute_expression_static (type_definitions, expr);
            cout << "    gives " << value.as_string();
            M_TEST_EQ(value.get_flt(), expected_result);
            M_TEST_EQ(value.get_bit_position_offset(), -1);
            M_TEST_EQ(value.get_bit_position_size(), -1);

            T_expression    expression;
            expression.build_expression(type_definitions, expr);
            C_value    value_new = expression.compute_expression_static(type_definitions);
            if (value_new.get_flt() != expected_result)
            {
                cout << "    and " << value_new.as_string();
            }
            M_TEST_EQ(value_new.get_flt(), expected_result);
            M_TEST_EQ(value_new.get_bit_position_offset(), -1);
            M_TEST_EQ(value_new.get_bit_position_size(), -1);

            cout << endl;
        }
    }

    //-------------------------------------------------------------------------
    // string static expression
    //-------------------------------------------------------------------------
    {
        struct T_test_expr_str_result
        {
            const char  * expr;
            const char  * expected_result;
        };

        const T_test_expr_str_result    expressions[] = {
            { "\"salut\"", "salut" },
            { "\"Hello world!\"", "Hello world!" },
            { "\"Hello \" + \"world!\"", "Hello world!" },
            { "\"Hello \"+ \"world!\"", "Hello world!" },
            { "\"Hello \" +\"world!\"", "Hello world!" },
            { "\"Hello \"+\"world!\"", "Hello world!" },
            { "(\"Hello \" + \"world!\")", "Hello world!" },
            { "(\"Hello \"+ \"world!\")", "Hello world!" },
            { "(\"Hello \" +\"world!\")", "Hello world!" },
            { "(\"Hello \"+\"world!\")", "Hello world!" },
            { "\"last test\"", "last test" },
            { "\"136\" + k::str", "136hello world" },			// const
            { " addition (\"3.0\", \"7\") ", "3.07"}
        };
        for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
        {
            const char  * expr            = expressions[idx].expr;
            const string  expected_result = expressions[idx].expected_result;

            cout << "str expression : ";
            cout.width(25);
            cout << expr;
            C_value    value = compute_expression_static (type_definitions, expr);
            cout << "    gives : " << value.as_string();
            M_TEST_EQ(value.get_str(), expected_result);
            M_TEST_EQ(value.get_bit_position_offset(), -1);
            M_TEST_EQ(value.get_bit_position_size(), -1);

            T_expression    expression;
            expression.build_expression(type_definitions, expr);
            C_value    value_new = expression.compute_expression_static(type_definitions);
            if (value_new.get_str() != expected_result)
            {
                cout << "    and " << value_new.as_string();
            }
            M_TEST_EQ(value_new.get_str(), expected_result);
            M_TEST_EQ(value_new.get_bit_position_offset(), -1);
            M_TEST_EQ(value_new.get_bit_position_size(), -1);

            cout << endl;
        }
    }

    //-------------------------------------------------------------------------
    // enum static expression
    //-------------------------------------------------------------------------
    {
        C_value    value = compute_expression_static (type_definitions, "T_Operation::MODIFICATION");
        M_TEST_EQ(value.get_int(), 1);
        M_TEST_EQ(value.get_bit_position_offset(), -1);
        M_TEST_EQ(value.get_bit_position_size(), -1);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "T_Operation::MODIFICATION");
        C_value    value = expression.compute_expression_static (type_definitions);
        M_TEST_EQ(value.get_int(), 1);
        M_TEST_EQ(value.get_bit_position_offset(), -1);
        M_TEST_EQ(value.get_bit_position_size(), -1);
        value.as_string();
    }

    //-------------------------------------------------------------------------
    // NOT static expression
    //-------------------------------------------------------------------------
    T_interpret_data                  interpret_data;

    interpret_data.read_variable_group_begin("niv");
    interpret_data.add_read_variable("salut", get_int_value_bit_pos_size(-32,12,7));
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut");
        M_TEST_EQ(value.get_int(), -32);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 7);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "salut");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), -32);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 7);
        value.as_string();
    }

    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "niv.salut");
        M_TEST_EQ(value.get_int(), -32);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 7);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "niv.salut");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), -32);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 7);
        value.as_string();
    }

    interpret_data.read_variable_group_begin("e2");
    interpret_data.add_read_variable("toi", get_int_value_bit_pos_size(-3450000,19,15));
    interpret_data.read_variable_group_end();
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut * e2.toi");
        M_TEST_EQ(value.get_int(), 110400000);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 22);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "salut * e2.toi");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 110400000);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 22);
        value.as_string();
    }

    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut * e2.toi + T_Operation::DELETION");
        M_TEST_EQ(value.get_int(), 110400000 + 2);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 22);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "salut * e2.toi + T_Operation::DELETION");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 110400000 + 2);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 22);
        value.as_string();
    }

    interpret_data.read_variable_group_begin("eau");
    interpret_data.add_read_variable("la", get_int_value_bit_pos_size(3402340,30,31));
    interpret_data.read_variable_group_end();
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut * e2.toi / eau.la");
        M_TEST_EQ(value.get_int(), 32);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 49);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "salut * e2.toi / eau.la");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 32);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 49);
        value.as_string();
    }

    interpret_data.add_read_variable("matelas", get_flt_value_bit_pos_size(45.3465,67,2));
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "niv.salut * niv.e2.toi / niv.eau.la - niv.matelas");
        M_TEST_EQ_APPROX(value.get_flt(), -13.3465);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 57);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "niv.salut * niv.e2.toi / niv.eau.la - niv.matelas");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ_APPROX(value.get_flt(), -13.3465);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 57);
        value.as_string();
    }

    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "addition(niv.salut * niv.e2.toi, niv.eau.la / niv.matelas)");
        M_TEST_EQ_APPROX(value.get_flt(), 110475607.5556);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 57);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "addition(niv.salut * niv.e2.toi, niv.eau.la / niv.matelas)");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ_APPROX(value.get_flt(), 110475607.5556);
        M_TEST_EQ(value.get_bit_position_offset(), 12);
        M_TEST_EQ(value.get_bit_position_size(), 57);
        value.as_string();
    }

    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "perf(niv.eau.la, 3402341)");
        M_TEST_EQ(value.get_int(), 6804680);
        M_TEST_EQ(value.get_bit_position_offset(), 30);
        M_TEST_EQ(value.get_bit_position_size(), 31);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "perf(niv.eau.la, 3402341)");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 6804680);
        M_TEST_EQ(value.get_bit_position_offset(), 30);
        M_TEST_EQ(value.get_bit_position_size(), 31);
        value.as_string();
    }

    //-------------------------------------------------------------------------
    // Test array
    //-------------------------------------------------------------------------
    interpret_data.add_read_variable("an_array[0]", 1000);
    interpret_data.add_read_variable("an_array[1]", 1001);
    interpret_data.add_read_variable("an_array[2]", 1002);
    interpret_data.add_read_variable("an_array[3]", 1003);
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[0]");
        M_TEST_EQ(value.get_int(), 1000);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "an_array[0]");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 1000);
        value.as_string();
    }

    interpret_data.add_read_variable("idx", 1);
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[idx]");
        M_TEST_EQ(value.get_int(), 1001);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "an_array[idx]");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 1001);
        value.as_string();
    }

    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[idx-1]");
        M_TEST_EQ(value.get_int(), 1000);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "an_array[idx-1]");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 1000);
        value.as_string();
    }

    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[idx+2]");
        M_TEST_EQ(value.get_int(), 1003);
        value.as_string();
    }
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "an_array[idx+2]");
        C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 1003);
        value.as_string();
    }

    //-------------------------------------------------------------------------
    // Test array multiple
    //-------------------------------------------------------------------------
    interpret_data.add_read_variable("a_multiple_array[0][0]", "00");
    interpret_data.add_read_variable("a_multiple_array[0][1]", "01");
    interpret_data.add_read_variable("a_multiple_array[1][0]", "10");
    interpret_data.add_read_variable("a_multiple_array[1][1]", "11");
    interpret_data.add_read_variable("a_multiple_array[2][0]", "20");
    interpret_data.add_read_variable("a_multiple_array[2][1]", "21");
    interpret_data.add_read_variable("a_multiple_array[3][0]", "30");
    interpret_data.add_read_variable("a_multiple_array[3][1]", "31");
    interpret_data.add_read_variable("idx", 1);
    {
        C_value   value = compute_expression_no_io (type_definitions, interpret_data, "a_multiple_array[idx+2][idx-1]");
        M_TEST_EQ(value.get_str(), "30");
    }

    interpret_data.add_read_variable("s_multiple_array[0][0].toto", "00");
    interpret_data.add_read_variable("s_multiple_array[0][1].toto", "01");
    interpret_data.add_read_variable("s_multiple_array[1][0].toto", "10");
    interpret_data.add_read_variable("s_multiple_array[1][1].toto", "11");
    interpret_data.add_read_variable("s_multiple_array[2][0].toto", "20");
    interpret_data.add_read_variable("s_multiple_array[2][1].toto", "21");
    interpret_data.add_read_variable("s_multiple_array[3][0].toto", "30");
    interpret_data.add_read_variable("s_multiple_array[3][1].toto", "31");
    interpret_data.add_read_variable("idx", 1);
    {
        C_value   value = compute_expression_no_io(type_definitions, interpret_data, "s_multiple_array[idx+1][idx].toto");
        M_TEST_EQ(value.get_str(), "21");
    }

    //-------------------------------------------------------------------------
    // NOT static expression build and compute
    //-------------------------------------------------------------------------
    {
        T_expression    expression;
        expression.build_expression(type_definitions, "another_value == 0 ? 4 : 6");

        C_value    value;

        interpret_data.add_read_variable("another_value", 1);
        value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 6);
        value.as_string();

        interpret_data.add_read_variable("another_value", 0);
        value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 4);
        value.as_string();

        interpret_data.add_read_variable("another_value", 2);
        value = expression.compute_expression_no_io (type_definitions, interpret_data);
        M_TEST_EQ(value.get_int(), 6);
        value.as_string();
    }

    //-------------------------------------------------------------------------
    // perf expression
    //-------------------------------------------------------------------------
    test_expression_perf_operation(type_definitions);
    test_expression_perf_function(type_definitions);
}

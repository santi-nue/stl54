/*
 * Copyright 2005-2019 Olivier Aveline <wsgd@free.fr>
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

#include <vector>
#include <algorithm>

using namespace std;

#include "precomp.h"
#include "C_value.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret_parse.h"


//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <value>
// <value> could be :
//   <int_value>
//   <float_value>
//   <enum_value> or <const_value>
//   <variable_value>
//   *   --> return E_rc_multiple_value
//   "string value"
//*****************************************************************************

E_return_code    get_complex_value (const T_type_definitions  & type_definitions,
                                    const T_interpret_data    & interpret_data,
                                    const string              & value_str,
                                          C_value             & value)
{
    long long    value_int = 0;
    double       value_flt = 0.0;

    if (get_number (value_str.c_str (), value_int))
    {
        value = value_int;
    }
    else if (get_number (value_str.c_str (), value_flt))
    {
        value = value_flt;
    }
    else if (type_definitions.get_type_value(value_str, value))
    {
    }
    // very bad perfomance
    else if (interpret_data.get_value_of_read_variable (value_str, value))
    {
    }
    else
    {
        string  real_value_str = value_str;
        remove_string_limits (real_value_str);

        if (real_value_str == value_str)
        {
            if (strchr(value_str.c_str(), '*') != NULL_PTR)
            {
                return  E_rc_multiple_value;
            }

            return  E_rc_not_ok;
        }

        value = real_value_str;
    }

    return  E_rc_ok;
}

//*****************************************************************************
// words_to_string
//*****************************************************************************

string    words_to_string(const vector<string>::const_iterator      & iter_begin,
                          const vector<string>::const_iterator      & iter_end)
{
    string  str;
    for (vector<string>::const_iterator  iter  = iter_begin;
                                         iter != iter_end;
                                       ++iter)
    {
        str += *iter;
        str += " ";
    }

    return  str;
}

string    words_to_string(const vector<string>      & words)
{
    string  str;
    for (vector<string>::const_iterator  iter  = words.begin();
                                         iter != words.end();
                                       ++iter)
    {
        str += *iter;
        str += " ";
    }

    return  str;
}

//*****************************************************************************
// compute_expression
// ----------------------------------------------------------------------------
// Format :
// 
//*****************************************************************************
#include "T_expression.h"
C_value    compute_expression (
                         const T_type_definitions      & type_definitions,
                               T_interpret_data        & interpret_data,
                               T_frame_data            & in_out_frame_data,
                         const string                  & in_str,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
    M_TRACE_ENTER("compute_expression", in_str);

    T_expression    expression;
    expression.build_expression(type_definitions, in_str);

    return  expression.compute_expression(
                                type_definitions,
                                interpret_data,
                                in_out_frame_data,
                                data_name,
                                data_simple_name,
                                os_out,
                                os_err);
}

//*****************************************************************************
// compute_expression_no_io
// ----------------------------------------------------------------------------
// 
//*****************************************************************************

C_value    compute_expression_no_io (
                         const T_type_definitions  & type_definitions,
                               T_interpret_data    & interpret_data,
                         const std::string         & str)
{
    T_frame_data        in_out_frame_data;
    const string        data_name;
    const string      & data_simple_name = data_name;
    ostringstream       os_out;
    ostringstream     & os_err = os_out;

    return  compute_expression(type_definitions, interpret_data, in_out_frame_data,
                               str,
                               data_name, data_simple_name, os_out, os_err);
}

//*****************************************************************************
// compute_expression_static
// ----------------------------------------------------------------------------
// 
//*****************************************************************************

C_value    compute_expression_static (const T_type_definitions  & type_definitions,
                                      const std::string         & str)
{
    T_interpret_data    interpret_data;

    return  compute_expression_no_io(type_definitions, interpret_data, str);
}

//*****************************************************************************
// compute_expression_array
// ----------------------------------------------------------------------------
// Format :
// 
//*****************************************************************************

string    compute_expressions_in_array (
                         const T_type_definitions      & type_definitions,
                               T_interpret_data        & interpret_data,
                               T_frame_data            & in_out_frame_data,
                         const string                  & in_str,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
    M_TRACE_ENTER("compute_expressions_in_array", in_str);

    // Compute the value of each array index.
    string    simple_type = in_str;
    string    str_array_index;
    string    str_arrays_result;
    while (decompose_type_sep_value_sep (simple_type,
                                                      '[',
                                                      ']',
                                                      simple_type,
                                                      str_array_index) == E_rc_ok)
    {
        // Compute the value of the extracted index.
        C_value  value_index = compute_expression (type_definitions, interpret_data, in_out_frame_data, str_array_index, data_name, data_simple_name, os_out, os_err);
        M_FATAL_IF_NE(value_index.get_type(), C_value::E_type_integer);

        // Add the computed index to the result array.
        const string  str_array_index_result = "[" + value_index.as_string() + "]";
        str_arrays_result.insert(0, str_array_index_result);
    }

    const string  result = simple_type + str_arrays_result;

    M_TRACE_DEBUG("result=" << result);
    return  result;
}


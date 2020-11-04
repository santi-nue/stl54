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

#include "T_type_definitions.h"
#include "T_interpret_data.h"
#include "byte_interpret_parse.h"
#include "byte_interpret.h"
#include "unitary_tests_ut_interpret_bytes.h"


//*****************************************************************************
// ut_interpret_bytes_base ****************************************************
//*****************************************************************************

bool    ut_interpret_bytes_base (const T_type_definitions  & type_definitions,
                                 const T_byte_vector       & in_byte_vector,
                                 const string              & in_input_string,
                                       ostream             & os_out,
                                       ostream             & os_err,
                                       T_interpret_data    & interpret_data)
{
    string    conditions;
    if (interpret_data.get_decode_function().empty())
    {
        conditions += "      ";
    }
    else
    {
        conditions += "decode";
    }

    conditions += " ";

    if (get_debug() == false)
    {
        conditions += "     ";
    }
    else
    {
        conditions += "debug";
    }


    const T_byte    * P_bytes = in_byte_vector.empty() ? NULL : &in_byte_vector[0];
    size_t            sizeof_bytes = in_byte_vector.size();

    bool  result = false;
    M_TRACE_WARNING("enter ut_interpret_bytes_base " << conditions << "  for " << in_input_string);
    get_state_ostream() << flush;
    {
        C_perf_chrono  perf_chrono(interpret_data, in_input_string.c_str(), sizeof_bytes);
        result = interpret_bytes (type_definitions,
                                    P_bytes,
                                    sizeof_bytes,
                                    in_input_string,
                                    os_out,
                                    os_err,
                                    interpret_data);
    }
    M_TRACE_WARNING("leave ut_interpret_bytes_base " << conditions << "  for " << in_input_string);

    M_TEST_EQ(result, true);
    // Check no remaining input
    M_TEST_EQ(sizeof_bytes, 0);
    // Even in internal_frame : this is already tested by interpret_bytes
    // Fail because decode_stream_frame does NOT exist anymore
//	M_TEST_EQ(interpret_data.get_decode_stream_frame().frame_data.get_remaining_bits(), 0);

    return  result;
}

//*****************************************************************************
// ut_interpret_bytes *********************************************************
//*****************************************************************************

bool  S_ut_interpret_bytes_decode_nothing = true;
bool  S_ut_interpret_bytes_debug          = false;


C_ut_interpret_bytes_no_decode_guard::C_ut_interpret_bytes_no_decode_guard()
    : _S_ut_interpret_bytes_decode_nothing_saved(S_ut_interpret_bytes_decode_nothing)
{
    S_ut_interpret_bytes_decode_nothing = false;
}
C_ut_interpret_bytes_no_decode_guard::~C_ut_interpret_bytes_no_decode_guard()
{
    S_ut_interpret_bytes_decode_nothing = _S_ut_interpret_bytes_decode_nothing_saved;
}

void ut_interpret_bytes (const T_type_definitions  & type_definitions,
                         const T_byte_vector       & in_byte_vector,
                         const string              & in_input_string,
                               T_interpret_data    & interpret_data,
                         const char                * output_expected)
{
    // Save original interpret_data (because of global data).
    T_interpret_data  interpret_data_save = interpret_data;

    string    output_str_normal;

    for (int  idx_decode = 0; idx_decode <= S_ut_interpret_bytes_decode_nothing; ++idx_decode)
    for (int  idx_debug  = 0; idx_debug  <= S_ut_interpret_bytes_debug; ++idx_debug)
    {
        // Restore original interpret_data (because of global data).
        interpret_data = interpret_data_save;

        ostringstream   oss;
        ostream       & os_out = oss;
        ostream       & os_err = oss;

        if (idx_decode > 0)  { interpret_data.set_decode_function("decode_stream_nothing"); }
        C_debug_set_temporary  debug_set_temporary((idx_debug == 0) ? E_debug_status_OFF : E_debug_status_ON);
        bool  result = ut_interpret_bytes_base (type_definitions,
                                        in_byte_vector,
                                        in_input_string,
                                        os_out,
                                        os_err,
                                        interpret_data);
        if (idx_decode > 0)  { interpret_data.set_decode_function(""); }
        if (result != true)
        {
            return;
        }

        if (idx_decode == 0)
        {
            // Without decoder, save output.
            output_str_normal = oss.str();

            // Compare to expected output.
            if (output_expected != NULL)
            {
                if (output_str_normal != output_expected)
                {
                    string  output_expected_str = output_expected;
                    string  put_a_breakpoint_here;
                }

                M_TEST_EQ(output_str_normal, output_expected);
            }
        }
        else
        {
            // With decoder
            string    output_str_decoder = oss.str();

            // Check output is identic to normal output
            M_TEST_EQ(output_str_decoder, output_str_normal);
        }
    }
}

//*****************************************************************************
// ut_interpret_bytes *********************************************************
//*****************************************************************************

void ut_interpret_bytes (const T_type_definitions  & type_definitions,
                         const char                * in_hexa_str_param,
                         const string              & in_input_string,
                               T_interpret_data    & interpret_data,
                         const char                * output_expected)
{
    string           in_hexa_str = in_hexa_str_param;
    mod_replace_all(in_hexa_str, " ", "");
    mod_replace_all(in_hexa_str, "	", "");

    T_byte_vector    byte_vector;
    string_hexa_to_frame(in_hexa_str, byte_vector);

    ut_interpret_bytes (type_definitions, byte_vector, in_input_string, interpret_data, output_expected);
}

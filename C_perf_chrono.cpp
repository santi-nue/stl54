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

#include "C_perf_chrono.h"


//*****************************************************************************
//*****************************************************************************
#if 0
#include <time.h> 
#include <sys/times.h> 
void   text_unix_time()
{
    cout << "CLOCKS_PER_SEC = " << CLOCKS_PER_SEC << endl;  // gives 1000000
//	cout << "CLK_TCK        = " << CLK_TCK        << endl;
    cout << "clock() = " << clock() << endl;
    cout << "times() = " << times(NULL) << endl;
    sleep(1);
    cout << "clock() = " << clock() << endl;
    cout << "times() = " << times(NULL) << endl;
}
#endif

//*****************************************************************************
// C_perf_chrono
//*****************************************************************************

long    C_perf_chrono::A_total_ms = 0;
long    C_perf_chrono::A_total_ms_decode = 0;
long    C_perf_chrono::A_total_ms_no_decode = 0;
long    C_perf_chrono::A_total_sizeof_bytes = 0;
long    C_perf_chrono::A_total_sizeof_bytes_decode = 0;
long    C_perf_chrono::A_total_sizeof_bytes_no_decode = 0;

C_perf_chrono::C_perf_chrono(      T_interpret_data    & interpret_data,
                             const char                * in_input_string,
                             const size_t                in_sizeof_bytes)
    :A_interpret_data(interpret_data),
     A_input_string(in_input_string),
     A_sizeof_bytes(in_sizeof_bytes)
{
}

C_perf_chrono::~C_perf_chrono()
{
    T_perf_time  perf_time_val;

    const long          ms_diff_ctor = perf_time_diff_ms(perf_time_val, A_perf_time_ctor);
    T_interpret_data    interpret_data = A_interpret_data;
    const string      & decode_function = interpret_data.get_decode_function();
    const string        decode = decode_function.empty() ? "      " : "decode" ;

    if (get_debug() == false)
    {
        A_total_ms           += ms_diff_ctor;
        A_total_sizeof_bytes += A_sizeof_bytes;

        if (decode_function.empty())
        {
            A_total_ms_no_decode           += ms_diff_ctor;
            A_total_sizeof_bytes_no_decode += A_sizeof_bytes;
        }
        else
        {
            A_total_ms_decode           += ms_diff_ctor;
            A_total_sizeof_bytes_decode += A_sizeof_bytes;
        }
    }

    output(ms_diff_ctor, A_sizeof_bytes, decode, get_debug(), A_input_string);
}

void
C_perf_chrono::output(
        const long              ms_diff_ctor,
        const int               nb_bytes,
        const string          & decode,
        const E_debug_status    debug,
        const string          & what)
{
    cout.width(5);
    cout << ms_diff_ctor << " ms  ";

    cout.width(5);
    cout << nb_bytes << " bytes  ";

    cout << decode;
    cout << " ";

    if (debug == E_debug_status_OFF)
    {
        cout << "     ";
    }
    else
    {
        cout << "debug";
    }

    cout << "  for " << what;
    cout << endl;
}

void
C_perf_chrono::end()
{
    output(A_total_ms_no_decode, A_total_sizeof_bytes_no_decode, "      ", E_debug_status_OFF, "sum of all times (do not compare with decode time)");
    output(A_total_ms_decode   , A_total_sizeof_bytes_decode   , "decode", E_debug_status_OFF, "sum of all times");
    output(A_total_ms          , A_total_sizeof_bytes          , "all   ", E_debug_status_OFF, "sum of all times");
}

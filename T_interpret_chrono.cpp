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

#include "precomp.h"
#include "T_interpret_chrono.h"
using namespace std;


//*****************************************************************************
// T_interpret_chrono
//*****************************************************************************

#ifndef WIN32
T_interpret_chrono::T_interpret_chrono()
{
}

std::string
T_interpret_chrono::compute_chrono_value_from_command(const std::string  & command)
{
    return  "NOT implemented (only on WIN32) " + command;
}

#else

#include "byte_interpret_common.h"

long    T_interpret_chrono::A_ms_cumulative = 0;


//*****************************************************************************
// T_interpret_chrono
//*****************************************************************************

T_interpret_chrono::T_interpret_chrono()
{
    A_timeb_last = A_timeb_ctor;
}

std::string
T_interpret_chrono::compute_chrono_value_from_command(const std::string  & command)
{
    T_perf_time  timeb_val;

    long    ms_diff_last = perf_time_diff_ms(timeb_val, A_timeb_last);
    long    ms_diff_ctor = perf_time_diff_ms(timeb_val, A_timeb_ctor);
    A_ms_cumulative += ms_diff_last;

    string  str_diff_last = get_diff_time_ms_str(ms_diff_last);
    string  str_diff_ctor = get_diff_time_ms_str(ms_diff_ctor);
    string  str_diff_cumulative = get_diff_time_ms_str(A_ms_cumulative);

    A_timeb_last = timeb_val;

    string  str = command + " " + str_diff_last + "  msg=" + str_diff_ctor+ "  cumulative=" + str_diff_cumulative;

    return  str;
}

#endif

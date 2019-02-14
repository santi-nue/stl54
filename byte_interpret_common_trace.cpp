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

// ****************************************************************************
// Includes.
// ****************************************************************************

#include "precomp.h"
#include "byte_interpret_common_trace.h"

#include <iostream>
#include <vector>
#include <cstdio>

// ****************************************************************************
// M_state_... trace output stream.
// ****************************************************************************

static ostream  * S_P_state_ostream = &cout;

ostream  & get_state_ostream()
{
    return  * S_P_state_ostream;
}

ostream  & set_state_ostream(ostream  & new_state_ostream)
{
    ostream  & old_state_ostream = * S_P_state_ostream;

    S_P_state_ostream = & new_state_ostream;

    return  old_state_ostream;
}

// ****************************************************************************
// M_state_... trace flag.
// ****************************************************************************
void    set_debug(E_debug_status    debug)
{
    C_trace::A_debug_status = debug;
}

E_debug_status  get_debug()
{
    return  C_trace::A_debug_status;
}


C_debug_set_temporary::C_debug_set_temporary(E_debug_status  debug)
    :previous_value(get_debug()),
     value_modified(false)
{
    set(debug);
}

C_debug_set_temporary::C_debug_set_temporary()
    :previous_value(get_debug()),
     value_modified(false)
{
}

C_debug_set_temporary::~C_debug_set_temporary()
{
    unset();
}

void
C_debug_set_temporary::set(E_debug_status  debug)
{
    set_debug(debug);
    value_modified = true;
}
void
C_debug_set_temporary::unset()
{
    if (value_modified)
    {
        set_debug(previous_value);
        value_modified = false;
    }
}
void
C_debug_set_temporary::forget()
{
    value_modified = false;
}

// ****************************************************************************
// M_state_... trace class.
// ****************************************************************************
E_debug_status  C_trace::A_debug_status = E_debug_status_OFF;

C_trace::C_trace(const char  * function_name)
    :A_function_name(function_name),
     A_must_do_leave_trace(true)
{
}

C_trace::~C_trace()
{
    if (A_must_do_leave_trace)
    {
        M_TRACE_base(" ", "Leave", A_function_name);
    }
}

//#ifdef WIN32
#include "T_perf_time.h"

bool         perf_time_val_last_trace_initialized = false;
T_perf_time  perf_time_val_last_trace;
//#endif

void
C_trace::print_beginning_of_trace(      ostream  & os,
                                  const char     * prefix1,
                                  const char     * prefix2)
{
    os << prefix1 << " ";

    if (A_debug_status == E_debug_status_ON)
    {
        T_perf_time  timeb_val;

        os << timeb_val << " ";

        if ((perf_time_val_last_trace_initialized) &&
            (timeb_val != perf_time_val_last_trace))
        {
            long    diff_time_ms = perf_time_diff_ms(timeb_val, perf_time_val_last_trace);
            if (diff_time_ms > 1)
            {
                char  time_str[99+1];
                sprintf(time_str, "%4ld ", diff_time_ms);
                os << time_str;
            }
            else
            {
                os << "     ";
            }
        }
        else
        {
            os << "     ";
        }
        perf_time_val_last_trace = timeb_val;
        perf_time_val_last_trace_initialized = true;
    }

    os << prefix2 << " "; 
}

void
C_trace::leave_trace_done()
{
    A_must_do_leave_trace = false;
}


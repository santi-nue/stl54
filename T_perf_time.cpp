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
#include "T_perf_time.h"

#ifdef WIN32
#else
#include <stdio.h>

#define WSGD_USE_TIMES
#ifdef  WSGD_USE_TIMES
extern long int __sysconf (int);
//#define WSGD_CLK_TCK  __sysconf (_SC_CLK_TCK)
#define WSGD_CLK_TCK  100
#else
#define WSGD_CLK_TCK  CLOCKS_PER_SEC
#endif
#endif
using namespace std;


//*****************************************************************************
// T_perf_time
//*****************************************************************************

bool
T_perf_time::operator!= (const T_perf_time  & rhs) const
{
#ifdef WIN32
    if ((A_perf_time.time != rhs.A_perf_time.time) ||
        (A_perf_time.millitm != rhs.A_perf_time.millitm))
    {
        return  true;
    }

    return  false;
#else
#ifdef  WSGD_USE_TIMES
    if (A_perf_times != rhs.A_perf_times)
#else
    if (A_perf_clock != rhs.A_perf_clock)
#endif
    {
        return  true;
    }

    return  false;
#endif
}

ostream &  operator<<(ostream & os, const T_perf_time  & rhs)
{
    char  time_str[99+1];
#ifdef WIN32
#ifdef _USE_32BIT_TIME_T
    sprintf(time_str, "%d.%03d", rhs.A_perf_time.time, rhs.A_perf_time.millitm);
#else
    sprintf(time_str, "%lld.%03d", rhs.A_perf_time.time, rhs.A_perf_time.millitm);
#endif
#else
#ifdef  WSGD_USE_TIMES
    clock_t    modulo = rhs.A_perf_times % WSGD_CLK_TCK;
    sprintf(time_str, "%ld.%03ld", (rhs.A_perf_times - modulo ) / WSGD_CLK_TCK, modulo * 1000 / WSGD_CLK_TCK);
#else
    clock_t    modulo = rhs.A_perf_clock % WSGD_CLK_TCK;
    sprintf(time_str, "%ld.%03ld", (rhs.A_perf_clock - modulo ) / WSGD_CLK_TCK, modulo * 1000 / WSGD_CLK_TCK);
#endif
#endif
    os << time_str;
    return  os;
}

long    perf_time_diff_ms(const T_perf_time  & timeb_val_lhs,
                          const T_perf_time  & timeb_val_rhs)
{
#ifdef WIN32
    long  diff_total_ms = static_cast<long>((timeb_val_lhs.A_perf_time.time - timeb_val_rhs.A_perf_time.time) * 1000 +
                                            (timeb_val_lhs.A_perf_time.millitm - timeb_val_rhs.A_perf_time.millitm));
#else
#ifdef  WSGD_USE_TIMES
    long  diff_total_ms = (timeb_val_lhs.A_perf_times - timeb_val_rhs.A_perf_times) * 1000 / WSGD_CLK_TCK;
#else
    long  diff_total_ms = (timeb_val_lhs.A_perf_clock - timeb_val_rhs.A_perf_clock) * 1000 / WSGD_CLK_TCK;
#endif
#endif
    return  diff_total_ms;
}

string  get_diff_time_ms_str(const long  diff_total_ms)
{
    long  diff_ms  =  diff_total_ms % 1000;
    long  diff_sec = (diff_total_ms - diff_ms) / 1000;

    char  diff_str[99+1];
#ifdef WIN32
    sprintf(diff_str, "%d.%03d", diff_sec, diff_ms);
#else
    sprintf(diff_str, "%ld.%03ld", diff_sec, diff_ms);
#endif

    return  diff_str;
}

string  perf_time_diff_ms_str(const T_perf_time  & timeb_val_lhs,
                              const T_perf_time  & timeb_val_rhs)
{
    long  diff_total_ms = perf_time_diff_ms(timeb_val_lhs, timeb_val_rhs);

    return  get_diff_time_ms_str(diff_total_ms);
}


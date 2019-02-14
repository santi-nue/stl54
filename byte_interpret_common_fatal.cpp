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
#include "byte_interpret_common_fatal.h"
#include "byte_interpret_common_trace.h"
#include "byte_interpret_common_exception.h"

#include <iostream>
#include <vector>
#include <cstdio>


//*****************************************************************************
// fatal_pb
//*****************************************************************************

void    fatal_pb (const string  & lhs,
                  const string  & comp,
                  const string  & rhs,
                  const char    * file_name,
                  const size_t    file_line)
{
    ostringstream  oss;
    oss << lhs << " "
        << comp << " "
        << rhs << " "
        << "at " << file_name << "[" << file_line << "]";
    const string  oss_str = oss.str ();

    M_TRACE_FATAL(oss_str);
    throw  C_byte_interpret_exception(M_WHERE, E_byte_interpret_exception_fatal, oss_str);
}

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

// Necessary for windows pre-compiled headers.
// At the end, does NOT work : missing functions at link time
// After other includes, does NOT work : does NOT compile
#include "precomp.h"

#include <cstdio>
#include <cerrno>
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

#include "C_value.h"
#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_build_types.h"
#include "byte_interpret_builder.h"
#include "F_common_print.h"


#include "T_interpret_data.h"


//*****************************************************************************
// T_interpret_data
//*****************************************************************************

void    swap(T_interpret_data  & lhs,
             T_interpret_data  & rhs)
{
    swap(  (T_interpret_byte_order&)lhs,   (T_interpret_byte_order&)rhs);
    swap(      (T_interpret_output&)lhs,       (T_interpret_output&)rhs);
    swap( (T_interpret_read_values&)lhs,  (T_interpret_read_values&)rhs);
    swap(      (T_interpret_decode&)lhs,       (T_interpret_decode&)rhs);
    swap((T_interpret_inside_frame&)lhs, (T_interpret_inside_frame&)rhs);
    swap(      (T_interpret_chrono&)lhs,       (T_interpret_chrono&)rhs);
    
}

//*****************************************************************************
// C_interpret_data_set_temporary
//*****************************************************************************
#if 0
static T_interpret_data  * S_P_interpret_data = nullptr;    // recursive call ok

C_interpret_data_set_temporary::C_interpret_data_set_temporary(T_interpret_data  & interpret_data)
    :previous_value(S_P_interpret_data),
     value_modified(false)
{
    set(interpret_data);
}

C_interpret_data_set_temporary::C_interpret_data_set_temporary()
    :previous_value(S_P_interpret_data),
     value_modified(false)
{
}

C_interpret_data_set_temporary::~C_interpret_data_set_temporary()
{
    unset();
}

void
C_interpret_data_set_temporary::set(T_interpret_data  & interpret_data)
{
    S_P_interpret_data = & interpret_data;
    value_modified = true;
}
void
C_interpret_data_set_temporary::unset()
{
    if (value_modified)
    {
        S_P_interpret_data = previous_value;
        value_modified = false;
    }
}
void
C_interpret_data_set_temporary::forget()
{
    value_modified = false;
}

//*****************************************************************************
// get_interpret_data
//*****************************************************************************

T_interpret_data  & get_interpret_data()
{
    M_FATAL_IF_FALSE(S_P_interpret_data != nullptr);
    return  * S_P_interpret_data;
}

//*****************************************************************************
// get_P_interpret_data
//*****************************************************************************

T_interpret_data  * get_P_interpret_data()
{
    return  S_P_interpret_data;
}
#endif

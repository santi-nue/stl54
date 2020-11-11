/*
 * Copyright 2008-2019 Olivier Aveline <wsgd@free.fr>
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
#include "T_attribute_value.h"
using namespace std;


//*****************************************************************************
// T_attribute_value
//*****************************************************************************

T_attribute_value::T_attribute_value()
    :transformed(""),
     P_error(NULL)
{
    original = transformed.as_string();
}

void
T_attribute_value::reset()
{
    transformed = C_value("");
    P_error = NULL;
    original = transformed.as_string();
}

T_attribute_value::T_attribute_value(const C_value  & value)
    :transformed(value),
     P_error(NULL)
{
    original = transformed.as_string();
}

void
T_attribute_value::reset(const C_value  & value)
{
    transformed = value;
    P_error = NULL;
    original = transformed.as_string();
}

T_attribute_value::T_attribute_value(const T_attribute_value  & rhs)
    :CT_debug_object_counter<T_attribute_value>(rhs),
     transformed(rhs.transformed),
     P_error(NULL),
     original(rhs.original)
{
    if (rhs.P_error != NULL)
    {
        P_error = new string(*rhs.P_error);
    }
}

T_attribute_value&
T_attribute_value::operator=(const T_attribute_value  & rhs)
{
    transformed = rhs.transformed;
    original = rhs.original;

    if (rhs.P_error != P_error)
    {
        if (P_error != NULL)
        {
            delete P_error;
            P_error = NULL;
        }
        if (rhs.P_error != NULL)
        {
            P_error = new string(*rhs.P_error);
        }
    }

    return  *this;
}

T_attribute_value::~T_attribute_value()
{
    if (P_error != NULL)
    {
        delete P_error;
        P_error = NULL;
    }
}

//*****************************************************************************
// set_error
//*****************************************************************************
void
T_attribute_value::set_error(const std::string  & in_error)
{
    if (P_error != NULL)
    {
        delete P_error;
        P_error = NULL;
    }

    if (in_error.empty() == false)
    {
        P_error = new string(in_error);
    }
}

//*****************************************************************************
// set_value_transformed
//*****************************************************************************
void
T_attribute_value::set_value_transformed(const C_value&  value)
{
    transformed = value;
}

//*****************************************************************************
// set_value_original
//*****************************************************************************
void
T_attribute_value::set_value_original(const C_value&  value)
{
    transformed = value;

    original = transformed.as_string();
}

//*****************************************************************************
// set_value_original_format_reset
//*****************************************************************************
void
T_attribute_value::set_value_original_format_reset(const C_value&  value)
{
    transformed = value;

    {
        C_value  original_value = transformed;

        // normalize the string original
        original_value.format_reset();

        original = original_value.as_string();
    }
}

//*****************************************************************************
// change_value_str_only
//*****************************************************************************
void
T_attribute_value::change_value_str_only(const string &  new_str)
{
    transformed.set_str(new_str);
}

//*****************************************************************************
// set_bit_position_offset_size
//*****************************************************************************
void
T_attribute_value::set_bit_position_offset_size(int  offset, int  size)
{
    transformed.set_bit_position_offset_size(offset, size);
}

//*****************************************************************************
// swap
//*****************************************************************************

void    swap(T_attribute_value  & lhs,
             T_attribute_value  & rhs)
{
    swap(lhs.transformed, rhs.transformed);
    swap(lhs.P_error,     rhs.P_error);
    swap(lhs.original,    rhs.original);
}

//*****************************************************************************
// attribute_value_to_string
//*****************************************************************************

string    attribute_value_to_string (const T_attribute_value  & attribute_value)
{
    string    value_str = attribute_value.transformed.as_string();

    if (attribute_value.original != attribute_value.transformed.as_string())
        value_str += " (" + attribute_value.original + ")";

    if (attribute_value.P_error != NULL)
        value_str += "\t" + *attribute_value.P_error;

    return  value_str;
}

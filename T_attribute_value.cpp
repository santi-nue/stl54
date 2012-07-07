/*
 * Copyright 2008-2012 Olivier Aveline <wsgd@free.fr>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

T_attribute_value::T_attribute_value(const C_value  & value)
	:transformed(value),
	 P_error(NULL)
{
	original = transformed.as_string();
}

T_attribute_value::T_attribute_value(const T_attribute_value  & rhs)
	:transformed(rhs.transformed),
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
// value_is_original
//*****************************************************************************
void
T_attribute_value::value_is_original()
{
	original = transformed.as_string();
}

//*****************************************************************************
// value_is_original_format_reset
//*****************************************************************************
void    
T_attribute_value::value_is_original_format_reset()
{
	C_value  value = transformed;

	// normalize the string original
	value.format_reset();

	original = value.as_string();
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

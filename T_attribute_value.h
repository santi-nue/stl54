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

#ifndef T_attribute_value_h
#define T_attribute_value_h

//*****************************************************************************
// Includes.
//*****************************************************************************

#include "C_value.h"
#include <string>

#include "byte_interpret_common.h"

//*****************************************************************************
// T_attribute_value
//*****************************************************************************

struct T_attribute_value : public C_debug_object_counter
{
	// Size = 56 + 12 + 12 = 80 bytes

	// enum : symbolic name
	// int  : after quantum,offset
	// int  : after hex, oct ip
	C_value    transformed;

	// error text (unknow enum value, constraints ...)
	std::string     error;

	void    value_is_original();
	void    value_is_original_format_reset();

	T_attribute_value();
	T_attribute_value(const C_value  & value);

private:
	// the original value read
	std::string     original;

	friend void    swap(T_attribute_value  & lhs,
						T_attribute_value  & rhs);

	friend std::string    attribute_value_to_string (const T_attribute_value  & attribute_value);
};

void    swap(T_attribute_value  & lhs,
			 T_attribute_value  & rhs);


std::string    attribute_value_to_string (const T_attribute_value  & attribute_value);


#endif /* T_attribute_value_h */

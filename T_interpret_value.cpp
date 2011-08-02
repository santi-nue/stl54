/*
 * Copyright 2005-2009 Olivier Aveline <wsgd@free.fr>
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
#include "T_interpret_value.h"
#include "byte_interpret_common.h"


//*****************************************************************************
// T_interpret_value
//*****************************************************************************

T_interpret_value::T_interpret_value()
	:A_reference_counter(0),
	 A_type(E_type_group)
{
}

T_interpret_value::T_interpret_value(const std::string        & name)
	:A_reference_counter(0),
	 A_type(E_type_group),
	 A_name(name)
{
}

// Element
T_interpret_value::T_interpret_value(
							   const std::string        & name,
							   const T_attribute_value  & attribute_value)
	:A_reference_counter(0),
	 A_type(E_type_value),
	 A_name(name),
	 A_attribute_value(attribute_value)
	 
{
	if (attribute_value.transformed.get_type() == C_value::E_type_msg)
	{
		A_type = E_type_msg;
	}
}
#if 0
T_interpret_value::T_interpret_value(const T_interpret_value  & rhs)
	:A_reference_counter(0),                        // reference_counter specific behavior NE fonctionne PAS
	 A_type(rhs.A_type),
	 A_name(rhs.A_name),
	 A_attribute_value(rhs.A_attribute_value){
}

T_interpret_value &
T_interpret_value::operator=(const T_interpret_value  & rhs)
{
	// reference_counter specific behavior NE fonctionne PAS
	A_type = rhs.A_type;
	A_name = rhs.A_name;
	A_attribute_value = rhs.A_attribute_value;

	return  * this;
}

T_interpret_value::~T_interpret_value()
{
	if (A_reference_counter != 0)
	{
		M_FATAL_COMMENT("type=" << A_type << " " <<
						"name=" << A_name << " " <<
						"reference_counter=" << A_reference_counter << " != 0");
	}
}
#endif

//*****************************************************************************
// swap
//*****************************************************************************

void    swap(T_interpret_value  & lhs,
			 T_interpret_value  & rhs)
{
	swap(lhs.A_reference_counter, rhs.A_reference_counter);
	swap(lhs.A_type,              rhs.A_type);
	swap(lhs.A_name,              rhs.A_name);
	swap(lhs.A_attribute_value,   rhs.A_attribute_value);
}


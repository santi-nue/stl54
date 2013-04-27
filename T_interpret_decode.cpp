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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//*****************************************************************************
// Includes.
//*****************************************************************************

#include "precomp.h"
#include "T_interpret_decode.h"


//*****************************************************************************
// T_interpret_decode
//*****************************************************************************
void
T_interpret_decode::set_decode_function (string  decode_function)
{
	if (decode_function == "nil")
	{
		A_decode_function = "";
	}
	else
	{
		A_decode_function = decode_function;
	}
}

//*****************************************************************************
// C_interpret_decode_in_progress
//*****************************************************************************

C_interpret_decode_in_progress::C_interpret_decode_in_progress(
											T_interpret_decode  & interpret_decode)
	:A_interpret_decode(interpret_decode)
{
	A_interpret_decode.set_decode_in_progress(true);
}

C_interpret_decode_in_progress::~C_interpret_decode_in_progress()
{
	A_interpret_decode.set_decode_in_progress(false);
}


//*****************************************************************************
// C_interpret_decode_set_temporary
//*****************************************************************************

C_interpret_decode_set_temporary::C_interpret_decode_set_temporary(
									 T_interpret_decode  & interpret_decode,
                               const std::string         & decoder_function)
	:A_interpret_decode(interpret_decode),
	 A_decoder_function(decoder_function),
	 A_previous_decoder_function(interpret_decode.get_decode_function())
{
	if (A_decoder_function != "")
	{
		A_interpret_decode.set_decode_function(A_decoder_function);
	}
}

C_interpret_decode_set_temporary::~C_interpret_decode_set_temporary()
{
	if (A_decoder_function != "")
	{
		A_interpret_decode.set_decode_function(A_previous_decoder_function);
	}
}

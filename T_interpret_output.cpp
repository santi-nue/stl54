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
#include "T_interpret_output.h"


//*****************************************************************************
// C_interpret_output_level_move_temporary
//*****************************************************************************

C_interpret_output_level_move_temporary::C_interpret_output_level_move_temporary(
											T_interpret_output  & interpret_output,
											int                   output_level_offset)
	:A_interpret_output(interpret_output),
	 A_output_level_offset(output_level_offset)
{
	A_interpret_output.set_output_level(A_interpret_output.get_output_level() + A_output_level_offset);
}

C_interpret_output_level_move_temporary::~C_interpret_output_level_move_temporary()
{
	A_interpret_output.set_output_level(A_interpret_output.get_output_level() - A_output_level_offset);
}


/*
 * Copyright 2005-2012 Olivier Aveline <wsgd@free.fr>
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

#ifndef T_interpret_output_h
#define T_interpret_output_h

//*****************************************************************************
// Includes.
//*****************************************************************************

#include "byte_interpret_common.h"

//*****************************************************************************
// T_interpret_output
//*****************************************************************************

struct T_interpret_output : public CT_debug_object_counter<T_interpret_output>
{
    int     get_output_level  () const                { return  output_level; }
    void    set_output_level  (int   val)             { output_level = val; }

    bool    must_output_error () const          { return  output_level >= -1; }
    bool    must_output       () const          { return  output_level >=  0; }
    bool    must_output_debug () const          { return  output_level >=  1; }

    bool    must_NOT_output   () const            { return  ! must_output (); }

    void    incr_output_level ()                            { ++output_level; }
    void    decr_output_level ()                            { --output_level; }

    T_interpret_output ()
        :output_level (0)
    {
    }

private:
    int    output_level;
};

//*****************************************************************************
// Permits recursive call.
//*****************************************************************************

class C_interpret_output_level_move_temporary
{
public:
	C_interpret_output_level_move_temporary(T_interpret_output  & interpret_output,
											int                   output_level_offset);
	~C_interpret_output_level_move_temporary();

private:
	// Copy and assignment are forbidden
	C_interpret_output_level_move_temporary(const C_interpret_output_level_move_temporary  &);
	C_interpret_output_level_move_temporary & operator=(const C_interpret_output_level_move_temporary  &);

	T_interpret_output  & A_interpret_output;
	int                   A_output_level_offset;
};



#endif /* T_interpret_output_h */

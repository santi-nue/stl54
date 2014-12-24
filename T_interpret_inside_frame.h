/*
 * Copyright 2013-2014 Olivier Aveline <wsgd@free.fr>
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

#ifndef T_interpret_inside_frame_h
#define T_interpret_inside_frame_h

//*****************************************************************************
// Includes.
//*****************************************************************************

#include <string>
using namespace std;

#include "byte_interpret_common.h"
#include "T_frame_data.h"


//*****************************************************************************
// T_decode_stream_frame ******************************************************
//*****************************************************************************

struct T_decode_stream_frame
{
	unsigned char    decoded_data[10000];       // ICIOA hard coded magic number
	int              decoded_data_bit_size;
	T_frame_data     frame_data;

	T_decode_stream_frame()
		:decoded_data_bit_size(0),
		 frame_data(decoded_data, 0, 0)
	{
	}

	// Permits to reset frame_data & decoded_data_size when all data has been read.
	// To avoid decoded_data_bit_size grows indefinitely.
	void            synchronize();

	void            write_n_bytes(const T_byte *  ptr, int  n_bytes);
	void            write_1_byte(T_byte  byte);
	void            write_less_1_byte(T_byte  byte, short  n_bits);
};


//*****************************************************************************
// T_interpret_inside_frame
//*****************************************************************************

struct T_interpret_inside_frame : public CT_debug_object_counter<T_interpret_inside_frame>
{
	// Fatal if AP_decode_stream_frame is NULL
	T_decode_stream_frame &  get_decode_stream_frame();

	void                     set_decode_stream_frame(T_decode_stream_frame *  P_rhs);


    T_interpret_inside_frame ()
        :AP_decode_stream_frame (NULL)
    {
    }

private:
	T_decode_stream_frame *  AP_decode_stream_frame;
};


#endif /* T_interpret_inside_frame_h */

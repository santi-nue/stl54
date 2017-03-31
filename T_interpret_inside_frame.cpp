/*
 * Copyright 2013-2015 Olivier Aveline <wsgd@free.fr>
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
#include "T_interpret_inside_frame.h"
#include "T_frame_data_write.h"


//*****************************************************************************
// T_decode_stream_frame
//*****************************************************************************
void
T_decode_stream_frame::synchronize()
{
	if (frame_data.get_remaining_bits() == 0)
	{
		reset();
	}
#if 0
	else if (frame_data.get_remaining_bits() < decoded_data_bit_size)
	{
		// Some data has been read, and so can be removed
		const long  move_bit_offset = frame_data.get_bit_offset();
		const long  move_bit_size   = frame_data.get_remaining_bits();

		if ((move_bit_offset % 8) == 0)
		{
			const long  move_byte_offset = move_bit_offset / 8;
			const long  move_byte_size   = move_bit_size   / 8 + 1;

			// Move entire bytes
			memmove(&decoded_data[0], &decoded_data[move_byte_offset], move_byte_size);
			decoded_data_bit_size = move_bit_size;
			frame_data = T_frame_data(decoded_data, 0, decoded_data_bit_size);
		}
		else
		{
		}
	}
#endif
}

void
T_decode_stream_frame::reset()
{
	decoded_data_bit_size = 0;
	frame_data = T_frame_data(decoded_data, 0, 0);
}

void
T_decode_stream_frame::write_n_bytes(const T_byte *  ptr, int  n_bytes)
{
	T_frame_data_write  frame_data_write(decoded_data, 0, sizeof(decoded_data) * 8);
	frame_data_write.set_bit_offset(decoded_data_bit_size);

	frame_data_write.write_n_bytes(n_bytes, ptr);
	decoded_data_bit_size = frame_data_write.get_bit_offset();

	frame_data.n_bits_data_appended(n_bytes * 8);
}

void
T_decode_stream_frame::write_1_byte(T_byte  byte)
{
	T_frame_data_write  frame_data_write(decoded_data, 0, sizeof(decoded_data) * 8);
	frame_data_write.set_bit_offset(decoded_data_bit_size);

	frame_data_write.write_1_byte(byte);
	decoded_data_bit_size = frame_data_write.get_bit_offset();

	frame_data.n_bits_data_appended(8);
}

void
T_decode_stream_frame::write_less_1_byte(T_byte  data, short  data_bit_size)
{
	T_frame_data_write  frame_data_write(decoded_data, 0, sizeof(decoded_data) * 8);
	frame_data_write.set_bit_offset(decoded_data_bit_size);

	if (data_bit_size <= 8)
	{
		frame_data_write.write_less_1_byte(data, data_bit_size);		// ICIOA signed !
		decoded_data_bit_size = frame_data_write.get_bit_offset();

		frame_data.n_bits_data_appended(data_bit_size);
		return;
	}

	M_FATAL_COMMENT("write_less_1_byte  data_bit_size=" << data_bit_size << " must be <= 8");
}


//*****************************************************************************
// T_interpret_inside_frame
//*****************************************************************************
T_decode_stream_frame &
T_interpret_inside_frame::get_decode_stream_frame() const
{
	M_FATAL_IF_EQ(AP_decode_stream_frame, NULL);
	return  * AP_decode_stream_frame;
}

void
T_interpret_inside_frame::set_decode_stream_frame(T_decode_stream_frame *  P_rhs)
{
	AP_decode_stream_frame = P_rhs;
}

//*****************************************************************************
// C_decode_stream_frame_set_temporary_if_necessary
//*****************************************************************************

C_decode_stream_frame_set_temporary_if_necessary::C_decode_stream_frame_set_temporary_if_necessary(
													 T_interpret_inside_frame  & interpret_inside_frame,
		                                             T_decode_stream_frame     & decode_stream_frame)
	: A_interpret_inside_frame(interpret_inside_frame)
	, A_decode_stream_frame(decode_stream_frame)
	, A_value_set(false)
{
	// For now, we reject possible already set decode_stream_frame :
	// - generic.cpp does NOT need it
	// - UT does not need it (...interpret_bytes check there is no remaining data in it)S
	M_FATAL_IF_NE(interpret_inside_frame.get_P_decode_stream_frame(), NULL);

//	if (interpret_inside_frame.get_P_decode_stream_frame() == NULL)
	{
		interpret_inside_frame.set_decode_stream_frame(&A_decode_stream_frame);
		A_value_set = true;
	}
}

C_decode_stream_frame_set_temporary_if_necessary::~C_decode_stream_frame_set_temporary_if_necessary()
{
	if (A_value_set == true)
	{
		A_interpret_inside_frame.set_decode_stream_frame(NULL);
	}
}

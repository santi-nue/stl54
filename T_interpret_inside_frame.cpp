/*
 * Copyright 2013 Olivier Aveline <wsgd@free.fr>
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
		decoded_data_bit_size = 0;
		frame_data = T_frame_data(decoded_data, 0, 0);
	}
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
T_interpret_inside_frame::get_decode_stream_frame()
{
	M_FATAL_IF_EQ(AP_decode_stream_frame, NULL);
	return  * AP_decode_stream_frame;
}

void
T_interpret_inside_frame::set_decode_stream_frame(T_decode_stream_frame *  P_rhs)
{
	AP_decode_stream_frame = P_rhs;
}


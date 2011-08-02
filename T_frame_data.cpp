/*
 * Copyright 2005-2008 Olivier Aveline <wsgd@free.fr>
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
#include "T_frame_data.h"

using namespace std;

//*****************************************************************************
// T_frame_data
//*****************************************************************************

T_frame_data::T_frame_data()
    :A_P_bytes (NULL),
	 A_remaining_bits (0),
	 A_initial_P_bytes (NULL),
	 A_bit_offset_in_1st_byte (0),
	 A_initial_sizeof_bits (0),
	 A_initial_frame_starting_bit_offset (0)
{
}

//*****************************************************************************
// T_frame_data
//*****************************************************************************

T_frame_data::T_frame_data(const void       * P_bytes_void,
                           const short        bit_offset_in_1st_byte_param,
                           const long         sizeof_bits)
    :A_P_bytes ((const T_byte *)P_bytes_void),
	 A_remaining_bits (sizeof_bits),
	 A_initial_P_bytes (A_P_bytes),
	 A_bit_offset_in_1st_byte (bit_offset_in_1st_byte_param),
	 A_initial_sizeof_bits (sizeof_bits),
	 A_initial_frame_starting_bit_offset (0)
{
}

//*****************************************************************************
// get_remaining_entire_bytes
//*****************************************************************************
long
T_frame_data::get_remaining_entire_bytes() const
{
	return  (A_remaining_bits - (A_remaining_bits % 8)) / 8;
}

//*****************************************************************************
// set_offset
//*****************************************************************************
void
T_frame_data::set_offset(long    byte_offset, short   bit_offset_in_byte)
{
	set_bit_offset((byte_offset * 8) + bit_offset_in_byte);
}

//*****************************************************************************
// set_bit_offset
//*****************************************************************************
void
T_frame_data::set_bit_offset(long    bit_offset)
{
	M_FATAL_IF_LT(bit_offset, 0);
	M_FATAL_IF_GT(bit_offset, A_initial_sizeof_bits);

	A_remaining_bits = A_initial_sizeof_bits - bit_offset;
	long    physical_bit_offset = get_physical_bit_offset();
	A_P_bytes = A_initial_P_bytes + ((physical_bit_offset - (physical_bit_offset % 8)) / 8);
}

//*****************************************************************************
// can_move_forward
//*****************************************************************************
bool
T_frame_data::can_move_forward(long    byte_offset, short   bit_offset_in_byte) const
{
	return  can_move_bit_forward((byte_offset * 8) + bit_offset_in_byte);
}

//*****************************************************************************
// move_forward
//*****************************************************************************
void
T_frame_data::move_forward(long    byte_offset, short   bit_offset_in_byte)
{
	move_bit_forward((byte_offset * 8) + bit_offset_in_byte);
}

//*****************************************************************************
// can_move_bit_forward
//*****************************************************************************
bool
T_frame_data::can_move_bit_forward(long    bit_offset) const
{
	M_FATAL_IF_LT(bit_offset, 0);

	if (A_remaining_bits < bit_offset)
		return  false;

	return  true;
}

//*****************************************************************************
// move_bit_forward
//*****************************************************************************
void
T_frame_data::move_bit_forward(long    bit_offset)
{
	M_FATAL_IF_FALSE(can_move_bit_forward(bit_offset));

	set_bit_offset(A_initial_sizeof_bits - (A_remaining_bits - bit_offset));
}

//*****************************************************************************
// can_move
//*****************************************************************************
bool
T_frame_data::can_move(long    byte_offset, short   bit_offset_in_byte) const
{
	return  can_move_bit((byte_offset * 8) + bit_offset_in_byte);
}

//*****************************************************************************
// move
//*****************************************************************************
void
T_frame_data::move(long    byte_offset, short   bit_offset_in_byte)
{
	move_bit((byte_offset * 8) + bit_offset_in_byte);
}

//*****************************************************************************
// can_move_bit
//*****************************************************************************
bool
T_frame_data::can_move_bit(long    bit_offset) const
{
	if (A_remaining_bits < bit_offset)
		return  false;

	if (A_remaining_bits-bit_offset > A_initial_sizeof_bits)
		return  false;

	return  true;
}

//*****************************************************************************
// move_bit
//*****************************************************************************
void
T_frame_data::move_bit(long    bit_offset)
{
	M_FATAL_IF_FALSE(can_move_bit(bit_offset));

	set_bit_offset(A_initial_sizeof_bits - (A_remaining_bits - bit_offset));
}

//*****************************************************************************
// read_1_byte
//*****************************************************************************
T_byte    
T_frame_data::read_1_byte()
{
	M_FATAL_IF_LT(A_remaining_bits, 8);

	T_byte    byte = *A_P_bytes;

	if (is_physically_at_beginning_of_byte() != true)
	{
		unsigned char    byte_array[2];
		memcpy(byte_array, A_P_bytes, 2);

		const int   decalage_1 = get_physical_bit_offset() % 8;
		byte_array[0] <<= decalage_1;
		byte_array[1] >>= (8 - decalage_1);

		byte = byte_array[0] | byte_array[1];
	}

	++A_P_bytes;
	A_remaining_bits -= 8;

	return  byte;
}

//*****************************************************************************
// read_n_bytes
//*****************************************************************************
void
T_frame_data::read_n_bytes(short  n_bytes, void  * P_n_bytes_read_void)
{
	M_FATAL_IF_LT(n_bytes, 1);
	M_FATAL_IF_EQ(P_n_bytes_read_void, NULL);

	const short    n_bits = n_bytes * 8;

	M_FATAL_IF_LT(A_remaining_bits, n_bits);

	if (is_physically_at_beginning_of_byte() == true)
	{
		memcpy (P_n_bytes_read_void, A_P_bytes, n_bytes);
		A_P_bytes += n_bytes;
		A_remaining_bits -= n_bits;
	}
	else
	{
		const int   decalage_1 = get_physical_bit_offset() % 8;
		const int   decalage_2 = 8 - decalage_1;

		unsigned char  * P_n_bytes_read = (unsigned char*)P_n_bytes_read_void;

		for (int  idx = 0; idx < n_bytes; ++idx)
		{
			unsigned char  byte_1 = *A_P_bytes << decalage_1;
			++A_P_bytes;
			A_remaining_bits -= 8;

			unsigned char  byte_2 = *A_P_bytes >> decalage_2;

			*P_n_bytes_read = byte_1 | byte_2;
			++P_n_bytes_read;
		}
	}
}

//*****************************************************************************
// read_less_1_byte
//*****************************************************************************
T_byte
T_frame_data::read_less_1_byte(short  n_bits)
{
	M_FATAL_IF_LT(n_bits, 1);
	M_FATAL_IF_GT(n_bits, 7);

	M_FATAL_IF_LT(A_remaining_bits, n_bits);

	T_byte    byte = *A_P_bytes;

	const short    remaining_bits_in_byte = 8 - (get_physical_bit_offset() % 8);

	if (remaining_bits_in_byte == 8)
	{
		byte >>= 8 - n_bits;
		A_remaining_bits -= n_bits;
		return  byte;
	}

	if (remaining_bits_in_byte >= n_bits)
	{
		byte = bit_erase_left(byte, 8 - remaining_bits_in_byte);

		byte >>= remaining_bits_in_byte - n_bits;
		A_remaining_bits -= n_bits;
		if (remaining_bits_in_byte == n_bits)
		{
			++A_P_bytes;
		}
		return  byte;
	}

	/* bits to read are on 2 bytes */
	{
		unsigned char    byte_array[2];
		memcpy(byte_array, A_P_bytes, 2);

		byte_array[0] <<= 8 - remaining_bits_in_byte;
		byte_array[0] >>= 8 - remaining_bits_in_byte - (n_bits - remaining_bits_in_byte);

		byte_array[1] >>= 8 - (n_bits - remaining_bits_in_byte);

		byte = byte_array[0] | byte_array[1];

		++A_P_bytes;
		A_remaining_bits -= n_bits;

		return  byte;
	}
}

//*****************************************************************************
// read_n_bits
//*****************************************************************************
void
T_frame_data::read_n_bits(short  n_bits, void  * P_n_bytes_read_void, short  n_bytes_read)
{
	M_FATAL_IF_LT(n_bits, 1);
	M_FATAL_IF_EQ(P_n_bytes_read_void, NULL);
	M_FATAL_IF_LT(n_bytes_read, 1);

	// I suppose that n_bytes_read is enough AND not more.

	if ((n_bits % 8) == 0)
	{
		read_n_bytes(n_bits / 8, P_n_bytes_read_void);
		return;
	}

	unsigned char  * P_n_bytes_read = (unsigned char*)P_n_bytes_read_void;

	*P_n_bytes_read = read_less_1_byte(n_bits % 8);
	++P_n_bytes_read;

	if (n_bits > 8)
	{
		read_n_bytes(n_bits / 8, P_n_bytes_read);
	}
}

//*****************************************************************************
// set_initial_frame_starting_bit_offset
//*****************************************************************************
void
T_frame_data::set_initial_frame_starting_bit_offset(long    bit_offset)
{
	A_initial_frame_starting_bit_offset = bit_offset;
}

//*****************************************************************************
// set_bit_offset_into_initial_frame
//*****************************************************************************
void         
T_frame_data::set_bit_offset_into_initial_frame(long    bit_offset)
{
	set_bit_offset(bit_offset - A_initial_frame_starting_bit_offset);
}

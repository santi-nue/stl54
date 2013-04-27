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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//*****************************************************************************
// Includes.
//*****************************************************************************

#include "precomp.h"
#include "T_frame_data_write.h"

using namespace std;

//*****************************************************************************
// T_frame_data_write
//*****************************************************************************

T_frame_data_write::T_frame_data_write()
    :T_frame_data ()
{
}

//*****************************************************************************
// T_frame_data_write
//*****************************************************************************

T_frame_data_write::T_frame_data_write(void       * P_bytes_void,
                           const short        bit_offset_in_1st_byte,
                           const long         sizeof_bits)
    :T_frame_data (P_bytes_void, bit_offset_in_1st_byte, sizeof_bits)
{
}

//*****************************************************************************
// write_1_byte
//*****************************************************************************
void
T_frame_data_write::write_1_byte(T_byte  byte)
{
	M_FATAL_IF_LT(A_remaining_bits, 8);

	if (is_physically_at_beginning_of_byte() != true)
	{
		unsigned char    byte_array[2];
		memset(byte_array, 0xff, sizeof(byte_array));

		const int   decalage_1 = get_physical_bit_offset() % 8;

		T_byte  byte0 = byte >> decalage_1;
		byte_array[0] &= byte0;

		T_byte  byte1 = byte << (8 - decalage_1);
		byte_array[1] &= byte1;

		memcpy(const_cast<T_byte*>(A_P_bytes), byte_array, sizeof(byte_array));
	}
	else
	{
		*const_cast<T_byte*>(A_P_bytes) = byte;
	}

	++A_P_bytes;
	A_remaining_bits -= 8;
}

//*****************************************************************************
// 
//*****************************************************************************
void
T_frame_data_write::write_n_bytes(short  n_bytes, void  * P_n_bytes_write_void)
{
	M_FATAL_IF_LT(n_bytes, 1);
	M_FATAL_IF_EQ(P_n_bytes_write_void, NULL);

	const short    n_bits = n_bytes * 8;

	M_FATAL_IF_LT(A_remaining_bits, n_bits);

	if (is_physically_at_beginning_of_byte() == true)
	{
		memcpy (const_cast<T_byte*>(A_P_bytes), P_n_bytes_write_void, n_bytes);
		A_P_bytes += n_bytes;
		A_remaining_bits -= n_bits;
	}
	else
	{
		unsigned char  * P_n_bytes_write = (unsigned char*)P_n_bytes_write_void;
		for (int  idx = 0; idx < n_bytes; ++idx)
		{
			write_1_byte(P_n_bytes_write[idx]);
		}
	}
}

//*****************************************************************************
// 
//*****************************************************************************
void
T_frame_data_write::write_less_1_byte(T_byte  byte, short  n_bits)
{
	if (n_bits == 8)
	{
		write_1_byte(byte);
		return;
	}

	M_FATAL_IF_LT(n_bits, 1);
	M_FATAL_IF_GT(n_bits, 7);

	M_FATAL_IF_LT(A_remaining_bits, n_bits);

	const short    remaining_bits_in_byte = 8 - (get_physical_bit_offset() % 8);

	if (remaining_bits_in_byte == 8)
	{
		// move to left & right bits = 0
		byte <<= 8 - n_bits;

		T_byte    byte_in_place = bit_erase_left(*A_P_bytes, n_bits);

		byte_in_place |= byte;

		*const_cast<T_byte*>(A_P_bytes) = byte_in_place;
		A_remaining_bits -= n_bits;
		return;
	}

	if (remaining_bits_in_byte >= n_bits)
	{
		// move to left & right bits = 0
		byte <<= 8 - n_bits;
		// move to right place & left bits = 0
		byte >>= 8 - remaining_bits_in_byte;

		T_byte    byte_in_place_left  = bit_erase_right(*A_P_bytes, remaining_bits_in_byte);
		T_byte    byte_in_place_right = bit_erase_left (*A_P_bytes, 8 - (remaining_bits_in_byte - n_bits));

		*const_cast<T_byte*>(A_P_bytes) = byte_in_place_left | byte | byte_in_place_right;

		A_remaining_bits -= n_bits;
		if (remaining_bits_in_byte == n_bits)
		{
			++A_P_bytes;
		}
		return;
	}

	/* bits to write are on 2 bytes */
	{
		byte = bit_erase_left(byte, 8 - n_bits);

		T_byte         byte_to_set_1 = byte >> (n_bits - remaining_bits_in_byte);
		T_byte         byte_in_place_1 = bit_erase_right(*A_P_bytes, remaining_bits_in_byte);
		byte_to_set_1 |= byte_in_place_1;

		T_byte         byte_to_set_2 = byte << (8 - (n_bits - remaining_bits_in_byte));
		T_byte         byte_in_place_2 = bit_erase_left(*A_P_bytes, n_bits - remaining_bits_in_byte);
		byte_to_set_2 |= byte_in_place_2;

		unsigned char    byte_array[2];
		byte_array[0] = byte_to_set_1 | byte_in_place_1;
		byte_array[1] = byte_to_set_2 | byte_in_place_2;

		memcpy(const_cast<T_byte*>(A_P_bytes), byte_array, 2);

		++A_P_bytes;
		A_remaining_bits -= n_bits;

		return;
	}
}

//*****************************************************************************
// 
//*****************************************************************************
void
T_frame_data_write::write_n_bits(short  n_bits, void  * P_n_bytes_write_void, short  n_bytes_write)
{
	M_FATAL_IF_LT(n_bits, 1);
	M_FATAL_IF_EQ(P_n_bytes_write_void, NULL);
	M_FATAL_IF_LT(n_bytes_write, 1);

	// I suppose that n_bytes_write is enough AND not more.

	if ((n_bits % 8) == 0)
	{
		write_n_bytes(n_bits / 8, P_n_bytes_write_void);
		return;
	}

	unsigned char  * P_n_bytes_write = (unsigned char*)P_n_bytes_write_void;

	write_less_1_byte(*P_n_bytes_write, n_bits % 8);
	++P_n_bytes_write;

	if (n_bits > 8)
	{
		write_n_bytes(n_bits / 8, P_n_bytes_write);
	}
}

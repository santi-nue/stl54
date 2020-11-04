/*
 * Copyright 2005-2020 Olivier Aveline <wsgd@free.fr>
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

using namespace std;

#include "unitary_tests_tools.h"
#include "T_interpret_inside_frame.h"


//*****************************************************************************
// test_decode_stream_frame
//*****************************************************************************

M_TEST_FCT(test_decode_stream_frame)
{
    T_decode_stream_frame  decode_stream_frame;
    memset(decode_stream_frame.decoded_data, 0, sizeof(decode_stream_frame.decoded_data));

    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // Add 1 byte : size changed, frame_data position inchanged
    decode_stream_frame.write_1_byte(136);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 8);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 8);

    // Add 1 byte : size changed, frame_data position inchanged
    decode_stream_frame.write_1_byte(137);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 16);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 16);

    // Add 3 bytes : size changed, frame_data position inchanged
    T_byte  data1[] = { 138, 139, 140 };
    decode_stream_frame.write_n_bytes(data1, sizeof(data1));
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 40);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 40);

    // Read 1 byte : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 136);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 40);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 8);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 32);

    // Read 1 byte : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 137);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 40);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 16);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 24);

    // Add 5 bytes : size changed, frame_data position inchanged
    T_byte  data2[] = { 141, 142, 143, 144, 145 };
    decode_stream_frame.write_n_bytes(data2, sizeof(data2));
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 16);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 64);

    // Change nothing because not all is read into frame_data
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 16);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 64);

    // Read 4 bytes : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 138);
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 139);
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 140);
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 141);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 48);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 32);

    // Change nothing because not all is read into frame_data
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 48);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 32);

    // Read 4 bytes : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 142);
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 143);
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 144);
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 145);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // Add 1 byte : size changed, frame_data position inchanged
    decode_stream_frame.write_1_byte(146);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 88);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 80);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 8);

    // Read 1 byte : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 146);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 88);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 88);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // All has been read into frame_data, so data is reseted
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // Add 4 bits : size changed, frame_data position inchanged
    decode_stream_frame.write_less_1_byte(147 >> 4, 4);         // less significant bits are take
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 4);

    // Change nothing because not all is read into frame_data
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 4);

    // Add 4 bits : size changed, frame_data position inchanged
    decode_stream_frame.write_less_1_byte(147, 4);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 8);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 8);

    // Change nothing because not all is read into frame_data
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 8);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 8);

    // Read 1 byte : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_1_byte(), 147);
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 8);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 8);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // All has been read into frame_data, so data is reseted
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // Add 4 bits : size changed, frame_data position inchanged
    decode_stream_frame.write_less_1_byte(148 >> 4, 4);         // less significant bits are take
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 4);

    // Read 2 bits : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_less_1_byte(2), 2);    // 2 = (148 >> 4) >> 2
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 2);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 2);

    // Change nothing because not all is read into frame_data
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 2);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 2);

    // Read 2 bits : size inchanged, frame_data position changed
    M_TEST_EQ(decode_stream_frame.frame_data.read_less_1_byte(2), 1);    // 1 = (148 >> 4) & b11
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 4);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);

    // All has been read into frame_data, so data is reseted
    decode_stream_frame.synchronize();
    M_TEST_EQ(decode_stream_frame.decoded_data_bit_size, 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_bit_offset(), 0);
    M_TEST_EQ(decode_stream_frame.frame_data.get_remaining_bits(), 0);
}

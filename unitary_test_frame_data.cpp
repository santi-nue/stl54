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
#include "T_frame_data_write.h"


bool    read_data(T_frame_data& in_out_frame_data,
    void* P_value_param,
    const char* TYPE_NAME,
    const size_t          TYPE_BIT_SIZE,
    const char* TYPE_IMPL_STR,
    const size_t          TYPE_IMPL_BIT_SIZE,
    const bool            must_invert_bytes,
    const bool            is_signed_integer);


//*****************************************************************************
// test_frame_data
//*****************************************************************************

M_TEST_FCT(test_frame_data)
{
    const unsigned char  bytes[] = { 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 0xff, 0xaa, 0x37 };
    const long           initial_sizeof_bytes = sizeof(bytes)/sizeof(bytes[0]);

    T_frame_data    frame_data(bytes, 0, initial_sizeof_bytes * 8);

    // Position and movements.

    M_TEST_EQ(frame_data.get_bit_offset(), 0);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), true);
    M_TEST_EQ(frame_data.get_P_bytes(), bytes);
    M_TEST_EQ(frame_data.get_remaining_bits(), initial_sizeof_bytes*8);
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    M_TEST_EQ(frame_data.can_move_1_byte_forward(), true);
    frame_data.move_1_byte_forward();
    M_TEST_EQ(frame_data.get_bit_offset(), 8);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), true);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes-1)*8);
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    M_TEST_EQ(frame_data.can_move_forward(2, 1), true);
    frame_data.move_forward(2, 1);
    M_TEST_EQ(frame_data.get_bit_offset(), 25);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    M_TEST_EQ(frame_data.can_move_bit_forward(3), true);
    frame_data.move_bit_forward(3);
    M_TEST_EQ(frame_data.get_bit_offset(), 28);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    M_TEST_EQ(frame_data.can_move_bit(-3), true);
    frame_data.move_bit(-3);
    M_TEST_EQ(frame_data.get_bit_offset(), 25);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    M_TEST_EQ(frame_data.can_move(-2, -1), true);
    frame_data.move(-2, -1);
    M_TEST_EQ(frame_data.get_bit_offset(), 8);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), true);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes-1)*8);
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    frame_data.set_offset(3, 7);
    M_TEST_EQ(frame_data.get_bit_offset(), 31);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

    frame_data.set_bit_offset(37);
    M_TEST_EQ(frame_data.get_bit_offset(), 37);
    M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
    M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
    M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-4-1);
    M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);


#define M_TEST_READ_EQ(READ_OPER,VALUE)                          \
{                                                                \
    const long  value_read = READ_OPER;                          \
    M_TEST_EQ(value_read, VALUE);                                \
}

    // Read bytes on byte position.
    frame_data.set_bit_offset(0);

    M_TEST_EQ(frame_data.read_1_byte(), 0);

    {
        unsigned char  bytes_read[4];
        const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

        frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

        T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 1);
        M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
    }
    {
        unsigned char  bytes_read[5];
        const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

        frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

        T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 1);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 2);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 3);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 4);
        M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
    }
    {
        unsigned char  bytes_read[3];
        const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

        frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

        T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0xff);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0xaa);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0x37);
        M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
    }
    M_TEST_EQ(frame_data.can_move_bit_forward(1), false);


    // Read byte on bit position.
    frame_data.set_bit_offset(0);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 32);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

    M_TEST_READ_EQ(frame_data.read_1_byte(), 64);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 128);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 192+1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 63);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 128+64+32+ 0+8+0+2+0);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 128+ 0+ 0+ 0+8+4+0+1);

    M_TEST_EQ(frame_data.can_move_bit_forward(2), true);
    M_TEST_EQ(frame_data.can_move_bit_forward(3), false);

    frame_data.move_bit(-6);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0x37);


    // Read bytes on bit position.
    frame_data.set_bit_offset(0);

    frame_data.move_bit_forward(1);
    M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

    {
        unsigned char  bytes_read[4];
        const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

        frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

        T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 2);
        M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
    }
    {
        unsigned char  bytes_read[5];
        const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

        frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

        T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 2);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 4);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 6);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 8+1);
        M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
    }
    {
        unsigned char  bytes_read[2];
        const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

        frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

        T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0xff);
        M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0x54);
        M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
    }
    M_TEST_EQ(frame_data.can_move_bit_forward(7),  true);
    M_TEST_EQ(frame_data.can_move_bit_forward(8), false);

    // Read bits.
    frame_data.set_bit_offset(0);

    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 4);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 1);

    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 1);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 64+32);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 64+ 0+ 0+8+4+2+1);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 64+32+16+8+4+0+1);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(7),  0+32+ 0+8+0+0+0);
    M_TEST_READ_EQ(frame_data.read_less_1_byte(6),    32+16+0+4+2+1);
    M_TEST_EQ(frame_data.can_move_bit_forward(1), false);
}

//*****************************************************************************
// test_frame_data_write
//*****************************************************************************

M_TEST_FCT(test_frame_data_write)
{
    unsigned char  bytes[200];
    long           initial_sizeof_bytes = sizeof(bytes)/sizeof(bytes[0]);

    unsigned char  data[] = { 0xe0, 0xd7, 0xc3, 0x20, 0x31, 0x40, 0x51, 0x72, 0x03, 0x04, 0xff, 0xaa, 0x37 };
    long           initial_sizeof_data = sizeof(data)/sizeof(data[0]);

    // write_1_byte
    {
        memset(bytes, 0, sizeof(bytes));
        T_frame_data_write    frame_data_write(bytes, 0, initial_sizeof_data * 8);
        T_frame_data          frame_data      (bytes, 0, initial_sizeof_data * 8);

        for (int  idx = 0; idx < initial_sizeof_data; ++idx)
        {
            frame_data_write.write_1_byte(data[idx]);
            M_TEST_EQ(frame_data.read_1_byte(), data[idx]);
        }

        M_TEST_EQ(frame_data_write.get_remaining_bits(), 0);
        M_TEST_EQ(frame_data.get_remaining_bits(), 0);
    }

    // write_less_1_byte
    {
        memset(bytes, 0, sizeof(bytes));
        T_frame_data_write    frame_data_write(bytes, 0, initial_sizeof_data * 8);
        T_frame_data          frame_data      (bytes, 0, initial_sizeof_data * 8);

        for (int  idx = 0; idx < initial_sizeof_data; ++idx)
        {
            const short  n_bits = 1 + (idx % 7);
            frame_data_write.write_less_1_byte(data[idx], n_bits);
            M_TEST_EQ(frame_data.read_less_1_byte(n_bits), bit_erase_left(data[idx], 8-n_bits));
        }

        M_TEST_EQ(frame_data_write.get_bit_offset(), frame_data.get_bit_offset());
    }

    // A tester : write_n_bytes
    // A tester : write_n_bits
}

//*****************************************************************************
// test_read_data
//*****************************************************************************

M_TEST_FCT(test_read_data)
{
    bool            is_signed_integer = false;

    // Float.
    is_signed_integer = false;
    {
        double          frame_data_value = -23655869876.44864;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        double          read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "float64", 64,
                            "double", 64,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }
    {
        float           frame_data_value = -23655869876.44864e32;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        float           read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "float32", 32,
                            "float", 32,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }

    // Signed byte integers.
    is_signed_integer = true;
    {
        signed long long      frame_data_value = -123456789012345LL;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        signed long long    read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "int64", 64,
                            "signed long long", 64,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }
    {
        signed int      frame_data_value = -23456789;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        signed int    read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "int32", 32,
                            "signed int", 32,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }
    {
        signed int      frame_data_value = -3456789;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);  // intel only

        signed int      read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "int24", 24,
                            "signed int", 32,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }
    {
        signed short    frame_data_value = -26789;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        signed short    read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "int16", 16,
                            "signed short", 16,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }
    {
        signed char     frame_data_value = -119;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        signed char     read_value = 0;
        M_TEST_EQ(read_data(frame_data,
                            &read_value,
                            "int8", 8,
                            "signed char", 8,
                            false,
                            is_signed_integer), true);
        M_TEST_EQ(read_value, frame_data_value);
    }

    // Signed bit integers.
    {
        signed char     frame_data_value = -128;
        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        {
            signed char     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int7", 7,
                                "signed char", 8,
                                false,
                                is_signed_integer), true);
            M_TEST_EQ(read_value, -64);
        }
        frame_data.set_bit_offset(0);
        {
            signed char     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int4", 4,
                                "signed char", 8,
                                false,
                                is_signed_integer), true);
            M_TEST_EQ(read_value, -8);
        }
        frame_data.set_bit_offset(0);
        {
            signed char     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int2", 2,
                                "signed char", 8,
                                false,
                                is_signed_integer), true);
            M_TEST_EQ(read_value, -2);
        }
        frame_data.set_bit_offset(0);
        {
            signed short     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int7", 7,
                                "signed short", 16,
                                false,
                                is_signed_integer), true);
            M_TEST_EQ(read_value, -64);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int4", 4,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);
            M_TEST_EQ(read_value, -8);
        }
        frame_data.set_bit_offset(0);
        {
            signed long long     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int2", 2,
                                "signed long long", 64,
                                false,
                                is_signed_integer), true);
            M_TEST_EQ(read_value, -2);
        }
    }
    {
        signed int      frame_data_value = -128;    // 0xffffff80 -> 80 ff ff ff

        T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int4", 4,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)000 -> 7 +1
            M_TEST_EQ(read_value, -8);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int7", 7,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)000000 -> 63 +1
            M_TEST_EQ(read_value, -64);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int8", 8,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)0000000 -> 127 +1
            M_TEST_EQ(read_value, -128);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int9", 9,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1) 00000001 -> 254 +1
            M_TEST_EQ(read_value, -255);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int10", 10,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)1 00000010 -> 253 +1
            M_TEST_EQ(read_value, -254);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int11", 11,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)11 00000100 -> 251 +1
            M_TEST_EQ(read_value, -252);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int15", 15,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)111111 01000000 -> 191 +1
            M_TEST_EQ(read_value, -192);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int16", 16,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)1111111 10000000 -> 127 +1
            M_TEST_EQ(read_value, -128);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int17", 17,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)1111111 00000001 00000001 -> 65278 +1
            M_TEST_EQ(read_value, -65279);
        }
        frame_data.set_bit_offset(0);
        {
            signed int     read_value = 0;
            M_TEST_EQ(read_data(frame_data,
                                &read_value,
                                "int31", 31,
                                "signed int", 32,
                                false,
                                is_signed_integer), true);    // bits read = (1)111111 1..1 01111111 01000000 ->  +1
            M_TEST_EQ(read_value, -32960);
        }
    }
}

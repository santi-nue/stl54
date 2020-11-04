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

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"

#include "C_value.h"
#include "T_attribute_value.h"
#include "T_interpret_value.h"



//*****************************************************************************
// test_bitfield_C
// Not a test, I just want to verify the position of the bits.
// a) From less significant bit to most significant bit
// b) take care of byte order
// Note that there is NO standard about this.
//*****************************************************************************

M_TEST_FCT(test_bitfield_C)
{
    struct {
          unsigned field1  :4;
          unsigned field2  :3;
          unsigned field3  :1;
          unsigned field4  :6;
          unsigned field5  :2;
          unsigned field6  :8;
          unsigned field7  :8;
    } full_of_fields;

    memset(&full_of_fields, 0, sizeof(full_of_fields));
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field1 = 3;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field2 = 4;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field3 = 1;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field4 = 3;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field5 = 3;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field6 = 85;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    full_of_fields.field7 = 255;
    cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

    cout << dec;
}

//*****************************************************************************
// test_size
// Not a test.
//*****************************************************************************
C_value  string_count_cpp_to_fdesc(const string::size_type  cpp_count);

M_TEST_FCT(test_size)
{
#define M_OUT_SIZEOF(PARAM)    \
    cout << "sizeof(" << #PARAM << ") = " << sizeof(PARAM) << endl

    M_OUT_SIZEOF(void*);
    M_OUT_SIZEOF(int);
    M_OUT_SIZEOF(long);
    M_OUT_SIZEOF(long long);
    M_OUT_SIZEOF(double);
    M_OUT_SIZEOF(size_t);
    M_OUT_SIZEOF(string::size_type);
    M_OUT_SIZEOF(string);
    M_OUT_SIZEOF(C_value);
    M_OUT_SIZEOF(T_attribute_value);
    M_OUT_SIZEOF(T_interpret_value);

#define M_OUT_VAL(PARAM)    \
    cout << "value(" << #PARAM << ") = " << PARAM << endl

    M_OUT_VAL(string::npos);
    M_OUT_VAL(string_count_cpp_to_fdesc(string::npos));
}

//*****************************************************************************
// test_cast
// Not a test.
//*****************************************************************************

M_TEST_FCT_IGNORE(test_cast)
{
#define M_OUT_VAL(PARAM)    \
    cout << "value(" << #PARAM << ") = " << PARAM << endl

    unsigned short  valu  = 40000;
    short           vals  = (int)valu;
    unsigned short  valu2 = (unsigned int)vals;

    M_OUT_VAL(valu);
    M_OUT_VAL(vals);
    M_OUT_VAL(valu2);
}

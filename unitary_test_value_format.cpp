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
#include "byte_interpret_parse.h"


//*****************************************************************************
// test_value_format
//*****************************************************************************

M_TEST_FCT(test_value_format)
{
    C_value            value;
    string             format;
    
#define M_TEST_OK(VALUE,RESULT)                                        \
    value = C_value(VALUE);                                            \
    M_TEST_EQ(value.as_string(), RESULT)

    M_TEST_OK( 123456789101112,     "123456789101112");
    M_TEST_OK( 1234567891011121314, "1234567891011121314");
    M_TEST_OK( -4427611715, "-4427611715");


#undef  M_TEST_OK

#define M_TEST_OK(VALUE,FORMAT,RESULT)                                 \
    value = C_value(VALUE);                                            \
    format = FORMAT;                                                   \
    promote_printf_string_to_64bits(format);                           \
    value.format(format);                                              \
    M_TEST_EQ(value.as_string(), RESULT)


    M_TEST_OK( 123, "hex", "0x7b");
    M_TEST_OK( 123, "oct", "0173");
    M_TEST_OK( 123, "bin", "b1111011");

//	M_TEST_OK(-123, "hex", "-0x7b");     // donne 0xffffff85
//	M_TEST_OK(-123, "oct", "-0173");     // donne 037777777605
//	M_TEST_OK(-123, "bin", "-b1111011"); // donne b1111111111111111111111111111111111111111111111111111111110000101

    M_TEST_OK( 123, "augmentation = %d%% (non consolide)", "augmentation = 123% (non consolide)");
    M_TEST_OK( 123, "augmentation = %s%% (non consolide)", "augmentation = 123% (non consolide)");

    M_TEST_OK( 123456789101112    , "%d", "123456789101112");
    M_TEST_OK( 1234567891011121314, "%d", "1234567891011121314");
    M_TEST_OK( -4427611715        , "%d", "-4427611715");

#undef  M_TEST_OK
}

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
#include "byte_interpret_parse.h"

//*****************************************************************************
// strtod accept/reject hexadecimal as float
//*****************************************************************************

#if WIRESHARK_VERSION_NUMBER < 20600
#if defined WIN32
#define STRTOD_REJECT_HEXADECIMAL_AS_FLOAT
#endif
#endif


//*****************************************************************************
// test_get_number
//*****************************************************************************

M_TEST_FCT(test_get_number)
{
    long long    int_number = 0;
    double       flt_number = 0.0;

#define M_TEST_NOT_OK(STR)                                                  \
    M_TEST_EQ(get_number(STR, int_number), false);                          \
    M_TEST_EQ(get_number(STR, flt_number), false)

#define M_TEST_INT(STR,VAL_INT)                                             \
    M_TEST_EQ(get_number(STR, int_number), true);                           \
    M_TEST_EQ(int_number, VAL_INT);                                         \
    M_TEST_EQ(get_number(STR, flt_number), false)

#define M_TEST_FLT(STR,VAL_FLT)                                             \
    M_TEST_EQ(get_number(STR, int_number), false);                          \
    M_TEST_EQ(get_number(STR, flt_number), true);                           \
    M_TEST_EQ(flt_number, VAL_FLT)

#define M_TEST_INT_FLT(STR,VAL_INT,VAL_FLT)                                 \
    M_TEST_EQ(get_number(STR, int_number), true);                           \
    M_TEST_EQ(int_number, VAL_INT);                                         \
    M_TEST_EQ(get_number(STR, flt_number), true);                           \
    M_TEST_EQ(flt_number, VAL_FLT)

    M_TEST_NOT_OK("aze");
    M_TEST_NOT_OK("12g");
    M_TEST_NOT_OK("g12");
    M_TEST_NOT_OK("12a");
    M_TEST_NOT_OK("a12");

    M_TEST_FLT(  " 32.56",   32.56);
    M_TEST_FLT(" -132.56", -132.56);
    M_TEST_FLT( " 3.56e3", 3.56e3);
    M_TEST_FLT( " 3.56e+3", 3.56e3);
    M_TEST_FLT(" -3.56e-13", -3.56e-13);

    M_TEST_INT_FLT(  "32",   32,   32.0);
    M_TEST_INT_FLT("-132", -132, -132.0);
    M_TEST_INT_FLT(" 32",   32,   32.0);
    M_TEST_INT_FLT(" -132", -132, -132.0);

#if defined STRTOD_REJECT_HEXADECIMAL_AS_FLOAT
    // VC++ strtod reject hexadecimal
    M_TEST_INT(  "0xa32",  0xa32);
    M_TEST_INT( " 0Xa32",  0xa32);
    M_TEST_INT("-0Xfa32",  -0xfa32);
#else
    // gcc strtod accept hexadecimal
    M_TEST_INT_FLT(  "0xa32",  0xa32, 2610.0);
    M_TEST_INT_FLT( " 0Xa32",  0xa32, 2610.0);
    M_TEST_INT_FLT("-0Xfa32",  -0xfa32, -64050.0);
#endif

    M_TEST_INT(   "032",   032);
    M_TEST_INT( "-0777", -0777);

#undef  M_TEST_NOT_OK
}

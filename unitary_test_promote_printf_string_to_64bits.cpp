/*
 * Copyright 2021 Olivier Aveline <wsgd@free.fr>
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
// test_promote_printf_string_to_64bits
//*****************************************************************************

M_TEST_FCT(test_promote_printf_string_to_64bits)
{
    string             format;
    
#define M_TEST_OK(FORMAT,RESULT)                                       \
    format = FORMAT;                                                   \
    promote_printf_string_to_64bits(format);                           \
    M_TEST_EQ(format, RESULT)

    // No modifications
    M_TEST_OK("12345 abcde", "12345 abcde");
    M_TEST_OK("12345 %%s e", "12345 %%s e");
    M_TEST_OK("%s 45 %scde", "%s 45 %scde");
    M_TEST_OK("%c %s %p %n", "%c %s %p %n");
    M_TEST_OK("%e %E %f %F %g %G %a %A", "%e %E %f %F %g %G %a %A");  // floats

    // Modifications for integers
#ifdef WIN32
    M_TEST_OK("%d %i %u %o %x %X", "%I64d %I64i %I64u %I64o %I64x %I64X");
    M_TEST_OK("%03d %+02i", "%03I64d %+02I64i");
#else
    M_TEST_OK("%d %i %u %o %x %X", "%lld %lli %llu %llo %llx %llX");
    M_TEST_OK("%03d %+02i", "%03lld %+02lli");
#endif

    // Errors
    M_TEST_CATCH_EXCEPTION(format = "12345 %"; promote_printf_string_to_64bits(format), C_byte_interpret_exception);
    M_TEST_CATCH_EXCEPTION(format = "%hd"; promote_printf_string_to_64bits(format), C_byte_interpret_exception);
    M_TEST_CATCH_EXCEPTION(format = "%Ld"; promote_printf_string_to_64bits(format), C_byte_interpret_exception);
    M_TEST_CATCH_EXCEPTION(format = "%ld"; promote_printf_string_to_64bits(format), C_byte_interpret_exception);
    M_TEST_CATCH_EXCEPTION(format = "%lld"; promote_printf_string_to_64bits(format), C_byte_interpret_exception);


#undef  M_TEST_OK
}

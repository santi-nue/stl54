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
// test_get_before_separator_after
//*****************************************************************************

M_TEST_FCT(test_get_before_separator_after)
{
    string    str_left;
    string    str_right;

#define M_TEST_NOT_OK(STR,SEP)                                              \
    M_TEST_EQ(get_before_separator_after(STR, SEP, str_left, str_right), E_rc_not_found)

#define M_TEST_OK(STR,SEP,LEFT,RIGHT)                                       \
    M_TEST_EQ(get_before_separator_after(STR, SEP, str_left, str_right), E_rc_ok);  \
    M_TEST_EQ(str_left, LEFT);                                              \
    M_TEST_EQ(str_right, RIGHT)

    M_TEST_NOT_OK("aqsdfghjk", ':');

    M_TEST_OK("aqsdfghjk", 'd', "aqs", "fghjk");
    M_TEST_OK("aqs:dfg:hjk", ':', "aqs", "dfg:hjk");    // takes the 1st one
    M_TEST_OK(":aqs:dfg:hjk", ':', "", "aqs:dfg:hjk");  // no warranty : accept empty left

    M_TEST_OK("aqs::fghjk", "::", "aqs", "fghjk");

#undef  M_TEST_NOT_OK
#undef  M_TEST_OK
}

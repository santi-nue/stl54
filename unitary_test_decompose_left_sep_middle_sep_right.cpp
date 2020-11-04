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
// test_decompose_left_sep_middle_sep_right
//*****************************************************************************

M_TEST_FCT(test_decompose_left_sep_middle_sep_right)
{
    string    str_left;
    string    str_middle;
    string    str_right;

#define M_TEST_NOT_OK(STR,SEP_LEFT,SEP_RIGHT)                                              \
    M_TEST_EQ(decompose_left_sep_middle_sep_right(STR, SEP_LEFT, SEP_RIGHT, str_left, str_middle, str_right), E_rc_not_found)

#define M_TEST_OK(STR,SEP_LEFT,SEP_RIGHT,LEFT,MIDDLE,RIGHT)                                                       \
    M_TEST_EQ(decompose_left_sep_middle_sep_right(STR, SEP_LEFT, SEP_RIGHT, str_left, str_middle, str_right), E_rc_ok);  \
    M_TEST_EQ(str_left, LEFT);                                                                                    \
    M_TEST_EQ(str_middle, MIDDLE);                                                                                \
    M_TEST_EQ(str_right, RIGHT)

    M_TEST_NOT_OK("aqsdfghjk", '(', ')');
    M_TEST_NOT_OK("aqsdfghj)", '(', ')');
    M_TEST_NOT_OK("aq(dfghjk", '(', ')');

    M_TEST_OK("aq(dfgh)k", '(', ')', "aq", "dfgh", "k");
    M_TEST_OK("(aqdfghk)", '(', ')', "", "aqdfghk", "");
    M_TEST_OK("asaqs(dfghj)", '(', ')', "asaqs", "dfghj", "");
    M_TEST_OK("{aqs}{dfghj}", '{', '}', "", "aqs", "{dfghj}");
    M_TEST_OK("a[a][f[gh]k]", '[', ']', "a", "a", "[f[gh]k]");
    M_TEST_OK("[]a[a][f[ghk", '[', ']', "", "", "a[a][f[ghk");

    M_TEST_NOT_OK("a[a[f[_gh]k]du", '[', ']');             // missing 1 ]
    M_TEST_OK("a[a[f[]g[h]k]]du", '[', ']', "a", "a[f[]g[h]k]", "du");


#undef  M_TEST_NOT_OK
#undef  M_TEST_OK
}

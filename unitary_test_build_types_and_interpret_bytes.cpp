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
#include "unitary_tests_ut_interpret_bytes.h"

#include "byte_interpret.h"


//*****************************************************************************
// test_build_types_and_interpret_bytes
//*****************************************************************************

M_TEST_FCT(test_build_types_and_interpret_bytes)
{
    T_byte_vector    msg_1_frame;
    {
        const char  * msg_1_str = "6c000b000000000000000000";
        string_hexa_to_frame(msg_1_str, msg_1_frame);
    }

    const T_byte  * P_bytes = &msg_1_frame[0];
    size_t          sizeof_bytes = msg_1_frame.size();
    istringstream   iss("uint8[12]  uint8;");
    ostringstream   oss;

    bool  result = build_types_and_interpret_bytes (
        P_bytes,
        sizeof_bytes,
        iss,
        oss,
        oss);

    M_TEST_EQ(result, true);
    M_TEST_EQ(sizeof_bytes, 0);
}

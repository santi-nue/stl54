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

#include "T_interpret_data.h"


//*****************************************************************************
// test_interpret_simple_decoder_base64
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_decoder_base64)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;

    interpret_data.set_big_endian();


    // 56334E6E 5A413D3D --> V3Nn ZA== --> 0x 57 73 67  64 --> "Wsgd" (if it is a string)
    M_TEST_SIMPLE("56334E6E5A413D3D", "decoder    decoder_base64;"
                                      " uint8     val1 ;"
                                      "uint24     val2 ;"
                                      "decoder    nil;",
                                      "val1 = 87" K_eol "val2 = 7563108" K_eol);
    // Idem, but previous presentation seems better
    // Here, we can think there are 2 different encoded buffers
    M_TEST_SIMPLE("56334E6E5A413D3D", " uint8{decoder=decoder_base64}     val1 ;"
                                      "uint24{decoder=decoder_base64}     val2 ;",
                                      "val1 = 87" K_eol "val2 = 7563108" K_eol);

    // Specifying a size for string works only if encoded size is the same size
    M_TEST_SIMPLE("56334E6E5A413D3D", "stringBase64(4)  val  ;", "val = Wsgd" K_eol);
    // Does not work, the 5th byte is missing
//	M_TEST_SIMPLE("56334E6E5A413D3D", "stringBase64(5)  val  ;", "val = Wsgd" K_eol);
    M_TEST_SIMPLE("56334E6E5A41413D", "stringBase64(5)  val  ;", "val = Wsgd" K_eol);
    // Does not work, the 6th byte is missing
//	M_TEST_SIMPLE("56334E6E5A41413D", "stringBase64(6)  val  ;", "val = Wsgd" K_eol);
    M_TEST_SIMPLE("56334E6E5A414141", "stringBase64(6)  val  ;", "val = Wsgd" K_eol);
    M_TEST_SIMPLE("56334E6E5A414156", "stringBase64(6)  val  ;", "val = Wsgd" K_eol);
    // It works, even without zero end of string, because there is no more data
    M_TEST_SIMPLE("56334E6E5A413D3D", "stringBase64     val  ;", "val = Wsgd" K_eol);
    // In normal case, zero end of string is needed
    // 56334E6E 5A41413D --> V3Nn ZAA= --> 0x 57 73 67  64 00 --> "wsgd" (if it is a string)
    M_TEST_SIMPLE("56334E6E5A41413D", "stringBase64     val  ;", "val = Wsgd" K_eol);


    // Test complement not preceded by A (zero)
    // 56334E6E 57673D3D --> V3Nn Wg== --> 0x 57 73 67  5A --> "WsgZ" (if it is a string)
    M_TEST_SIMPLE("56334E6E57673D3D", "stringBase64(4)  val  ;", "val = WsgZ" K_eol);
    // 56334E6E 5A466F3D --> V3Nn ZFo= --> 0x 57 73 67  64 5A --> "WsgdZ" (if it is a string)
    M_TEST_SIMPLE("56334E6E5A466F3D", "stringBase64(5)  val  ;", "val = WsgdZ" K_eol);


    // Complement = not at the end.
    // Not a valid syntax.
    // Behavior NOT guaranteed.
    // 56334E6E 57673D3D 56334E6E 5A466F3D --> V3Nn Wg== V3Nn ZFo= --> 0x 57 73 67  5A  57 73 67  64 5A--> "WsgZWsgZ"
    M_TEST_SIMPLE("56334E6E57673D3D56334E6E5A466F3D", "stringBase64(9)  val  ;", "val = WsgZWsgdZ" K_eol);
    M_TEST_SIMPLE("56334E6E57673D3D56334E6E5A466F3D", "stringBase64     val  ;", "val = WsgZWsgdZ" K_eol);

    // Idem previous + space, \t, \n, \r
    // These characters are ignored.
    // 562033094E0A6E 570D673D3D 56334E6E 5A466F3D --> V 3\tN\nn W\rg== V3Nn ZFo= --> 0x 57 73 67  5A  57 73 67  64 5A--> "WsgZWsgZ"
    M_TEST_SIMPLE("562033094E0A6E570D673D3D56334E6E5A466F3D", "stringBase64     val  ;", "val = WsgZWsgdZ" K_eol);
}

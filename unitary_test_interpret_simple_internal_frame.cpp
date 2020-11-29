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
// test_interpret_simple_internal_frame
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_internal_frame)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",    // decode_invert_4_bytes
                 type_definitions);

    T_interpret_data      interpret_data;
    interpret_data.set_big_endian();

    //*************************************************************************
    // internal_frame
    //*************************************************************************

    // No real data, data is read from internal_frame.
    M_TEST_SIMPLE("",
                  "call frame_append_hexa_data (internal_frame, \" e2 3f 6a 77 \") ; "
                  "uint32{d=hex}  val ;",
                  "val = 0xe23f6a77 (3795806839)" K_eol);

    // First, data is read from internal_frame, then from real data.
    M_TEST_SIMPLE("de",
                  "call frame_append_hexa_data (internal_frame, \" e2 3f 6a 77 \") ; "
                  "uint16{d=hex}  val1 ; "
                  "uint4{d=hex}   val2 ; "
                  "uint4{d=hex}   val3 ; "
                  "uint8{d=hex}   val4 ; "
                  "uint8{d=hex}   val5 ; ",
                  "val1 = 0xe23f (57919)" K_eol
                  "val2 = 0x6 (6)" K_eol
                  "val3 = 0xa (10)" K_eol
                  "val4 = 0x77 (119)" K_eol
                  "val5 = 0xde (222)" K_eol);

    //*************************************************************************
    // Decoder which append more data than asked.
    // Data goes into internal_frame.
    //*************************************************************************
    interpret_data.set_decode_function("decode_invert_4_bytes");  // read 4 bytes and invert them

    C_ut_interpret_bytes_no_decode_guard  uibndg;

    // Bytes inverted by decoder
    M_TEST_SIMPLE("776a3fe2",    // e23f6a77
                  "uint8{d=hex}  val1 ; uint8{d=hex}  val2 ; uint8{d=hex}  val3 ; uint8{d=hex}  val4 ;",
                  "val1 = 0xe2 (226)" K_eol
                  "val2 = 0x3f (63)" K_eol
                  "val3 = 0x6a (106)" K_eol
                  "val4 = 0x77 (119)" K_eol);

    // Bytes inverted by decoder
    M_TEST_SIMPLE("776a3fe2", "uint32  val ;", "val = 3795806839" K_eol);
    M_TEST_SIMPLE("776a3fe2", " int32  val ;", "val = -499160457" K_eol);

    // val2 is over first 4 bytes and second 4 bytes
    M_TEST_SIMPLE("776a3fe2 01020304",    // e23f6a77 04030201
                  "uint24{d=hex}  val1 ; uint16{d=hex}  val2 ; uint8{d=hex}  val3 ; uint16{d=hex}  val4 ;",
                  "val1 = 0xe23f6a (14827370)" K_eol
                  "val2 = 0x7704 (30468)" K_eol
                  "val3 = 0x3 (3)" K_eol
                  "val4 = 0x201 (513)" K_eol);

    // bits
    M_TEST_SIMPLE("776a3fe2 01020304",    // e23f6a77 04030201
        "uint16{d=hex}  val1 ; uint4{d=hex}  val2; uint8{d=hex}  val3; uint16{d=hex}  val4 ; uint8{d=hex}  val5 ; uint12{d=hex}  val6 ;",
                  "val1 = 0xe23f (57919)" K_eol
                  "val2 = 0x6 (6)" K_eol
                  "val3 = 0xa7 (167)" K_eol
                  "val4 = 0x7040 (28736)" K_eol
                  "val5 = 0x30 (48)" K_eol
                  "val6 = 0x201 (513)" K_eol);

    // string fixed size
    M_TEST_SIMPLE("4249472d 52455155 45535453",    // 2d474942 55514552 53545345    -GIB UQER STSE
                  "string(12)  val ;",
                  "val = -GIBUQERSTSE" K_eol);  // BIG-REQUESTS if not inverted
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "string(3)  val1; string(3)  val2; string(3)  val3; string(3)  val4; ",
                  "val1 = -GI" K_eol
                  "val2 = BUQ" K_eol
                  "val3 = ERS" K_eol
                  "val4 = TSE" K_eol);
    // string fixed size at bit position
    M_TEST_SIMPLE("94 74 d4 b2    55 14 55 25    34 45 35 25      56 34 12 5e",  // b 4249472d 52455155 45535453 e ...
                  "uint4  begin; string(12)  val ; uint4  end; uint24{d=hex}  end2;",
                  "begin = 11" K_eol
                  "val = -GIBUQERSTSE" K_eol
                  "end = 14" K_eol
                  "end2 = 0x123456 (1193046)" K_eol);

    // string unknow size
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "string  val ;",
                  "val = -GIBUQERSTSE" K_eol);  // BIG-REQUESTS if not inverted
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "string(3)  val1; string  val2; ",
                  "val1 = -GI" K_eol
                  "val2 = BUQERSTSE" K_eol);
    M_TEST_SIMPLE("4249472d 52005155 45535453",
                  "string(3)  val1; string  val2; string  val3; ",
                  "val1 = -GI" K_eol
                  "val2 = BUQ" K_eol
                  "val3 = RSTSE" K_eol);
    // string unknow size at bit position
    M_TEST_SIMPLE("94 74 d4 b2  55 14 55 25  30 45 35 25  56 34 12 0e",  // b2 d4 74 94   25 55 14 55   25 35 45 30  0e 12 34 56
                  "uint4  begin; string  val ; uint4  end; uint24{d=hex}  end2;",
                  "begin = 11" K_eol
                  "val = -GIBUQERSTS" K_eol
                  "end = 14" K_eol
                  "end2 = 0x123456 (1193046)" K_eol);
    M_TEST_SIMPLE("94 74 d4 b2  55 04 50 25  30 45 35 25  56 34 12 0e",  // b2 d4 74 94   25 50 04 55   25 35 45 30  0e 12 34 56
                  "uint4  begin; string  val1 ; string  val2 ; uint4  end; uint24{d=hex}  end2;",
                  "begin = 11" K_eol
                  "val1 = -GIBU" K_eol
                  "val2 = ERSTS" K_eol
                  "end = 14" K_eol
                  "end2 = 0x123456 (1193046)" K_eol);

    // string unknow size at bit position, with :
    // - zero (end of string) not at the end of a 4 bytes block
    // - decoder changed just after the block containing the zero
    //   --> following block must NOT be read by initial decoder
    M_TEST_SIMPLE("94 74 d4 b2  55 04 50 25  30 45 35 25",  // b2 d4 74 94   25 50 04 55  decoder nil  30 45 35 25
                  "uint4  begin; string  val1 ; uint4  end; uint8{d=hex}  end2; decoder nil; uint8{d=hex}[4]  not_inverted;",
                  "begin = 11" K_eol
                  "val1 = -GIBU" K_eol
                  "end = 4" K_eol
                  "end2 = 0x55 (85)" K_eol
                  "not_inverted[0] = 0x30 (48)" K_eol
                  "not_inverted[1] = 0x45 (69)" K_eol
                  "not_inverted[2] = 0x35 (53)" K_eol
                  "not_inverted[3] = 0x25 (37)" K_eol);
    M_TEST_EQ(interpret_data.get_decode_function(), "");
    interpret_data.set_decode_function("decode_invert_4_bytes");  // must set again the decoder (set to nil in previous test)

    // raw fixed size
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "raw(12)  val ;",
                  "val = " K_eol
                  "00000000 : 2d 47 49 42 55 51 45 52 53 54 53 45              - -GIBUQERSTSE    " K_eol);
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "raw(3)  val1; raw(3)  val2; raw(3)  val3; raw(3)  val4; ",
                  "val1 = " K_eol
                  "00000000 : 2d 47 49                                         - -GI             " K_eol
                  "val2 = " K_eol
                  "00000000 : 42 55 51                                         - BUQ             " K_eol
                  "val3 = " K_eol
                  "00000000 : 45 52 53                                         - ERS             " K_eol
                  "val4 = " K_eol
                  "00000000 : 54 53 45                                         - TSE             " K_eol);

    // raw unknow size
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "raw(*)  val ;",
                  "val = " K_eol
                  "00000000 : 2d 47 49 42 55 51 45 52 53 54 53 45              - -GIBUQERSTSE    " K_eol);
    M_TEST_SIMPLE("4249472d 52455155 45535453",
                  "raw(3)  val1; raw(*)  val2;",
                  "val1 = " K_eol
                  "00000000 : 2d 47 49                                         - -GI             " K_eol
                  "val2 = " K_eol
                  "00000000 : 42 55 51 45 52 53 54 53 45                       - BUQERSTSE       " K_eol);
}

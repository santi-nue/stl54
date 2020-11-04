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
#include "C_perf_chrono.h"

#include "C_value.h"
#include "T_type_definitions.h"
#include "T_frame_data.h"
#include "T_frame_data_write.h"
#include "T_attribute_value.h"
#include "T_interpret_data.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_builder.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret.h"
#include "byte_interpret_build_types_read_token.h"
#include "T_expression.h"

//*****************************************************************************
// float print format :
// - 8.8275e+020		WIN32 before 20600
// - 8.8275e+20			others
//*****************************************************************************

#if WIRESHARK_VERSION_NUMBER < 20600
#if defined WIN32
#define FLOAT_PRINT_FORMAT_EXP_XXX
#endif
#endif

//*****************************************************************************
// test_interpret_simple
//*****************************************************************************

M_TEST_FCT(test_interpret_simple)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;


    // global and pinfo must be instantiated at the beginning
    interpret_data.global_variable_group_begin();
    M_TEST_SIMPLE("", " var uint32  msg_counter = 0 ;", "msg_counter = 0" K_eol);
    interpret_data.global_variable_group_end();

    interpret_data.pinfo_variable_group_begin();
    M_TEST_SIMPLE("", " var uint32  msg_counter = 0 ;", "msg_counter = 0" K_eol);
    M_TEST_SIMPLE("", " var uint32  dstport = 133 ;"  , "dstport = 133" K_eol);
    M_TEST_SIMPLE("", " var uint32  srcport = 5000 ;" , "srcport = 5000" K_eol);
    interpret_data.read_variable_group_begin("fd");
    M_TEST_SIMPLE("", " var string  pipo = \"str\" ;" , "pipo = str" K_eol);
    interpret_data.read_variable_group_end();
    interpret_data.pinfo_variable_group_end();


    interpret_data.set_big_endian();
    M_TEST_EQ(interpret_data.is_little_endian(), false);

    // char test
M_TEST_ERROR_ALREADY_KNOWN__OPEN(3535660, "char are displayed as integer")
    M_TEST_SIMPLE("41", "char  char ;", "char = A" K_eol);
    M_TEST_SIMPLE("81", "char  char ;", "char = ü" K_eol);
    M_TEST_SIMPLE("81", "uchar  char ;", "char = ü" K_eol);
}
    M_TEST_SIMPLE("41", "char  char ;", "char = 65" K_eol);
    M_TEST_SIMPLE("81", "char  char ;", "char = -127" K_eol);
    M_TEST_SIMPLE("81", "uchar  char ;", "char = 129" K_eol);

    // 2010/08/16
    // no decode time = 0 ms
    //    decode time = 2 ms
    // 2010/10/16       0 ms
    M_TEST_SIMPLE("ad", "uint8  uint8 ;", "uint8 = 173" K_eol);
    M_TEST_SIMPLE("ad", " int8   int8 ;", "int8 = -83" K_eol);

    M_TEST_SIMPLE("c23f", "uint16  val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("c23f", " int16  val ;", "val = -15809" K_eol);

    M_TEST_SIMPLE("c23fde", "uint24  val ;", "val = 12730334" K_eol);
    M_TEST_SIMPLE("c23fde", " int24  val ;", "val = -4046882" K_eol);

    M_TEST_SIMPLE("e23f6a77", "uint32  val ;", "val = 3795806839" K_eol);
    M_TEST_SIMPLE("e23f6a77", " int32  val ;", "val = -499160457" K_eol);

    M_TEST_SIMPLE("e23f6a77cb", "uint40  val ;", "val = 971726550987" K_eol);
    M_TEST_SIMPLE("e23f6a77cb", " int40  val ;", "val = -127785076789" K_eol);

    M_TEST_SIMPLE("e23f6a77cbf3", "uint48  val ;", "val = 248761997052915" K_eol);
    M_TEST_SIMPLE("e23f6a77cbf3", " int48  val ;", "val = -32712979657741" K_eol);

    // 2010/08/16
    // no decode time =  0 ms
    //    decode time = 18 ms
    // 2010/09/25       14 ms
    // 2010/10/14       13 ms
    // 2010/10/16        0 ms
    M_TEST_SIMPLE("e23f6a77cbf367a9", "int64  val ;", "val = -2143877834849687639" K_eol);

    // ATTENTION : NOT really checked
#ifdef FLOAT_PRINT_FORMAT_EXP_XXX
    M_TEST_SIMPLE("e23f6a77", "float32  val ;", "val = -8.8275e+020" K_eol);
#else
    M_TEST_SIMPLE("e23f6a77", "float32  val ;", "val = -8.8275e+20" K_eol);
#endif
    M_TEST_SIMPLE("e23f6a77cbf367a9", "float64  val ;", "val = -1.80912e+165" K_eol);


    // local byte order
    M_TEST_SIMPLE("776a3fe2", "uint32{byte_order=little_endian}  val ;", "val = 3795806839" K_eol);
    M_TEST_SIMPLE("776a3fe2", " int32{byte_order=little_endian}  val ;", "val = -499160457" K_eol);

    M_TEST_EQ(interpret_data.is_little_endian(), false);


    interpret_data.set_little_endian();
    M_TEST_EQ(interpret_data.is_little_endian(), true);

    // integer
    M_TEST_SIMPLE("ad", "uint8  uint8 ;", "uint8 = 173" K_eol);
    M_TEST_SIMPLE("ad", " int8   int8 ;", "int8 = -83" K_eol);

    M_TEST_SIMPLE("3fc2", "uint16  val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("3fc2", " int16  val ;", "val = -15809" K_eol);

    M_TEST_SIMPLE("de3fc2", "uint24  val ;", "val = 12730334" K_eol);
    M_TEST_SIMPLE("de3fc2", " int24  val ;", "val = -4046882" K_eol);

    M_TEST_SIMPLE("776a3fe2", "uint32  val ;", "val = 3795806839" K_eol);
    M_TEST_SIMPLE("776a3fe2", " int32  val ;", "val = -499160457" K_eol);

    M_TEST_SIMPLE("cb776a3fe2", "uint40  val ;", "val = 971726550987" K_eol);
    M_TEST_SIMPLE("cb776a3fe2", " int40  val ;", "val = -127785076789" K_eol);

    M_TEST_SIMPLE("f3cb776a3fe2", "uint48  val ;", "val = 248761997052915" K_eol);
    M_TEST_SIMPLE("f3cb776a3fe2", " int48  val ;", "val = -32712979657741" K_eol);

    M_TEST_SIMPLE("a967f3cb776a3fe2", "int64  val ;", "val = -2143877834849687639" K_eol);

    // float
    // ATTENTION : NOT really checked
#ifdef FLOAT_PRINT_FORMAT_EXP_XXX
    M_TEST_SIMPLE("776a3fe2", "float32  val ;", "val = -8.8275e+020" K_eol);
#else
    M_TEST_SIMPLE("776a3fe2", "float32  val ;", "val = -8.8275e+20" K_eol);
#endif
    M_TEST_SIMPLE("a967f3cb776a3fe2", "float64  val ;", "val = -1.80912e+165" K_eol);

    // string
    M_TEST_SIMPLE("4249472d5245515545535453", "string(12)  val ;", "val = BIG-REQUESTS" K_eol);
    M_TEST_SIMPLE("424947005245515545535453", "string(12)  val ;", "val = BIG" K_eol);
    M_TEST_SIMPLE(""                        , "string(0)   val ;", "val = " K_eol);
    M_TEST_SIMPLE("42494700"                , "string      val ;", "val = BIG" K_eol);
    M_TEST_SIMPLE("424947005245515545535453",
                  "string      val ; string(8)  val2 ;",
                  "val = BIG" K_eol "val2 = REQUESTS" K_eol);
    M_TEST_SIMPLE("424947005245515545535400",
                  "string()    val ; string()  val2 ;",
                  "val = BIG" K_eol "val2 = REQUEST" K_eol);
    M_TEST_SIMPLE("424947005245515545535400",
                  "string()    val ; string(0)  val2 ; string()  val3 ;",
                  "val = BIG" K_eol "val2 = " K_eol "val3 = REQUEST" K_eol);
    // string at bit position
    M_TEST_SIMPLE("b4249472d5245515545535453e",
                  "uint4  begin; string(12)  val ; uint4  end;",
                  "begin = 11" K_eol
                  "val = BIG-REQUESTS" K_eol
                  "end = 14" K_eol);

    // raw
    M_TEST_SIMPLE("4249472d5245515545535453",
                  "raw(12)  val ;",
                  "val = " K_eol
                  "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    " K_eol);
    M_TEST_SIMPLE("4249472d5245515545535453",
                  "raw(*)  val ;",
                  "val = " K_eol
                  "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    " K_eol);
    M_TEST_SIMPLE("4249472d5245515545535453",
                  "raw(2)  val ; raw(*)  val2 ;",
                  "val = " K_eol
                  "00000000 : 42 49                                            - BI              " K_eol
                  "val2 = " K_eol
                  "00000000 : 47 2d 52 45 51 55 45 53 54 53                    - G-REQUESTS      " K_eol);
    M_TEST_SIMPLE("4249472d5245515545535453",
                  "raw(0)  val ; raw(*)  val2 ;",
                  "val2 = " K_eol
                  "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    " K_eol);

    // int2 ...
    M_TEST_SIMPLE("f3", "uint4  val ; uint4  val2 ;",
                  "val = 15" K_eol
                  "val2 = 3" K_eol);
    M_TEST_SIMPLE("f3", "int4  val ; int4  val2 ;",
                  "val = -1" K_eol
                  "val2 = 3" K_eol);
    M_TEST_SIMPLE("f3cb",
                  "uint1  val ; uint2  val2; uint3  val3; uint4  val4; uint5  val5; uint1  val6;",
                  "val = 1" K_eol
                  "val2 = 3" K_eol
                  "val3 = 4" K_eol
                  "val4 = 15" K_eol
                  "val5 = 5" K_eol
                  "val6 = 1" K_eol);
//	M_TEST_SIMPLE("f3cb776a3fe2", "uint48  val ;", "val = 248761997052915");

    // enum
    M_TEST_SIMPLE("de3fc2", "T_enum24                         val ;", "val = val12730334 (12730334)" K_eol);
    M_TEST_SIMPLE("c23fde", "T_enum24{byte_order=big_endian}  val ;", "val = val12730334 (12730334)" K_eol);

    M_TEST_SIMPLE("f3cb",
                  "T_enum1  val ; T_enum2  val2; T_enum3  val3; T_enum4  val4; T_enum5  val5; T_enum1  val6;",
                  "val = value1 (1)" K_eol
                  "val2 = enu3 (3)" K_eol
                  "val3 = val4 (4)" K_eol
                  "val4 = Val15 (15)" K_eol
                  "val5 = val5 (5)" K_eol
                  "val6 = value1 (1)" K_eol);

    // ICIOA ...

    // transform quantum/offset
    M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}    val ;", "val = 99467 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.0}  val ;", "val = -31605 (-15809)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.1}  val ;", "val = -31604.9 (-15809)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{q=1+1:o=10.1+1*3}  val ;", "val = -31604.9 (-15809)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{q=konst::int+0:o=konst::flt+15.7}  val ;", "val = -31604.9 (-15809)" K_eol);
    M_TEST_SIMPLE("02"  , "T_enum8{q=2:o=11}    val ;", "val = Val15 (2)" K_eol);

    // transform expression
    M_TEST_SIMPLE("3fc2", "uint16{tei=2*this+13}    val ;", "val = 99467 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{tef=2*this+13.0}  val ;", "val = -31605 (-15809)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{tef=2*this+13.1}  val ;", "val = -31604.9 (-15809)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{tef=addition(2*this,13.1)}  val ;", "val = -31604.9 (-15809)" K_eol);
    M_TEST_SIMPLE("02"  , "T_enum8{tei=2*this+11}    val ;", "val = Val15 (2)" K_eol);

    // decoder
    M_TEST_SIMPLE("3fc2", "uint16{decoder=nil}  val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{decoder=nil}  val ;", "val = -15809" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{decoder=decode_stream_nothing}  val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{decoder=decode_stream_nothing}  val ;", "val = -15809" K_eol);
    // 0x3f | 0xC2 = 0xFF
    // 0x3f & 0xC2 = 0x02
    // 0x02ff = 767
    M_TEST_SIMPLE("3fc2", "uint16{decoder=decode_stream_test16}  val ;", "val = 767" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{decoder=decode_stream_test16}  val ;", "val = 767" K_eol);

    // stringUtf8, stringUtf16Le & stringUtf16Be using decoder
    // string original = abcde\r\n12345\r\n&é"'(-è_çà)=\r\n€\r\n^$ù*,;:!\r\n¨£%µ?./§
    // Characters not directly transformable to ascii are replaced by "."
    M_TEST_SIMPLE("61626364650D0A31323334350D0A26C3A92227282DC3A85FC3A7C3A0293D0D0AE282AC0D0A5E24C3B92A2C3B3A210D0AC2A8C2A325C2B53F2E2FC2A7",
        " stringUtf8     val ;",
        "val = abcde\r\n12345\r\n&.\"'(-._..)=\r\n.\r\n^$.*,;:!\r\n..%.?./." K_eol);
    // pb avec E900 -> é ou Ú (wsgd 2.6) ??? 
    M_TEST_SIMPLE("610062006300640065000D000A00310032003300340035000D000A0026002200270028002D00E8005F00E700E00029003D000D000A00AC200D000A005E002400F9002A002C003B003A0021000D000A00A800A3002500B5003F002E002F00A700",
        " stringUtf16Le  val ;",
        "val = abcde\r\n12345\r\n&\"'(-è_çà)=\r\n.\r\n^$ù*,;:!\r\n¨£%µ?./§" K_eol);
    M_TEST_SIMPLE("00610062006300640065000D000A00310032003300340035000D000A0026002200270028002D00E8005F00E700E00029003D000D000A20AC000D000A005E002400F9002A002C003B003A0021000D000A00A800A3002500B5003F002E002F00A7",
        " stringUtf16Be  val ;",
        "val = abcde\r\n12345\r\n&\"'(-è_çà)=\r\n.\r\n^$ù*,;:!\r\n¨£%µ?./§" K_eol);

    // stringUtf8 using decoder with fixed asked size.
    // Shows that, usingdecoder, the specified size is not the input raw size and the input raw size is variable.
    // There is currently no way to specify input raw size with a decoder
    M_TEST_SIMPLE("6126C3A9", " stringUtf8(3)     val ;", "val = a&." K_eol);
    M_TEST_SIMPLE("612600"  , " stringUtf8(3)     val ;", "val = a&" K_eol);
    M_TEST_SIMPLE("610000"  , " stringUtf8(3)     val ;", "val = a" K_eol);
    

    // min/max ok
    M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}{min=99000}    val ;", "val = 99467 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.6}{max=-30000} val ;", "val = -31604.4 (-15809)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}{min=99000:max=99467}       val ;", "val = 99467 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.6}{min=-31604.5:max=-30000} val ;", "val = -31604.4 (-15809)" K_eol);

    // min/max ERROR
    M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}{min=99468}    val ;", "val = 99467 (49727)\tERROR is < to 99468" K_eol);

    // display
    M_TEST_SIMPLE("3fc2", "uint16{d=hex}  val ;", "val = 0xc23f (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=oct}  val ;", "val = 0141077 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=bin}  val ;", "val = b1100001000111111 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%s}   val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%dmeters}   val ;", "val = 49727meters (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%d meters}   val ;", "val = 49727 meters (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%.1f}   val ;", "val = 49727.0 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%07d} val ;", "val = 0049727 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%7d}  val ;", "val =   49727 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%03d} val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{d=%3d}  val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("de3fc2", "T_enum24{d=%d and ...}  val ;", "val = 12730334 and ... (12730334)" K_eol);

    // display expression
    M_TEST_SIMPLE("3fc2", "uint16{de=\"123\"}  val ;", "val = 123 (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{de=\"HelloWorld\"}  val ;", "val = HelloWorld (49727)" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{de=\"hello world\"}  val ;", "val = hello world (49727)" K_eol);  // space NOT ok
    M_TEST_SIMPLE("3fc2", "uint16{de=to_string(this)}  val ;", "val = 49727" K_eol);
    M_TEST_SIMPLE("3fc2", "uint16{de=print_ip(this)}  val ;", "val = 0.0.194.63 (49727)" K_eol);
    M_TEST_SIMPLE("776a3fe2", "uint32{de=print_ip(this)}  val ;", "val = 226.63.106.119 (3795806839)" K_eol);
    M_TEST_SIMPLE("de3fc2", "T_enum24{de=\"hello world\"}        val ;", "val = hello world (12730334)" K_eol);
    M_TEST_SIMPLE("de3fc2", "T_enum24{de=print(\"%s 0x%x %d\", this, this, this)}  val ;", "val = val12730334 0xc23fde 12730334 (12730334)" K_eol);

    // struct
    M_TEST_SIMPLE("3fc2", "struct { uint8  val1; uint8 val2; }  str ;", "str.val1 = 63" K_eol "str.val2 = 194" K_eol);
    M_TEST_SIMPLE("776a3fc2", "struct { uint16  val1; uint16 val2; }  str ;", "str.val1 = 27255" K_eol "str.val2 = 49727" K_eol);
    M_TEST_SIMPLE("776a3fc2", "T_struct_16_16                         str ;", "str.val1 = 27255" K_eol "str.val2 = 49727" K_eol);

    // struct with decoder
    M_TEST_SIMPLE("776a3fc2", "T_struct_16_16{decoder=decode_stream_nothing} str ;", "str.val1 = 27255" K_eol "str.val2 = 49727" K_eol);
    // 0x77 | 0x6a = 0x7F
    // 0x77 & 0x6a = 0x62
    // 0x627F = 25215
    M_TEST_SIMPLE("776a3fc2", "T_struct_16_16{decoder=decode_stream_test16}  str ;", "str.val1 = 25215" K_eol "str.val2 = 767" K_eol);

    // switch inline
    M_TEST_SIMPLE("", "switch(T_enum1::value1) { "
                          "  case 0                : var int8  val =  0;"
                          "  case T_enum3::value1  : var int8  val = 10;"
                          "  case T_enum2::enu2    : var int8  val = 30;"
                          "  default               : var int8  val = -100;"
                          "}", "val = 10" K_eol);
    M_TEST_SIMPLE("", "switch(7) { "
                          "  case 0 : var int8  val =  0;"
                          "  case 1 : var int8  val = 10;"
                          "  case 2 : var int8  val = 30;"
                          "  default : var int8  val = -100;"
                          "}", "val = -100" K_eol);
    M_TEST_SIMPLE("", "switch_expr { "
                          "  case (0 != 0) : var int8  val =  0;"
                          "  case (1 >= 2) : var int8  val = 10;"
                          "  case (2 == 2) : var int8  val = 30;"
                          "  default : var int8  val = -100;"
                          "}", "val = 30" K_eol);
    M_TEST_SIMPLE("", "switch_expr { "
                          "  case (pinfo.dstport <  100) : var int16  val = 100;"
                          "  case (pinfo.dstport >= 200) : var int16  val = 200;"
                          "  case (pinfo.dstport == 133) : var int16  val = 133;"
                          "  case (pinfo.fd.pipo == \"tsr\")      : var int16  val =   1;"
                          "  case (pinfo.fd.pipo == \"rst\")      : var int16  val =   2;"
                          "  case (pinfo.fd.pipo == \"str\")      : var int16  val =   3;"
                          "  default : var int8  val = -100;"
                          "}", "val = 133" K_eol);
    M_TEST_SIMPLE("", "switch_expr { "
                          "  case (pinfo.dstport == 133 && pinfo.fd.pipo == \"rst\") : var int16  val = 1332;"
                          "  case (pinfo.dstport <  100 && pinfo.fd.pipo == \"str\") : var int16  val = 1003;"
                          "  case (pinfo.dstport <  100 && pinfo.fd.pipo == \"tsr\") : var int16  val = 1001;"
                          "  case (pinfo.dstport >= 200 && pinfo.fd.pipo == \"rst\") : var int16  val = 2002;"
                          "  case (pinfo.dstport == 133 && pinfo.fd.pipo == \"str\") : var int16  val = 1333;"
                          "  case (pinfo.dstport <  100) : var int16  val = 100;"
                          "  case (pinfo.dstport >= 200) : var int16  val = 200;"
                          "  case (pinfo.dstport == 133) : var int16  val = 133;"
                          "  default : var int8  val = -100;"
                          "}", "val = 1333" K_eol);

    // switch
    M_TEST_SIMPLE("", "T_switch(3)   \"\";", "val = enu3" K_eol);
    M_TEST_SIMPLE("", "T_switch_expr \"\";", "val = str" K_eol);

    // array
    M_TEST_SIMPLE("4249472d5245515545535453",
                  "uint8[12]  val ;",
                  "val[0] = 66" K_eol
                  "val[1] = 73" K_eol
                  "val[2] = 71" K_eol
                  "val[3] = 45" K_eol
                  "val[4] = 82" K_eol
                  "val[5] = 69" K_eol
                  "val[6] = 81" K_eol
                  "val[7] = 85" K_eol
                  "val[8] = 69" K_eol
                  "val[9] = 83" K_eol
                  "val[10] = 84" K_eol
                  "val[11] = 83" K_eol);
    {
        C_ut_interpret_bytes_no_decode_guard  uibndg;    // [*] & [+] rejected with decode
        M_TEST_SIMPLE("4249472d5245515545535453",
                      "uint8[*]  val ;",
                      "val[0] = 66" K_eol
                      "val[1] = 73" K_eol
                      "val[2] = 71" K_eol
                      "val[3] = 45" K_eol
                      "val[4] = 82" K_eol
                      "val[5] = 69" K_eol
                      "val[6] = 81" K_eol
                      "val[7] = 85" K_eol
                      "val[8] = 69" K_eol
                      "val[9] = 83" K_eol
                      "val[10] = 84" K_eol
                      "val[11] = 83" K_eol);
        M_TEST_SIMPLE("4249472d5245515545535453",
                      "uint8[+]  val ;",
                      "val[0] = 66" K_eol
                      "val[1] = 73" K_eol
                      "val[2] = 71" K_eol
                      "val[3] = 45" K_eol
                      "val[4] = 82" K_eol
                      "val[5] = 69" K_eol
                      "val[6] = 81" K_eol
                      "val[7] = 85" K_eol
                      "val[8] = 69" K_eol
                      "val[9] = 83" K_eol
                      "val[10] = 84" K_eol
                      "val[11] = 83" K_eol);
        M_TEST_SIMPLE("",
                      "uint8[*]  val ;",
                      "");
        M_TEST_SIMPLE("42",
                      "uint8[+]  val ;",
                      "val[0] = 66" K_eol);
    }
    M_TEST_SIMPLE("4249472d5245515545535453" "4249472d5245515545535453",
                  "uint8[24]  val ;",
                  "val[0] = 66" K_eol
                  "val[1] = 73" K_eol
                  "val[2] = 71" K_eol
                  "val[3] = 45" K_eol
                  "val[4] = 82" K_eol
                  "val[5] = 69" K_eol
                  "val[6] = 81" K_eol
                  "val[7] = 85" K_eol
                  "val[8] = 69" K_eol
                  "val[9] = 83" K_eol
                  "val[10] = 84" K_eol
                  "val[11] = 83" K_eol
                  "val[12] = 66" K_eol
                  "val[13] = 73" K_eol
                  "val[14] = 71" K_eol
                  "val[15] = 45" K_eol
                  "val[16] = 82" K_eol
                  "val[17] = 69" K_eol
                  "val[18] = 81" K_eol
                  "val[19] = 85" K_eol
                  "val[20] = 69" K_eol
                  "val[21] = 83" K_eol
                  "val[22] = 84" K_eol
                  "val[23] = 83" K_eol);
    // Loop which show an acumulative problem
    // 2010/08/16
    // no decode time =    6,    7,    7,    7,    8,     8,     9,     8,     9,    10 ms
    //    decode time = 2078, 3546, 5110, 6715, 8062, 10077, 12275, 14037, 16014, 17724 ms  -> +1600 ms at each test
    // 2010/09/18
    // removed "more" (a string always empty) inside T_attribute_value
    // no decode time =    6,    7,    7,    7,    8,     9,     8,     9,     9,     9 ms
    //    decode time = 2233, 3551, 5134, 6608, 8164,  9784, 11657, 13382, 15225, 16900 ms
    // -> win ~600 ms on final time (17724)
    // --> memory load is a problem
    // --> vector<T_interpret_value> into T_interpret_read_values is a problem
    // --> to not really erase a variable into T_interpret_read_values::sup_read_variable  is a problem
    // --> to not erase local variable of a function is a bigger problem
    // 2010/09/18
    // removed 48 bytes inside T_attribute_value & T_interpret_value
    // -> win between ~600 ms and zero on final time
    // --> memory load is NOT a problem ?
    // 2010/09/24
    // erase done into T_interpret_read_values::sup_read_variable
    // -> win ~3000 ms on final time (13550)
    // 2010/09/25
    // erase all data created during a function (behavior modification)
    // -> win ~11690 ms on final time (1860)
    //    acumulative problem still here
    // 2010/10/09
    // modif T_interpret_read_values::get_P_attribute_value_of_read_variable
    // -> win ~190 ms on final time (1670)
    // 2010/10/13  1622 ms
    // 2010/10/13  1654 ms  avec le nouveau code de compute_expression !!!
    // 2010/10/16
    // Into get_complex_value,
    //  move get_value_of_read_variable in 4th place (instead 1st)
    // -> final time = 358 - 395 ms
    //    no acumulative problem visible
    // 2010/10/29 265 - 280 ms  T_expression var set call
    // 2010/10/29 141 - 156 ms  T_expression condition return
    // 2010/11/10 148 - 170 ms  T_expression pre_compute value & operation

    for (int  idx_tst = 0; idx_tst < 10; ++idx_tst)
    {
        M_TRACE_WARNING("msg.size=" << interpret_data.DEBUG_get_msg().size());
        ut_interpret_bytes(type_definitions,
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453"
                       "4249472d5245515545535453" "4249472d5245515545535453",
                       "uint8[240]  val ;",
                       interpret_data,
                       NULL);
    }
  
    interpret_data.read_variable_group_begin("A_current_path_not_empty");
    for (int  idx_tst = 0; idx_tst < 40; ++idx_tst)
    {
        // global
        ut_interpret_bytes(type_definitions, "", " set global.msg_counter = 0 ;", interpret_data, "");
        ut_interpret_bytes(type_definitions, "", " set global.msg_counter = global.msg_counter + 1 ;", interpret_data, "");

        // pinfo
        ut_interpret_bytes(type_definitions, "", " set pinfo.msg_counter = 0 ;", interpret_data, "");
        ut_interpret_bytes(type_definitions, "", " set pinfo.msg_counter = pinfo.msg_counter + 1 ;", interpret_data, "");
        ut_interpret_bytes(type_definitions, "", " set pinfo.srcport = pinfo.dstport ;", interpret_data, "");
        ut_interpret_bytes(type_definitions, "", " set pinfo.fd.pipo = \"a new string\" ;", interpret_data, "");
    }
    interpret_data.read_variable_group_end();
    
    type_definitions = T_type_definitions();
    build_types ("unitary_tests_example_with_capture.fdesc",
                 type_definitions);
}

//*****************************************************************************
// test_interpret_simple_decoder_aes
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_decoder_aes)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;

    interpret_data.set_big_endian();


    // Encrypted with :
    // http://aes.online-domain-tools.com/   function=AES  mode=ECB (electronic codebook)


    // key="1234567890123456"  value="string{decoder=decoder_aes}  val ;"
    // value length = 35 (34 + zero end of string)
    // decrypted length = 48 (3*16)
    // So 13 padding bytes that must be read
    interpret_data.add_read_variable("decoder_aes_key", "1234567890123456");
    M_TEST_SIMPLE("1a	03	8c	23	70	bb	e8	59	d1	4f	d6	9e	b5	4d	f9	ad"
                  "7c	bc	ca	75	d1	99	89	5d	0c	86	fa	8e	2f	35	63	0e"
                  "de	5d	db	de	5e	56	af	7c	15	82	cb	fa	5e	e4	11	ce",
                  "string{decoder=decoder_aes}  val ;"
                  "hide raw(13)                 padding16bytes ;",
                  "val = string{decoder=decoder_aes}  val ;" K_eol);


    // key="abcdefghijklmnop"  value="string{decoder=decoder_aes}  val ;"
    // value length = 35 (34 + zero end of string)
    // decrypted length = 48 (3*16)
    // So 13 padding bytes that must be read
    interpret_data.add_read_variable("decoder_aes_key", "abcdefghijklmnop");
    M_TEST_SIMPLE("dd	4f	b4	94	e3	03	43	b0	17	e1	eb	5c	c1	70	cb	2b"
                  "68	57	7c	00	c6	4c	ad	18	e3	a3	2d	53	e1	9c	11	9e"
                  "2f	b3	d3	f3	88	d8	f7	8d	03	56	af	15	c5	87	14	07",
                  "string{decoder=decoder_aes}  val ;"
                  "hide raw(13)                 padding16bytes ;",
                  "val = string{decoder=decoder_aes}  val ;" K_eol);


    // key="1234567890123456"  file=favicon.ico
    // file length = 822
    // decrypted length = 832 (52*16)
    // So 10 padding bytes that must be read
    interpret_data.set_read_variable("decoder_aes_key", "1234567890123456");
    M_TEST_SIMPLE("42	d5	77	d8	f4	75	18	12	7b	ab	7c	38	02	17	c2	fb"
                  "32	8e	d0	74	b6	64	99	ab	c0	f7	92	da	f7	22	6d	bb"
                  "c0	d0	66	2f	70	a8	9a	b6	8f	45	7e	b9	96	1f	7d	e1"
                  "94	50	78	17	60	ec	85	d9	0c	45	4e	71	ad	af	b9	b8"
                  "12	9d	33	f7	05	14	61	f5	1d	12	73	d6	1d	c7	09	8c"
                  "3b	7e	22	2f	d5	5d	17	3a	ad	40	d7	e7	28	d7	2d	f1"
                  "ae	1d	df	5b	22	8d	78	92	c9	4c	36	f8	d6	a8	af	33"
                  "aa	a9	15	5b	16	43	19	b4	78	45	da	c7	ce	23	e3	9a"
                  "39	39	9a	58	e7	ba	65	a3	f3	d4	de	8e	fd	e7	4d	3d"
                  "84	9a	4a	9a	2d	1d	0d	c8	1a	c0	c4	e4	b8	2c	68	4d"
                  "a3	85	ba	a6	43	0d	18	0f	89	30	84	e7	91	7b	4b	ea"
                  "e9	2d	b1	a7	8d	26	75	97	c5	51	78	2e	74	0a	72	9d"
                  "5e	82	5a	f0	53	3c	52	d7	cf	50	42	a5	41	26	a4	f9"
                  "03	ff	01	56	5a	4e	1d	f5	93	79	e5	43	fc	ce	6b	23"
                  "5d	84	f5	f5	14	8d	a9	87	5b	4f	a2	b8	a7	ec	86	48"
                  "00	97	1b	66	ff	50	0d	01	66	8e	77	45	a3	51	be	06"
                  "66	cc	98	21	a4	eb	1c	d6	4e	3e	01	0f	56	74	de	1a"
                  "4f	37	67	14	c3	13	f6	71	60	b6	61	0b	52	0c	ad	ea"
                  "3f	61	83	f5	a3	70	ec	18	35	93	5a	0d	f9	6d	16	52"
                  "32	27	51	ed	e4	34	a4	c3	d1	70	46	21	4c	77	b6	49"
                  "3b	47	1f	8d	86	c9	44	e0	6a	01	66	93	a5	61	93	7e"
                  "66	48	64	33	a2	51	e6	e1	b0	1d	68	dc	1d	34	90	51"
                  "3a	f4	7b	76	c3	29	8b	64	23	17	aa	fc	2d	a8	e4	06"
                  "a9	eb	47	88	f7	b5	8b	b8	a4	69	7d	0c	25	0d	cd	57"
                  "15	69	27	c2	44	a1	29	98	88	54	48	fd	26	5d	1d	b1"
                  "c4	55	d4	c6	25	ad	63	86	70	0b	72	74	86	09	c8	1e"
                  "d6	f4	21	b2	c3	ef	31	04	70	3e	76	bc	e7	b2	2e	dd"
                  "e6	81	2e	ae	6c	b9	78	b6	50	95	31	9a	da	43	fa	9e"
                  "a0	61	55	4e	da	85	89	de	85	ea	65	0d	64	34	36	4c"
                  "12	e3	7b	cf	c0	35	30	70	c3	8d	d5	61	a2	03	3f	a2"
                  "e9	f3	59	b6	f5	b5	16	f5	2c	dc	bd	5c	81	d7	90	03"
                  "e3	73	61	38	11	da	6f	d2	3d	5f	cc	b4	95	a6	66	2e"
                  "aa	25	19	ac	51	2e	ad	04	2c	f1	f4	40	e1	83	c5	f5"
                  "33	09	6f	dd	d6	d5	36	1d	5c	d2	e9	7e	ad	db	a6	31"
                  "5e	61	27	48	da	6d	1c	2d	db	88	57	29	1a	4a	0f	55"
                  "bb	49	c3	6b	15	37	ed	ac	d9	87	52	f4	e0	1d	2c	f0"
                  "e3	ab	37	6b	38	e1	1b	ca	d0	cf	95	3e	76	1e	97	65"
                  "46	a4	ec	e0	5a	78	59	2a	61	9e	2f	22	35	e2	ff	23"
                  "d0	4a	27	87	e4	a5	eb	11	11	35	b3	31	54	a6	93	5f"
                  "96	0f	54	a0	ef	b9	33	c5	15	f7	16	57	bb	02	1f	0f"
                  "c3	55	f2	cf	d9	33	f2	27	66	45	56	81	44	da	82	c3"
                  "e6	99	e2	7e	cb	4e	27	63	1c	9b	42	41	8a	41	7a	5e"
                  "ec	74	d9	66	f1	39	da	cb	33	aa	7c	ac	20	61	21	bd"
                  "fe	4b	a6	1d	10	dc	db	9a	e0	7a	c3	a5	a6	14	a8	d1"
                  "23	ea	d9	2f	2a	9d	05	cc	5f	58	aa	02	28	52	fe	71"
                  "b3	cf	7a	d3	20	55	31	f3	54	b9	c5	8a	c4	e2	1e	c5"
                  "a0	e2	73	2e	28	76	52	e9	70	99	fd	30	00	1c	f9	98"
                  "e5	2a	2c	62	7b	d4	79	27	b9	18	62	c3	b8	01	88	73"
                  "12	69	9f	3d	e5	81	db	16	d6	bc	a9	23	69	0c	8b	4d"
                  "3c	c6	96	24	f9	23	86	52	5b	82	9d	ea	48	f6	2f	f9"
                  "f6	51	30	5f	76	0a	82	fe	8e	75	c5	45	f4	47	cf	0f"
                  "17	23	a2	6b	8a	78	2e	b4	2f	24	ff	1c	6c	12	35	71",
                  "decoder  decoder_aes;"
                  "uint32{d=hex}[4*51]   val ;"
                  "uint16{d=hex}         val1 ;"
                  "uint16{d=hex}         val2 ;"
                  "uint16{d=hex}         val3 ;"
                  "hide raw(10)          padding16bytes ;",
                  "val[0] = 0x424d3603 (1112356355)" K_eol
                  "val[1] = 0x0 (0)" K_eol
                  "val[2] = 0x3600 (13824)" K_eol
                  "val[3] = 0x2800 (10240)" K_eol
                  "val[4] = 0x1000 (4096)" K_eol
                  "val[5] = 0x1000 (4096)" K_eol
                  "val[6] = 0x100 (256)" K_eol
                  "val[7] = 0x18000000 (402653184)" K_eol
                  "val[8] = 0x3 (3)" K_eol
                  "val[9] = 0x0 (0)" K_eol
                  "val[10] = 0x0 (0)" K_eol
                  "val[11] = 0x0 (0)" K_eol
                  "val[12] = 0x0 (0)" K_eol
                  "val[13] = 0x0 (0)" K_eol
                  "val[14] = 0x3a3325 (3814181)" K_eol
                  "val[15] = 0x44391c41 (1144593473)" K_eol
                  "val[16] = 0x361b4134 (907755828)" K_eol
                  "val[17] = 0x1b41341b (457258011)" K_eol
                  "val[18] = 0x41341b41 (1093933889)" K_eol
                  "val[19] = 0x341b4134 (874201396)" K_eol
                  "val[20] = 0x1b41341b (457258011)" K_eol
                  "val[21] = 0x41341b41 (1093933889)" K_eol
                  "val[22] = 0x341b4134 (874201396)" K_eol
                  "val[23] = 0x1b44391c (457455900)" K_eol
                  "val[24] = 0x3a332500 (976430336)" K_eol
                  "val[25] = 0x332d (13101)" K_eol
                  "val[26] = 0x1e9c7215 (513569301)" K_eol
                  "val[27] = 0xb0831fac (2961383340)" K_eol
                  "val[28] = 0x811eac81 (2166271105)" K_eol
                  "val[29] = 0x1eae811f (514752799)" K_eol
                  "val[30] = 0xb18320b3 (2978160819)" K_eol
                  "val[31] = 0x8421b384 (2216801156)" K_eol
                  "val[32] = 0x21b38421 (565412897)" K_eol
                  "val[33] = 0xb18320ae (2978160814)" K_eol
                  "val[34] = 0x811fac81 (2166336641)" K_eol
                  "val[35] = 0x1eb0831f (514884383)" K_eol
                  "val[36] = 0x9c721533 (2624722227)" K_eol
                  "val[37] = 0x2d1e4235 (756957749)" K_eol
                  "val[38] = 0x19b48720 (431261472)" K_eol
                  "val[39] = 0xaf8426b0 (2944673456)" K_eol
                  "val[40] = 0x8526b286 (2233905798)" K_eol
                  "val[41] = 0x27b28528 (666010920)" K_eol
                  "val[42] = 0xb68729b8 (3062311352)" K_eol
                  "val[43] = 0x8a29b98a (2317990282)" K_eol
                  "val[44] = 0x29b88a29 (699959849)" K_eol
                  "val[45] = 0xb68729b6 (3062311350)" K_eol
                  "val[46] = 0x8928b587 (2301146503)" K_eol
                  "val[47] = 0x27b08526 (665879846)" K_eol
                  "val[48] = 0xb4872042 (3028754498)" K_eol
                  "val[49] = 0x35193b31 (890846001)" K_eol
                  "val[50] = 0x1b946f1a (462712602)" K_eol
                  "val[51] = 0x936f1e99 (2473533081)" K_eol
                  "val[52] = 0x7322af84 (1931652996)" K_eol
                  "val[53] = 0x28ba8d2a (683314474)" K_eol
                  "val[54] = 0xbb8d2cbf (3146591423)" K_eol
                  "val[55] = 0x8f2cbf91 (2402074513)" K_eol
                  "val[56] = 0x2dbe8f2c (767463212)" K_eol
                  "val[57] = 0xc0912caf (3230739631)" K_eol
                  "val[58] = 0x84289b73 (2217253747)" K_eol
                  "val[59] = 0x22997222 (580481570)" K_eol
                  "val[60] = 0x946f1a3b (2490309179)" K_eol
                  "val[61] = 0x311b2520 (823862560)" K_eol
                  "val[62] = 0x1644310c (373567756)" K_eol
                  "val[63] = 0x45340f3f (1161039679)" K_eol
                  "val[64] = 0x2f0f4d3a (789531962)" K_eol
                  "val[65] = 0x12c8982f (315136047)" K_eol
                  "val[66] = 0xc2932fc4 (3264425924)" K_eol
                  "val[67] = 0x9631c597 (2519844247)" K_eol
                  "val[68] = 0x31c79831 (835164209)" K_eol
                  "val[69] = 0xbb8d2d36 (3146591542)" K_eol
                  "val[70] = 0x2a0d4b39 (705514297)" K_eol
                  "val[71] = 0x12473410 (306656272)" K_eol
                  "val[72] = 0x44320d25 (1144130853)" K_eol
                  "val[73] = 0x20164236 (538329654)" K_eol
                  "val[74] = 0x1bb68721 (464946977)" K_eol
                  "val[75] = 0xbd8d29b8 (3180145080)" K_eol
                  "val[76] = 0x8c2a3e2f (2351578671)" K_eol
                  "val[77] = 0xfbd8f2f (264081199)" K_eol
                  "val[78] = 0xcb9a33cb (3415880651)" K_eol
                  "val[79] = 0x9c34cc9d (2620705949)" K_eol
                  "val[80] = 0x34d7a536 (886547766)" K_eol
                  "val[81] = 0x85652159 (2237997401)" K_eol
                  "val[82] = 0x4517d5a0 (1159189920)" K_eol
                  "val[83] = 0x31c0922c (834703916)" K_eol
                  "val[84] = 0xbc8b2444 (3163235396)" K_eol
                  "val[85] = 0x371c4136 (924598582)" K_eol
                  "val[86] = 0x1bb38421 (464749601)" K_eol
                  "val[87] = 0xb88929ca (3095996874)" K_eol
                  "val[88] = 0x992e4e3a (2569948730)" K_eol
                  "val[89] = 0x13997526 (328824102)" K_eol
                  "val[90] = 0xd7a638d3 (3617994963)" K_eol
                  "val[91] = 0xa135d5a2 (2704659874)" K_eol
                  "val[92] = 0x36e4ae3a (920956474)" K_eol
                  "val[93] = 0x634c1a8a (1665931914)" K_eol
                  "val[94] = 0x6922d19f (1763889567)" K_eol
                  "val[95] = 0x32be8f2c (851349292)" K_eol
                  "val[96] = 0xb9892444 (3112772676)" K_eol
                  "val[97] = 0x371c4236 (924598838)" K_eol
                  "val[98] = 0x1bb38521 (464749857)" K_eol
                  "val[99] = 0xb88b2acb (3096128203)" K_eol
                  "val[100] = 0x9a308465 (2586870885)" K_eol
                  "val[101] = 0x1f5a4417 (526009367)" K_eol
                  "val[102] = 0xe8b13cda (3903929562)" K_eol
                  "val[103] = 0xa638dba8 (2788744104)" K_eol
                  "val[104] = 0x3ae5ae3b (988130875)" K_eol
                  "val[105] = 0x544116a7 (1413551783)" K_eol
                  "val[106] = 0x7f2ace9d (2133511837)" K_eol
                  "val[107] = 0x32c0912d (851480877)" K_eol
                  "val[108] = 0xbb8b2444 (3146458180)" K_eol
                  "val[109] = 0x381c4236 (941376054)" K_eol
                  "val[110] = 0x1bb48522 (464815394)" K_eol
                  "val[111] = 0xb98c2ac2 (3112970946)" K_eol
                  "val[112] = 0x922ecb99 (2452540313)" K_eol
                  "val[113] = 0x323b2d0f (842738959)" K_eol
                  "val[114] = 0xb78c2fe9 (3079417833)" K_eol
                  "val[115] = 0xb43ee2ae (3024020142)" K_eol
                  "val[116] = 0x3fe5b13d (1072017725)" K_eol
                  "val[117] = 0x534016b1 (1396709041)" K_eol
                  "val[118] = 0x862ccd9b (2251083163)" K_eol
                  "val[119] = 0x33c1912e (868323630)" K_eol
                  "val[120] = 0xbc8c2545 (3163301189)" K_eol
                  "val[121] = 0x381d4236 (941441590)" K_eol
                  "val[122] = 0x1bb38521 (464749857)" K_eol
                  "val[123] = 0x9ce4f59c (2632250780)" K_eol
                  "val[124] = 0xe4f5d4a0 (3841316000)" K_eol
                  "val[125] = 0x34997626 (882472486)" K_eol
                  "val[126] = 0x3a2c0fdf (975966175)" K_eol
                  "val[127] = 0xac3be9b5 (2889607605)" K_eol
                  "val[128] = 0x40e7b23c (1088926268)" K_eol
                  "val[129] = 0x564117a6 (1447106470)" K_eol
                  "val[130] = 0x7f29cf9e (2133446558)" K_eol
                  "val[131] = 0x33c1912e (868323630)" K_eol
                  "val[132] = 0xbb8c2544 (3146523972)" K_eol
                  "val[133] = 0x381c4136 (941375798)" K_eol
                  "val[134] = 0x1b9ce4f5 (463267061)" K_eol
                  "val[135] = 0xb88a29bf (3096062399)" K_eol
                  "val[136] = 0x8f2c9ce4 (2402065636)" K_eol
                  "val[137] = 0xf5dca937 (4124879159)" K_eol
                  "val[138] = 0x7c60213d (2086674749)" K_eol
                  "val[139] = 0x2e10c192 (772850066)" K_eol
                  "val[140] = 0x31fabf41 (838516545)" K_eol
                  "val[141] = 0x795c1f81 (2036080513)" K_eol
                  "val[142] = 0x611fd4a0 (1629476000)" K_eol
                  "val[143] = 0x33bf912d (868192557)" K_eol
                  "val[144] = 0xba8a2444 (3129615428)" K_eol
                  "val[145] = 0x371c4134 (924598580)" K_eol
                  "val[146] = 0x1b9ce4f5 (463267061)" K_eol
                  "val[147] = 0xb687299c (3062311324)" K_eol
                  "val[148] = 0xe4f59ce4 (3841301732)" K_eol
                  "val[149] = 0xf5c79631 (4123498033)" K_eol
                  "val[150] = 0xdca83893 (3702012051)" K_eol
                  "val[151] = 0x71263f30 (1898331952)" K_eol
                  "val[152] = 0x10664d1b (275139867)" K_eol
                  "val[153] = 0x61491932 (1632180530)" K_eol
                  "val[154] = 0x260cc897 (638371991)" K_eol
                  "val[155] = 0x30bd902b (817729579)" K_eol
                  "val[156] = 0xb7872343 (3079086915)" K_eol
                  "val[157] = 0x371c4136 (924598582)" K_eol
                  "val[158] = 0x1b9ce4f5 (463267061)" K_eol
                  "val[159] = 0xb18322b7 (2978161335)" K_eol
                  "val[160] = 0x8928bd8d (2301148557)" K_eol
                  "val[161] = 0x2bc2912d (734171437)" K_eol
                  "val[162] = 0xc4952fd6 (3298111446)" K_eol
                  "val[163] = 0xa133c997 (2704525719)" K_eol
                  "val[164] = 0x308d6a20 (814574112)" K_eol
                  "val[165] = 0x6a4f1767 (1783568231)" K_eol
                  "val[166] = 0x4c16b789 (1276557193)" K_eol
                  "val[167] = 0x28ba8b28 (683313960)" K_eol
                  "val[168] = 0xb3852142 (3011846466)" K_eol
                  "val[169] = 0x361b4033 (907755571)" K_eol
                  "val[170] = 0x18ac7d17 (413957399)" K_eol
                  "val[171] = 0x9ce4f59c (2632250780)" K_eol
                  "val[172] = 0xe4f59ce4 (3841301732)" K_eol
                  "val[173] = 0xf5c6a151 (4123435345)" K_eol
                  "val[174] = 0xc9a24ec9 (3382857417)" K_eol
                  "val[175] = 0xa04bcca1 (2689322145)" K_eol
                  "val[176] = 0x49d2a547 (1238541639)" K_eol
                  "val[177] = 0xd4a643ce (3567666126)" K_eol
                  "val[178] = 0x9f3ebc91 (2671688849)" K_eol
                  "val[179] = 0x36b5892d (917866797)" K_eol
                  "val[180] = 0xb0811b3f (2961251135)" K_eol
                  "val[181] = 0x3419322a (874066474)" K_eol
                  "val[182] = 0x1cbea367 (482255719)" K_eol
                  "val[183] = 0xdaccacd5 (3670846677)" K_eol
                  "val[184] = 0xc5a1d5c3 (3315717571)" K_eol
                  "val[185] = 0x9dd6c399 (2648097689)" K_eol
                  "val[186] = 0xd7c295d6 (3619853782)" K_eol
                  "val[187] = 0xc091d6be (3230783166)" K_eol
                  "val[188] = 0x8cd4bc88 (2362752136)" K_eol
                  "val[189] = 0xd2b983cf (3535373263)" K_eol
                  "val[190] = 0xb57ecdb3 (3044986291)" K_eol
                  "val[191] = 0x78cfb377 (2026877815)" K_eol
                  "val[192] = 0xb3914632 (3012642354)" K_eol
                  "val[193] = 0x2a1b0000 (706412544)" K_eol
                  "val[194] = 0x88898b (8948107)" K_eol
                  "val[195] = 0x95939096 (2509475990)" K_eol
                  "val[196] = 0x95919997 (2509347223)" K_eol
                  "val[197] = 0x949c9995 (2493290901)" K_eol
                  "val[198] = 0x9e9c96a1 (2661062305)" K_eol
                  "val[199] = 0x9e97a19e (2660737438)" K_eol
                  "val[200] = 0x969f9c95 (2527042709)" K_eol
                  "val[201] = 0x9c989196 (2627244438)" K_eol
                  "val[202] = 0x948b918e (2492174734)" K_eol
                  "val[203] = 0x858e8880 (2240710784)" K_eol
                  "val1 = 0x7573 (30067)" K_eol
                  "val2 = 0x6e00 (28160)" K_eol
                  "val3 = 0x0 (0)" K_eol
                  );
                  
    // key="WqA&1XsZ~2CdE\"3VfR'4BgT(" (24 bytes)  value="string{decoder=decoder_aes}  val ;"
    // value length = 35 (34 + zero end of string)
    // decrypted length = 48 (3*16)
    // So 13 padding bytes that must be read
    interpret_data.add_read_variable("decoder_aes_key", "WqA&1XsZ~2CdE\"3VfR'4BgT(");
    M_TEST_SIMPLE("2a	61	39	d6	12	4c	04	32	7e	d1	7b	a3	2e	f5	9e	ae"
                  "02	c9	1e	9e	eb	1a	54	10	31	44	d1	21	b9	71	88	cb"
                  "f0	c1	7d	61	46	75	7c	db	a7	c7	86	4d	ad	b5	9c	77",
                  "string{decoder=decoder_aes}  val ;"
                  "hide raw(13)                 padding16bytes ;",
                  "val = string{decoder=decoder_aes}  val ;" K_eol);

    // key="WqA&1XsZ~2CdE\"3VfR'4BgT(5NhY-6,;" (32 bytes)  value="string{decoder=decoder_aes}  val ;"
    // value length = 35 (34 + zero end of string)
    // decrypted length = 48 (3*16)
    // So 13 padding bytes that must be read
    interpret_data.add_read_variable("decoder_aes_key", "WqA&1XsZ~2CdE\"3VfR'4BgT(5NhY-6,;");
    M_TEST_SIMPLE("f8	68	ea	04	3b	d9	c2	f8	52	05	18	b7	2f	7c	6d	c9"
                  "7c	ec	50	5a	4c	0e	91	74	c3	c6	82	cb	92	8d	c4	e7"
                  "53	63	ee	9e	1f	b5	1b	57	80	38	3a	2d	60	ba	b6	0a",
                  "string{decoder=decoder_aes}  val ;"
                  "hide raw(13)                 padding16bytes ;",
                  "val = string{decoder=decoder_aes}  val ;" K_eol);

#if 0
    // Binary key : NOT implemented
    // key="00010203040506778899AABBCCDDEEFF101112131415161718191A2B3C4D5E6F" (32 bytes)  value="string{decoder=decoder_aes}  val ;"
    // value length = 35 (34 + zero end of string)
    // decrypted length = 48 (3*16)
    // So 13 padding bytes that must be read
    interpret_data.add_read_variable("decoder_aes_key", "decoder_aes_key", "00010203040506778899AABBCCDDEEFF101112131415161718191A2B3C4D5E6F");
    M_TEST_SIMPLE("9c	20	40	56	82	e5	ac	eb	3b	62	dd	39	81	c9	f0	70"
                  "85	e7	b2	01	a0	42	33	e3	53	09	cb	c6	01	60	9d	45"
                  "81	69	29	79	ff	19	3a	78	c9	79	9f	d3	0b	70	47	76",
                  "string{decoder=decoder_aes}  val ;"
                  "hide raw(13)                 padding16bytes ;",
                  "val = string{decoder=decoder_aes}  val ;" K_eol);
#endif
}

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
    M_TEST_SIMPLE("56334E6E5A413D3D", " uint8{decoder=decoder_base64}     val1 ;"
                                      "uint24{decoder=decoder_base64}     val2 ;", "val1 = 87" K_eol "val2 = 7563108" K_eol);
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

//*****************************************************************************
// test_interpret_simple_int64
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_int64)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;

    interpret_data.set_big_endian();

    M_TEST_SIMPLE("e23f6a77cbf367a9", "int64  val ;", "val = -2143877834849687639" K_eol);
}

//*****************************************************************************
// test_interpret_simple_uint8_array
//*****************************************************************************

void    test_interpret_simple_uint8_array(const int  max_iter)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;

    interpret_data.set_little_endian();

    for (int  idx_tst = 0; idx_tst < max_iter; ++idx_tst)
    {
        M_TRACE_WARNING("msg.size=" << interpret_data.DEBUG_get_msg().size());
        ut_interpret_bytes(type_definitions,
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453"
                           "4249472d5245515545535453" "4249472d5245515545535453",
                           "uint8[240]  val ;",
                           interpret_data,
                           NULL);
    }
}

M_TEST_FCT(test_interpret_simple_uint8_array_1)
{
    test_interpret_simple_uint8_array(1);
}

//*****************************************************************************
// test_interpret_simple_internal_frame
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_internal_frame)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
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

//*****************************************************************************
// test_interpret_simple_trailer
//*****************************************************************************

M_TEST_FCT(test_interpret_simple_trailer)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;

	type_definitions.trailer_sizeof_bits = 24;
#define K_TRAILER_INPUT   "767778"
#define K_TRAILER_READ    "uint8[3]  trailer ;"
#define K_TRAILER_OUTPUT  "trailer[0] = 118" K_eol "trailer[1] = 119" K_eol "trailer[2] = 120" K_eol

    {
        M_TEST_SIMPLE("4249472d5245515545535453" K_TRAILER_INPUT,
                      "raw(*)  val ;" K_TRAILER_READ,
                      "val = " K_eol
                      "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    " K_eol
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("" K_TRAILER_INPUT,
                      "raw(*)  val ;" K_TRAILER_READ,
                      ""
                      K_TRAILER_OUTPUT);
    }

    {
        C_ut_interpret_bytes_no_decode_guard  uibndg;    // [*] & [+] rejected with decode
        M_TEST_SIMPLE("4249472d5245515545535453" K_TRAILER_INPUT,
                      "uint8[*]  val ;" K_TRAILER_READ,
                      "val[0] = 66" K_eol
                      "val[1] = 73" K_eol
                      "val[2] = 71" K_eol
                      "val[3] = 45" K_eol
                      "val[4] = 82" K_eol
                      "val[5] = 69" K_eol
                      "val[6] = 81" K_eol
                      "val[7] = 85" K_eol
                      "val[8] = 69" K_eol
                      "val[9] = 83" K_eol
                      "val[10] = 84" K_eol
                      "val[11] = 83" K_eol
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("4249472d5245515545535453" K_TRAILER_INPUT,
                      "uint8[+]  val ;" K_TRAILER_READ,
                      "val[0] = 66" K_eol
                      "val[1] = 73" K_eol
                      "val[2] = 71" K_eol
                      "val[3] = 45" K_eol
                      "val[4] = 82" K_eol
                      "val[5] = 69" K_eol
                      "val[6] = 81" K_eol
                      "val[7] = 85" K_eol
                      "val[8] = 69" K_eol
                      "val[9] = 83" K_eol
                      "val[10] = 84" K_eol
                      "val[11] = 83" K_eol
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("" K_TRAILER_INPUT,
                      "uint8[*]  val ;" K_TRAILER_READ,
                      ""
                      K_TRAILER_OUTPUT);
        M_TEST_SIMPLE("42" K_TRAILER_INPUT,
                      "uint8[+]  val ;" K_TRAILER_READ,
                      "val[0] = 66" K_eol
                      K_TRAILER_OUTPUT);
    }
}

//*****************************************************************************
// test_is_existing_field_or_variable
//*****************************************************************************

M_TEST_FCT(test_is_existing_field_or_variable)
{
    T_type_definitions    type_definitions;
    build_types("unitary_tests_basic.fdesc",
                type_definitions);

    T_interpret_data      interpret_data;

    M_TEST_SIMPLE("", "T_is_existing_field_or_variable    st;", "st.in_main_struct = 1" K_eol);

    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_none_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_not_exist_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_main_struct_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_if_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_else_begin")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_while_begin")->get_bool(), false);

    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_none_end")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_not_exist_end")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_main_struct_end")->get_bool(), true);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_if_end")->get_bool(), true);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_else_end")->get_bool(), false);
    M_TEST_EQ(interpret_data.get_P_value_of_read_variable("is_in_while_end")->get_bool(), true);
}

//*****************************************************************************
// test_interpret_forget
//*****************************************************************************

M_TEST_FCT(test_interpret_forget)
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

    T_interpret_data      interpret_data;

    // Test with simple variable
    M_TEST_EQ(interpret_data.is_read_variable("value"), false);
    M_TEST_SIMPLE("", " forget var uint32  value = 3457 ;", "value = 3457" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), false);            // value is forgotten
    M_TEST_SIMPLE("", "        var uint32  value = 7 ;", "value = 7" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), true);
    M_TEST_SIMPLE("", "        print (\"%d\", value);", "7" K_eol);
    M_TEST_SIMPLE("", " forget var uint32  value = 3457 ;", "value = 3457" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), true);
    M_TEST_SIMPLE("", "        print (\"%d\", value);", "7" K_eol);            // value 3457 is forgotten
    M_TEST_SIMPLE("", "        var uint32  value = 3 ;", "value = 3" K_eol);
    M_TEST_SIMPLE("", "        print (\"%d\", value);", "3" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("value"), true);

    // Test with struct
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), false);
    M_TEST_SIMPLE("", "    var int32  struct_counter = 1000 ;", "struct_counter = 1000" K_eol);
    M_TEST_SIMPLE("", "        T_forget      st1 ;", "st1.struct_counter = 1001" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), true);
    M_TEST_SIMPLE("", " forget T_forget      st2 ;", "st2.struct_counter = 1002" K_eol);
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), true);
    M_TEST_SIMPLE("", "        T_forget      st3 ;", "st3.struct_counter = 1002" K_eol);   // st2 (and its struct_counter) is forgotten
    M_TEST_EQ(interpret_data.is_read_variable("struct_counter"), true);

    // Test with array : each item of the array is forgot (one by one, not at the end of the array)
    M_TEST_SIMPLE("", " forget T_forget[3]   sta ;", "sta[0].struct_counter = 1003" K_eol
                                                     "sta[1].struct_counter = 1003" K_eol
                                                     "sta[2].struct_counter = 1003" K_eol);
    M_TEST_SIMPLE("", "        T_forget      st7 ;", "st7.struct_counter = 1003" K_eol);
}

//*****************************************************************************
// test_interpret_msg
//*****************************************************************************

void    test_interpret_msg(int   msg_to_test = -1)
{
    // Messages which come from example_with_capture.pcap
    // The number are the packet's number.
    T_byte_vector    msg_1_frame;
    T_byte_vector    msg_2_frame;
    T_byte_vector    msg_4_frame;
    T_byte_vector    msg_35_frame;
    {
        const char  * msg_1_str = "6c000b000000000000000000";
        const char  * msg_2_str = "01000b000000f400807468020000a000ffff1f00000000001800ffff01070000202008ff8072470054686520584672656538362050726f6a6563742c20496e630101200000000000040820000000000008082000000000000f102000000000001010200000000000182020000000000020202000000000005700000020000000ffffff000000000000001a0090061a0439026401010001002200000000001807182720005ede020022000000040800010000ff0000ff0000ff000000d2ea020023000000040800010000ff0000ff0000ff000000d2ea020024000000040800010000ff0000ff0000ff000000d2ea020025000000040800010000ff0000ff0000ff000000d2ea020026000000040800010000ff0000ff0000ff000000d2ea020027000000040800010000ff0000ff0000ff000000d2ea020028000000040800010000ff0000ff0000ff000000d2ea020029000000040800010000ff0000ff0000ff000000d2ea02002a000000040800010000ff0000ff0000ff000000d2ea02002b000000040800010000ff0000ff0000ff000000d2ea02002c000000040800010000ff0000ff0000ff000000d2ea02002d000000040800010000ff0000ff0000ff000000d2ea02002e000000040800010000ff0000ff0000ff000000d2ea02002f000000040800010000ff0000ff0000ff000000d2ea020030000000040800010000ff0000ff0000ff000000d2ea020031000000040800010000ff0000ff0000ff000000d2ea020032000000040800010000ff0000ff0000ff000000d2ea020033000000040800010000ff0000ff0000ff000000d2ea020034000000040800010000ff0000ff0000ff000000d2ea020035000000040800010000ff0000ff0000ff000000d2ea020036000000040800010000ff0000ff0000ff000000d2ea020037000000040800010000ff0000ff0000ff000000d2ea020038000000040800010000ff0000ff0000ff000000d2ea020039000000040800010000ff0000ff0000ff000000d2ea02003a000000040800010000ff0000ff0000ff000000d2ea02003b000000040800010000ff0000ff0000ff000000d2ea02003c000000040800010000ff0000ff0000ff000000d2ea02003d000000040800010000ff0000ff0000ff000000d2ea02003e000000040800010000ff0000ff0000ff000000d2ea02003f000000040800010000ff0000ff0000ff000000d2ea020040000000040800010000ff0000ff0000ff000000d2ea020041000000040800010000ff0000ff0000ff000000d2ea0200012700005ede0200042700005ede0200082700005ede02000f2700005ede0200102700005ede0200202700005ede0200";
        const char  * msg_4_str = "010002000000000001870000000000000000000000000000280000005bc00100";
        const char  * msg_35_str = "370005000000a0005700000008000000ffffff00620005000c0000004249472d524551554553545387000100";
        string_hexa_to_frame(msg_1_str, msg_1_frame);
        string_hexa_to_frame(msg_2_str, msg_2_frame);
        string_hexa_to_frame(msg_4_str, msg_4_frame);
        string_hexa_to_frame(msg_35_str, msg_35_frame);
    }

    T_type_definitions    type_definitions;
    build_types ("unitary_tests_example_with_capture.fdesc",
                 type_definitions);

    if ((msg_to_test < 0) || (msg_to_test == 1))
    {
        T_interpret_data  interpret_data;
        interpret_data.global_variable_group_begin();
        interpret_data.add_read_variable("msg_counter" ,0);
        interpret_data.global_variable_group_end();

        ut_interpret_bytes(type_definitions,
                           msg_1_frame,
                           "T_msg_cs_start         msg_1;",
                           interpret_data,
                           "msg_1.Header.Type = cs_start (7077899)" K_eol
                           "msg_1.Header.Size = 8" K_eol
                           "msg_1.id = 0" K_eol
                           "msg_1.acronym = " K_eol);
    }
    if ((msg_to_test < 0) || (msg_to_test == 2))
    {
        T_interpret_data  interpret_data;
        interpret_data.global_variable_group_begin();
        interpret_data.add_read_variable("msg_counter" ,1);
        interpret_data.global_variable_group_end();

        // 2010/10/15         2013 ms
        // 2010/10/16  1513 - 1545 ms
        // 2010/10/29         1190 ms  T_expression var set call
        // 2010/10/29          655 ms  T_expression condition return
        // 2010/11/10          675 ms  T_expression pre_compute value & operation
        // 2014/12/20          172 ms
        ut_interpret_bytes(type_definitions,
                           msg_2_frame,
                           "T_msg_sc_start_ack         msg_2;",
                           interpret_data);
    }
    if ((msg_to_test < 0) || (msg_to_test == 4))
    {
        T_interpret_data  interpret_data;
        interpret_data.global_variable_group_begin();
        interpret_data.add_read_variable("msg_counter" ,2);
        interpret_data.global_variable_group_end();

        ut_interpret_bytes(type_definitions,
                           msg_4_frame,
                           "T_msg_sc_ctx_data         msg_4;",
                           interpret_data);
    }
    if ((msg_to_test < 0) || (msg_to_test == 35))
    {
        T_interpret_data  interpret_data;
        interpret_data.global_variable_group_begin();
        interpret_data.add_read_variable("msg_counter" ,3);
        interpret_data.global_variable_group_end();

        ut_interpret_bytes(type_definitions,
                           msg_35_frame,
                           "T_msg_cs_request        msg_35;",
                           interpret_data);
    }
}

M_TEST_FCT(test_interpret_msg_all)
{
    test_interpret_msg();
}

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

//*****************************************************************************
// test_library
//*****************************************************************************

#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
M_TEST_FCT(test_library)
#else
M_TEST_FCT_IGNORE(test_library)
#endif
{
    T_type_definitions    type_definitions;
#if WIRESHARK_VERSION_NUMBER >= 20200
    build_types ("unitary_tests_library.fdesc",
#else
    build_types ("unitary_tests_library.old_path.fdesc",
#endif
                 type_definitions);

    T_interpret_data      interpret_data;

    // ascii_strdown_inplace & ascii_strup_inplace
    M_TEST_SIMPLE("", "var string  original = \"Hello WORLD !\";",          "original = Hello WORLD !" K_eol);
    M_TEST_SIMPLE("", "var string  before = original;",                       "before = Hello WORLD !" K_eol);
    M_TEST_SIMPLE("", "var string  ret    = ascii_strdown_inplace (before);",    "ret = hello world !" K_eol);
    M_TEST_SIMPLE("", "var string  after  = before;",                          "after = hello world !" K_eol);
    M_TEST_SIMPLE("", "var string  ret    = ascii_strup_inplace (before);",      "ret = HELLO WORLD !" K_eol);
    M_TEST_SIMPLE("", "var string  after  = before;",                          "after = HELLO WORLD !" K_eol);
    M_TEST_SIMPLE("", "var string  notmodified = original;",             "notmodified = Hello WORLD !" K_eol);


    // crc is not inside wsutil before wireshark 18X
#if WIRESHARK_VERSION_NUMBER >= 10800
    build_types ("unitary_tests_library_part2.fdesc",
                 type_definitions);

    // crc32c_calculate_byte
#define DATA  "6c 00 0b 00 00 00 00 00 00 00 00 00"

#define M_TEST_CRC32(POS,OFFSET,SEED,RESULT)  \
    M_TEST_SIMPLE(DATA,  \
        "var uint32  crc32 = crc32c_calculate_byte (" #POS "," #OFFSET "," #SEED "); move_position_bytes 12;",  \
        "crc32 = " #RESULT)

    M_TEST_CRC32( 0, 0,   0,            0);
    M_TEST_CRC32( 2, 0, 136,          136);
    M_TEST_CRC32( 0, 4,   0,   1483871832);
    M_TEST_CRC32( 4, 4,   0,            0);
    M_TEST_CRC32( 0, 4, 136,   4010883408);
    M_TEST_CRC32( 4, 4, 136,   3076730632);
    M_TEST_CRC32( 8, 4,   0,            0);
//	M_TEST_CRC32( 9, 4,   0,            0);    // out of bounds
//	M_TEST_CRC32(12, 4,   0,            0);    // out of bounds
#endif

    // base64 is not inside wsutil before wireshark 112X
#if WIRESHARK_VERSION_NUMBER >= 11200
    build_types ("unitary_tests_library_part3.fdesc",
                 type_definitions);

#define M_TEST_BASE64(INPUT,RESULT)  \
    M_TEST_SIMPLE("",  \
        "hide var string  input_output = " #INPUT "; "  \
        "hide var int64   size = ws_base64_decode_inplace (input_output); "  \
        "print (\"%s\", input_output); ",  \
        RESULT)

    // Only test with output = string, but output can be anything
    M_TEST_BASE64("V3NnZA==", "Wsgd" K_eol);
    M_TEST_BASE64("SSBoYXZlIGEgYmFzZTY0IGVuY29kZWQgc3RyaW5n", "I have a base64 encoded string" K_eol);
    M_TEST_BASE64("T3V0a2FzdCAvIEhleSBZYQ==", "Outkast / Hey Ya" K_eol);
#endif
}

//*****************************************************************************
// main
//*****************************************************************************

int   main(const int           argc,
           const char * const  argv[])
{
    // Traces
    string      trace_file_name = string(argv[0]) + ".traces.txt";
    ofstream    ofs(trace_file_name.c_str());
    set_state_ostream(ofs);

    int   arg_idx = 1;

    if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-debug") == 0))
    {
        set_debug(E_debug_status_ON);
        ++arg_idx;
    }

    if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-uint8_array") == 0))
    {
        test_interpret_simple_uint8_array(1);
    }
    else if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-uint8_array_10") == 0))
    {
        test_interpret_simple_uint8_array(10);
    }
    else if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-msg_2") == 0))
    {
        test_interpret_msg(2);
    }
    else
    {
        T_static_executor_manager::getInstance().execute(argc, argv, arg_idx);
    }

    C_perf_chrono::end();
    M_TEST_RETURN_OF_MAIN();
}

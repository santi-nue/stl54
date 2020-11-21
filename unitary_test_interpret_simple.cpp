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
    M_TEST_SIMPLE("e23f6a77", "float32  val ;", "val = -8.8275e+20" K_eol);
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
    M_TEST_SIMPLE("776a3fe2", "float32  val ;", "val = -8.8275e+20" K_eol);
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

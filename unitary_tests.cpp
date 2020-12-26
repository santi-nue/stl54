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
                           nullptr);
    }
}

M_TEST_FCT(test_interpret_simple_uint8_array_1)
{
    test_interpret_simple_uint8_array(1);
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
// test_library
//*****************************************************************************

#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
M_TEST_FCT(test_library)
#else
M_TEST_FCT_IGNORE(test_library)
#endif
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_library.fdesc",
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


    // crc
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


    // base64
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

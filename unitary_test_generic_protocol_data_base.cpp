/*
 * Copyright 2020 Olivier Aveline <wsgd@free.fr>
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
#include "byte_interpret_common_exception.h"
#include "T_generic_protocol_data_base.h"


//*****************************************************************************
// test_read_file_wsgd_until_types__ko_PROTO_TYPE_DEFINITIONS_not_found
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ko_PROTO_TYPE_DEFINITIONS_not_found)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    istringstream  iss("");

    M_TEST_CATCH_EXCEPTION(
        read_file_wsgd_until_types(iss, protocol_data, stats),
        C_byte_interpret_exception);
    M_TEST_EQ(protocol_data.DEBUG, false);
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_empty
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_empty)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    {
        istringstream  iss("PROTO_TYPE_DEFINITIONS");
        read_file_wsgd_until_types(iss, protocol_data, stats);
        M_TEST_EQ(protocol_data.DEBUG, E_debug_status_OFF);
    }
    {
        istringstream  iss(R"(


PROTO_TYPE_DEFINITIONS

)");
        read_file_wsgd_until_types(iss, protocol_data, stats);
        M_TEST_EQ(protocol_data.DEBUG, E_debug_status_OFF);
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_DEBUG
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_DEBUG)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    {
        istringstream  iss(R"(
DEBUG
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);
        M_TEST_EQ(protocol_data.DEBUG, E_debug_status_ON);
    }
    {
        istringstream  iss(R"(
DEBUG_NO_TIME
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);
        M_TEST_EQ(protocol_data.DEBUG, E_debug_status_ON_NO_TIME);
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_proto_ident
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_proto_ident)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    {
        istringstream  iss(R"(
PROTONAME         Samsung TV Remote Control  
PROTOSHORTNAME    SR  
PROTOABBREV       samsung_remote  
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.PROTONAME, "Samsung TV Remote Control");
        M_TEST_EQ(protocol_data.PROTOSHORTNAME, "SR");
        M_TEST_EQ(protocol_data.PROTOABBREV, "samsung_remote");
    }
    // Double definition is forbidden
    {
        istringstream  iss(R"(
PROTONAME         New TV Remote Control  
PROTO_TYPE_DEFINITIONS)");
        M_TEST_CATCH_EXCEPTION(
            read_file_wsgd_until_types(iss, protocol_data, stats),
            C_byte_interpret_exception);

        M_TEST_EQ(protocol_data.PROTONAME, "Samsung TV Remote Control");
    }
    {
        istringstream  iss(R"(
PROTOSHORTNAME    NEW  
PROTO_TYPE_DEFINITIONS)");
        M_TEST_CATCH_EXCEPTION(
            read_file_wsgd_until_types(iss, protocol_data, stats),
            C_byte_interpret_exception);

        M_TEST_EQ(protocol_data.PROTOSHORTNAME, "SR");
    }
    {
        istringstream  iss(R"(
PROTOABBREV       new_remote  
PROTO_TYPE_DEFINITIONS)");
        M_TEST_CATCH_EXCEPTION(
            read_file_wsgd_until_types(iss, protocol_data, stats),
            C_byte_interpret_exception);

        M_TEST_EQ(protocol_data.PROTOABBREV, "samsung_remote");
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_PARENT_SUBFIELD
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_PARENT_SUBFIELD)
{
    {
        // values = string
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
PARENT_SUBFIELD          tcp.port
PARENT_SUBFIELD_VALUES   "55000" "toto"
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.PARENTS.size(), 1);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD, "tcp.port");
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_str.size(), 2);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_str[0], "55000");
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_str[1], "toto");
    }
    {
        // values = int
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
PARENT_SUBFIELD          udp.port
PARENT_SUBFIELD_VALUES   55000 55001
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.PARENTS.size(), 1);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD, "udp.port");
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_int.size(), 2);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_int[0], 55000);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_int[1], 55001);
    }
    {
        // values = range
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
PARENT_SUBFIELD          some.thing
PARENT_SUBFIELD_RANGE    55000 55100
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.PARENTS.size(), 1);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD, "some.thing");
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_RANGES_int.size(), 1);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_RANGES_int[0].first, 55000);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_RANGES_int[0].second, 55100);
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ko_PARENT_SUBFIELD_mixed
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ko_PARENT_SUBFIELD_mixed)
{
    {
        // KO mixed values 
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
PARENT_SUBFIELD          tcp.port
PARENT_SUBFIELD_VALUES   55000 "55001"
PROTO_TYPE_DEFINITIONS)");
        M_TEST_CATCH_EXCEPTION(
            read_file_wsgd_until_types(iss, protocol_data, stats),
            C_byte_interpret_exception);
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_multiple_PARENT_SUBFIELD
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_multiple_PARENT_SUBFIELD)
{
    {
        // values = string
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
PARENT_SUBFIELD          tcp.port
PARENT_SUBFIELD_VALUES   "55000" "toto"

PARENT_SUBFIELD          udp.port
PARENT_SUBFIELD_VALUES   55000 55001

PARENT_SUBFIELD          some.thing
PARENT_SUBFIELD_RANGE    55000 55100

PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.PARENTS.size(), 3);

        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD, "tcp.port");
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_str.size(), 2);
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_str[0], "55000");
        M_TEST_EQ(protocol_data.PARENTS[0].PARENT_SUBFIELD_VALUES_str[1], "toto");

        M_TEST_EQ(protocol_data.PARENTS[1].PARENT_SUBFIELD, "udp.port");
        M_TEST_EQ(protocol_data.PARENTS[1].PARENT_SUBFIELD_VALUES_int.size(), 2);
        M_TEST_EQ(protocol_data.PARENTS[1].PARENT_SUBFIELD_VALUES_int[0], 55000);
        M_TEST_EQ(protocol_data.PARENTS[1].PARENT_SUBFIELD_VALUES_int[1], 55001);

        M_TEST_EQ(protocol_data.PARENTS[2].PARENT_SUBFIELD, "some.thing");
        M_TEST_EQ(protocol_data.PARENTS[2].PARENT_SUBFIELD_RANGES_int.size(), 1);
        M_TEST_EQ(protocol_data.PARENTS[2].PARENT_SUBFIELD_RANGES_int[0].first, 55000);
        M_TEST_EQ(protocol_data.PARENTS[2].PARENT_SUBFIELD_RANGES_int[0].second, 55100);
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_PARENT_HEURISTIC
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_PARENT_HEURISTIC)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    istringstream  iss(R"(
PARENT_HEURISTIC         tcp  
PARENT_HEURISTIC         another_proto
HEURISTIC_FUNCTION       "the_function_to_call"
PROTO_TYPE_DEFINITIONS)");
    read_file_wsgd_until_types(iss, protocol_data, stats);

    M_TEST_EQ(protocol_data.PARENTS_HEURISTIC.size(), 2);
    M_TEST_EQ(protocol_data.PARENTS_HEURISTIC[0], "tcp");
    M_TEST_EQ(protocol_data.PARENTS_HEURISTIC[1], "another_proto");
    M_TEST_EQ(protocol_data.HEURISTIC_FUNCTION, "the_function_to_call");
}

//*****************************************************************************
// test_read_file_wsgd_until_types__ok_ADD_FOR_DECODE_AS_TABLE
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__ok_ADD_FOR_DECODE_AS_TABLE)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    istringstream  iss(R"(
ADD_FOR_DECODE_AS_TABLE         table  
ADD_FOR_DECODE_AS_TABLE         another_table
PROTO_TYPE_DEFINITIONS)");
    read_file_wsgd_until_types(iss, protocol_data, stats);

    M_TEST_EQ(protocol_data.ADD_FOR_DECODE_AS_TABLES.size(), 2);
    M_TEST_EQ(protocol_data.ADD_FOR_DECODE_AS_TABLES[0], "table");
    M_TEST_EQ(protocol_data.ADD_FOR_DECODE_AS_TABLES[1], "another_table");
}

//*****************************************************************************
// test_read_file_wsgd_until_types__SUBFIELD
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__SUBFIELD)
{
    {
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
SUBFIELD         field  uint8  
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD, "field");
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_TYPE, "uint8");
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_TYPE_WS, FT_UINT8);

        {
            // ko SUBPROTO_SUBFIELD already defined
            istringstream  iss(R"(
SUBFIELD         field  uint8  
PROTO_TYPE_DEFINITIONS)");
            M_TEST_CATCH_EXCEPTION(
                read_file_wsgd_until_types(iss, protocol_data, stats),
                C_byte_interpret_exception);
        }
    }
    {
        // ko bad type
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
SUBFIELD         field  int8  
PROTO_TYPE_DEFINITIONS)");
        M_TEST_CATCH_EXCEPTION(
            read_file_wsgd_until_types(iss, protocol_data, stats),
            C_byte_interpret_exception);
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__SUBFIELD_from
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__SUBFIELD_from)
{
    {
        T_generic_protocol_data_base  protocol_data;
        T_stats                       stats;

        istringstream  iss(R"(
SUBFIELD         pseudo_field  string  from  real1  real2  real3
PROTO_TYPE_DEFINITIONS)");
        read_file_wsgd_until_types(iss, protocol_data, stats);

        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD, "pseudo_field");
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_TYPE, "string");
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_TYPE_WS, FT_STRING);
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1, "real1");
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_2, "real2");
        M_TEST_EQ(protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_3, "real3");
    }
}

//*****************************************************************************
// test_read_file_wsgd_until_types__MSG
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__MSG)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    istringstream  iss(R"(
MSG_HEADER_TYPE                      a_type
MSG_ID_FIELD_NAME                    msg_id
MSG_TITLE                            msg_title
MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES   field1  field2
MSG_MAIN_TYPE                        main_type
MSG_FROM_MAIN_TYPE                   from_type
MSG_TO_MAIN_TYPE                     to_type
MSG_TOTAL_LENGTH                     Size + 12  
MSG_HEADER_LENGTH                    12
MSG_TRAILER_LENGTH                   2
GLOBAL_DATA_TYPE                     global_type
PROTO_TYPE_DEFINITIONS)");
    read_file_wsgd_until_types(iss, protocol_data, stats);

    M_TEST_EQ(protocol_data.MSG_HEADER_TYPE, "a_type");
    M_TEST_EQ(protocol_data.MSG_ID_FIELD_NAME, "msg_id");
    M_TEST_EQ(protocol_data.MSG_TITLE, "msg_title");
    M_TEST_EQ(protocol_data.MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES.size(), 2);
    M_TEST_EQ(protocol_data.MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES[0], "field1");
    M_TEST_EQ(protocol_data.MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES[1], "field2");
    M_TEST_EQ(protocol_data.MSG_MAIN_TYPE, "main_type");
    M_TEST_EQ(protocol_data.MSG_FROM_MAIN_TYPE, "from_type");
    M_TEST_EQ(protocol_data.MSG_TO_MAIN_TYPE, "to_type");
    M_TEST_EQ(protocol_data.MSG_TOTAL_LENGTH, "Size + 12  ");
    M_TEST_EQ(protocol_data.MSG_HEADER_LENGTH, 12);
    M_TEST_EQ(protocol_data.MSG_TRAILER_LENGTH, 2);
    M_TEST_EQ(protocol_data.GLOBAL_DATA_TYPE, "global_type");
}

//*****************************************************************************
// test_read_file_wsgd_until_types__MSG_bool
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__MSG_bool)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    istringstream  iss(R"(
PACKET_CONTAINS_ONLY_1_MSG         yes
PACKET_CONTAINS_ONLY_COMPLETE_MSG  true
MANAGE_WIRESHARK_PINFO             no
PROTO_TYPE_DEFINITIONS)");
    read_file_wsgd_until_types(iss, protocol_data, stats);

    M_TEST_EQ(protocol_data.PACKET_CONTAINS_ONLY_1_MSG, true);
    M_TEST_EQ(protocol_data.PACKET_CONTAINS_ONLY_COMPLETE_MSG, true);
    M_TEST_EQ(protocol_data.MANAGE_WIRESHARK_PINFO, false);
}

//*****************************************************************************
// test_read_file_wsgd_until_types__statistics
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_until_types__statistics)
{
    T_generic_protocol_data_base  protocol_data;
    T_stats                       stats;

    istringstream  iss(R"(
STATISTICS    "menu"  ""  "topic"  field
PROTO_TYPE_DEFINITIONS)");
    read_file_wsgd_until_types(iss, protocol_data, stats);

    M_TEST_EQ(stats.groups.size(), 1);
    M_TEST_EQ(stats.groups[0].group_name, "menu");
    // other checks : see unitary_test_generic_statistics.cpp
}

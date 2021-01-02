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
#include "T_generic_statistics.h"
#include "byte_interpret_common_exception.h"


//*****************************************************************************
// test_read_file_wsgd_statistics_basic
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics__basic)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control" [ "Type" [ "Message Type"   MessageType ] ]# comment  
)");
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 1);
    M_TEST_EQ(stats.groups[0].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[0].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].sub_group_name, "Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].full_name, "Samsung TV Remote Control/Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].variable_name, "MessageType");
}

//*****************************************************************************
// test_read_file_wsgd_statistics_ko_1_token_missing
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics_ko_1_token_missing)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control" [ "Type" [ "Message Type" ] ]# comment  
)");
    M_TEST_CATCH_EXCEPTION(
        read_file_wsgd_statistics(iss, stats),
        C_byte_interpret_exception);
}

//*****************************************************************************
// test_read_file_wsgd_statistics_ko_1_token_too_much
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics_ko_1_token_too_much)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control" [ "Type" [ "Message Type"   MessageType TooMuch ] ]# comment  
)");
    M_TEST_CATCH_EXCEPTION(
        read_file_wsgd_statistics(iss, stats),
        C_byte_interpret_exception);
}

//*****************************************************************************
// test_read_file_wsgd_statistics_ko_not_a_string
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics_ko_not_a_string)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control" [ Type [ "Message Type"   MessageType ] ]# comment  
)");
    M_TEST_CATCH_EXCEPTION(
        read_file_wsgd_statistics(iss, stats),
        C_byte_interpret_exception);
}

//*****************************************************************************
// test_read_file_wsgd_statistics_ko_is_a_string
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics_ko_is_a_string)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control" [ "Type" [ "Message Type"   "MessageType" ] ]# comment  
)");
    M_TEST_CATCH_EXCEPTION(
        read_file_wsgd_statistics(iss, stats),
        C_byte_interpret_exception);
}

//*****************************************************************************
// test_read_file_wsgd_statistics__no_sub_group
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics__no_sub_group)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control Type" [   # comment  
    "Message Type"   MessageType,                               # comment2a
    "Message Title"  Msg_Title ,                                # comment2b
    "Message Size"   MessageSize,                               # comment2c
]                                                               # comment3  
)");
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 1);
    M_TEST_EQ(stats.groups[0].group_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].sub_group_name, "");
    M_TEST_EQ(stats.groups[0].sub_groups[0].full_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics.size(), 3);
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].variable_name, "MessageType");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[1].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[1].variable_name, "Msg_Title");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[2].topic_name, "Message Size");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[2].variable_name, "MessageSize");
}

//*****************************************************************************
// test_read_file_wsgd_statistics__multiple_sub_group
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics__multiple_sub_group)
{
    T_stats        stats;
    istringstream  iss(R"( "Samsung TV Remote Control" [   # comment  
    "Type"   [ "Message Type"   MessageType ] ,
    "Title"  [ "Message Title"  Msg_Title,  "Message Size" MessageSize , ],
    "Length" [ "Message Length"   MessageLength ] ,
]
)");
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 1);
    M_TEST_EQ(stats.groups[0].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[0].sub_groups.size(), 3);
    M_TEST_EQ(stats.groups[0].sub_groups[0].sub_group_name, "Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].full_name, "Samsung TV Remote Control/Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].variable_name, "MessageType");
    M_TEST_EQ(stats.groups[0].sub_groups[1].sub_group_name, "Title");
    M_TEST_EQ(stats.groups[0].sub_groups[1].full_name, "Samsung TV Remote Control/Title");
    M_TEST_EQ(stats.groups[0].sub_groups[1].topics.size(), 2);
    M_TEST_EQ(stats.groups[0].sub_groups[1].topics[0].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[0].sub_groups[1].topics[0].variable_name, "Msg_Title");
    M_TEST_EQ(stats.groups[0].sub_groups[1].topics[1].topic_name, "Message Size");
    M_TEST_EQ(stats.groups[0].sub_groups[1].topics[1].variable_name, "MessageSize");
    M_TEST_EQ(stats.groups[0].sub_groups[2].sub_group_name, "Length");
    M_TEST_EQ(stats.groups[0].sub_groups[2].full_name, "Samsung TV Remote Control/Length");
    M_TEST_EQ(stats.groups[0].sub_groups[2].topics.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[2].topics[0].topic_name, "Message Length");
    M_TEST_EQ(stats.groups[0].sub_groups[2].topics[0].variable_name, "MessageLength");
}

//*****************************************************************************
// test_read_file_wsgd_statistics__ko_doubles
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics__ko_doubles)
{
    {
        T_stats        stats;
        istringstream  iss(R"( "Samsung TV Remote Control" [ "something" [ "Message Type"   MessageType ] ]# comment  
)");
        read_file_wsgd_statistics(iss, stats);

        // Same menu name is forbidden
        istringstream  iss2(R"( "Samsung TV Remote Control" [ "other" [ "Message Type"   MessageType ] ]# comment  
)");
        M_TEST_CATCH_EXCEPTION(read_file_wsgd_statistics(iss2, stats), C_byte_interpret_exception);
    }
    {
        T_stats        stats;
        istringstream  iss(R"( "Samsung TV Remote Control" [   # comment  
    "Type"   [ "Message Type"   MessageType ] ,
    "Title"  [ "Message Title"  Msg_Title,  "Message Size" MessageSize , ],
    "Type"   [ "Message Type"   MessageType ] ,
]
)");
        M_TEST_CATCH_EXCEPTION(read_file_wsgd_statistics(iss, stats), C_byte_interpret_exception);
    }
}

//*****************************************************************************
// test_read_file_wsgd_statistics
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics)
{
    T_stats        stats;
    istringstream  iss(R"(
STATISTICS "Samsung TV Remote Control Type"  [ "Message Type"   MessageType ,]
STATISTICS "Samsung TV Remote Control Title" [ "Message Title"  Msg_Title, ]
STATISTICS "Samsung TV Remote Control" [
    "Type"  [ "Message Type"   MessageType ] ,
    "Title" [ "Message Title"  Msg_Title   ] ,
    "Type & Title" [        "Message Type"   MessageType, 
        "Message Title"   Msg_Title ,
        "Message Length"  MessageLength, 
    ]
]  
another_thing
)");

    // STATISTICS \"Samsung TV Remote Control Type\"  [ \"Message Type\"   MessageType ,]
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 1);
    M_TEST_EQ(stats.groups[0].group_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].sub_group_name, "");
    M_TEST_EQ(stats.groups[0].sub_groups[0].full_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].variable_name, "MessageType");

    // STATISTICS \"Samsung TV Remote Control Title\" [ \"Message Title\"  Msg_Title, ]
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 2);
    M_TEST_EQ(stats.groups[1].group_name, "Samsung TV Remote Control Title");
    M_TEST_EQ(stats.groups[1].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[1].sub_groups[0].sub_group_name, "");
    M_TEST_EQ(stats.groups[1].sub_groups[0].full_name, "Samsung TV Remote Control Title");
    M_TEST_EQ(stats.groups[1].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[1].sub_groups[0].topics[0].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[1].sub_groups[0].topics[0].variable_name, "Msg_Title");

    // STATISTICS \"Samsung TV Remote Control\" [ ...
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 3);
    M_TEST_EQ(stats.groups[2].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[2].sub_groups.size(), 3);
    M_TEST_EQ(stats.groups[2].sub_groups[0].sub_group_name, "Type");
    M_TEST_EQ(stats.groups[2].sub_groups[0].full_name, "Samsung TV Remote Control/Type");
    M_TEST_EQ(stats.groups[2].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[2].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[2].sub_groups[0].topics[0].variable_name, "MessageType");
    M_TEST_EQ(stats.groups[2].sub_groups[1].sub_group_name, "Title");
    M_TEST_EQ(stats.groups[2].sub_groups[1].full_name, "Samsung TV Remote Control/Title");
    M_TEST_EQ(stats.groups[2].sub_groups[1].topics.size(), 1);
    M_TEST_EQ(stats.groups[2].sub_groups[1].topics[0].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[2].sub_groups[1].topics[0].variable_name, "Msg_Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].sub_group_name, "Type & Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].full_name, "Samsung TV Remote Control/Type & Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics.size(), 3);
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[0].variable_name, "MessageType");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[1].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[1].variable_name, "Msg_Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[2].topic_name, "Message Length");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[2].variable_name, "MessageLength");

    // Check previous data are still here
    M_TEST_EQ(stats.groups[0].group_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].sub_group_name, "");
    M_TEST_EQ(stats.groups[0].sub_groups[0].full_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].variable_name, "MessageType");

    M_TEST_EQ(stats.groups[1].group_name, "Samsung TV Remote Control Title");
    M_TEST_EQ(stats.groups[1].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[1].sub_groups[0].sub_group_name, "");
    M_TEST_EQ(stats.groups[1].sub_groups[0].full_name, "Samsung TV Remote Control Title");
    M_TEST_EQ(stats.groups[1].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[1].sub_groups[0].topics[0].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[1].sub_groups[0].topics[0].variable_name, "Msg_Title");

    // another_thing
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "another_thing");
    }
}

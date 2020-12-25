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
    istringstream  iss("  \"Samsung TV Remote Control\"   \"Type\"              \"Message Type\"   MessageType   \n");
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
// test_read_file_wsgd_statistics__no_sub_group
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics__no_sub_group)
{
    T_stats        stats;
    istringstream  iss("  \"Samsung TV Remote Control Type\"   \"\"              \"Message Type\"   MessageType   \n");
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 1);
    M_TEST_EQ(stats.groups[0].group_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].sub_group_name, "");
    M_TEST_EQ(stats.groups[0].sub_groups[0].full_name, "Samsung TV Remote Control Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[0].sub_groups[0].topics[0].variable_name, "MessageType");
}

//*****************************************************************************
// test_read_file_wsgd_statistics__ko_sub_group_incoherent
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics__ko_sub_group_incoherent)
{
    {
        T_stats        stats;
        istringstream  iss("  \"Samsung TV Remote Control Type\"   \"something\"              \"Message Type\"   MessageType   \n");
        read_file_wsgd_statistics(iss, stats);

        // Define a sub_group is madatory if already exist one in the same group
        istringstream  iss2("  \"Samsung TV Remote Control Type\"   \"\"     \"Message Type\"   MessageType   \n");
        M_TEST_CATCH_EXCEPTION(read_file_wsgd_statistics(iss2, stats), C_byte_interpret_exception);
    }
    {
        T_stats        stats;
        istringstream  iss("  \"Samsung TV Remote Control Type\"   \"\"              \"Message Type\"   MessageType   \n");
        read_file_wsgd_statistics(iss, stats);

        // Define a sub_group forbidden if ...
        istringstream  iss2("  \"Samsung TV Remote Control Type\"   \"something\"     \"Message Type\"   MessageType   \n");
        M_TEST_CATCH_EXCEPTION(read_file_wsgd_statistics(iss2, stats), C_byte_interpret_exception);
    }
}

//*****************************************************************************
// test_read_file_wsgd_statistics
//*****************************************************************************

M_TEST_FCT(test_read_file_wsgd_statistics)
{
    T_stats        stats;
    istringstream  iss(
        "STATISTICS  \"Samsung TV Remote Control Type\"   \"\"              \"Message Type\"   MessageType   \n"
        "STATISTICS  \"Samsung TV Remote Control Title\"  \"\"              \"Message Title\"  Msg_Title     \n"
        "STATISTICS  \"Samsung TV Remote Control\"        \"Type\"          \"Message Type\"   MessageType   \n"
        "STATISTICS  \"Samsung TV Remote Control\"        \"Title\"         \"Message Title\"  Msg_Title     \n"
        "STATISTICS  \"Samsung TV Remote Control\"        \"Type & Title\"  \"Message Type\"   MessageType   \n"
        "STATISTICS  \"Samsung TV Remote Control\"        \"Type & Title\"  \"Message Title\"  Msg_Title     \n"
        "another_thing");

    // STATISTICS  "Samsung TV Remote Control Type"   ""              "Message Type"   MessageType
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

    // STATISTICS  "Samsung TV Remote Control Title"  ""              "Message Title"  Msg_Title
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

    // STATISTICS  "Samsung TV Remote Control"        "Type"          "Message Type"   MessageType
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 3);
    M_TEST_EQ(stats.groups[2].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[2].sub_groups.size(), 1);
    M_TEST_EQ(stats.groups[2].sub_groups[0].sub_group_name, "Type");
    M_TEST_EQ(stats.groups[2].sub_groups[0].full_name, "Samsung TV Remote Control/Type");
    M_TEST_EQ(stats.groups[2].sub_groups[0].topics.size(), 1);
    M_TEST_EQ(stats.groups[2].sub_groups[0].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[2].sub_groups[0].topics[0].variable_name, "MessageType");

    // STATISTICS  "Samsung TV Remote Control"        "Title"         "Message Title"  Msg_Title
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 3);
    M_TEST_EQ(stats.groups[2].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[2].sub_groups.size(), 2);
    M_TEST_EQ(stats.groups[2].sub_groups[1].sub_group_name, "Title");
    M_TEST_EQ(stats.groups[2].sub_groups[1].full_name, "Samsung TV Remote Control/Title");
    M_TEST_EQ(stats.groups[2].sub_groups[1].topics.size(), 1);
    M_TEST_EQ(stats.groups[2].sub_groups[1].topics[0].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[2].sub_groups[1].topics[0].variable_name, "Msg_Title");

    // STATISTICS  "Samsung TV Remote Control"        "Type & Title"  "Message Type"   MessageType
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 3);
    M_TEST_EQ(stats.groups[2].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[2].sub_groups.size(), 3);
    M_TEST_EQ(stats.groups[2].sub_groups[2].sub_group_name, "Type & Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].full_name, "Samsung TV Remote Control/Type & Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics.size(), 1);
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[0].variable_name, "MessageType");

    // STATISTICS  "Samsung TV Remote Control"        "Type & Title"  "Message Title"  Msg_Title
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "STATISTICS");
    }
    read_file_wsgd_statistics(iss, stats);

    M_TEST_EQ(stats.groups.size(), 3);
    M_TEST_EQ(stats.groups[2].group_name, "Samsung TV Remote Control");
    M_TEST_EQ(stats.groups[2].sub_groups.size(), 3);
    M_TEST_EQ(stats.groups[2].sub_groups[2].sub_group_name, "Type & Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].full_name, "Samsung TV Remote Control/Type & Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics.size(), 2);
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[1].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[1].variable_name, "Msg_Title");

    // another_thing
    {
        std::string  keyword;
        iss >> keyword;
        M_TEST_EQ(keyword, "another_thing");
    }



    // Check all data
    M_TEST_EQ(stats.groups.size(), 3);

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
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics.size(), 2);
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[0].topic_name, "Message Type");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[0].variable_name, "MessageType");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[1].topic_name, "Message Title");
    M_TEST_EQ(stats.groups[2].sub_groups[2].topics[1].variable_name, "Msg_Title");
}

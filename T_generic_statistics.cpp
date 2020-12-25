/* 
 * Copyright 2020 Olivier Aveline <wsgd@free.fr>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
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
// Includes
//*****************************************************************************

#include "T_generic_statistics.h"
#include "byte_interpret_parse.h"


//*****************************************************************************
// T_stats_group::get_or_create_sub_group
//*****************************************************************************
T_stats_sub_group&
T_stats_group::get_or_create_sub_group(const std::string& sub_group_name)
{
    for (auto sub_group_iter = sub_groups.begin(); sub_group_iter != sub_groups.end(); ++sub_group_iter)
    {
        T_stats_sub_group& sub_group = *sub_group_iter;

        if (sub_group_name == sub_group.sub_group_name)
        {
            return  sub_group;
        }
    }

    if (sub_groups.empty() == false)
    {
        if (sub_group_name.empty() != sub_groups[0].sub_group_name.empty())
        {
            M_FATAL_COMMENT("Statistics " << group_name  << " : all sub menus must be empty or not empty");
        }
    }

    T_stats_sub_group  sub_group;
    sub_group.sub_group_name = sub_group_name;
    sub_group.full_name = group_name;
    if (sub_group_name != "")
    {
        sub_group.full_name += "/";
        sub_group.full_name += sub_group_name;
    }

    sub_groups.push_back(sub_group);
    return  sub_groups.back();
}

//*****************************************************************************
// T_stats::get_sub_group_by_full_name
//*****************************************************************************
T_stats_group&
T_stats::get_or_create_group(const std::string& group_name)
{
    for (auto group_iter = groups.begin(); group_iter != groups.end(); ++group_iter)
    {
        T_stats_group& group = *group_iter;

        if (group_name == group.group_name)
        {
            return  group;
        }
    }

    T_stats_group  group;
    group.group_name = group_name;

    groups.push_back(group);
    return  groups.back();
}

//*****************************************************************************
// T_stats::get_sub_group_by_full_name
//*****************************************************************************
T_stats_sub_group&
T_stats::get_sub_group_by_full_name(const std::string& full_name)
{
    for (auto group_iter = groups.begin(); group_iter != groups.end(); ++group_iter)
    {
        T_stats_group& group = *group_iter;

        for (auto sub_group_iter = group.sub_groups.begin(); sub_group_iter != group.sub_groups.end(); ++sub_group_iter)
        {
            T_stats_sub_group& sub_group = *sub_group_iter;

            if (full_name == sub_group.full_name)
            {
                return  sub_group;
            }
        }
    }

    M_FATAL_COMMENT("sub_group.full_name == " << full_name << " not found");
}

//*****************************************************************************
// read_file_wsgd_statistics
//*****************************************************************************

void    read_file_wsgd_statistics(std::istream& is,
    T_stats& stats)
{
    std::vector<std::string> words;
    {
        is >> std::ws;
        std::string    line;
        std::getline(is, line);
        string_to_words(line, words);
    }
    M_FATAL_IF_NE(words.size(), 4);

    std::string  menu_name = words[0];
    remove_string_limits(menu_name);
    T_stats_group& group = stats.get_or_create_group(menu_name);

    std::string  sub_menu_name = words[1];
    remove_string_limits(sub_menu_name);
    T_stats_sub_group& sub_group = group.get_or_create_sub_group(sub_menu_name);

    T_stats_topic  topic;
    topic.topic_name = words[2];
    remove_string_limits(topic.topic_name);
    topic.variable_name = words[3];
    remove_string_limits(topic.variable_name);
    sub_group.topics.push_back(topic);
}

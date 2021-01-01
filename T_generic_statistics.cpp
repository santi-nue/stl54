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
    for (auto& sub_group : sub_groups)
    {
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
    for (auto& group : groups)
    {
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
    for (auto& group : groups)
    {
        for (auto& sub_group : group.sub_groups)
        {
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
#include "peglib.h"

void  read_file_wsgd_statistics(std::istringstream& iss,
                                T_stats&            stats)
{
    M_TRACE_ENTER("read_file_wsgd_statistics", "");

    peg::parser parser;
    parser.log = [](size_t line, size_t col, const string& msg) {
        M_TRACE_FATAL("read_file_wsgd_statistics parse fail on " << line << ":" << col << ": " << msg);
    };

    const bool load_result = parser.load_grammar(R"(
    Root                      <- STATISTICS RemainingContent
    STATISTICS                <- WS+  <String>  WS+  <String>  WS+  <String>  WS+  < IdentificatorSimple >  WS* EndOfLine

    RemainingContent          <- < .* >
    IdentificatorSimple       <- < [a-zA-Z_] [a-zA-Z0-9_]* >
    String                    <- '"' < [^"]* > '"'

    ~WS                       <- SpaceChar / LineComment
    ~LineComment              <- '#' (!EndOfLine .)*
    ~SpaceChar                <- ' ' / '\t'
    ~EndOfLine                <- '\r\n' / '\n' / '\r'
    )");
    if (!load_result)
    {
        M_FATAL_COMMENT("peglib load fail");
    }

    parser["String"] = [](const peg::SemanticValues& sv) {
        return sv.token();
    };
    parser["IdentificatorSimple"] = [](const peg::SemanticValues& sv) {
        return sv.token();
    };

    struct T_parse_final_result
    {
        T_stats&     stats;
        std::string  remaining_content;

        T_parse_final_result(T_stats& stats_param)
            : stats(stats_param)
        {}
    };

    parser["STATISTICS"] = [](const peg::SemanticValues& sv, peg::any& dt) {
        T_parse_final_result& parse_final_result = *peg::any_cast<T_parse_final_result*>(dt);
        auto& stats = parse_final_result.stats;

        M_FATAL_IF_NE(sv.size(), 4);

        const std::string  menu_name = peg::any_cast<std::string>(sv[0]);
        T_stats_group&     group = stats.get_or_create_group(menu_name);

        const std::string  sub_menu_name = peg::any_cast<std::string>(sv[1]);
        T_stats_sub_group& sub_group = group.get_or_create_sub_group(sub_menu_name);

        T_stats_topic  topic;
        topic.topic_name = peg::any_cast<std::string>(sv[2]);
        topic.variable_name = peg::any_cast<std::string>(sv[3]);
        sub_group.topics.push_back(topic);
    };

    parser["RemainingContent"] = [](const peg::SemanticValues& sv, peg::any& dt) {
        T_parse_final_result& parse_final_result = *peg::any_cast<T_parse_final_result*>(dt);

        M_FATAL_IF_NE(sv.tokens.size(), 1);
        parse_final_result.remaining_content = sv.token(0);
    };

    parser.enable_packrat_parsing();


    const std::string     content{ std::istreambuf_iterator<char>(iss),
                                   std::istreambuf_iterator<char>() };
    T_parse_final_result  parse_final_result(stats);
    peg::any              dt(&parse_final_result);

    const bool parse_result = parser.parse(content.c_str(), dt);

    if (!parse_result)
    {
        M_FATAL_COMMENT("STATISTICS peglib parse fail");
    }

    // Reset iss with the remaining content
    iss.str(parse_final_result.remaining_content);
}

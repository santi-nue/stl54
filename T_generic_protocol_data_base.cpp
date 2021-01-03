/*
 * Copyright 2008-2020 Olivier Aveline <wsgd@free.fr>
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

#include "byte_interpret_common.h"
void fatal_PROTO_TYPE_DEFINITIONS_not_found()
{
    M_FATAL_COMMENT("PROTO_TYPE_DEFINITIONS not found");  // must stay line 30 (for test purpose)
}

#include "T_generic_protocol_data_base.h"
#include "byte_interpret_common_utils.h"
#include "byte_interpret_parse.h"
#include "byte_interpret.h"

#include <istream>


//*****************************************************************************
// check_not_already_initialized
//*****************************************************************************
void    check_not_already_initialized(const string  & val)
{
    M_FATAL_IF_NE(val, "");
}
void    check_not_already_initialized(const int    val)
{
    M_FATAL_IF_NE(val, -1);
}

//*****************************************************************************
// check_initialized
//*****************************************************************************
void    check_initialized(const string  & val)
{
    M_FATAL_IF_EQ(val, "");
}
void    check_initialized(const int    val)
{
    M_FATAL_IF_EQ(val, -1);
}
void    check_initialized(const vector<int>  & val)
{
    M_FATAL_IF_EQ(val.size(), 0);
}

//*****************************************************************************
// T_generic_protocol_data
//*****************************************************************************
void
T_generic_protocol_data_base::check_config_parameters_initialized() const
{
    ::check_initialized(PROTONAME);
    ::check_initialized(PROTOSHORTNAME);
    ::check_initialized(PROTOABBREV);

    if (PARENTS.empty() &&
        PARENTS_HEURISTIC.empty())
    {
        M_FATAL_COMMENT("Must set PARENT_SUBFIELD & PARENT_SUBFIELD_VALUES");
    }
    for (vector<T_generic_protocol_data_base::T_parent>::const_iterator
        parent_iter = PARENTS.begin();
        parent_iter != PARENTS.end();
        ++parent_iter)
    {
        const T_generic_protocol_data_base::T_parent  & parent = *parent_iter;
        ::check_initialized(parent.PARENT_SUBFIELD);
        if (parent.PARENT_SUBFIELD_VALUES_int.empty() &&
            parent.PARENT_SUBFIELD_VALUES_str.empty() &&
            parent.PARENT_SUBFIELD_RANGES_int.empty())
        {
            M_FATAL_COMMENT("Must set PARENT_SUBFIELD_VALUES or PARENT_SUBFIELD_RANGE");
        }
    }

    ::check_initialized(MSG_HEADER_TYPE);
    ::check_initialized(MSG_ID_FIELD_NAME);
    //  ::check_initialized(MSG_TITLE);  optional
    //  ::check_initialized(MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES);  optional
    if (MSG_MAIN_TYPE == "")
    {
        if ((MSG_FROM_MAIN_TYPE == "") || (MSG_TO_MAIN_TYPE == ""))
        {
            M_FATAL_COMMENT("Must set MSG_MAIN_TYPE or MSG_FROM_MAIN_TYPE and MSG_TO_MAIN_TYPE");
        }
    }
}

//*****************************************************************************
// read_file_wsgd_until_types
//*****************************************************************************

void    read_file_wsgd_until_types(
                              istringstream                 & iss,
                              T_generic_protocol_data_base  & protocol_data,
                              T_stats                       & stats)
{
    M_TRACE_ENTER("read_file_wsgd_until_types", "");

    // string ou int
#define M_READ_NAME_VALUE(NAME,VARIABLE)                     \
    else if (keyword == #NAME)  {                            \
        check_not_already_initialized(VARIABLE);             \
        iss >> VARIABLE;                                     \
        M_TRACE_DEBUG (#NAME << " = " << VARIABLE);          \
    }

#define M_READ_VALUE(NAME)                                   \
    M_READ_NAME_VALUE(NAME,protocol_data.NAME)

// bool
#define M_READ_NAME_BOOL(NAME,VARIABLE)                      \
    else if (keyword == #NAME)  {                            \
        string    word;                                      \
        iss >> word;                                         \
        if ((word == "false") || (word == "no")) {           \
            VARIABLE = false;                                \
        }                                                    \
        else if ((word == "true") || (word == "yes")) {      \
            VARIABLE = true;                                 \
        }                                                    \
        else {                                               \
            M_FATAL_COMMENT("Bad value for " #NAME);         \
        }                                                    \
        M_TRACE_DEBUG (#NAME << " = " << VARIABLE);          \
    }

// bool
#define M_READ_BOOL(NAME)                                    \
    M_READ_NAME_BOOL(NAME,protocol_data.NAME)

// vector<int>
#define M_READ_VALUES(NAME)                                  \
    else if (keyword == #NAME)  {                            \
        string    line;                                      \
        getline (iss, line);                                 \
        istringstream  is_line(line.c_str());                \
        while (is_istream_empty(is_line) != true)            \
        {                                                    \
            int    val;                                      \
            is_line >> val;                                  \
            protocol_data.NAME.push_back(val);               \
        }                                                    \
    }

// vector<string>
#define M_READ_STRINGS(NAME,VARIABLE)                        \
    else if (keyword == #NAME)  {                            \
        iss >> ws;                                           \
        string    line;                                      \
        getline (iss, line);                                 \
        istringstream  is_line(line.c_str());                \
        while (is_istream_empty(is_line) != true)            \
        {                                                    \
            string    val;                                   \
            is_line >> val;                                  \
            protocol_data.VARIABLE.push_back(val);           \
        }                                                    \
    }

// string
#define M_READ_LINE(NAME)                                                \
    else if (keyword == #NAME)  {                                        \
        check_not_already_initialized(protocol_data.NAME);               \
        iss >> ws;                                                       \
        getline (iss, protocol_data.NAME);                               \
        const string::size_type  NAME_size = protocol_data.NAME.size();  \
        if ((NAME_size > 0) &&                                           \
            (protocol_data.NAME[NAME_size-1] == '\r'))                   \
        {                                                                \
            protocol_data.NAME.erase(NAME_size-1);                       \
        }                                                                \
        M_TRACE_DEBUG (#NAME << " = " << protocol_data.NAME);            \
    }

    // Read the 1st part of the file (until PROTO_TYPE_DEFINITIONS).
    bool  PROTO_TYPE_DEFINITIONS_found = false;
    while (is_istream_empty(iss) != true)
    {
        string    keyword;
        iss >> keyword;

        if (keyword == "DEBUG")
        {
            protocol_data.DEBUG = E_debug_status_ON;
//            set_debug(protocol_data.DEBUG);
            continue;
        }
        else if (keyword == "DEBUG_NO_TIME")
        {
            protocol_data.DEBUG = E_debug_status_ON_NO_TIME;
            continue;
        }
        else if (keyword == "PROTO_TYPE_DEFINITIONS")
        {
            PROTO_TYPE_DEFINITIONS_found = true;
            break;
        }
        M_READ_LINE(PROTONAME)
        M_READ_VALUE(PROTOSHORTNAME)
        M_READ_VALUE(PROTOABBREV)

        else if (keyword == "PARENT_SUBFIELD")
        {
            protocol_data.PARENTS.push_back(T_generic_protocol_data_base::T_parent());
            iss >> protocol_data.PARENTS.back().PARENT_SUBFIELD;
            M_TRACE_DEBUG("PARENT_SUBFIELD = " << protocol_data.PARENTS.back().PARENT_SUBFIELD);
        }
        M_READ_STRINGS(PARENT_SUBFIELD_VALUES, PARENTS.back().PARENT_SUBFIELD_VALUES_str)
        else if (keyword == "PARENT_SUBFIELD_RANGE")
        {
            pair<int, int>    low_high;
            iss >> low_high.first;
            iss >> low_high.second;
            protocol_data.PARENTS.back().PARENT_SUBFIELD_RANGES_int.push_back(low_high);
        }
        else if (keyword == "PARENT_HEURISTIC")
        {
            string   parent_name;
            iss >> parent_name;
            protocol_data.PARENTS_HEURISTIC.push_back(parent_name);
        }
        M_READ_VALUE(HEURISTIC_FUNCTION)
        else if (keyword == "ADD_FOR_DECODE_AS_TABLE")
        {
            string   table_name;
            iss >> table_name;
            protocol_data.ADD_FOR_DECODE_AS_TABLES.push_back(table_name);
        }

        else if (keyword == "SUBFIELD")
        {
            check_not_already_initialized(protocol_data.SUBPROTO_SUBFIELD);
            iss >> ws;
            string    line;
            getline(iss, line);
            {
                istringstream  is_line(line.c_str());
                is_line >> protocol_data.SUBPROTO_SUBFIELD;
                is_line >> protocol_data.SUBPROTO_SUBFIELD_TYPE;
                if (protocol_data.SUBPROTO_SUBFIELD == "")
                {
                    M_FATAL_COMMENT("Missing name on SUBFIELD line = " << line);
                }
                if (protocol_data.SUBPROTO_SUBFIELD_TYPE == "")
                {
                    M_FATAL_COMMENT("Missing type on SUBFIELD line = " << line);
                }
                if (protocol_data.SUBPROTO_SUBFIELD_TYPE == "uint8")
                    protocol_data.SUBPROTO_SUBFIELD_TYPE_WS = FT_UINT8;
                else if (protocol_data.SUBPROTO_SUBFIELD_TYPE == "uint16")
                    protocol_data.SUBPROTO_SUBFIELD_TYPE_WS = FT_UINT16;
                else if (protocol_data.SUBPROTO_SUBFIELD_TYPE == "uint24")
                    protocol_data.SUBPROTO_SUBFIELD_TYPE_WS = FT_UINT24;
                else if (protocol_data.SUBPROTO_SUBFIELD_TYPE == "uint32")
                    protocol_data.SUBPROTO_SUBFIELD_TYPE_WS = FT_UINT32;
                else if (protocol_data.SUBPROTO_SUBFIELD_TYPE == "string")
                    protocol_data.SUBPROTO_SUBFIELD_TYPE_WS = FT_STRING;
                else
                {
                    M_FATAL_COMMENT("Bad type on SUBFIELD line = " << line);
                }
                if (is_istream_empty(is_line) != true)
                {
                    string    key_word_from;
                    is_line >> key_word_from;
                    if (key_word_from != "from")
                    {
                        M_FATAL_COMMENT("Expecting key word from on SUBFIELD line = " << line);
                    }

                    is_line >> protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1;
                    is_line >> protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_2;
                    is_line >> protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_3;
                    if (protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1 == "")
                    {
                        M_FATAL_COMMENT("Expecting at least one field after from on SUBFIELD line = " << line);
                    }
                    if (is_istream_empty(is_line) != true)
                    {
                        M_FATAL_COMMENT("Too much data on SUBFIELD line = " << line);
                    }
                }
                if (protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1 == "")
                    protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1 = protocol_data.SUBPROTO_SUBFIELD;
            }
        }

        M_READ_VALUE(MSG_HEADER_TYPE)
        M_READ_VALUE(MSG_ID_FIELD_NAME)
        M_READ_VALUE(MSG_TITLE)
        M_READ_STRINGS(MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES, MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES)
        M_READ_VALUE(MSG_MAIN_TYPE)
        M_READ_VALUE(MSG_FROM_MAIN_TYPE)
        M_READ_VALUE(MSG_TO_MAIN_TYPE)
        M_READ_LINE(MSG_TOTAL_LENGTH)
        M_READ_VALUE(MSG_HEADER_LENGTH)
        M_READ_VALUE(MSG_TRAILER_LENGTH)
        M_READ_VALUE(GLOBAL_DATA_TYPE)
        M_READ_BOOL(PACKET_CONTAINS_ONLY_1_MSG)
        M_READ_BOOL(PACKET_CONTAINS_ONLY_COMPLETE_MSG)
        M_READ_BOOL(MANAGE_WIRESHARK_PINFO)
        else if (keyword == "STATISTICS")
        {
            read_file_wsgd_statistics(iss, stats);
        }
        else
        {
            M_FATAL_COMMENT("unexpected keyword=" << keyword);
        }
    }

    rtrim(protocol_data.PROTONAME);

    if (protocol_data.MSG_TRAILER_LENGTH > 0)
    {
        protocol_data.type_definitions.trailer_sizeof_bits = protocol_data.MSG_TRAILER_LENGTH * 8;
    }

    if (PROTO_TYPE_DEFINITIONS_found == false)
    {
        fatal_PROTO_TYPE_DEFINITIONS_not_found();
    }

    // Check/update PARENTS
    for (auto & parent : protocol_data.PARENTS)
    {
        // Transform strings to integer values (if they are integers).
        for (auto& value_str : parent.PARENT_SUBFIELD_VALUES_str)
        {
            long    subfield_val_int = 0;
            if (get_number(value_str.c_str(), &subfield_val_int))
            {
                parent.PARENT_SUBFIELD_VALUES_int.push_back(subfield_val_int);
            }
            else
            {
                remove_string_limits(value_str);
            }
        }

        if (parent.PARENT_SUBFIELD_VALUES_int.size() == parent.PARENT_SUBFIELD_VALUES_str.size())
        {
            // They are all integers.
            parent.PARENT_SUBFIELD_VALUES_str.clear();
        }
        else if (parent.PARENT_SUBFIELD_VALUES_int.empty())
        {
            // None are integers : nothing to do.
        }
        else
        {
            // Mixed integers and strings : NOT accepted.
            M_FATAL_COMMENT("PARENT_SUBFIELD_VALUES must be integers or strings, NOT both. NB: if you want a string like 12, use \"12\".");

        }
    }

    remove_string_limits(protocol_data.HEURISTIC_FUNCTION);
}

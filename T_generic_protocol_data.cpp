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

#include "T_generic_protocol_data.h"
#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "byte_interpret.h"
#include "byte_interpret_build_types_context.h"

#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <wsutil/filesystem.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

//*****************************************************************************
// 
//*****************************************************************************

// Only 1 data handle for all dissectors.
// Initialized only once.
dissector_handle_t  T_generic_protocol_subdissector_data::data_handle = nullptr;    // recursive call ok


/******************************************************************************
 * T_generic_protocol_ws_data
 *****************************************************************************/

#if 0

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <glib.h>
#include <epan/packet.h>


//*****************************************************************************
// dump_hf_data
//*****************************************************************************

void    dump_hf_data(const hf_register_info  * hf)
{
    M_FATAL_IF_FALSE(hf != nullptr);

    M_FATAL_IF_FALSE(hf->p_id != nullptr);
    trace_value_ptr ("c   hf->p_id",  hf->p_id);
    trace_value_int ("c  *hf->p_id", *hf->p_id);

    trace_value_str ("c   hf->hfinfo.name", hf->hfinfo.name);
    trace_value_str ("c   hf->hfinfo.abbrev", hf->hfinfo.abbrev);
    trace_value_int ("c   hf->hfinfo.type", hf->hfinfo.type);
    if (hf->hfinfo.type == 0)
    {
        M_FATAL_IF_FALSE(hf->hfinfo.display == 0);
    }
    else
        trace_value_int ("c   hf->hfinfo.display", hf->hfinfo.display);
    trace_value_ptr ("c   hf->hfinfo.strings", hf->hfinfo.strings);
    //trace_value_int ("c   hf->hfinfo.bitmask", hf->hfinfo.bitmask);
    //trace_value_str ("c   hf->hfinfo.blurb", hf->hfinfo.blurb);
    M_FATAL_IF_FALSE(hf->hfinfo.bitmask == 0);
    M_FATAL_IF_FALSE(hf->hfinfo.blurb == hf->hfinfo.name);

    //trace_value_int ("c   hf->hfinfo.id", hf->hfinfo.id);
    //trace_value_int ("c   hf->hfinfo.parent", hf->hfinfo.parent);
    //trace_value_int ("c   hf->hfinfo.ref_count", hf->hfinfo.ref_count);
    //trace_value_int ("c   hf->hfinfo.bitshift", hf->hfinfo.bitshift);
    //trace_value_ptr ("c   hf->hfinfo.same_name_next", hf->hfinfo.same_name_next);
    //trace_value_ptr ("c   hf->hfinfo.same_name_prev", hf->hfinfo.same_name_prev);
    M_FATAL_IF_FALSE(hf->hfinfo.id == 0);
    M_FATAL_IF_FALSE(hf->hfinfo.parent == 0);
    M_FATAL_IF_FALSE(hf->hfinfo.ref_count == 0);
    M_FATAL_IF_FALSE(hf->hfinfo.bitshift == 0);
    M_FATAL_IF_FALSE(hf->hfinfo.same_name_next == nullptr);
    M_FATAL_IF_FALSE(hf->hfinfo.same_name_prev == nullptr);
}

//*****************************************************************************
// dump_hf_datas
//*****************************************************************************

void    dump_hf_datas(const int                 nb_of_hfs,
                      const hf_register_info  * hf,
                      const char              * title)
{
    int   idx_data;
    trace_str (title);
    for (idx_data = 0; idx_data < nb_of_hfs; ++idx_data)
    {
        trace_value_int ("c   idx_data", idx_data);
        dump_hf_data(&hf[idx_data]);
    }
}

//*****************************************************************************
// dump_fields_data
//*****************************************************************************

void    dump_fields_data(const T_generic_protocol_ws_data  * P_protocol_ws_data,
                         const char                         * title,
                         const int                            before_register)
{
    int   idx_data;
    trace_str (title);
    for (idx_data = 0; idx_data <= P_protocol_ws_data->fields_data.max_field_idx; ++idx_data)
    {
        trace_value_int ("c   idx_data", idx_data);

        if ((idx_data > 0) && (before_register == FALSE))
        {
            M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.ett_id[idx_data] == P_protocol_ws_data->fields_data.ett_id[idx_data-1]+1);
        }
        else
            trace_value_int ("c   ett_id[idx_data]", P_protocol_ws_data->fields_data.ett_id[idx_data]);

        //trace_value_ptr ("c  &ett_id[idx_data]", &P_protocol_ws_data->fields_data.ett_id[idx_data]);
        //trace_value_ptr ("c   ett[idx_data]", P_protocol_ws_data->fields_data.ett[idx_data]);
        M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.ett[idx_data] == &P_protocol_ws_data->fields_data.ett_id[idx_data]);

        trace_value_int ("c   hf_id[idx_data]", P_protocol_ws_data->fields_data.hf_id[idx_data]);
        //trace_value_ptr ("c  &hf_id[idx_data]", &P_protocol_ws_data->fields_data.hf_id[idx_data]);
        //trace_value_ptr ("c   hf[idx_data].p_id", P_protocol_ws_data->fields_data.hf[idx_data].p_id);
        M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].p_id == &P_protocol_ws_data->fields_data.hf_id[idx_data]);

        trace_value_str ("c   hf[idx_data].hfinfo.name", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.name);
        trace_value_str ("c   hf[idx_data].hfinfo.abbrev", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.abbrev);
        trace_value_int ("c   hf[idx_data].hfinfo.type", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.type);
        if (P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.type == 0)
        {
            M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.display == 0);
        }
        else
            trace_value_int ("c   hf[idx_data].hfinfo.display", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.display);
        trace_value_ptr ("c   hf[idx_data].hfinfo.strings", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.strings);
        //trace_value_int ("c   hf[idx_data].hfinfo.bitmask", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.bitmask);
        //trace_value_str ("c   hf[idx_data].hfinfo.blurb", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.blurb);
        M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.bitmask == 0);
        M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.blurb == P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.name);

        trace_value_int ("c   hf[idx_data].hfinfo.id", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.id);
        trace_value_int ("c   hf[idx_data].hfinfo.parent", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.parent);
        trace_value_int ("c   hf[idx_data].hfinfo.ref_count", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.ref_count);
        trace_value_int ("c   hf[idx_data].hfinfo.bitshift", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.bitshift);
        trace_value_ptr ("c   hf[idx_data].hfinfo.same_name_next", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.same_name_next);
        trace_value_ptr ("c   hf[idx_data].hfinfo.same_name_prev", P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.same_name_prev);
        //M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.id == 0);
        //M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.parent == 0);
        //M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.ref_count == 0);
        //M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.bitshift == 0);
        //M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.same_name_next == nullptr);
        //M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.same_name_prev == nullptr);
    }
}
#endif

/******************************************************************************
 * dissect_generic_<proto_idx>
 *****************************************************************************/

int   S_proto_idx_dissect_in_progress = -1;

#define M_DEFINE_DISSECT_FCT(PROTO_IDX)                                                   \
static gint                                                                               \
dissect_generic_ ## PROTO_IDX (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *)       \
{                                                                                         \
    M_TRACE_ENTER("dissect_generic_proto", PROTO_IDX);                                    \
                                                                                          \
    const int   previous_proto_idx_dissect_in_progress = S_proto_idx_dissect_in_progress; \
    S_proto_idx_dissect_in_progress = PROTO_IDX;                                          \
                                                                                          \
    const int   result = dissect_generic_proto(PROTO_IDX, tvb, pinfo, tree);              \
                                                                                          \
    S_proto_idx_dissect_in_progress = previous_proto_idx_dissect_in_progress;             \
                                                                                          \
    return  result;                                                                       \
}

#define M_DEFINE_HEURISTIC_FCT(PROTO_IDX)                                                 \
static gboolean                                                                           \
heuristic_generic_ ## PROTO_IDX (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *)     \
{                                                                                         \
    M_TRACE_ENTER("heuristic_generic_proto", PROTO_IDX);                                  \
                                                                                          \
    const int   previous_proto_idx_dissect_in_progress = S_proto_idx_dissect_in_progress; \
    S_proto_idx_dissect_in_progress = PROTO_IDX;                                          \
                                                                                          \
    const int   result = heuristic_generic_proto(PROTO_IDX, tvb, pinfo, tree);            \
                                                                                          \
    S_proto_idx_dissect_in_progress = previous_proto_idx_dissect_in_progress;             \
                                                                                          \
    return  result;                                                                       \
}


#define M_DEFINE_PROTO_FCT(PROTO_IDX)                                                     \
        M_DEFINE_DISSECT_FCT(PROTO_IDX)                                                   \
        M_DEFINE_HEURISTIC_FCT(PROTO_IDX)


M_DEFINE_PROTO_FCT(0)
M_DEFINE_PROTO_FCT(1)
M_DEFINE_PROTO_FCT(2)
M_DEFINE_PROTO_FCT(3)
M_DEFINE_PROTO_FCT(4)
M_DEFINE_PROTO_FCT(5)
M_DEFINE_PROTO_FCT(6)
M_DEFINE_PROTO_FCT(7)
M_DEFINE_PROTO_FCT(8)
M_DEFINE_PROTO_FCT(9)
M_DEFINE_PROTO_FCT(10)
M_DEFINE_PROTO_FCT(11)
M_DEFINE_PROTO_FCT(12)
M_DEFINE_PROTO_FCT(13)
M_DEFINE_PROTO_FCT(14)
M_DEFINE_PROTO_FCT(15)
M_DEFINE_PROTO_FCT(16)
M_DEFINE_PROTO_FCT(17)
M_DEFINE_PROTO_FCT(18)
M_DEFINE_PROTO_FCT(19)

#undef M_DEFINE_PROTO_FCT

//*****************************************************************************
// The protocol's data
//*****************************************************************************

/*static*/ vector<T_generic_protocol_data>    S_protocol_data;

T_generic_protocol_data  & get_protocol_data(const int   proto_idx)
{
    M_FATAL_IF_GE(proto_idx, static_cast<long>(S_protocol_data.size()));

    return  S_protocol_data[proto_idx];
}

T_generic_protocol_data  & new_protocol_data(const int   proto_idx)
{
    M_FATAL_IF_GE(proto_idx, static_cast<long>(S_protocol_data.size()));

    S_protocol_data[proto_idx].proto_idx = proto_idx;

#define M_DEFINE_PROTO_FCT(PROTO_IDX)                                                         \
    else if (proto_idx == PROTO_IDX)                                                          \
    {                                                                                         \
        S_protocol_data[proto_idx].ws_data.P_dissect_fct   =   dissect_generic_ ## PROTO_IDX; \
        S_protocol_data[proto_idx].ws_data.P_heuristic_fct = heuristic_generic_ ## PROTO_IDX; \
    }

    if (FALSE)  ;
    M_DEFINE_PROTO_FCT(0)
    M_DEFINE_PROTO_FCT(1)
    M_DEFINE_PROTO_FCT(2)
    M_DEFINE_PROTO_FCT(3)
    M_DEFINE_PROTO_FCT(4)
    M_DEFINE_PROTO_FCT(5)
    M_DEFINE_PROTO_FCT(6)
    M_DEFINE_PROTO_FCT(7)
    M_DEFINE_PROTO_FCT(8)
    M_DEFINE_PROTO_FCT(9)
    M_DEFINE_PROTO_FCT(10)
    M_DEFINE_PROTO_FCT(11)
    M_DEFINE_PROTO_FCT(12)
    M_DEFINE_PROTO_FCT(13)
    M_DEFINE_PROTO_FCT(14)
    M_DEFINE_PROTO_FCT(15)
    M_DEFINE_PROTO_FCT(16)
    M_DEFINE_PROTO_FCT(17)
    M_DEFINE_PROTO_FCT(18)
    M_DEFINE_PROTO_FCT(19)

#undef M_DEFINE_PROTO_FCT

    M_FATAL_IF_EQ(S_protocol_data[proto_idx].ws_data.P_dissect_fct, nullptr);


    return  S_protocol_data[proto_idx];
}

// This could NOT work because
//  it could happen that the protocol is already registered and in this case :
// - I could NOT unregister it        and so
// - I must keep data because whireshark use them (proto names)
// So if I remove data, all following protocols are said already existent !!!
#if 0
void    remove_protocol_data_if_exist(const int   proto_idx)
{
    M_FATAL_IF_GE(proto_idx, static_cast<long>(S_protocol_data.size()));

    // WARNING_ADDRESSES
    // NB: this function is supposed to be called only during register initialisation.
    // At this moment, the protocols after this one are supposed NOT initialized.
    // pop_back do NOT change the address of the vector.
    S_protocol_data[proto_idx] = T_generic_protocol_data();
    S_protocol_data.pop_back();

    return;
}
#endif

T_generic_protocol_data  & get_protocol_data_from_proto_abbrev(const char  * proto_abbrev)
{
    for (vector<T_generic_protocol_data>::iterator  iter  = S_protocol_data.begin();
                                                    iter != S_protocol_data.end();
                                                  ++iter)
    {
        if (proto_abbrev == iter->PROTOABBREV)
        {
            return  *iter;
        }
    }

    M_FATAL_COMMENT("Proto abbrev " << proto_abbrev << " NOT found.");

    return  *(T_generic_protocol_data*)nullptr;        // avoid compiler warning
}

void    set_max_nb_of_protocol_data(const size_t   max_nb)
{
    M_TRACE_ENTER("set_max_nb_of_protocol_data", max_nb);

    // WARNING_ADDRESSES
    // It is mandatory to add all items now.
    for (size_t proto_idx = 0; proto_idx < max_nb; ++proto_idx)
    {
        T_generic_protocol_data    protocol_data;

        // Do NOT set proto_idx attribute.
        // It permits to know if protocol_data has been initialized.

        S_protocol_data.push_back(protocol_data);
    }
}

//*****************************************************************************
// add_initial_types
//*****************************************************************************
#if 0
void    add_initial_types (T_generic_protocol_data  & protocol_data)
{
    // Define bool8 (and others) as enums.
    // Necessary for wireshark display filters accept "false" and "true".
    //  if (protocol_data.type_definitions.is_a_type_name("bool8") == false)
    {
        istringstream           iss("enum8     bool8   { false 0  true 1 }");

        const string    result = build_types(iss, protocol_data.type_definitions);
        M_FATAL_IF_NE(result, "");
    }

    //  if (protocol_data.type_definitions.is_a_type_name("bool16") == false)
    {
        istringstream           iss("enum16    bool16  as  bool8 ;");

        const string    result = build_types(iss, protocol_data.type_definitions);
        M_FATAL_IF_NE(result, "");
    }
    //  if (protocol_data.type_definitions.is_a_type_name("bool32") == false)
    {
        istringstream           iss("enum32    bool32  as  bool8 ;");

        const string    result = build_types(iss, protocol_data.type_definitions);
        M_FATAL_IF_NE(result, "");
    }
    //  if (protocol_data.type_definitions.is_a_type_name("bool1") == false)
    {
        istringstream           iss("enum1    bool1  as  bool8 ;");

        const string    result = build_types(iss, protocol_data.type_definitions);
        M_FATAL_IF_NE(result, "");
    }
}
#endif
//*****************************************************************************
// read_file_wsgd
//*****************************************************************************

void    read_file_wsgd (const string                   & wsgd_file_name,
                              T_generic_protocol_data  & protocol_data)
{
    M_TRACE_ENTER("read_file_wsgd", wsgd_file_name);
    ifstream              ifs(wsgd_file_name.c_str());
    if (!ifs)
    {
        M_FATAL_COMMENT("Impossible to open file " << wsgd_file_name);
    }

    const std::string file_content{ std::istreambuf_iterator<char>(ifs),
                                    std::istreambuf_iterator<char>() };
    istringstream  iss(file_content);

    // Read the 1st part of the file (until PROTO_TYPE_DEFINITIONS)
    read_file_wsgd_until_types(iss, protocol_data, protocol_data.ws_data.tap_data.stats);

    // Read the type definitions of the protocol.
    C_debug_set_temporary  debug_register_proto_main(protocol_data.DEBUG);
    C_debug_set_temporary  debug_type_definitions;
    bool                   must_ouput_types = false;

    //  add_initial_types (protocol_data);
    string    wsgd_file_name_short = wsgd_file_name;
    {
        const string::size_type  idx_last_dir = wsgd_file_name_short.find_last_of("/\\");
        if (idx_last_dir != string::npos)
        {
            wsgd_file_name_short.erase(0, idx_last_dir + 1);
        }
    }
    build_types_context_include_file_open(wsgd_file_name_short);

    {
        protocol_data.type_definitions.map_const_value["shark::progfile_dir"] = get_progfile_dir();
        protocol_data.type_definitions.map_const_value["shark::plugin_dir"] = get_plugins_dir_with_version();
        protocol_data.type_definitions.map_const_value["shark::datafile_dir"] = get_datafile_dir();
        protocol_data.type_definitions.map_const_value["shark::systemfile_dir"] = get_systemfile_dir();
        protocol_data.type_definitions.map_const_value["shark::profiles_dir"] = get_profiles_dir();
        protocol_data.type_definitions.map_const_value["shark::persdatafile_dir"] = get_persdatafile_dir();
    }

    while (is_istream_empty(iss) != true)
    {
        const string    result = build_types(iss,
                                             protocol_data.type_definitions);

        if (result == "DEBUG")
        {
            debug_type_definitions.set(E_debug_status_ON);
            continue;
        }
        else if (result == "DEBUG_NO_TIME")
        {
            debug_type_definitions.set(E_debug_status_ON_NO_TIME);
            continue;
        }

        M_FATAL_IF_NE(result, "");
    }

    build_types_context_include_file_close(wsgd_file_name_short);

    // Verify that all mandatory data have been initialized.
    protocol_data.check_config_parameters_initialized();

    // Check HEURISTIC_FUNCTION
    {
        if (protocol_data.HEURISTIC_FUNCTION != "")
        {
            if (protocol_data.type_definitions.is_a_function(protocol_data.HEURISTIC_FUNCTION) == false)
            {
                M_FATAL_COMMENT("HEURISTIC_FUNCTION (" << protocol_data.HEURISTIC_FUNCTION << ") is NOT a defined function name");
            }
        }
    }


    // Check MSG_HEADER_TYPE.
    if (protocol_data.type_definitions.is_a_defined_type_name(protocol_data.MSG_HEADER_TYPE) == false)
    {
        M_FATAL_COMMENT("MSG_HEADER_TYPE (" << protocol_data.MSG_HEADER_TYPE << ") is NOT a defined type name");
    }

    // Check GLOBAL_DATA_TYPE.
    if (protocol_data.GLOBAL_DATA_TYPE != "")
    {
        if (protocol_data.type_definitions.is_a_struct(protocol_data.GLOBAL_DATA_TYPE) == false)
        {
            M_FATAL_COMMENT("GLOBAL_DATA_TYPE (" << protocol_data.GLOBAL_DATA_TYPE << ") is NOT a defined struct type name");
        }
    }

    string    switch_orig_type;
    string    switch_discriminant_str;

    // Check MSG_MAIN_TYPE.
    if ((protocol_data.MSG_MAIN_TYPE != "") &&
        (protocol_data.type_definitions.is_a_defined_type_name(protocol_data.MSG_MAIN_TYPE) == false) &&
        (is_a_switch_value(protocol_data.type_definitions, protocol_data.MSG_MAIN_TYPE, switch_orig_type, switch_discriminant_str) == false))
    {
        M_FATAL_COMMENT("MSG_MAIN_TYPE (" << protocol_data.MSG_MAIN_TYPE << ") is NOT a defined type name");
    }

    // Check MSG_FROM_MAIN_TYPE.
    if ((protocol_data.MSG_FROM_MAIN_TYPE != "") &&
        (protocol_data.type_definitions.is_a_defined_type_name(protocol_data.MSG_FROM_MAIN_TYPE) == false) &&
        (is_a_switch_value(protocol_data.type_definitions, protocol_data.MSG_FROM_MAIN_TYPE, switch_orig_type, switch_discriminant_str) == false))
    {
        M_FATAL_COMMENT("MSG_FROM_MAIN_TYPE (" << protocol_data.MSG_FROM_MAIN_TYPE << ") is NOT a defined type name");
    }

    // Check MSG_TO_MAIN_TYPE.
    if ((protocol_data.MSG_TO_MAIN_TYPE != "") &&
        (protocol_data.type_definitions.is_a_defined_type_name(protocol_data.MSG_TO_MAIN_TYPE) == false) &&
        (is_a_switch_value(protocol_data.type_definitions, protocol_data.MSG_TO_MAIN_TYPE, switch_orig_type, switch_discriminant_str) == false))
    {
        M_FATAL_COMMENT("MSG_TO_MAIN_TYPE (" << protocol_data.MSG_TO_MAIN_TYPE << ") is NOT a defined type name");
    }

    if (must_ouput_types && get_debug())
    {
        get_state_ostream() << protocol_data.type_definitions;
    }
}

//*****************************************************************************
// get_P_interpret_read_values_of_msg
//*****************************************************************************
#if 0
const T_interpret_read_values  * get_P_interpret_read_values_of_msg(int   msg_id)
{
    // Retrieve the current protocol.
    M_FATAL_IF_LT(S_proto_idx_dissect_in_progress, 0);
    M_FATAL_IF_GE(S_proto_idx_dissect_in_progress, S_protocol_data.size());

    T_generic_protocol_data  & protocol_data = S_protocol_data[S_proto_idx_dissect_in_progress];

    // Retrieve the specified message.
    T_generic_protocol_global_data  & global_data = protocol_data.ws_data.global_data;
    M_FATAL_IF_LT(msg_id, 0);
    M_FATAL_IF_GE(msg_id, global_data.saved_interpreted_datas.size());

    if (msg_id == global_data.saved_interpreted_datas.size())
    {
        // ICIOA c'est le message courant
    }

    return  global_data.saved_interpreted_datas[msg_id].RCP_interpret_data;
}
#endif

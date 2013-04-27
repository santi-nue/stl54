/* generic.c
 * Copyright 2008-2012 Olivier Aveline <wsgd@free.fr>
 *
 * $Id: 
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
#include "byte_interpret_build_types_netpdl.h"

#include <fstream>

//*****************************************************************************
// 
//*****************************************************************************

// Only 1 data handle for all dissectors.
// Initialized only once.
dissector_handle_t  T_generic_protocol_subdissector_data::data_handle = NULL;    // recursive call ok


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
	M_FATAL_IF_FALSE(hf != NULL);

	M_FATAL_IF_FALSE(hf->p_id != NULL);
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
	M_FATAL_IF_FALSE(hf->hfinfo.same_name_next == NULL);
	M_FATAL_IF_FALSE(hf->hfinfo.same_name_prev == NULL);
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
		//M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.same_name_next == NULL);
		//M_FATAL_IF_FALSE(P_protocol_ws_data->fields_data.hf[idx_data].hfinfo.same_name_prev == NULL);
    }
}
#endif

//*****************************************************************************
// T_generic_protocol_data
//*****************************************************************************
void
T_generic_protocol_data::check_config_parameters_initialized() const
{
  ::check_initialized(PROTONAME);
  ::check_initialized(PROTOSHORTNAME);
  ::check_initialized(PROTOABBREV);

  if (PARENTS.empty() &&
	  PARENTS_HEURISTIC.empty())
  {
    M_FATAL_COMMENT("Must set PARENT_SUBFIELD & PARENT_SUBFIELD_VALUES");
  }
  for (vector<T_generic_protocol_data::T_parent>::const_iterator
									parent_iter  = PARENTS.begin();
									parent_iter != PARENTS.end();
								  ++parent_iter)
  {
	  const T_generic_protocol_data::T_parent  & parent = * parent_iter;
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

/******************************************************************************
 * dissect_generic_<proto_idx>
 *****************************************************************************/

int   S_proto_idx_dissect_in_progress = -1;

#define M_DEFINE_DISSECT_FCT(PROTO_IDX)                                                   \
static gint                                                                               \
dissect_generic_ ## PROTO_IDX (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)       \
{                                                                                         \
	M_STATE_ENTER("dissect_generic_proto", PROTO_IDX);                                    \
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
heuristic_generic_ ## PROTO_IDX (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)     \
{                                                                                         \
	M_STATE_ENTER("heuristic_generic_proto", PROTO_IDX);                                  \
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

	M_FATAL_IF_EQ(S_protocol_data[proto_idx].ws_data.P_dissect_fct, NULL);


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

	return  *(T_generic_protocol_data*)NULL;        // avoid compiler warning
}

void    set_max_nb_of_protocol_data(const int   max_nb)
{
  M_STATE_ENTER("set_max_nb_of_protocol_data", max_nb);

  M_FATAL_IF_LT(max_nb, 0);

  // WARNING_ADDRESSES
  // It is mandatory to add all items now.
  for (int  proto_idx = 0; proto_idx < max_nb; ++proto_idx)
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
    istrstream           iss("enum8     bool8   { false 0  true 1 }");

	const string    result = build_types (iss, protocol_data.type_definitions);
    M_FATAL_IF_NE(result, "");
  }

//  if (protocol_data.type_definitions.is_a_type_name("bool16") == false)
  {
    istrstream           iss("enum16    bool16  as  bool8 ;");

	const string    result = build_types (iss, protocol_data.type_definitions);
    M_FATAL_IF_NE(result, "");
  }
//  if (protocol_data.type_definitions.is_a_type_name("bool32") == false)
  {
    istrstream           iss("enum32    bool32  as  bool8 ;");

	const string    result = build_types (iss, protocol_data.type_definitions);
    M_FATAL_IF_NE(result, "");
  }
//  if (protocol_data.type_definitions.is_a_type_name("bool1") == false)
  {
    istrstream           iss("enum1    bool1  as  bool8 ;");

	const string    result = build_types (iss, protocol_data.type_definitions);
    M_FATAL_IF_NE(result, "");
  }
}
#endif
//*****************************************************************************
// read_file_wsgd
//*****************************************************************************

void    read_file_wsgd (const string                       & wsgd_file_name,
						      T_generic_protocol_data  & protocol_data)
{
  M_STATE_ENTER ("read_file_wsgd", wsgd_file_name);
  ifstream              ifs (wsgd_file_name.c_str());
  if (!ifs)
  {
    M_FATAL_COMMENT("Impossible to open file " << wsgd_file_name);
  }

// string ou int
#define M_READ_NAME_VALUE(NAME,VARIABLE)                     \
  else if (keyword == #NAME)  {                              \
    check_not_already_initialized(VARIABLE);                 \
    ifs >> VARIABLE;                                         \
    M_STATE_DEBUG (#NAME << " = " << VARIABLE);              \
  }

#define M_READ_VALUE(NAME)                                   \
	M_READ_NAME_VALUE(NAME,protocol_data.NAME)

// bool
#define M_READ_NAME_BOOL(NAME,VARIABLE)                      \
  else if (keyword == #NAME)  {                              \
	string    word;                                          \
    ifs >> word;                                             \
	if ((word == "false") || (word == "no")) {               \
      VARIABLE = false;                                      \
	}                                                        \
    else if ((word == "true") || (word == "yes")) {          \
      VARIABLE = true;                                       \
	}                                                        \
    else {                                                   \
	  M_FATAL_COMMENT("Bad value for " #NAME);               \
	}                                                        \
    M_STATE_DEBUG (#NAME << " = " << VARIABLE);              \
  }

// bool
#define M_READ_BOOL(NAME)                                    \
	M_READ_NAME_BOOL(NAME,protocol_data.NAME)

// vector<int>
#define M_READ_VALUES(NAME)                                 \
  else if (keyword == #NAME)  {                             \
	string    line;                                         \
    getline (ifs, line);                                    \
	istrstream  is_line(line.c_str());                      \
    while (is_istream_empty(is_line) != true)               \
    {                                                       \
	  int    val;                                           \
      is_line >> val;                                       \
	  protocol_data.NAME.push_back(val);                    \
	}                                                       \
  }

// vector<string>
#define M_READ_STRINGS(NAME,VARIABLE)                       \
  else if (keyword == #NAME)  {                             \
    ifs >> ws;                                              \
	string    line;                                         \
    getline (ifs, line);                                    \
	istrstream  is_line(line.c_str());                      \
    while (is_istream_empty(is_line) != true)               \
    {                                                       \
	  string    val;                                        \
      is_line >> val;                                       \
	  protocol_data.VARIABLE.push_back(val);                \
	}                                                       \
  }

// string
#define M_READ_LINE(NAME)                                   \
  else if (keyword == #NAME)  {                             \
    check_not_already_initialized(protocol_data.NAME);      \
    ifs >> ws;                                              \
    getline (ifs, protocol_data.NAME);                      \
    const string::size_type  NAME_size = protocol_data.NAME.size(); \
    if ((NAME_size > 0) &&                                  \
        (protocol_data.NAME[NAME_size-1] == '\r'))          \
    {                                                       \
        protocol_data.NAME.erase(NAME_size-1);              \
    }                                                       \
    M_STATE_DEBUG (#NAME << " = " << protocol_data.NAME);   \
  }

  // Read the 1st part of the file (until PROTO_TYPE_DEFINITIONS).
  while (is_istream_empty(ifs) != true)
  {
    string    keyword;
	ifs >> keyword;

	if (keyword == "DEBUG")
	{
		protocol_data.DEBUG = true;
//		set_debug(protocol_data.DEBUG);
		continue;
	}
	else if (keyword == "PROTO_TYPE_DEFINITIONS")
	{
	  break;
	}
	M_READ_LINE(PROTONAME)
	M_READ_VALUE(PROTOSHORTNAME)
	M_READ_VALUE(PROTOABBREV)

    else if (keyword == "PARENT_SUBFIELD")
	{
		protocol_data.PARENTS.push_back(T_generic_protocol_data::T_parent());
		ifs >> protocol_data.PARENTS.back().PARENT_SUBFIELD;
	    M_STATE_DEBUG ("PARENT_SUBFIELD = " << protocol_data.PARENTS.back().PARENT_SUBFIELD);
	}
	M_READ_STRINGS(PARENT_SUBFIELD_VALUES,PARENTS.back().PARENT_SUBFIELD_VALUES_str)
    else if (keyword == "PARENT_SUBFIELD_RANGE")
	{
		pair<int,int>    low_high;
		ifs >> low_high.first;
		ifs >> low_high.second;
		protocol_data.PARENTS.back().PARENT_SUBFIELD_RANGES_int.push_back(low_high);
	}
	else if (keyword == "PARENT_HEURISTIC")
	{
		string   parent_name;
		ifs >> parent_name;
		protocol_data.PARENTS_HEURISTIC.push_back(parent_name);
	}

    else if (keyword == "SUBFIELD")
	{
	    check_not_already_initialized(protocol_data.SUBPROTO_SUBFIELD);
	    ifs >> ws;
		string    line;
	    getline (ifs, line);
	    {
          istrstream  is_line(line.c_str());
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
    else if (keyword == "PACKET_ID_FIELD_TYPE")
	{
		M_STATE_ERROR("PACKET_ID_FIELD_TYPE is no more used -> remove it");
		string  do_not_care;
	    ifs >> do_not_care;
	}
	M_READ_STRINGS(MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES,MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES)
	M_READ_VALUE(MSG_MAIN_TYPE)
	M_READ_VALUE(MSG_FROM_MAIN_TYPE)
	M_READ_VALUE(MSG_TO_MAIN_TYPE)
	M_READ_LINE(MSG_TOTAL_LENGTH)
	M_READ_VALUE(MSG_HEADER_LENGTH)
	M_READ_VALUE(GLOBAL_DATA_TYPE)
	M_READ_BOOL(PACKET_CONTAINS_ONLY_1_MSG)
	M_READ_BOOL(PACKET_CONTAINS_ONLY_COMPLETE_MSG)
	M_READ_BOOL(MANAGE_WIRESHARK_PINFO)
	else
	{
		M_FATAL_COMMENT("unexpected keyword=" << keyword);
	}
  }

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
		wsgd_file_name_short.erase(0, idx_last_dir+1);
	}
  }
  build_types_context_include_file_open (wsgd_file_name_short);

  while (is_istream_empty(ifs) != true)
  {
	const string    result = build_types (ifs,
										  protocol_data.type_definitions);

	if (result == "DEBUG")
	{
		debug_type_definitions.set(true);
		continue;
	}
	else if (result == "NETPDL")
	{
        string  file_name;
	    ifs >> file_name;
		build_netpdl_types (file_name, protocol_data.type_definitions);
		must_ouput_types = true;
		continue;
	}

	M_FATAL_IF_NE(result, "");
  }

  build_types_context_include_file_close(wsgd_file_name_short);

  // Verify that all mandatory data have been initialized.
  protocol_data.check_config_parameters_initialized();

  for (vector<T_generic_protocol_data::T_parent>::iterator
									parent_iter  = protocol_data.PARENTS.begin();
									parent_iter != protocol_data.PARENTS.end();
								  ++parent_iter)
  {
	  T_generic_protocol_data::T_parent  & parent = * parent_iter;

	  // Transform strings to integer values (if they are integers).
	  for (vector<string>::iterator   iter  = parent.PARENT_SUBFIELD_VALUES_str.begin();
									  iter != parent.PARENT_SUBFIELD_VALUES_str.end();
									++iter)
	  {
		  long    subfield_val_int = 0;
		  if (get_number(iter->c_str(), &subfield_val_int))
		  {
			  parent.PARENT_SUBFIELD_VALUES_int.push_back(subfield_val_int);
		  }
		  else
		  {
			  remove_string_limits(*iter);
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
	  (is_a_switch (protocol_data.type_definitions, protocol_data.MSG_MAIN_TYPE, switch_orig_type, switch_discriminant_str) == false))
  {
	  M_FATAL_COMMENT("MSG_MAIN_TYPE (" << protocol_data.MSG_MAIN_TYPE << ") is NOT a defined type name");
  }

  // Check MSG_FROM_MAIN_TYPE.
  if ((protocol_data.MSG_FROM_MAIN_TYPE != "") &&
	  (protocol_data.type_definitions.is_a_defined_type_name(protocol_data.MSG_FROM_MAIN_TYPE) == false) &&
	  (is_a_switch (protocol_data.type_definitions, protocol_data.MSG_FROM_MAIN_TYPE, switch_orig_type, switch_discriminant_str) == false))
  {
	  M_FATAL_COMMENT("MSG_FROM_MAIN_TYPE (" << protocol_data.MSG_FROM_MAIN_TYPE << ") is NOT a defined type name");
  }

  // Check MSG_TO_MAIN_TYPE.
  if ((protocol_data.MSG_TO_MAIN_TYPE != "") &&
	  (protocol_data.type_definitions.is_a_defined_type_name(protocol_data.MSG_TO_MAIN_TYPE) == false) &&
	  (is_a_switch (protocol_data.type_definitions, protocol_data.MSG_TO_MAIN_TYPE, switch_orig_type, switch_discriminant_str) == false))
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

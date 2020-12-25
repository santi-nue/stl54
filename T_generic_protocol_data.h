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

#ifndef __T_GENERIC_PROTOCOL_DATA_H__
#define __T_GENERIC_PROTOCOL_DATA_H__

//*****************************************************************************
// Includes
//*****************************************************************************

#include "config.h"

#include <glib.h>
#include <epan/packet.h>
#include <epan/expert.h>

#include <string>
#include <vector>
#include <utility>
using namespace std;

#include "byte_interpret_build_types.h"
#include "T_interpret_data.h"
#include "T_generic_statistics.h"


/******************************************************************************
 * T_generic_protocol_ws_data
 *****************************************************************************/

struct T_generic_protocol_fields_data : public CT_debug_object_counter<T_generic_protocol_fields_data>
{
  vector<gint>                hf_id;
  vector<gint>                ett_id;
  vector<hf_register_info>    hf;         // WARNING_ADDRESSES
  vector<gint *>              ett;        // WARNING_ADDRESSES
};

struct T_generic_protocol_enum_value : public CT_debug_object_counter<T_generic_protocol_enum_value>
{
  vector<value_string>    value_strings;
};

struct T_generic_protocol_enum_values_data : public CT_debug_object_counter<T_generic_protocol_enum_values_data>
{
  vector<T_generic_protocol_enum_value>    enum_values;
};

struct T_generic_protocol_expert_data : public CT_debug_object_counter<T_generic_protocol_expert_data>
{
  expert_field              ei_malformed_comment;
  expert_field              ei_malformed_chat;
  expert_field              ei_malformed_note;
  expert_field              ei_malformed_warn;
  expert_field              ei_malformed_error;
  vector<ei_register_info>  ei;

  T_generic_protocol_expert_data()
  {
    expert_field  ei_initializer = EI_INIT;
    ei_malformed_comment = ei_initializer;
    ei_malformed_chat = ei_initializer;
    ei_malformed_note = ei_initializer;
    ei_malformed_warn = ei_initializer;
    ei_malformed_error = ei_initializer;
  }
};

struct T_generic_protocol_subdissector_data : public CT_debug_object_counter<T_generic_protocol_subdissector_data>
{
  dissector_table_t      dissector_table;
  heur_dissector_list_t  heur_dissector_list;
  gboolean               try_heuristic_first;

  static
  dissector_handle_t     data_handle;

  T_generic_protocol_subdissector_data()
      :dissector_table (NULL),
       heur_dissector_list (NULL),
       try_heuristic_first (FALSE)
  { }
};

struct T_generic_protocol_tap_data : public CT_debug_object_counter<T_generic_protocol_tap_data>
{
  int                    proto_tap;
  T_stats                stats;
  bool                   tap_is_needed;    // for statistics
  T_RCP_interpret_data   RCP_last_msg_interpret_data;

  T_generic_protocol_tap_data()
      :proto_tap (-1),
       tap_is_needed(false)
  { }
};

struct T_generic_protocol_saved_interpreted_data : public CT_debug_object_counter<T_generic_protocol_saved_interpreted_data>
{
  long                    packet_number;
  long                    msg_number_inside_packet;
  T_RCP_interpret_data    RCP_interpret_data;

  T_generic_protocol_saved_interpreted_data()
      :packet_number(0),
       msg_number_inside_packet(0)
  { }
};

struct T_generic_protocol_global_data : public CT_debug_object_counter<T_generic_protocol_global_data>
{
  // Contains the initial values of global data
  T_RCP_interpret_data    RCP_initialized_data;

  // Contains all interpret_data of msg
  vector<T_generic_protocol_saved_interpreted_data>  saved_interpreted_datas;

  T_generic_protocol_global_data()
      : RCP_initialized_data(new T_interpret_data())
  {
  }
};

struct T_generic_protocol_ws_data : public CT_debug_object_counter<T_generic_protocol_ws_data>
{
  /* Wireshark ID of the protocol */
  int       proto_generic;

  gint      ( * P_dissect_fct )(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *);
  gboolean  ( * P_heuristic_fct )(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *);

  dissector_handle_t                    dissector_handle;

  T_generic_protocol_fields_data        fields_data;
  T_generic_protocol_enum_values_data   enum_values_data;
  T_generic_protocol_expert_data        expert_data;

  T_generic_protocol_subdissector_data  subdissector_data;

  T_generic_protocol_tap_data           tap_data;

  T_generic_protocol_global_data        global_data;

  T_generic_protocol_ws_data()
      :proto_generic (-1),
       P_dissect_fct (NULL),
       dissector_handle (NULL)
  { }
};

//*****************************************************************************
// T_generic_protocol_data
//*****************************************************************************

struct T_generic_protocol_data : public CT_debug_object_counter<T_generic_protocol_data>
{
  int       proto_idx;  // identify the proto

  bool      is_proto_usable() const        { return  proto_idx >= 0; }
  void      proto_is_NOT_usable()          { proto_idx = -1; }

  string    wsgd_file_name;

  E_debug_status  DEBUG;

  string    PROTONAME;
  string    PROTOSHORTNAME;
  string    PROTOABBREV;

  struct T_parent
  {
    string                   PARENT_SUBFIELD;
    vector<int>              PARENT_SUBFIELD_VALUES_int;
    vector<string>           PARENT_SUBFIELD_VALUES_str;
    vector<pair<int,int> >   PARENT_SUBFIELD_RANGES_int;
  };
  vector<T_parent>  PARENTS;

  vector<string>    PARENTS_HEURISTIC;
  string            HEURISTIC_FUNCTION;

  vector<string>    ADD_FOR_DECODE_AS_TABLES;

  string            SUBPROTO_SUBFIELD;
  string            SUBPROTO_SUBFIELD_PARAM;
  string            SUBPROTO_SUBFIELD_PARAM_UI;
  string            SUBPROTO_SUBFIELD_TYPE;
  ftenum_t          SUBPROTO_SUBFIELD_TYPE_WS;
  string            SUBPROTO_SUBFIELD_FROM_REAL_1;
  string            SUBPROTO_SUBFIELD_FROM_REAL_2;
  string            SUBPROTO_SUBFIELD_FROM_REAL_3;

  bool              PACKET_CONTAINS_ONLY_1_MSG;
  bool              PACKET_CONTAINS_ONLY_COMPLETE_MSG;
  bool              MANAGE_WIRESHARK_PINFO;

  string            MSG_HEADER_TYPE;
  string            MSG_ID_FIELD_NAME;
  string            MSG_TITLE;
  vector<string>    MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES;
  string            MSG_MAIN_TYPE;
  string            MSG_FROM_MAIN_TYPE;
  string            MSG_TO_MAIN_TYPE;
  string            MSG_TOTAL_LENGTH;
  int               MSG_HEADER_LENGTH;
  int               MSG_TRAILER_LENGTH;

  string            GLOBAL_DATA_TYPE;

  T_type_definitions    type_definitions;

  void    check_config_parameters_initialized() const;

  T_generic_protocol_ws_data    ws_data;

  T_generic_protocol_data()
      : proto_idx(-1)
      , DEBUG(E_debug_status_OFF)
      , PACKET_CONTAINS_ONLY_1_MSG(false)
      , PACKET_CONTAINS_ONLY_COMPLETE_MSG(false)
      , MANAGE_WIRESHARK_PINFO(false)
      , MSG_HEADER_LENGTH(-1)
      , MSG_TRAILER_LENGTH(-1)
  { }
};

/******************************************************************************
 * called by dissect_generic_<proto_idx>
 * called by heuristic_generic_<proto_idx>
 *****************************************************************************/
gint
dissect_generic_proto(const int    proto_idx, tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree);

gboolean
heuristic_generic_proto(const int    proto_idx, tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree);

//*****************************************************************************
// The protocol's data
//*****************************************************************************

T_generic_protocol_data  & get_protocol_data(const int   proto_idx);
T_generic_protocol_data  & new_protocol_data(const int   proto_idx);

T_generic_protocol_data  & get_protocol_data_from_proto_abbrev(const char  * proto_abbrev);

void    set_max_nb_of_protocol_data(const size_t   max_nb);

//*****************************************************************************
// read_file_wsgd
//*****************************************************************************

void    read_file_wsgd (const string                   & wsgd_file_name,
                              T_generic_protocol_data  & protocol_data);


#endif /* __T_GENERIC_PROTOCOL_DATA_H__ */

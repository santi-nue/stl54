/*
 * Copyright 2008-2015 Olivier Aveline <wsgd@free.fr>
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

#include "byte_interpret_builder.h"
#include "byte_interpret.h"                 // is_a_raw_field ...

#include "C_byte_interpret_wsgd_builder.h"
#include "T_generic_protocol_data.h"

#if WIRESHARK_VERSION_NUMBER < 10600
#define dissector_try_uint  dissector_try_port
#endif


/******************************************************************************
 * dissector_try_heuristic
 * return a gboolean !
 *****************************************************************************/
int     dissector_try_heuristic(  T_generic_protocol_data  & protocol_data,
								  tvbuff_t                 * next_tvb,
								  packet_info              * pinfo,
								  proto_tree               * tree,
							const int                        bit_length_of_subdissector_data)
{
    T_generic_protocol_subdissector_data  & subdissector_data = protocol_data.ws_data.subdissector_data;

	/* do lookup with the heuristic subdissector table */
#if WIRESHARK_VERSION_NUMBER >= 11200
    heur_dtbl_entry_t *hdtbl_entry = NULL;
    const int  dissector_result = dissector_try_heuristic(subdissector_data.heur_dissector_list, next_tvb, pinfo, tree, &hdtbl_entry, NULL);
#elif WIRESHARK_VERSION_NUMBER >= 11000
    const int  dissector_result = dissector_try_heuristic(subdissector_data.heur_dissector_list, next_tvb, pinfo, tree, NULL);
#else
    const int  dissector_result = dissector_try_heuristic(subdissector_data.heur_dissector_list, next_tvb, pinfo, tree);
#endif

	return  dissector_result;
}

/******************************************************************************
 * dissector_try_string_return_size
 * return  new dissector : size affectively dissected
 * return  old dissector : tvb size
 *****************************************************************************/
int   dissector_try_string_return_size(dissector_table_t   sub_dissectors,
                                 const gchar              *string,
								       tvbuff_t           *tvb,
									   packet_info        *pinfo,
									   proto_tree         *tree)
{
    int  dissector_result = 0;    // means nothing done (dissector not found)

	dissector_handle_t  dissector_handle = dissector_get_string_handle(sub_dissectors, string);
	if (dissector_handle != NULL)
	{
	    dissector_result = call_dissector(dissector_handle, tvb, pinfo, tree);
	}

	return  dissector_result;
}

/******************************************************************************
 * dissector_try_uint_return_size
 * return  new dissector : size affectively dissected
 * return  old dissector : tvb size
 *****************************************************************************/
int     dissector_try_uint_return_size(dissector_table_t   sub_dissectors,
                                 const guint32             uint_val,
								       tvbuff_t           *tvb,
									   packet_info        *pinfo,
									   proto_tree         *tree)
{
    int  dissector_result = 0;    // means nothing done (dissector not found)
#if 1
	dissector_handle_t  dissector_handle = dissector_get_uint_handle(sub_dissectors, uint_val);
	if (dissector_handle != NULL)
	{
	    dissector_result = call_dissector(dissector_handle, tvb, pinfo, tree);
	}

	return  dissector_result;
#else
	int   result = dissector_try_uint(sub_dissectors, uint_val, tvb, pinfo, tree);
	return  result;
#endif
}

/******************************************************************************
 * dissector_try_value
 * return  new dissector : size affectively dissected
 * return  old dissector : tvb size
 *****************************************************************************/
int     dissector_try_value(      T_generic_protocol_data  & protocol_data,
								  tvbuff_t                 * next_tvb,
								  packet_info              * pinfo,
								  proto_tree               * tree,
							const int                        bit_length_of_subdissector_data,
							const C_value                  * P_value)
{
  int  dissector_result = 0;    // means nothing done (no value or dissector not found)

  if (P_value != NULL)
  {
    M_STATE_DEBUG ("subdissector try *P_value_x=" << P_value->as_string());
    T_generic_protocol_subdissector_data  & subdissector_data = protocol_data.ws_data.subdissector_data;
	if (protocol_data.SUBPROTO_SUBFIELD_TYPE_WS == FT_STRING)
	{
      dissector_result = dissector_try_string_return_size(subdissector_data.dissector_table, P_value->get_str().c_str(), next_tvb, pinfo, tree);
	}
	else
	{
      dissector_result = dissector_try_uint_return_size(subdissector_data.dissector_table, P_value->get_int(), next_tvb, pinfo, tree);
	}
  }

  return  dissector_result;
}

/******************************************************************************
 * call_subdissector_or_data2
 * return        new dissector : size affectively dissected
 * return        old dissector : tvb size
 * return  heuristic dissector : false/true
 * So you must NOT rely on this return value
 *****************************************************************************/

int     call_subdissector_or_data2(T_generic_protocol_data  & protocol_data,
						    const dissector_handle_t         dissector_to_call_handle,
								  tvbuff_t                 * tvb,
							const int                        bit_offset,
								  packet_info              * pinfo,
								  proto_tree               * tree,
							const int                        bit_length_of_subdissector_data,
							const C_value                  * P_value_1,
							const C_value                  * P_value_2,
							const C_value                  * P_value_3)
{
  M_STATE_ENTER ("call_subdissector_or_data", bit_length_of_subdissector_data);

  T_generic_protocol_subdissector_data  & subdissector_data = protocol_data.ws_data.subdissector_data;

  if ((bit_offset % 8) != 0)
  {
	  M_FATAL_COMMENT("raw/subproto/insproto only on an entire byte position");
  }
  if ((bit_length_of_subdissector_data % 8) != 0)
  {
	  M_FATAL_COMMENT("raw/subproto/insproto only with an entire byte size");
  }

  const int    offset = bit_offset / 8;
  const int    length_of_subdissector_data = bit_length_of_subdissector_data / 8;

  tvbuff_t  * next_tvb = tvb_new_subset(tvb, offset, length_of_subdissector_data, length_of_subdissector_data);

  /* If the user has a "Follow UDP Stream" window loading, pass a pointer
   * to the payload tvb through the tap system. */
#if 0
// ICIOA
  if(have_tap_listener(udp_follow_tap))
	  tap_queue_packet(udp_follow_tap, pinfo, next_tvb);
#endif

  /* determine if this packet is part of a conversation and call dissector */
  /* for the conversation if available */
#if 0
// ICIOA ne linke pas
  if (try_conversation_dissector(&pinfo->dst, &pinfo->src, PT_UDP,        // ICIOA PT_UDP ?
		uh_dport, uh_sport, next_tvb, pinfo, tree)){
    return;
  }
#endif

  if (dissector_to_call_handle != NULL)
  {
	  return  call_dissector(dissector_to_call_handle, next_tvb, pinfo, tree);
  }

  if (subdissector_data.try_heuristic_first)
  {
    M_STATE_DEBUG ("subdissector try heuristic first");
    const int  dissector_result = dissector_try_heuristic(protocol_data, next_tvb,  pinfo, tree, bit_length_of_subdissector_data);
    if (dissector_result != 0)
      return  dissector_result;
  }

  {
    const int  dissector_result = dissector_try_value(protocol_data, next_tvb,  pinfo, tree, bit_length_of_subdissector_data, P_value_1);
    if (dissector_result != 0)
      return  dissector_result;
  }
  {
    const int  dissector_result = dissector_try_value(protocol_data, next_tvb,  pinfo, tree, bit_length_of_subdissector_data, P_value_2);
    if (dissector_result != 0)
      return  dissector_result;
  }
  {
    const int  dissector_result = dissector_try_value(protocol_data, next_tvb,  pinfo, tree, bit_length_of_subdissector_data, P_value_3);
    if (dissector_result != 0)
      return  dissector_result;
  }

  if (!subdissector_data.try_heuristic_first)
  {
    M_STATE_DEBUG ("subdissector try heuristic");
    const int  dissector_result = dissector_try_heuristic(protocol_data, next_tvb,  pinfo, tree, bit_length_of_subdissector_data);
    if (dissector_result != 0)
      return  dissector_result;
  }

  M_STATE_DEBUG ("subdissector not found ?");

  return  call_dissector(T_generic_protocol_subdissector_data::data_handle, next_tvb, pinfo, tree);
}

/******************************************************************************
 * call_subdissector_or_data
 *****************************************************************************/

void    call_subdissector_or_data(T_generic_protocol_data  & protocol_data,
						    const dissector_handle_t         dissector_to_call_handle,
								  tvbuff_t                 * tvb,
							const int                        bit_offset,
								  packet_info              * pinfo,
								  proto_tree               * tree,
							const int                        bit_length_of_subdissector_data,
							const C_value                  * P_value_1,
							const C_value                  * P_value_2,
							const C_value                  * P_value_3)
{
	// Do not rely on return value of call_subdissector_or_data2, see its comment

	const int     desegment_offset_prev = pinfo->desegment_offset;
	const guint32 desegment_len_prev    = pinfo->desegment_len;
	const int     dissector_result =
		call_subdissector_or_data2(protocol_data,
									 dissector_to_call_handle,
									 tvb,
									 bit_offset,
									 pinfo,
									 tree,
									 bit_length_of_subdissector_data,
									 P_value_1,
									 P_value_2,
									 P_value_3);
	const int     desegment_offset = pinfo->desegment_offset;
	const guint32 desegment_len    = pinfo->desegment_len;

	M_STATE_DEBUG("result=" << dissector_result <<
		          "  offset=" << desegment_offset << " (was " << desegment_offset_prev << ")" <<
		          "  len=" << desegment_len << " (was " << desegment_len_prev << ")"
				  );

	return;
}

//*****************************************************************************
// C_byte_interpret_wsgd_builder
//*****************************************************************************

C_byte_interpret_wsgd_builder::C_byte_interpret_wsgd_builder(
							 int                   proto_idx,
                             tvbuff_t            * wsgd_tvb,
                             packet_info         * wsgd_pinfo,
                             proto_tree          * wsgd_tree,
                             proto_tree          * wsgd_msg_root_tree)
	:C_byte_interpret_wsgd_builder_base(proto_idx, wsgd_tvb, wsgd_pinfo, wsgd_tree, wsgd_msg_root_tree)
{
}

//*****************************************************************************
// value
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::value(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_field_type_name   & field_type_name,
										const string              & data_name,
										const string              & data_simple_name,
										const T_attribute_value   & attribute_value,
										const string              & data_value,
										const string              & final_type,
										const int                   type_bit_size_param,
										const bool                  is_little_endian,
										const bool                  error)
{
	M_STATE_ENTER("interpret_builder_value",
                  "data_name=" << data_name <<
				  "  data_simple_name=" << data_simple_name <<
				  "  data_value=" << data_value <<
				  "  final_type=" << final_type <<
				  "  error=" << error
				  );

	if (A_interpret_wsgd.proto_idx < 0)
		return;

	// 2010/12/30 pb on type_bit_size_param for a variable :
	// - could be zero
	// - could be anything depending on the expression
	//   ie "var type a_var = a_field;"
	//   -> a_var will have the same type_bit_size_param as a_field
	int   type_bit_size = type_bit_size_param;

	if (field_type_name.wsgd_field_idx < 0)
		return;

//	const string    text = field_type_name.name + ": " + data_value;
#if 0
	const string    text = data_simple_name + ": " + data_value;    // new array management
#else
	string    text = data_simple_name + ": " + data_value;    // new array management
	{
		const string &  field_name = field_type_name.name;
		const string &  field_display_name = field_type_name.get_display_name();
		if (field_name != field_display_name)
		{
			text.replace(0, field_name.size(), field_display_name);
		}
	}
#endif
	const int       error_code = error ? PI_ERROR : 0;

	M_STATE_DEBUG ("wsgd add item = " << text);

	if (field_type_name.must_force_manage_as_biggest_float())
	{
		M_STATE_DEBUG ("PROMOTION wsgd add item double = " << text);
		cpp_dissect_generic_add_item_double(A_interpret_wsgd.proto_idx,
									 A_interpret_wsgd.wsgd_tvb,
									 A_interpret_wsgd.wsgd_pinfo,
									 A_interpret_wsgd.wsgd_tree,
									 field_type_name.wsgd_field_idx,
									 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
									 type_bit_size /* field_type_name.basic_type_bit_size */,
									 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
									 text.c_str(),
									 error_code,
									 attribute_value.transformed.get_flt());
	}
	else if (field_type_name.must_force_manage_as_biggest_int())
	{
		if (type_bit_size < 0)
		{
			// This is a trick to know it is an enum !!!
			// NB: wireshark forbids enum size > 4
			// NB: wireshark forbids type_size < 0 for simple types

			type_bit_size = 0;

			if (final_type[0] != 'u')
			{
				M_STATE_DEBUG ("PROMOTION wsgd add item int32 = " << text);
				cpp_dissect_generic_add_item_int32(A_interpret_wsgd.proto_idx,
											 A_interpret_wsgd.wsgd_tvb,
											 A_interpret_wsgd.wsgd_pinfo,
											 A_interpret_wsgd.wsgd_tree,
											 field_type_name.wsgd_field_idx,
											 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
											 type_bit_size /* field_type_name.basic_type_bit_size */,
											 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
											 text.c_str(),
											 error_code,
											 attribute_value.transformed.get_int());
			}
			else
			{
				M_STATE_DEBUG ("PROMOTION wsgd add item uint32 = " << text);
				cpp_dissect_generic_add_item_uint32(A_interpret_wsgd.proto_idx,
											 A_interpret_wsgd.wsgd_tvb,
											 A_interpret_wsgd.wsgd_pinfo,
											 A_interpret_wsgd.wsgd_tree,
											 field_type_name.wsgd_field_idx,
											 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
											 type_bit_size /* field_type_name.basic_type_bit_size */,
											 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
											 text.c_str(),
											 error_code,
											 attribute_value.transformed.get_int());
			}
		}
		else
		{
			M_STATE_DEBUG ("PROMOTION wsgd add item int64 = " << text);
			cpp_dissect_generic_add_item_int64(A_interpret_wsgd.proto_idx,
										 A_interpret_wsgd.wsgd_tvb,
										 A_interpret_wsgd.wsgd_pinfo,
										 A_interpret_wsgd.wsgd_tree,
										 field_type_name.wsgd_field_idx,
										 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
										 type_bit_size /* field_type_name.basic_type_bit_size */,
										 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
										 text.c_str(),
										 error_code,
										 attribute_value.transformed.get_int());
		}
	}
	else if (attribute_value.transformed.get_type() == C_value::E_type_integer)
	{
		// bsew : Necessary to give the value to wireshark
		//         since byte offset and size will not permit to retrieve the value
		if (final_type[0] != 'u')
		{
			if (type_bit_size <= 32)
			{
				M_STATE_DEBUG ("bsew PROMOTION wsgd add item int32 = " << text);
				cpp_dissect_generic_add_item_int32(A_interpret_wsgd.proto_idx,
											 A_interpret_wsgd.wsgd_tvb,
											 A_interpret_wsgd.wsgd_pinfo,
											 A_interpret_wsgd.wsgd_tree,
											 field_type_name.wsgd_field_idx,
											 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
											 type_bit_size /* field_type_name.basic_type_bit_size */,
											 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
											 text.c_str(),
											 error_code,
											 attribute_value.transformed.get_int());
			}
			else
			{
				M_STATE_DEBUG ("bsew PROMOTION wsgd add item int64 = " << text);
				cpp_dissect_generic_add_item_int64(A_interpret_wsgd.proto_idx,
											 A_interpret_wsgd.wsgd_tvb,
											 A_interpret_wsgd.wsgd_pinfo,
											 A_interpret_wsgd.wsgd_tree,
											 field_type_name.wsgd_field_idx,
											 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
											 type_bit_size /* field_type_name.basic_type_bit_size */,
											 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
											 text.c_str(),
											 error_code,
											 attribute_value.transformed.get_int());
			}
		}
		else
		{
			if (type_bit_size <= 32)
			{
				M_STATE_DEBUG ("bsew PROMOTION wsgd add item uint32 = " << text);
				cpp_dissect_generic_add_item_uint32(A_interpret_wsgd.proto_idx,
											 A_interpret_wsgd.wsgd_tvb,
											 A_interpret_wsgd.wsgd_pinfo,
											 A_interpret_wsgd.wsgd_tree,
											 field_type_name.wsgd_field_idx,
											 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
											 type_bit_size /* field_type_name.basic_type_bit_size */,
											 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
											 text.c_str(),
											 error_code,
											 attribute_value.transformed.get_int());
			}
			else
			{
				M_STATE_DEBUG ("bsew PROMOTION wsgd add item uint64 = " << text);
				cpp_dissect_generic_add_item_uint64(A_interpret_wsgd.proto_idx,
											 A_interpret_wsgd.wsgd_tvb,
											 A_interpret_wsgd.wsgd_pinfo,
											 A_interpret_wsgd.wsgd_tree,
											 field_type_name.wsgd_field_idx,
											 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
											 type_bit_size /* field_type_name.basic_type_bit_size */,
											 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
											 text.c_str(),
											 error_code,
											 attribute_value.transformed.get_int());
			}
		}
	}
	else if (attribute_value.transformed.get_type() == C_value::E_type_float)
	{
		// bsew : Necessary to give the value to wireshark
		//         since byte offset and size will not permit to retrieve the value
		if (type_bit_size == 32)
		{
			cpp_dissect_generic_add_item_float(A_interpret_wsgd.proto_idx,
										 A_interpret_wsgd.wsgd_tvb,
										 A_interpret_wsgd.wsgd_pinfo,
										 A_interpret_wsgd.wsgd_tree,
										 field_type_name.wsgd_field_idx,
										 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
										 type_bit_size /* field_type_name.basic_type_bit_size */,
										 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
										 text.c_str(),
										 error_code,
										 attribute_value.transformed.get_flt());
		}
		else
		{
			cpp_dissect_generic_add_item_double(A_interpret_wsgd.proto_idx,
										 A_interpret_wsgd.wsgd_tvb,
										 A_interpret_wsgd.wsgd_pinfo,
										 A_interpret_wsgd.wsgd_tree,
										 field_type_name.wsgd_field_idx,
										 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
										 type_bit_size /* field_type_name.basic_type_bit_size */,
										 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
										 text.c_str(),
										 error_code,
										 attribute_value.transformed.get_flt());
		}
	}
	else if (attribute_value.transformed.get_type() == C_value::E_type_string)
	{
		// Ajout d'un item.
		// La taille specifiee ici est connue ET necessaire pour les types simples
		cpp_dissect_generic_add_item_string(A_interpret_wsgd.proto_idx,
									 A_interpret_wsgd.wsgd_tvb,
									 A_interpret_wsgd.wsgd_pinfo,
									 A_interpret_wsgd.wsgd_tree,
									 field_type_name.wsgd_field_idx,
									 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
									 type_bit_size /* field_type_name.basic_type_bit_size */,
									 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
									 text.c_str(),
									 error_code,
									 attribute_value.transformed.get_str().c_str());
	}
	else // used ???
	{
		// Ajout d'un item.
		// La taille specifiee ici est connue ET necessaire pour les types simples
		cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
									 A_interpret_wsgd.wsgd_tvb,
									 A_interpret_wsgd.wsgd_pinfo,
									 A_interpret_wsgd.wsgd_tree,
									 field_type_name.wsgd_field_idx,
									 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
									 type_bit_size /* field_type_name.basic_type_bit_size */,
									 is_little_endian,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
									 text.c_str(),
									 error_code);
	}
}

//*****************************************************************************
// raw_data
//*****************************************************************************

#include "T_interpret_data.h"

void
C_byte_interpret_wsgd_builder::raw_data(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_interpret_data    & interpret_data,
										const T_field_type_name   & field_type_name,
										const string              & data_name,
										const string              & data_simple_name,
										const int                   type_bit_size,
										const E_raw_data_type       raw_data_type)
{
	M_STATE_ENTER("interpret_builder_raw_data",
                  "data_name=" << data_name <<
				  "  data_simple_name=" << data_simple_name
				  );

	if (A_interpret_wsgd.proto_idx < 0)
		return;

	if (field_type_name.wsgd_field_idx < 0)
	{
		// subproto & insproto are NOT registered fields
		const bool    is_subproto = (raw_data_type == E_raw_data_sub_proto);
		const bool    is_insproto = (raw_data_type == E_raw_data_ins_proto);
		if (is_subproto || is_insproto)
		{
			T_generic_protocol_data  & protocol_data = get_protocol_data(A_interpret_wsgd.proto_idx);

			const C_value  * P_value_1 = interpret_data.get_P_value_of_read_variable (protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1);
			const C_value  * P_value_2 = interpret_data.get_P_value_of_read_variable (protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_2);
			const C_value  * P_value_3 = interpret_data.get_P_value_of_read_variable (protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_3);

//			proto_tree   * root_tree_saved = A_interpret_wsgd.wsgd_msg_root_tree;
			proto_tree   * tree = A_interpret_wsgd.wsgd_tree;
			if (is_subproto)
			{
				// Change tree to set the root one.
				tree = A_interpret_wsgd.wsgd_msg_root_tree;
			}


			// Compute the handle of the dissector to call
			dissector_handle_t  dissector_to_call_handle = NULL;
			if (field_type_name.str_dissector != "")
			{
				dissector_to_call_handle = find_dissector(field_type_name.str_dissector.c_str());
			}

            if (is_insproto == true)
            {
                // col_set_fence forbid to clear, but still can add (a set becomes automatically a add)
                //col_set_fence(A_interpret_wsgd.wsgd_pinfo->cinfo, COL_PROTOCOL);
                //col_set_fence(A_interpret_wsgd.wsgd_pinfo->cinfo, COL_INFO);

#if WIRESHARK_VERSION_NUMBER < 20200
                // <= 2.0 : can not specify the column
                col_set_writable(A_interpret_wsgd.wsgd_pinfo->cinfo, false);
#else
                // Seems to NOT work for destination and source
                // Normally, I need only COL_PROTOCOL & COL_INFO
                col_set_writable(A_interpret_wsgd.wsgd_pinfo->cinfo, -1/*all*/, false);
#endif                
            }

			call_subdissector_or_data(protocol_data,
									 dissector_to_call_handle,
									 A_interpret_wsgd.wsgd_tvb,
									 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
									 A_interpret_wsgd.wsgd_pinfo,
									 tree,
									 type_bit_size,
									 P_value_1,
									 P_value_2,
									 P_value_3);

            if (is_insproto == true)
            {
                //col_clear_fence(A_interpret_wsgd.wsgd_pinfo->cinfo, COL_PROTOCOL);
                //col_clear_fence(A_interpret_wsgd.wsgd_pinfo->cinfo, COL_INFO);

#if WIRESHARK_VERSION_NUMBER < 20200
                col_set_writable(A_interpret_wsgd.wsgd_pinfo->cinfo, true);
#else
                col_set_writable(A_interpret_wsgd.wsgd_pinfo->cinfo, -1/*all*/, true);
#endif
            }

//			A_interpret_wsgd.wsgd_msg_root_tree = root_tree_saved;
		}

		return;
	}

//	const string    text = data_simple_name;    // new array management

	M_STATE_DEBUG ("wsgd add item raw data");

	{
		// Ajout d'un item.
		// La taille specifiee ici est connue ET necessaire pour les types simples
		cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
									 A_interpret_wsgd.wsgd_tvb,
									 A_interpret_wsgd.wsgd_pinfo,
									 A_interpret_wsgd.wsgd_tree,
									 field_type_name.wsgd_field_idx,
									 in_out_frame_data.get_bit_offset_into_initial_frame() - type_bit_size,
									 type_bit_size /* field_type_name.basic_type_bit_size */,
									 false,    // ICIOA je suppose que wireshark applique ca seulement aux types adequats
									 NULL,     // l'affichage du texte est fait par Wireshark
									 0);       // no error_code
	}
}

//*****************************************************************************
// group_begin
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::group_begin(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_field_type_name   & field_type_name,
										const string              & data_name,
										const string              & data_simple_name)
{
	M_STATE_ENTER("interpret_builder_group_begin",
                  "data_name=" << data_name << "  "
				  "data_simple_name=" << data_simple_name );

	A_wsgd_group_data.push_back(T_wsgd_group_data(A_interpret_wsgd.wsgd_tree));

	if (A_interpret_wsgd.proto_idx < 0)
		return;

	if (field_type_name.wsgd_field_idx < 0)
		return;

	if ((field_type_name.basic_type_bit_size >= 0) ||
		(field_type_name.name == "") ||
		(field_type_name.type == "raw") ||
		(field_type_name.type == "subproto") ||
		(field_type_name.type == "insproto") ||
		(field_type_name.type == "string") ||
		(field_type_name.type == "string_nl"))
		return;

	M_STATE_DEBUG ("wsgd       add item & tree");

	T_wsgd_group_data  & group_data = A_wsgd_group_data.back();

#if 1
	string    text = data_simple_name;    // new array management
	{
		const string &  field_name = field_type_name.name;
		const string &  field_display_name = field_type_name.get_display_name();
		if (field_name != field_display_name)
		{
			text.replace(0, field_name.size(), field_display_name);
		}
	}
#endif

	// Ajout d'un item.
	// La taille est re-specifiee apres.
	// La taille specifiee ici est :
	// - inconnue pour les struct
	// - connue ET necessaire pour les types simples
	group_data.item = cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
			                         A_interpret_wsgd.wsgd_tvb,
			                         A_interpret_wsgd.wsgd_pinfo,
			                         A_interpret_wsgd.wsgd_tree,
		                             field_type_name.wsgd_field_idx,
									 in_out_frame_data.get_bit_offset_into_initial_frame(),
									 field_type_name.basic_type_bit_size,   // ICIOA -1 ou 0
									 false,                             // little_endian
//									 field_type_name.name.c_str(),
#if 0
									 data_simple_name.c_str(),          // new array management
#else
									 text.c_str(),          // new array management
#endif
									 0);                                // no error_code

	A_interpret_wsgd.wsgd_tree = cpp_dissect_generic_add_tree(A_interpret_wsgd.proto_idx, group_data.item);
}

//*****************************************************************************
// group_append_text
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::group_append_text(const T_type_definitions  & /* type_definitions */,
										   const T_frame_data        & /* in_out_frame_data */,
//										   const T_field_type_name   & field_type_name,
										   const string              & data_name,
										   const string              & data_simple_name,
										   const string              & text)
{
	M_STATE_ENTER("interpret_builder_group_append_text",
                  "data_name=" << data_name <<
				  "  data_simple_name=" << data_simple_name <<
				  "  text=" << text);

	T_wsgd_group_data    group_data = A_wsgd_group_data.back();

	if (group_data.item != NULL)
	{
		proto_item_append_text(group_data.item, "%s", text.c_str());
	}
}

//*****************************************************************************
// group_end
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::group_end(const T_type_definitions  & /* type_definitions */,
										   const T_frame_data        & /* in_out_frame_data */,
										   const T_field_type_name   & /* field_type_name */,
										   const string              & data_name,
										   const string              & data_simple_name,
										   const int                   type_bit_size)
{
	M_STATE_ENTER("interpret_builder_group_end",
                  "data_name=" << data_name <<
				  "  data_simple_name=" << data_simple_name <<
				  "  type_bit_size=" << type_bit_size);

	T_wsgd_group_data    group_data = A_wsgd_group_data.back();
	
	A_wsgd_group_data.pop_back();


	if (group_data.item != NULL)
	{
		if ((type_bit_size % 8) != 0)
		{
			M_FATAL_COMMENT("struct/... only with an entire byte size");
		}
		const int    type_size = type_bit_size / 8;
	    proto_item_set_len(group_data.item, type_size);
	}

	A_interpret_wsgd.wsgd_tree = group_data.tree;
}

//*****************************************************************************
// error
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::error(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_field_type_name   & /* field_type_name */,
										const string              & /* data_name */,
										const string              & /* data_simple_name */,
										const string              & error)
{
	if (A_interpret_wsgd.proto_idx < 0)
		return;

	string    text = "ERROR: " + error;
	cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
		                         A_interpret_wsgd.wsgd_tvb,
		                         A_interpret_wsgd.wsgd_pinfo,
		                         A_interpret_wsgd.wsgd_tree,
	                             K_ERROR_WSGD_FIELD_IDX,
								 in_out_frame_data.get_bit_offset_into_initial_frame(),
								 0,                                // size (-1 does not work at the end of tvb)
								 false,                            // little_endian, do not care
								 text.c_str(),
								 PI_ERROR);
}

//*****************************************************************************
// cmd_error
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::cmd_error(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_field_type_name   & field_type_name,
										const string              & /* data_name */,
										const string              & text_to_print)
{
	if (A_interpret_wsgd.proto_idx < 0)
		return;

	int   error_code = 0;
	int   field_idx = K_ANY_WSGD_FIELD_IDX;
	if (field_type_name.type == "chat")
	{
		error_code = PI_CHAT;
	}
	else if (field_type_name.type == "note")
	{
		error_code = PI_NOTE;
	}
	else if (field_type_name.type == "warning")
	{
		error_code = PI_WARN;
	}
	else /* error, fatal and anything else */
	{
		error_code = PI_ERROR;
		field_idx = K_ERROR_WSGD_FIELD_IDX;
	}

	string    text = field_type_name.type + ": " + text_to_print;
	cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
		                         A_interpret_wsgd.wsgd_tvb,
		                         A_interpret_wsgd.wsgd_pinfo,
		                         A_interpret_wsgd.wsgd_tree,
	                             field_idx,
								 in_out_frame_data.get_bit_offset_into_initial_frame(),
								 0,                                // size (-1 does not work at the end of tvb)
								 false,                            // little_endian, do not care
								 text.c_str(),
								 error_code);
}

//*****************************************************************************
// cmd_print
//*****************************************************************************
void
C_byte_interpret_wsgd_builder::cmd_print(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_field_type_name   & field_type_name,
										const string              & /* data_name */,
										const string              & text_to_print)
{
	if (A_interpret_wsgd.proto_idx < 0)
		return;

	string    text;
	if (field_type_name.type != "print")
	{
		text += field_type_name.type + ": ";
	}
	text += text_to_print;

	cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
		                         A_interpret_wsgd.wsgd_tvb,
		                         A_interpret_wsgd.wsgd_pinfo,
		                         A_interpret_wsgd.wsgd_tree,
	                             K_ANY_WSGD_FIELD_IDX,
								 in_out_frame_data.get_bit_offset_into_initial_frame(),
								 0,                                // size (-1 does not work at the end of tvb)
								 false,                            // little_endian, do not care
								 text.c_str(),
								 0);                               // no error_code
}


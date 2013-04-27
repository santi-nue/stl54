/*
 * Copyright 2008-2012 Olivier Aveline <wsgd@free.fr>
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

#include "C_byte_interpret_wsgd_builder_base.h"
#include "T_generic_protocol_data.h"
#include "T_interpret_data.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <epan/expert.h>
#include <epan/filesystem.h>
#include <epan/report_err.h>
#include <epan/conversation.h>
#include <epan/tap.h>
#include <epan/prefs.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

//*****************************************************************************
// test_remote_command
//*****************************************************************************
#if 0
// compile mais ne linke pas

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <epan/packet.h>
#include <capture.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

void    test_remote_command()
{
	capture_options    capture_opts;

	capture_stop(&capture_opts);
}
#endif

/******************************************************************************
 * 
 *****************************************************************************/

// ICIOA bsew
#define M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size)       \
	int    offset = bit_offset / 8;                                          \
    int    field_byte_size = 0;                                              \
	if ((bit_offset >= 0) && (field_bit_size > 0))                           \
	{                                                                        \
		int    bit_offset_end = bit_offset + field_bit_size;                 \
		int    offset_end = bit_offset_end / 8;                              \
		int    offset_end_modulo = bit_offset_end % 8;                       \
		if (offset_end_modulo != 0)                                          \
		{                                                                    \
			++offset_end;                                                    \
		}                                                                    \
		field_byte_size = offset_end - offset;                               \
	}


/******************************************************************************
 * cpp_dissect_generic_add_item
 *****************************************************************************/

proto_item  * cpp_dissect_generic_add_item(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size);

  proto_item_1 = proto_tree_add_item(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           little_endian);
#if 0
  // This version could not be used :
  // - does not accept field_bit_size = 0 or -1
  // - works only for unsigned integers, bool and enums
  // - works only with size <= 64 bits
  // Fails on : raw, intX, floatX, string
  // Works on : boolX, uintX
  proto_item_1 = proto_tree_add_bits_item(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   bit_offset,
					               field_bit_size,
						           little_endian);
#endif

  if (text != NULL)
  {
    proto_item_set_text(proto_item_1, "%s", text);
  }
//  proto_item_set_expert_flags(proto_item_1, PI_CHECKSUM, PI_ERROR);   // result = nothing ???

  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);

	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	// ICIOA : je pourrais optimiser pour ajouter qu'un seul champ ERROR
	//  mais bon il n'est pas sense avoir d'erreur !
	// NB: since 12X (?), it is possible to filter on exoert info
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}



proto_item  * cpp_dissect_generic_add_item_string(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const char  * value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_string",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_string_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);
	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

proto_item  * cpp_dissect_generic_add_item_uint32(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const long long  value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_uint32",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_uint_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);
	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

proto_item  * cpp_dissect_generic_add_item_int32(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const long long  value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_int32",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_int_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);
	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

proto_item  * cpp_dissect_generic_add_item_uint64(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const long long  value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_uint64",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_uint64_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);
	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

proto_item  * cpp_dissect_generic_add_item_int64(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const long long  value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_int64",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_int64_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);
	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

proto_item  * cpp_dissect_generic_add_item_float(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const float   value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_float",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_float_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);
	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

proto_item  * cpp_dissect_generic_add_item_double(const int     proto_idx,
											        tvbuff_t     * tvb,
												    packet_info  * pinfo,
										          proto_tree  * tree,
										 const int     field_idx,
										 const int     bit_offset,
										 const int     field_bit_size,
										 const int     little_endian,
										 const char  * text,
										 const int     error_code,
										 const double  value)
{
  T_generic_protocol_data    & protocol_data = get_protocol_data(proto_idx);
  T_generic_protocol_ws_data        * P_protocol_ws_data = &protocol_data.ws_data;
  proto_item                     * proto_item_1 = NULL;

  M_COMPUTE_BYTE_FROM_BIT_OFFSET_SIZE(bit_offset,field_bit_size);

  M_STATE_ENTER("cpp_dissect_generic_add_item_double",
                "proto_idx=" << proto_idx <<
				"  field_idx=" << field_idx <<
				"  field_bit_size=" << field_bit_size <<
				"  value=" << value);

  proto_item_1 = proto_tree_add_double_format(tree,
		                           P_protocol_ws_data->fields_data.hf_id[field_idx],
	           					   tvb,
			           			   offset,
					               field_byte_size,
						           value,
								   "%s", text);
  if (error_code > 0)
  {
    expert_add_info_format(pinfo, proto_item_1, PI_MALFORMED, error_code, "%s", text);

	// Add a hidden error_in_packet field (if not already on a error_in_packet field).
	// Permits filter on error_in_packet.
	if ((error_code >= PI_ERROR) && (field_idx != K_ERROR_WSGD_FIELD_IDX))
	{
	  proto_item   * proto_item_error = proto_tree_add_item(tree,
                                            P_protocol_ws_data->fields_data.hf_id[K_ERROR_WSGD_FIELD_IDX],
											tvb,
											offset,
											field_byte_size,
								            little_endian);
	  PROTO_ITEM_SET_HIDDEN(proto_item_error);
	}
  }

  return  proto_item_1;
}

//*****************************************************************************
// C_byte_interpret_wsgd_builder_base
//*****************************************************************************

C_byte_interpret_wsgd_builder_base::C_byte_interpret_wsgd_builder_base(
							 int                   proto_idx,
                             tvbuff_t            * wsgd_tvb,
                             packet_info         * wsgd_pinfo,
                             proto_tree          * wsgd_tree,
                             proto_tree          * wsgd_msg_root_tree)
{
	A_interpret_wsgd.proto_idx = proto_idx;
	A_interpret_wsgd.wsgd_tvb = wsgd_tvb;
	A_interpret_wsgd.wsgd_pinfo = wsgd_pinfo;
	A_interpret_wsgd.wsgd_tree = wsgd_tree;
	A_interpret_wsgd.wsgd_msg_root_tree = wsgd_msg_root_tree;
	A_is_input_data_complete = false;
}

//*****************************************************************************
// 
//*****************************************************************************
void
C_byte_interpret_wsgd_builder_base::set_is_input_data_complete(bool    is_input_data_complete)
{
	A_is_input_data_complete = is_input_data_complete;
}

bool
C_byte_interpret_wsgd_builder_base::is_input_data_complete() const
{
	return  A_is_input_data_complete;
}

//*****************************************************************************
// missing_data
//*****************************************************************************
void
C_byte_interpret_wsgd_builder_base::missing_data(const T_type_definitions  & /* type_definitions */,
										const T_frame_data        & in_out_frame_data,
										const T_interpret_data    & interpret_data,
										const T_field_type_name   & /* field_type_name */,
										const string              & /* data_name */,
										const string              & /* data_simple_name */,
										const string              & error)
{
    T_generic_protocol_data  & protocol_data = get_protocol_data(A_interpret_wsgd.proto_idx);

	if (is_input_data_complete())
	{
		string    text = "MISSING_DATA: " + error;
		cpp_dissect_generic_add_item(A_interpret_wsgd.proto_idx,
									 A_interpret_wsgd.wsgd_tvb,
									 A_interpret_wsgd.wsgd_pinfo,
									 A_interpret_wsgd.wsgd_msg_root_tree,
									 K_ERROR_WSGD_FIELD_IDX,
									 in_out_frame_data.get_bit_offset_into_initial_frame(),
									 0,                                // size (-1 does not work at the end of tvb)
									 false,                            // little_endian, do not care
									 text.c_str(),
									 PI_ERROR);
	}
	else
	{
		M_STATE_INFO ("missing_data  bit_offset=" << in_out_frame_data.get_bit_offset_into_initial_frame());

        const T_attribute_value *  P_attr_MSG_ID_FIELD_NAME = interpret_data.get_P_attribute_value_of_read_variable (protocol_data.MSG_ID_FIELD_NAME);
		if (P_attr_MSG_ID_FIELD_NAME == NULL)
		{
			// Perhaps the header has not been entirely read ?
			M_STATE_WARNING ("Did NOT find " << protocol_data.MSG_ID_FIELD_NAME << " (MSG_ID_FIELD_NAME) ?");
		}
		else if (P_attr_MSG_ID_FIELD_NAME->has_error() == true)
		{
			// I do not recognize the message ...
			// I suppose it is not a 1st segment,
			//  I could NOT do anything with it,
			//  I must ignore it.
			M_STATE_FATAL ("Did NOT recognize the message");
			string    text = "Did NOT recognize the message, perhaps a message segment ?";
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
			return;
		}

		// Not an error, wait for the next segment.
		A_interpret_wsgd.wsgd_pinfo->desegment_offset = 0;  /* Start at beginning next time */
		A_interpret_wsgd.wsgd_pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
	}
}

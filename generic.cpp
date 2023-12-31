/* generic.c
 * Copyright 2008-2020 Olivier Aveline <wsgd@free.fr>
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

#include "generic.h"
#include "T_generic_protocol_data.h"

#include <limits.h>
#include <stdlib.h>

#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_build_types.h"
#include "byte_interpret_build_types_context.h"
#include "byte_interpret.h"
#include "byte_interpret_compute_expression.h"
#include "T_interpret_data.h"
#include "byte_interpret_builder.h"
#include "C_byte_interpret_wsgd_builder.h"
#include "C_byte_interpret_wsgd_builder_base.h"
#include "C_setlocale_numeric_C_guard.h"
#include <fstream>

#ifdef WIN32
#include <process.h>
#define getpid  _getpid
#define strdup  _strdup
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <epan/tvbuff.h>
#include <wsutil/filesystem.h>
#include <wsutil/report_message.h>
#include <epan/conversation.h>
#include <epan/tap.h>
#include <epan/stats_tree.h>
#include <epan/stats_tree_priv.h>  // to retrieve the protocol_data
#include <epan/prefs.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifndef WIRESHARK_VERSION_NUMBER
#error you must define WIRESHARK_VERSION_NUMBER as <major><minor_on_2_digits><micro_on_2_digits>
#endif

//*****************************************************************************
// register_enum_values
//*****************************************************************************

void    register_enum_values(T_generic_protocol_data  & protocol_data)
{
    M_TRACE_ENTER ("register_enum_values", "proto_idx=" << protocol_data.proto_idx);

    T_type_definitions  & type_definitions = protocol_data.type_definitions;
    T_map_enum_definition_representation  & map_enum_definition_representation = type_definitions.map_enum_definition_representation;

    M_TRACE_DEBUG ("max_nb_of_enums=" << map_enum_definition_representation.size());

    int   enum_idx = 0;
    for (T_map_enum_definition_representation::iterator
                             iter  = map_enum_definition_representation.begin();
                             iter != map_enum_definition_representation.end();
                           ++iter)
    {
        const string                      & enum_type_name = iter->first;
        T_enum_definition_representation  & definition_representation = iter->second;
        definition_representation.wsgd_enum_values_idx = enum_idx;

        const T_enum_definition                 & definition = definition_representation.definition;

        M_TRACE_DEBUG ("add enum  enum_idx=" << enum_idx <<
                       "  name=" << enum_type_name <<
                       "  nb_values=" << definition.size());

        T_generic_protocol_enum_value  enum_value;

        int     enum_value_idx = 0;
        for (T_enum_definition::const_iterator
                             iter_value  = definition.begin();
                             iter_value != definition.end();
                           ++iter_value)
        {
            const T_enum_name_val  & enum_name_val = *iter_value;

            M_TRACE_DEBUG ("add enum value  enum_idx=" << enum_idx << "  enum_value_idx="  << enum_value_idx <<
                            "  (" << enum_name_val.value << ", " << enum_name_val.name << ")");

            value_string    vs;
            vs.value  = static_cast<const int>(enum_name_val.value);
            vs.strptr = strdup (enum_name_val.name.c_str());

            enum_value.value_strings.push_back(vs);

            ++enum_value_idx;
        }

        // Must add a last value_string with nullptr ptr.
        {
            value_string    vs;
            vs.value  = 0;
            vs.strptr = nullptr;

            enum_value.value_strings.push_back(vs);
        }

        protocol_data.ws_data.enum_values_data.enum_values.push_back(enum_value);

        ++enum_idx;
    }
}

//*****************************************************************************
// register_fields_add_field_...
//*****************************************************************************

void    register_fields_add_field_none(
                                         T_generic_protocol_data      & protocol_data,
                                   const int                            field_idx,
                                   const char                         * field_name,
                                   const char                         * field_filter_name_param,
                                   const char                         * field_extended_name)
{
    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_FATAL_IF_NE(field_idx, (int)P_protocol_ws_data->fields_data.hf.size());

    const string                   filter_field_name = protocol_data.PROTOABBREV + "." + field_filter_name_param;
    hf_register_info               hf = 
        { nullptr,    // initialized later
          { strdup(field_name),
            strdup(filter_field_name.c_str()), FT_NONE, BASE_NONE, nullptr, 0x0,
            strdup(field_extended_name), HFILL }};

    P_protocol_ws_data->fields_data.hf.push_back(hf);
}

void    register_fields_add_field_none(
                                         T_generic_protocol_data      & protocol_data,
                                   const int                            field_idx,
                                   const T_field_type_name            & field_name)
{
    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_FATAL_IF_NE(field_idx, (int)P_protocol_ws_data->fields_data.hf.size());

    const string                   filter_field_name = protocol_data.PROTOABBREV + "." + field_name.get_filter_name();
    hf_register_info               hf = 
        { nullptr,    // initialized later
          { strdup(field_name.get_display_name().c_str()),
            strdup(filter_field_name.c_str()), FT_NONE, BASE_NONE, nullptr, 0x0,
            strdup(field_name.get_extended_name().c_str()), HFILL }};

    P_protocol_ws_data->fields_data.hf.push_back(hf);
}

void    register_fields_add_field_bytes(
                                         T_generic_protocol_data      & protocol_data,
                                   const int                            field_idx,
                                   const T_field_type_name            & field_name)
{
    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_FATAL_IF_NE(field_idx, (int)P_protocol_ws_data->fields_data.hf.size());

    const string                   filter_field_name = protocol_data.PROTOABBREV + "." + field_name.get_filter_name();
    hf_register_info               hf = 
        { nullptr,    // initialized later
          { strdup(field_name.get_display_name().c_str()),
            strdup(filter_field_name.c_str()), FT_BYTES, BASE_NONE, nullptr, 0x0,
            strdup(field_name.get_extended_name().c_str()), HFILL }};

    P_protocol_ws_data->fields_data.hf.push_back(hf);
}

void    register_fields_add_field_string(
                                         T_generic_protocol_data      & protocol_data,
                                   const int                            field_idx,
                                   const T_field_type_name            & field_name)
{
    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_FATAL_IF_NE(field_idx, (int)P_protocol_ws_data->fields_data.hf.size());

    const string                   filter_field_name = protocol_data.PROTOABBREV + "." + field_name.get_filter_name();
    hf_register_info               hf = 
        { nullptr,    // initialized later
          { strdup(field_name.get_display_name().c_str()),
            strdup(filter_field_name.c_str()), FT_STRING, BASE_NONE, nullptr, 0x0,
            strdup(field_name.get_extended_name().c_str()), HFILL }};

    P_protocol_ws_data->fields_data.hf.push_back(hf);
}

void    register_fields_add_field_float(
                                         T_generic_protocol_data      & protocol_data,
                                   const int                            field_idx,
                                   const T_field_type_name            & field_name,
                                   const int                            field_bit_size)
{
    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_FATAL_IF_NE(field_idx, (int)P_protocol_ws_data->fields_data.hf.size());

    const string                   filter_field_name = protocol_data.PROTOABBREV + "." + field_name.get_filter_name();
    ftenum                         ws_size = field_bit_size<=32 ? FT_FLOAT : FT_DOUBLE;
    hf_register_info               hf = 
        { nullptr,    // initialized later
          { strdup(field_name.get_display_name().c_str()),
            strdup(filter_field_name.c_str()), ws_size, BASE_NONE, nullptr, 0x0,
            strdup(field_name.get_extended_name().c_str()), HFILL }};

    P_protocol_ws_data->fields_data.hf.push_back(hf);
}

void    register_fields_add_field_int(
                                         T_generic_protocol_data      & protocol_data,
                                   const int                            field_idx,
                                   const T_field_type_name            & field_name,
                                   const int                            field_bit_size,
                                   const bool                           is_signed,
                                   const int                            enum_idx)
{
    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_FATAL_IF_NE(field_idx, (int)P_protocol_ws_data->fields_data.hf.size());

    const string                   filter_field_name = protocol_data.PROTOABBREV + "." + field_name.get_filter_name();

    const ftenum    ws_size_unsign[] = { FT_UINT8, FT_UINT16, FT_UINT24, FT_UINT32, FT_NONE, FT_NONE, FT_NONE, FT_UINT64 };
    const ftenum    ws_size_signed[] = {  FT_INT8,  FT_INT16,  FT_INT24,  FT_INT32, FT_NONE, FT_NONE, FT_NONE,  FT_INT64 };

    int           field_byte_size = field_bit_size / 8;

    M_FATAL_IF_LT(field_byte_size, 1);
    M_FATAL_IF_GT(field_byte_size, (int)(sizeof(ws_size_unsign)/sizeof(ws_size_unsign[0])));

    // wireshark (proto.c) forbids enum with size > 4 bytes
    // NB: could have enum with size = 8 only
    //      with must_force_manage_as_biggest_int and so
    //      in case of var (i.e. not a field read inside the packet)
    if (enum_idx >= 0)
        field_byte_size = 4;

    ftenum                         ws_size;
    if (is_signed)
    {
        ws_size = ws_size_signed[field_byte_size - 1];
    }
    else
    {
        ws_size = ws_size_unsign[field_byte_size - 1];
    }
    M_FATAL_IF_EQ(ws_size, FT_NONE);

    hf_register_info               hf = 
        { nullptr,    // initialized later
          { strdup(field_name.get_display_name().c_str()),
            strdup(filter_field_name.c_str()), ws_size, BASE_DEC, nullptr, 0x0,
            strdup(field_name.get_extended_name().c_str()), HFILL }};

    if (enum_idx >= 0)
    {
        hf.hfinfo.strings = VALS(&P_protocol_ws_data->enum_values_data.enum_values[enum_idx].value_strings[0]);
    }

    P_protocol_ws_data->fields_data.hf.push_back(hf);
}

//*****************************************************************************
// register_fields_field_type_name
// Must be coherent with frame_to_any.
//*****************************************************************************
bool    S_ICIOA_in_bitfield = false;

void    register_fields_struct_fields(T_generic_protocol_data  & protocol_data,
                                      int                      & field_idx,
                                      T_struct_fields          & struct_fields);

void    register_fields_struct_definition(T_generic_protocol_data      & protocol_data,
                                          int                          & field_idx,
                                          T_struct_definition          & struct_definition);

void    register_fields_bitfield_definition(T_generic_protocol_data      & protocol_data,
                                            int                          & field_idx,
                                            T_bitfield_definition        & bitfield_definition);

void    register_fields_switch_definition(T_generic_protocol_data      & protocol_data,
                                          int                          & field_idx,
                                          T_switch_definition          & switch_definition);

void    register_fields_field_type_name(T_generic_protocol_data          & protocol_data,
                                        int                              & field_idx,
                                        T_field_type_name                & field_type_name,
                                  const string                           & data_type)
{
    M_TRACE_ENTER ("register_fields_field_type_name",
                    "proto_idx=" << protocol_data.proto_idx << "  " <<
                    "field_idx=" << field_idx << "  " <<
                    "field_type_name.type=" << field_type_name.type << "  " <<
                    "field_type_name.name=" << field_type_name.name << "  " <<
                    "data_type=" << data_type << "  ");

    T_type_definitions       & type_definitions = protocol_data.type_definitions;

    string    final_type = data_type;
    M_FATAL_IF_NE(data_type, field_type_name.type);

    // Ignore commands (not a field).
    if ((final_type == "debug_print") || (final_type == "print"))  return;
    if (final_type == "chat")                                      return;
    if (final_type == "note")                                      return;
    if (final_type == "warning")                                   return;
    if (final_type == "error")                                     return;
    if (final_type == "fatal")                                     return;
    if (final_type == "output")                                    return;
    if (final_type == "byte_order")                                return;
    if (final_type == "decoder")                                   return;
    if (final_type == "save_position")                             return;
    if (final_type == "goto_position")                             return;
    if (final_type == "move_position_bytes")                       return;
    if (final_type == "move_position_bits")                        return;
    if (final_type == "check_eof_distance_bytes")                  return;
    if (final_type == "check_eof_distance_bits")                   return;
    if (final_type == "chrono")                                    return;
    if (final_type == "set")                                       return;
    if (final_type == "call")                                      return;
    if (final_type == "deep_break")                                return;
    if (final_type == "deep_continue")                             return;
    if (final_type == "break")                                     return;
    if (final_type == "continue")                                  return;
    if (final_type == "return")                                    return;

    // array ? Not specifically managed.

    // switch ?
    if ((final_type == "switch") ||
        (type_definitions.is_a_switch(final_type)))
    {
        // name NOT specified means :
        // - user do NOT want to see it
        // - wireshark will reject it 
        if (field_type_name.name != "")
        {
            // ajout champ none (size inconnue)
            field_type_name.wsgd_field_idx = field_idx;
            M_TRACE_DEBUG ("add_field_none("
                << protocol_data.proto_idx << ", "
                << field_idx << ", "
                << field_type_name.name << ") switch");

            register_fields_add_field_none(protocol_data,
                                                field_idx,
                                                field_type_name);
            ++field_idx;
        }
        if (final_type == "switch")
        {
            // must manage inline switch
            M_FATAL_IF_EQ (field_type_name.P_switch_inline.get(), nullptr);
            register_fields_switch_definition(protocol_data, field_idx, *field_type_name.P_switch_inline);
        }
        return;
    }

    // struct ?
    {
        T_struct_definition  * P_struct = nullptr;

        if (final_type == "struct")
        {
            P_struct = field_type_name.P_sub_struct.get();
            M_FATAL_IF_EQ (P_struct, nullptr);
        }
        else
        {
            P_struct = type_definitions.get_P_struct(final_type);
        }

        if (P_struct != nullptr)
        {
            // name NOT specified means :
            // - user do NOT want to see it
            // - wireshark will reject it 
            if (field_type_name.name != "")
            {
                // ajout champ none (size inconnue)
                field_type_name.wsgd_field_idx = field_idx;
                M_TRACE_DEBUG ("add_field_none("
                    << protocol_data.proto_idx << ", "
                    << field_idx << ", "
                    << field_type_name.name << ") struct");

                register_fields_add_field_none(protocol_data,
                                                field_idx,
                                                field_type_name);
                ++field_idx;

                if (P_struct->is_a_field_struct())
                {
                    T_field_type_name  fs_field_type_name = P_struct->fields[P_struct->field_struct_idx];
                    fs_field_type_name.name = field_type_name.name + "." + fs_field_type_name.name;
                    register_fields_field_type_name(protocol_data, field_idx, fs_field_type_name, fs_field_type_name.type);
                }
            }
            if (final_type == "struct")
            {
                // must manage inline struct
                M_FATAL_IF_EQ (field_type_name.P_sub_struct.get(), nullptr);
                register_fields_struct_definition(protocol_data, field_idx, *field_type_name.P_sub_struct);
            }
            return;
        }
    }

    // bitfield ?
    {
        if ((final_type == "bitfield") ||
            (type_definitions.is_a_bitfield(final_type)))
        {
            // name NOT specified means :
            // - user do NOT want to see it
            // - wireshark will reject it 
            if (field_type_name.name != "")
            {
                // ajout champ none (size inconnue)
                field_type_name.wsgd_field_idx = field_idx;
                M_TRACE_DEBUG ("add_field_none("
                    << protocol_data.proto_idx << ", "
                    << field_idx << ", "
                    << field_type_name.name << ") bitfield");

                register_fields_add_field_none(protocol_data,
                                                    field_idx,
                                                    field_type_name);
                ++field_idx;
            }
            if (final_type == "bitfield")
            {
                // must manage inline bitfield
                M_FATAL_IF_EQ (field_type_name.P_bitfield_inline.get(), nullptr);
                register_fields_bitfield_definition(protocol_data, field_idx, *field_type_name.P_bitfield_inline);
            }
            return;
        }
    }

    // enum ?
    int     wsgd_enum_values_idx = -1;
    bool    is_enum = false;
    bool    is_enum_signed = false;
    {
        const T_enum_definition_representation  * P_enum = type_definitions.get_P_enum(final_type);
        if (P_enum != nullptr)
        {
            is_enum = true;
            is_enum_signed = P_enum->is_signed;

            final_type = P_enum->representation_type;

            wsgd_enum_values_idx = P_enum->wsgd_enum_values_idx;
        }
    }

    // simple fields and subproto, insproto

#define M_CHECK_FIELD_NAME()                                                                        \
    if (field_type_name.name == "")                                                                 \
    {                                                                                               \
        M_FATAL_COMMENT("\"\" field name is only accepted for struct, switch or bitfield types.");  \
    }

#define M_READ_SIMPLE_TYPE_BASE(TYPE_NAME,TYPE_NAME_BIT_SIZE,TYPE_IMPL,TYPE_IMPL_BIT_SIZE,TYPE_IMPL_STR,CHANGE_basic_type_bit_size)    \
    else if (final_type == TYPE_NAME)                                         \
    {                                                                         \
        M_CHECK_FIELD_NAME();                                                 \
                                                                              \
        M_FATAL_IF_GT (TYPE_NAME_BIT_SIZE, TYPE_IMPL_BIT_SIZE);               \
                                                                              \
        field_type_name.wsgd_field_idx = field_idx;                           \
    if (CHANGE_basic_type_bit_size)    field_type_name.basic_type_bit_size = TYPE_IMPL_BIT_SIZE; /* ce champ est deja calcule dans post_build_field_base MAIS pas suffisant pb sur les bitfield ? a priori corrige 20091102 ??? */ \
                                                                              \
        if (field_type_name.must_force_manage_as_biggest_float())             \
        {                                                                     \
            M_TRACE_DEBUG ("PROMOTION add_field_float("                       \
               << protocol_data.proto_idx << ", "                             \
               << field_idx << ", "                                           \
               << field_type_name.name << ", "                                \
               << TYPE_IMPL_BIT_SIZE << ")");                                 \
            register_fields_add_field_float(                                  \
                                               protocol_data,                 \
                                               field_idx,                     \
                                               field_type_name,               \
                                               64);                           \
        }                                                                     \
        else if (strncmp(TYPE_NAME, "float", 5) == 0)                         \
        {                                                                     \
            M_TRACE_DEBUG ("add_field_float("                                 \
               << protocol_data.proto_idx << ", "                             \
               << field_idx << ", "                                           \
               << field_type_name.name << ", "                                \
               << TYPE_IMPL_BIT_SIZE << ")");                                 \
            register_fields_add_field_float(                                  \
                                               protocol_data,                 \
                                               field_idx,                     \
                                               field_type_name,               \
                                               TYPE_IMPL_BIT_SIZE);           \
        }                                                                     \
        else if (field_type_name.must_force_manage_as_biggest_int())          \
        {                                                                     \
            bool  is_signed = (is_enum == true) ? is_enum_signed : true;      \
            M_TRACE_DEBUG ("PROMOTION add_field_int("                         \
               << protocol_data.proto_idx << ", "                             \
               << field_idx << ", "                                           \
               << field_type_name.name << ", "                                \
               << TYPE_IMPL_BIT_SIZE << ", "                                  \
               << is_signed << ")");                                          \
            register_fields_add_field_int(                                    \
                                       protocol_data,                         \
                                       field_idx,                             \
                                       field_type_name,                       \
                                       64,                                    \
                                       is_signed,                             \
                                       wsgd_enum_values_idx);                 \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            M_TRACE_DEBUG ("add_field_int("                                   \
               << protocol_data.proto_idx << ", "                             \
               << field_idx << ", "                                           \
               << field_type_name.name << ", "                                \
               << TYPE_IMPL_BIT_SIZE << ")");                                 \
            register_fields_add_field_int(                                    \
                                       protocol_data,                         \
                                       field_idx,                             \
                                       field_type_name,                       \
                                       TYPE_IMPL_BIT_SIZE,                    \
                                       final_type[0] != 'u',                  \
                                       wsgd_enum_values_idx);                 \
        }                                                                     \
    }

#define M_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_NAME_SIZE,TYPE_IMPL)                \
        M_READ_SIMPLE_TYPE_BASE(TYPE_NAME, TYPE_NAME_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL, true)

    string    str_string_size;

    if (false) ;
    M_READ_SIMPLE_TYPE ( "int8",    8,    signed char)
    M_READ_SIMPLE_TYPE ("uint8",    8,  unsigned char)
    M_READ_SIMPLE_TYPE ( "int16",  16,    signed short)
    M_READ_SIMPLE_TYPE ("uint16",  16,  unsigned short)
    M_READ_SIMPLE_TYPE ( "int24",  24,    signed int)
    M_READ_SIMPLE_TYPE ("uint24",  24,  unsigned int)
    M_READ_SIMPLE_TYPE ( "int32",  32,    signed int)
    M_READ_SIMPLE_TYPE ("uint32",  32,  unsigned int)
    M_READ_SIMPLE_TYPE ( "int40",  40,    signed long long)
    M_READ_SIMPLE_TYPE ("uint40",  40,  unsigned long long)
    M_READ_SIMPLE_TYPE ( "int48",  48,    signed long long)
    M_READ_SIMPLE_TYPE ("uint48",  48,  unsigned long long)
    M_READ_SIMPLE_TYPE ( "int64",  64,    signed long long)
    M_READ_SIMPLE_TYPE ("uint64",  64,  unsigned long long)
    M_READ_SIMPLE_TYPE ("float32", 32, float)
    M_READ_SIMPLE_TYPE ("float64", 64, double)
// alias    M_READ_SIMPLE_TYPE ("bool",      sizeof(bool)*8,  bool)
// enum     M_READ_SIMPLE_TYPE ("bool8",    8, unsigned char)
// enum     M_READ_SIMPLE_TYPE ("bool16",  16, unsigned short)
// enum     M_READ_SIMPLE_TYPE ("bool32",  32, unsigned int)
    M_READ_SIMPLE_TYPE ("spare",    8, unsigned char)
    M_READ_SIMPLE_TYPE ( "char",    8,          char)
    M_READ_SIMPLE_TYPE ("schar",    8,   signed char)
    M_READ_SIMPLE_TYPE ("uchar",    8, unsigned char)
    else if ((final_type == "string") ||
             (final_type == "string_nl"))
    {
        M_CHECK_FIELD_NAME();

        // ajout champ str
        field_type_name.wsgd_field_idx = field_idx;
        M_TRACE_DEBUG ("add_field_string("
           << protocol_data.proto_idx << ", "
           << field_idx << ", "
           << field_type_name.name << ")");
        register_fields_add_field_string(protocol_data,
                                               field_idx,
                                               field_type_name);
    }
    else if (final_type == "raw")
    {
        M_CHECK_FIELD_NAME();

        // ajout champ none (size inconnue)
        field_type_name.wsgd_field_idx = field_idx;
        M_TRACE_DEBUG ("add_field_none("
           << protocol_data.proto_idx << ", "
           << field_idx << ", "
           << field_type_name.name << ") " << final_type);
        register_fields_add_field_bytes(protocol_data,
                                           field_idx,
                                           field_type_name);
    }
    else if ((final_type == "subproto") ||
             (final_type == "insproto"))
    {
        // I think I can accept for subproto and insproto, but there is no interest.
        M_CHECK_FIELD_NAME();

        // subdissector : nothing to do, wireshark will manage it as Data
        return;
    }
    else if ((final_type == "padding_bits") ||
             (final_type == "msg"))
    {
        M_CHECK_FIELD_NAME();
        return;    // nothing to do
    }
    else
    {
        long    bit_size = 0;
        if (((strncmp(final_type.c_str(), "uint", 4) == 0) &&
             (get_number(final_type.c_str()+4, &bit_size) == true) &&
             (bit_size >   0) &&
             (bit_size <= 32)) ||
            ((strncmp(final_type.c_str(), "int", 3) == 0) &&
             (get_number(final_type.c_str()+3, &bit_size) == true) &&
             (bit_size >   1) &&
             (bit_size <  32)))
        {
            M_CHECK_FIELD_NAME();

            if (S_ICIOA_in_bitfield == false)
            {
            // Ceci ne fonctionne pas a cause des bitfield !!!
            // voir proto_test
            if (false) { }
            M_READ_SIMPLE_TYPE_BASE(final_type.c_str(),bit_size,signed int,32,"signed int", false);
//            M_READ_SIMPLE_TYPE_BASE(final_type.c_str(),bit_size,signed int,32,"signed int", S_ICIOA_in_bitfield);
//            field_type_name.basic_type_bit_size = TYPE_BIT_SIZE;  // ce champ est deja calcule dans post_build_field_base MAIS pas suffisant pb sur les bitfield
            }
            else
            {
            /* bsew uintXX intXX */
            field_type_name.wsgd_field_idx = field_idx;
            M_TRACE_DEBUG ("BIT add_field_int("
               << protocol_data.proto_idx << ", "
               << field_idx << ", "
               << field_type_name.name << ", "
               << bit_size << ")");
            register_fields_add_field_int(     protocol_data,
                                               field_idx,
                                               field_type_name,
                                               32,
                                               final_type[0] != 'u',
                                               wsgd_enum_values_idx);
            }
        }
        else
        {
            // Oups ...
            M_FATAL_COMMENT ("Type " << final_type << " unknown.");
            return;
        }
    }

    ++field_idx;
}

//*****************************************************************************
// register_fields_struct_definition
// Must be coherent with frame_to_field.
//*****************************************************************************

void    register_fields_struct_fields(T_generic_protocol_data  & protocol_data,
                                      int                      & field_idx,
                                      T_struct_fields          & struct_fields)
{
    for (T_struct_fields::iterator
                             iter_field  = struct_fields.begin();
                             iter_field != struct_fields.end();
                           ++iter_field)
    {
        T_field_type_name  & field_type_name = *iter_field;

        if ((field_type_name.type != "if") &&
            (field_type_name.type != "while") &&
            (field_type_name.type != "do_while") &&
            (field_type_name.type != "loop_size_bytes") &&
            (field_type_name.type != "loop_size_bits") &&
            (field_type_name.type != "loop_nb_times"))
        {
            register_fields_field_type_name(protocol_data, field_idx, field_type_name, field_type_name.type);
            continue;
        }

        // if or any loop
        if (field_type_name.P_sub_struct)
            register_fields_struct_definition(protocol_data, field_idx, *field_type_name.P_sub_struct);
        register_fields_struct_fields(protocol_data, field_idx, field_type_name.sub_struct_2);
    }
}

//*****************************************************************************
// register_fields_struct_definition
// Must be coherent with frame_to_field.
//*****************************************************************************

void    register_fields_struct_definition(T_generic_protocol_data      & protocol_data,
                                          int                          & field_idx,
                                          T_struct_definition          & struct_definition)
{
    register_fields_struct_fields(protocol_data, field_idx, struct_definition.fields);
}

//*****************************************************************************
// register_fields_bitfield_definition
//*****************************************************************************

void    register_fields_bitfield_definition(T_generic_protocol_data      & protocol_data,
                                          int                          & field_idx,
                                          T_bitfield_definition          & bitfield_definition)
{
    register_fields_field_type_name(protocol_data, field_idx, bitfield_definition.master_field, bitfield_definition.master_field.type);
    S_ICIOA_in_bitfield = true;
    register_fields_struct_fields(protocol_data, field_idx, bitfield_definition.fields_definition);
    S_ICIOA_in_bitfield = false;
}

//*****************************************************************************
// register_fields_switch_definition
//*****************************************************************************

void    register_fields_switch_definition(T_generic_protocol_data      & protocol_data,
                                          int                          & field_idx,
                                          T_switch_definition          & switch_definition)
{
    T_switch_cases       & switch_cases = switch_definition.switch_cases;

    for (T_switch_cases::iterator
                            sw_iter  = switch_cases.begin();
                            sw_iter != switch_cases.end();
                          ++sw_iter)
    {
        T_struct_fields  & struct_fields = sw_iter->fields;

        register_fields_struct_fields(protocol_data, field_idx, struct_fields);
    }
}

//*****************************************************************************
// register_fields
//*****************************************************************************

void    register_fields(T_generic_protocol_data  & protocol_data)
{
    M_TRACE_ENTER ("register_fields", "proto_idx=" << protocol_data.proto_idx);

    T_type_definitions       & type_definitions = protocol_data.type_definitions;

    int   field_idx = 0;

    // whole or any data
    M_FATAL_IF_NE(field_idx, K_WHOLE_WSGD_FIELD_IDX);
    M_FATAL_IF_NE(field_idx, K_ANY_WSGD_FIELD_IDX);
    register_fields_add_field_none(protocol_data, field_idx, "data", "data", "data");
    ++field_idx;

    // pseudo field to do a coloring rule or a display filter on it
    // I do not know how to code a coloring rule.
    // -> must do it manually at this time.
    if (K_ERROR_WSGD_FIELD_IDX != K_ANY_WSGD_FIELD_IDX)
    {
        M_FATAL_IF_NE(field_idx, K_ERROR_WSGD_FIELD_IDX);
        register_fields_add_field_none(protocol_data, field_idx, "error_in_packet", "error_in_packet", "error_in_packet");
        ++field_idx;
    }

    {
        T_map_struct_definition  & map_struct_definition = type_definitions.map_struct_definition;
        for (T_map_struct_definition::iterator
                                    iter  = map_struct_definition.begin();
                                    iter != map_struct_definition.end();
                                  ++iter)
        {
            T_struct_definition  & struct_definition = iter->second;

            register_fields_struct_definition(protocol_data, field_idx, struct_definition);
        }
    }

    {
        T_map_bitfield_definition  & map_bitfield_definition = type_definitions.map_bitfield_definition;
        for (T_map_bitfield_definition::iterator
                                    iter  = map_bitfield_definition.begin();
                                    iter != map_bitfield_definition.end();
                                  ++iter)
        {
            T_bitfield_definition  & bitfield_definition = iter->second;

            register_fields_bitfield_definition(protocol_data, field_idx, bitfield_definition);
        }
    }

    {
        T_map_switch_definition  & map_switch_definition = type_definitions.map_switch_definition;
        for (T_map_switch_definition::iterator
                                    iter  = map_switch_definition.begin();
                                    iter != map_switch_definition.end();
                                  ++iter)
        {
            T_switch_definition  & switch_definition = iter->second;

            register_fields_switch_definition(protocol_data, field_idx, switch_definition);
        }
    }

    {
        T_map_function_definition  & map_function_definition = type_definitions.map_function_definition;
        for (T_map_function_definition::iterator
                                    iter  = map_function_definition.begin();
                                    iter != map_function_definition.end();
                                  ++iter)
        {
            T_function_definition  & function_definition = iter->second;

            register_fields_struct_fields(protocol_data, field_idx, function_definition.fields);
        }
    }

    T_generic_protocol_ws_data         * P_protocol_ws_data = &protocol_data.ws_data;
    T_generic_protocol_fields_data     & fields_data = P_protocol_ws_data->fields_data;

    fields_data.hf_id.resize(fields_data.hf.size());
    fields_data.ett.resize(fields_data.hf.size());
    fields_data.ett_id.resize(fields_data.ett.size());

    // WARNING_ADDRESSES
    // Must NOT be done before,
    //  because the addresses of hf_id[idx] and ett_id[idx] could change
    //  when the size of the vector change
    for (uint  idx = 0; idx < fields_data.hf.size(); ++idx)
    {
        fields_data.hf_id[idx]    = -1;
        fields_data.hf[idx].p_id  = &fields_data.hf_id[idx];
        fields_data.ett_id[idx] = -1;
        fields_data.ett[idx]    = &fields_data.ett_id[idx];

//      M_TRACE_DEBUG("hf[" << idx << "].p_id  = " << &fields_data.hf_id[idx]);
//      M_TRACE_DEBUG("ett[" << idx << "].p_id  = " << &fields_data.ett_id[idx]);
    }
}

//*****************************************************************************
// get_wsgd_files_in_dir
//*****************************************************************************

int   get_wsgd_files_in_dir (const string          & dir_name,
                                   vector<string>  & file_names)
{
    // true to have the full name (including directory name).
    return  get_files_in_dir(dir_name, "", ".wsgd", file_names, true);
}

//*****************************************************************************
// compute_wsgd_file_names
//*****************************************************************************

void    compute_wsgd_file_names(vector<string>  & wsgd_file_names)
{

#define M_SEARCH_IN_DIR_SUBDIR(DIR,SUBDIR)                                  \
{                                                                           \
    const char  * p_dir_name = DIR;                                         \
    if (p_dir_name != nullptr)                                              \
    {                                                                       \
        std::string  dir_name = p_dir_name;                                 \
        dir_name += SUBDIR;                                                 \
        get_wsgd_files_in_dir(dir_name, wsgd_file_names);                   \
        if (wsgd_file_names.empty() == false)                               \
        {                                                                   \
            byte_interpret_set_include_directory(dir_name);                 \
            return;                                                         \
        }                                                                   \
    }                                                                       \
}

#define M_SEARCH_IN_DIR(DIR)               M_SEARCH_IN_DIR_SUBDIR(DIR, "")
#define M_SEARCH_IN_DIR_EPAN(DIR)          M_SEARCH_IN_DIR_SUBDIR(DIR, "/epan")

    M_SEARCH_IN_DIR(getenv("WIRESHARK_GENERIC_DISSECTOR_DIR"));
    M_SEARCH_IN_DIR(get_profiles_dir());
    M_SEARCH_IN_DIR(get_persdatafile_dir());
    M_SEARCH_IN_DIR(get_datafile_dir());
    M_SEARCH_IN_DIR_EPAN(get_plugins_dir_with_version());
    M_SEARCH_IN_DIR_EPAN(get_plugins_pers_dir_with_version());
    M_SEARCH_IN_DIR(get_progfile_dir());
    M_SEARCH_IN_DIR(".");
}

//*****************************************************************************
// prefs_apply_cb
//*****************************************************************************
#if 0
void    prefs_apply_cb(void)
{
}
#endif

//*****************************************************************************
// cpp_proto_register_generic
//*****************************************************************************

void    cpp_proto_register_generic(const string   & wsgd_file_name,
                                   const int        proto_idx)
{
    M_TRACE_ENTER ("cpp_proto_register_generic", "wsgd_file_name=" << wsgd_file_name);

    T_generic_protocol_data  & protocol_data = new_protocol_data(proto_idx);

    protocol_data.wsgd_file_name = wsgd_file_name;

    read_file_wsgd (wsgd_file_name, protocol_data);

    C_debug_set_temporary  debug_register_proto_main(protocol_data.DEBUG);

    // Check that PROTOABBREV does NOT already exist to avoid wireshark crash.
    // No function available into proto.h to test PROTONAME & PROTOSHORTNAME.
    if (proto_get_id_by_filter_name(protocol_data.PROTOABBREV.c_str()) >= 0)
    {
        M_FATAL_COMMENT("Protocol " << protocol_data.PROTOABBREV << " already exist");
    }

    M_TRACE_DEBUG("proto_register_protocol " <<
                  protocol_data.PROTONAME << " " <<
                  protocol_data.PROTOSHORTNAME << " " <<
                  protocol_data.PROTOABBREV);
    protocol_data.ws_data.proto_generic =
                        proto_register_protocol (protocol_data.PROTONAME.c_str(),
                                                 protocol_data.PROTOSHORTNAME.c_str(),
                                                 protocol_data.PROTOABBREV.c_str());

    register_enum_values(protocol_data);
    register_fields(protocol_data);


    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    M_TRACE_DEBUG("proto_register_field_array " <<
                  P_protocol_ws_data->proto_generic << " " <<
                  &P_protocol_ws_data->fields_data.hf[0] << " " <<
                  P_protocol_ws_data->fields_data.hf.size());
    proto_register_field_array (P_protocol_ws_data->proto_generic,
                                &P_protocol_ws_data->fields_data.hf[0],
                                P_protocol_ws_data->fields_data.hf.size());
#if 0
    {
        for (int idx = 0; idx < P_protocol_ws_data->fields_data.hf.size(); ++idx)
        {
            M_TRACE_DEBUG("hf[" << idx << "].p_id=" << P_protocol_ws_data->fields_data.hf[idx].p_id <<
                          "  &hf_id[" << idx << "]=" << &P_protocol_ws_data->fields_data.hf_id[idx] <<
                          "   hf_id[" << idx << "]=" <<  P_protocol_ws_data->fields_data.hf_id[idx]);
        }
    }
#endif

    M_TRACE_DEBUG("proto_register_subtree_array " <<
                  &P_protocol_ws_data->fields_data.ett[0] << " " <<
                  P_protocol_ws_data->fields_data.ett.size());
    proto_register_subtree_array (&P_protocol_ws_data->fields_data.ett[0],
                                  P_protocol_ws_data->fields_data.ett.size());

    {
        ei_register_info   eri = 
            { &P_protocol_ws_data->expert_data.ei_malformed_comment,
              { strdup(string(protocol_data.PROTOABBREV + ".malformed").c_str()),
                PI_MALFORMED, PI_COMMENT, "comment", EXPFILL }};

        P_protocol_ws_data->expert_data.ei.push_back(eri);
    }
    {
        ei_register_info   eri = 
            { &P_protocol_ws_data->expert_data.ei_malformed_chat,
              { strdup(string(protocol_data.PROTOABBREV + ".malformed").c_str()),
                PI_MALFORMED, PI_CHAT, "chat", EXPFILL }};

        P_protocol_ws_data->expert_data.ei.push_back(eri);
    }
    {
        ei_register_info   eri = 
            { &P_protocol_ws_data->expert_data.ei_malformed_note,
              { strdup(string(protocol_data.PROTOABBREV + ".malformed").c_str()),
                PI_MALFORMED, PI_NOTE, "note", EXPFILL }};

        P_protocol_ws_data->expert_data.ei.push_back(eri);
    }
    {
        ei_register_info   eri = 
            { &P_protocol_ws_data->expert_data.ei_malformed_warn,
              { strdup(string(protocol_data.PROTOABBREV + ".malformed").c_str()),
                PI_MALFORMED, PI_WARN, "warn", EXPFILL }};

        P_protocol_ws_data->expert_data.ei.push_back(eri);
    }
    {
        ei_register_info   eri = 
            { &P_protocol_ws_data->expert_data.ei_malformed_error,
              { strdup(string(protocol_data.PROTOABBREV + ".malformed").c_str()),
                PI_MALFORMED, PI_ERROR, "error", EXPFILL }};

        P_protocol_ws_data->expert_data.ei.push_back(eri);
    }

    M_TRACE_DEBUG("expert_register_field_array " <<
                  &P_protocol_ws_data->expert_data.ei[0] << " " <<
                  P_protocol_ws_data->expert_data.ei.size());

    expert_module_t* expert_proto = expert_register_protocol(P_protocol_ws_data->proto_generic);
    expert_register_field_array(expert_proto,
                                &P_protocol_ws_data->expert_data.ei[0],
                                 P_protocol_ws_data->expert_data.ei.size());


    /* subdissector code */
    if (protocol_data.SUBPROTO_SUBFIELD != "")
    {
        protocol_data.SUBPROTO_SUBFIELD_PARAM    = protocol_data.PROTOABBREV    + "." + protocol_data.SUBPROTO_SUBFIELD;
        protocol_data.SUBPROTO_SUBFIELD_PARAM_UI = protocol_data.PROTOSHORTNAME + " " + protocol_data.SUBPROTO_SUBFIELD;

        M_TRACE_DEBUG ("register_dissector_table " <<
                        protocol_data.SUBPROTO_SUBFIELD_PARAM.c_str() << " " <<
                        protocol_data.SUBPROTO_SUBFIELD_PARAM_UI.c_str());

        protocol_data.ws_data.subdissector_data.dissector_table =
        register_dissector_table(protocol_data.SUBPROTO_SUBFIELD_PARAM.c_str(),
                                 protocol_data.SUBPROTO_SUBFIELD_PARAM_UI.c_str(),
                                 protocol_data.ws_data.proto_generic,
                                 protocol_data.SUBPROTO_SUBFIELD_TYPE_WS,
                                 BASE_DEC);

        M_FATAL_IF_EQ(protocol_data.ws_data.subdissector_data.dissector_table, nullptr);

        protocol_data.ws_data.subdissector_data.heur_dissector_list = register_heur_dissector_list(protocol_data.PROTOABBREV.c_str(),
                                                                                                   protocol_data.ws_data.proto_generic);
    }

    /* Register configuration preferences */
//    module_t  * module = prefs_register_protocol(protocol_data.ws_data.proto_generic, prefs_apply_cb);
    module_t  * module = prefs_register_protocol(protocol_data.ws_data.proto_generic, nullptr);
    prefs_register_bool_preference(module, "try_heuristic_first",
        "Try heuristic sub-dissectors first",
        "Try to decode a packet using an heuristic sub-dissector before using a sub-dissector registered to a specific port",
        &protocol_data.ws_data.subdissector_data.try_heuristic_first);

    M_TRACE_DEBUG("new_register_dissector " <<
                  protocol_data.PROTOABBREV << " " <<
                  P_protocol_ws_data->P_dissect_fct << " " <<
                  P_protocol_ws_data->proto_generic);
    register_dissector(protocol_data.PROTOABBREV.c_str(), P_protocol_ws_data->P_dissect_fct, P_protocol_ws_data->proto_generic);

    // Must use PROTOABBREV because generic_stats_tree_... rely on it
    protocol_data.ws_data.tap_data.proto_tap = register_tap(protocol_data.PROTOABBREV.c_str());
    M_TRACE_DEBUG("proto_tap=" << protocol_data.ws_data.tap_data.proto_tap);
}

//*****************************************************************************
// wsgd_report_failure
//*****************************************************************************

void    wsgd_report_failure(string    str)
{
    str += "\n";
    str += "\n";
    str += "More information could be available at http://wsgd.free.fr/";
    report_failure(str.c_str(), "silly arg to avoid compiler warning, so must not appear");
}

//*****************************************************************************
// trace_version_infos
//*****************************************************************************

void    trace_version_infos()
{
    M_TRACE_DEBUG ("wireshark version                 = " << epan_get_version());
    M_TRACE_DEBUG ("wsgd generation data :");
    M_TRACE_DEBUG ("- wireshark compilation version   = " << VERSION);
    M_TRACE_DEBUG ("- WIRESHARK_VERSION_NUMBER        = " << WIRESHARK_VERSION_NUMBER);
    M_TRACE_DEBUG ("- compilation date                = " << __DATE__);
#ifdef MSC_VER_REQUIRED
    M_TRACE_DEBUG ("- compiler                        = " << "Visual C++ " << MSC_VER_REQUIRED);
#elif defined(__GNUC__)
    M_TRACE_DEBUG ("- compiler                        = " << "gcc " << __GNUC__);
#endif
}

//*****************************************************************************
// trace_dirs
//*****************************************************************************

void    trace_dirs()
{
    M_TRACE_DEBUG ("get_progfile_dir                  = " << get_progfile_dir());
    M_TRACE_DEBUG ("get_plugins_dir_with_version      = " << get_plugins_dir_with_version());
    M_TRACE_DEBUG ("get_plugins_pers_dir_with_version = " << get_plugins_pers_dir_with_version());
    M_TRACE_DEBUG ("get_datafile_dir                  = " << get_datafile_dir());
    M_TRACE_DEBUG ("get_systemfile_dir                = " << get_systemfile_dir());
    M_TRACE_DEBUG ("get_profiles_dir                  = " << get_profiles_dir());
    M_TRACE_DEBUG ("get_persdatafile_dir              = " << get_persdatafile_dir());
}

//*****************************************************************************
// trace_locales
//*****************************************************************************

#include <locale.h>

void    trace_locales()
{
    M_TRACE_DEBUG ("locale LC_ALL         = " << setlocale(LC_ALL, nullptr));
    M_TRACE_DEBUG ("locale LC_COLLATE     = " << setlocale(LC_COLLATE, nullptr));
    M_TRACE_DEBUG ("locale LC_CTYPE       = " << setlocale(LC_CTYPE, nullptr));
    M_TRACE_DEBUG ("locale LC_MONETARY    = " << setlocale(LC_MONETARY, nullptr));
    M_TRACE_DEBUG ("locale LC_NUMERIC     = " << setlocale(LC_NUMERIC, nullptr));
    M_TRACE_DEBUG ("locale LC_TIME        = " << setlocale(LC_TIME, nullptr));
#if 0
LC_ALL          The entire locale.
LC_COLLATE      Affects the behavior of strcoll and strxfrm.
LC_CTYPE        Affects character handling functions (all functions of <cctype>, except isdigit and isxdigit), and the multibyte and wide character functions
LC_MONETARY     Affects monetary formatting information returned by localeconv.
LC_NUMERIC      Affects the decimal-point character in formatted input/output operations and string formatting functions, as well as non-monetary information returned by localeconv.
LC_TIME         Affects the behavior of strftime.
#endif
}

//*****************************************************************************
// cpp_proto__register_generic2
// double _ needed to avoid wireshark build add proto_register_generic2
//  into plugin_register
//*****************************************************************************

void    cpp_proto__register_generic2(void)
{
    vector<string>    wsgd_file_names;
    compute_wsgd_file_names(wsgd_file_names);

    set_max_nb_of_protocol_data(wsgd_file_names.size());

    int   proto_idx = 0;
    for (vector<string>::const_iterator  iter  = wsgd_file_names.begin();
                                         iter != wsgd_file_names.end();
                                        ++iter)
    {
        const string   & wsgd_file_name = *iter;

        build_types_context_reset();

        try
        {
#if 0
            string    wsgd_file_name_short = wsgd_file_name;
            {
                const string::size_type  idx_last_dir = wsgd_file_name_short.find_last_of("/\\");
                if (idx_last_dir != string::npos)
                {
                    wsgd_file_name_short.erase(0, idx_last_dir+1);
                }
            }

            build_types_context_include_file_open (wsgd_file_name_short);
#endif
            cpp_proto_register_generic(wsgd_file_name, proto_idx);
//            build_types_context_include_file_close(wsgd_file_name_short);
        }
        catch(C_byte_interpret_exception  & error_str)
        {
            get_protocol_data(proto_idx).proto_is_NOT_usable();
            wsgd_report_failure(
                        "Generic dissector did NOT succeed to read/interpret/register :\n" +
                        wsgd_file_name + " \n"
                        "\n"
                        "The given protocol is NOT available.\n"
                        "\n" +
                        build_types_context_where() +
                        "\n" +
                        error_str.get_explanation());
        }
        catch(...)
        {
            get_protocol_data(proto_idx).proto_is_NOT_usable();
            wsgd_report_failure(
                        "Generic dissector did NOT succeed to read/interpret/register :\n" +
                        wsgd_file_name + " \n"
                        "\n"
                        "The given protocol is NOT available.\n" +
                        "\n" +
                        build_types_context_where());
        }

        ++proto_idx;
    }
}

//*****************************************************************************
// get_traces_file_name
//*****************************************************************************
string    get_traces_file_name()
{
    string  traces_file_name = "";

    const char *  file = getenv("WIRESHARK_GENERIC_DISSECTOR_TRACES_FILE");
    if (file != nullptr)
    {
        traces_file_name += file;
    }
    else
    {
        traces_file_name += "wireshark_generic_dissector_traces.txt";
    }


    mod_replace_all(traces_file_name, "{pid}", get_string(getpid()));

    return  traces_file_name;
}

//*****************************************************************************
// get_traces_file_full_name
//*****************************************************************************
string    get_traces_file_full_name()
{
    const std::string  traces_file_name = get_traces_file_name();
    string  traces_file_full_name = "";
    
    const char *  dir = getenv("WIRESHARK_GENERIC_DISSECTOR_TRACES_DIR");
    if (dir != nullptr)
    {
        traces_file_full_name = dir;
        traces_file_full_name += G_DIR_SEPARATOR_S;
        traces_file_full_name += traces_file_name;
    }
    else
    {
        // Let's try the local directory
        traces_file_full_name = traces_file_name;
        ofstream    ofs(traces_file_full_name.c_str());
        if (!ofs.is_open())
        {
            // File can NOT be created in local directory
            // Should be a permission problem
            // So take a personal directory
            traces_file_full_name = get_persdatafile_dir();
            traces_file_full_name += G_DIR_SEPARATOR_S;
            traces_file_full_name += traces_file_name;
        }
    }

    mod_replace_all(traces_file_full_name, "{pid}", get_string(getpid()));

    return  traces_file_full_name;
}

//*****************************************************************************
// create_traces_file
//*****************************************************************************
void    create_traces_file()
{
    const std::string  traces_file_full_name = get_traces_file_full_name();

    static ofstream    ofs(traces_file_full_name.c_str());
    set_state_ostream(ofs);
}

//*****************************************************************************
// cpp_proto_register_generic
//*****************************************************************************
extern "C"
void    cpp_proto_register_generic(void)
{
    create_traces_file();

    C_debug_set_temporary    debug_register_main(E_debug_status_ON_NO_TIME);

    trace_version_infos();
    trace_dirs();
    trace_locales();

    M_TRACE_ENTER ("cpp_proto_register_generic", "");

    C_setlocale_numeric_C_guard  locale_guard;

    try
    {
        cpp_proto__register_generic2();
    }
    catch(...)
    {
    }
}

//*****************************************************************************
// stats
//*****************************************************************************

static void    generic_stats_tree_init(stats_tree  * st)
{
    // st->cfg->abbr is not the proto_abbrev
    // tapname is identic to proto_abbrev
    T_generic_protocol_data      & protocol_data = get_protocol_data_from_proto_abbrev((const char*)st->cfg->tapname);
    C_debug_set_temporary          debug_stats(protocol_data.DEBUG);
    M_TRACE_ENTER ("generic_stats_tree_init", st->cfg->name);

    T_generic_protocol_tap_data  & tap_data = protocol_data.ws_data.tap_data;
    tap_data.tap_is_needed = true;

    T_stats_sub_group  & sub_group = tap_data.stats.get_sub_group_by_full_name(st->cfg->name);

    sub_group.node_id = stats_tree_create_node(st, sub_group.node_name.c_str(), 0,
#if WIRESHARK_VERSION_NUMBER >= 30000
                                                   STAT_DT_INT,
#endif
                                                   TRUE);

    for (auto iter = sub_group.topics.begin(); iter != sub_group.topics.end(); ++iter)
    {
        T_stats_topic  & topic = *iter;

        topic.pivot_id = stats_tree_create_pivot(st, topic.topic_name.c_str(), sub_group.node_id);
    }
}

#if WIRESHARK_VERSION_NUMBER >= 30000
#define stat_tree_packet_return_type     tap_packet_status
#define stat_tree_packet_return_value    TAP_PACKET_REDRAW
#else
#define stat_tree_packet_return_type     int
#define stat_tree_packet_return_value    1
#endif
static
#if WIRESHARK_VERSION_NUMBER >= 40000
stat_tree_packet_return_type
              generic_stats_tree_packet(stats_tree      * st,
                                        packet_info     * pinfo,
                                        epan_dissect_t  * edt,
                                  const void            * p,
                                        tap_flags_t       UNUSED(flags))
#else
stat_tree_packet_return_type
              generic_stats_tree_packet(stats_tree      * st,
                                        packet_info     * pinfo,
                                        epan_dissect_t  * edt,
                                  const void            * p)
#endif
{
    T_generic_protocol_data  & protocol_data = *(T_generic_protocol_data*)p;
    C_debug_set_temporary      debug_stats(protocol_data.DEBUG);
    M_TRACE_ENTER ("generic_stats_tree_packet", st->cfg->name <<
                   "  pinfo=" << pinfo <<
                   "  edt=" << edt <<
                   "  p=" << p);

    T_generic_protocol_tap_data  & tap_data = protocol_data.ws_data.tap_data;
    T_stats_sub_group            & sub_group = tap_data.stats.get_sub_group_by_full_name(st->cfg->name);

    M_FATAL_IF_EQ(tap_data.RCP_last_msg_interpret_data.get(), nullptr);
    T_interpret_data& last_msg_interpret_data = *tap_data.RCP_last_msg_interpret_data;

    tick_stat_node(st, sub_group.node_name.c_str(), 0, FALSE);

    for (auto iter = sub_group.topics.begin(); iter != sub_group.topics.end(); ++iter)
    {
        T_stats_topic  & topic = *iter;

        if (last_msg_interpret_data.is_read_variable(topic.variable_name))
        {
            const string  value = last_msg_interpret_data.get_full_str_value_of_read_variable(topic.variable_name);
            stats_tree_tick_pivot(st, topic.pivot_id, value.c_str());
        }
    }

    // Msg is ended, some data are no more necessary
    last_msg_interpret_data.msg_is_ended();

    return  stat_tree_packet_return_value;
}

static void    generic_stats_tree_cleanup(stats_tree  * st)
{
    // st->cfg->abbr is not the proto_abbrev
    // tapname is identic to proto_abbrev
    T_generic_protocol_data  & protocol_data = get_protocol_data_from_proto_abbrev((const char*)st->cfg->tapname);
    C_debug_set_temporary      debug_stats(protocol_data.DEBUG);
    M_TRACE_ENTER ("generic_stats_tree_cleanup", st->cfg->name);

    T_generic_protocol_tap_data& tap_data = protocol_data.ws_data.tap_data;
    tap_data.tap_is_needed = false;
}

static void    register_generic_stats_trees(T_generic_protocol_data  & protocol_data)
{
    M_TRACE_ENTER ("register_generic_stats_trees", "");

    auto& stats_groups = protocol_data.ws_data.tap_data.stats.groups;
    for (auto iter = stats_groups.begin(); iter != stats_groups.end(); ++iter)
    {
        const T_stats_group& group = *iter;

        for (auto iter = group.sub_groups.begin(); iter != group.sub_groups.end(); ++iter)
        {
            const T_stats_sub_group& sub_group = *iter;

            const std::string & full_name = sub_group.full_name;

            stats_tree_register_plugin(protocol_data.PROTOABBREV.c_str(),
                                       full_name.c_str(),              // must be unique
                                       full_name.c_str(),
                                       0,
                                       generic_stats_tree_packet,
                                       generic_stats_tree_init,
                                       generic_stats_tree_cleanup);
        }
    }
}

//*****************************************************************************
// is_an_heuristic_dissector
//*****************************************************************************
bool  is_an_heuristic_dissector(const string &  parent_name)
{
    const bool  result = has_heur_dissector_list(parent_name.c_str());
    return  result;
}

//*****************************************************************************
// cpp_proto_reg_handoff_generic_proto
//*****************************************************************************

void    cpp_proto_reg_handoff_generic_proto(T_generic_protocol_data  & protocol_data)
{
    C_debug_set_temporary    debug_handoff_main(protocol_data.DEBUG);

    M_TRACE_ENTER ("cpp_proto_reg_handoff_generic", protocol_data.proto_idx);

    T_generic_protocol_ws_data      * P_protocol_ws_data = &protocol_data.ws_data;

    // Create the dissector handle.
    P_protocol_ws_data->dissector_handle = create_dissector_handle(P_protocol_ws_data->P_dissect_fct,
                                                                   P_protocol_ws_data->proto_generic);
    M_FATAL_IF_EQ(P_protocol_ws_data->dissector_handle, nullptr);


    // Declare parent dissectors
    for (vector<T_generic_protocol_data::T_parent>::const_iterator
                                parent_iter  = protocol_data.PARENTS.begin();
                                parent_iter != protocol_data.PARENTS.end();
                              ++parent_iter)
    {
        const T_generic_protocol_data::T_parent  & parent = * parent_iter;

        // Add dissector for each PARENT_SUBFIELD_VALUES.
        M_TRACE_DEBUG ("PARENT_SUBFIELD = " << parent.PARENT_SUBFIELD.c_str());

        // Check that PARENT_SUBFIELD exist.
        if (find_dissector_table(parent.PARENT_SUBFIELD.c_str()) == nullptr)
        {
            wsgd_report_failure(
                        "Generic dissector did NOT succeed to attach " + protocol_data.PROTONAME +
                        " to its PARENT_SUBFIELD " + parent.PARENT_SUBFIELD +
                        " (because it does NOT exist).\n" 
                        "\n"
                        "The given protocol will NOT be called (at least automatically).\n");
            continue;
        }

        for (uint  idx = 0; idx < parent.PARENT_SUBFIELD_VALUES_int.size(); ++idx)
        {
            M_TRACE_DEBUG ("PARENT_SUBFIELD_VALUE = " << parent.PARENT_SUBFIELD_VALUES_int[idx]);

            dissector_add_uint(parent.PARENT_SUBFIELD.c_str(),
                               parent.PARENT_SUBFIELD_VALUES_int[idx],
                               P_protocol_ws_data->dissector_handle);
        }
        for (uint  idx = 0; idx < parent.PARENT_SUBFIELD_RANGES_int.size(); ++idx)
        {
            int    value_low  = parent.PARENT_SUBFIELD_RANGES_int[idx].first;
            int    value_high = parent.PARENT_SUBFIELD_RANGES_int[idx].second;

            M_TRACE_DEBUG ("PARENT_SUBFIELD_RANGE = " << value_low << " - " << value_high);

            for (int  value = value_low; value <= value_high; ++value)
            {
                dissector_add_uint(parent.PARENT_SUBFIELD.c_str(),
                                   value,
                                   P_protocol_ws_data->dissector_handle);
            }
        }
        for (uint  idx = 0; idx < parent.PARENT_SUBFIELD_VALUES_str.size(); ++idx)
        {
            M_TRACE_DEBUG ("PARENT_SUBFIELD_VALUE = " << parent.PARENT_SUBFIELD_VALUES_str[idx]);

            dissector_add_string(parent.PARENT_SUBFIELD.c_str(),
                                 parent.PARENT_SUBFIELD_VALUES_str[idx].c_str(),
                                 P_protocol_ws_data->dissector_handle);
        }
    }

    // Declare heuristic dissectors
    for (vector<string>::const_iterator
                                parent_iter  = protocol_data.PARENTS_HEURISTIC.begin();
                                parent_iter != protocol_data.PARENTS_HEURISTIC.end();
                              ++parent_iter)
    {
        const string  & parent_name = *parent_iter;

        if (is_an_heuristic_dissector(parent_name) != true)
        {
            if (find_dissector_table(parent_name.c_str()) == nullptr)
            {
                wsgd_report_failure(
                            "Generic dissector did NOT succeed to find parent heuristic dissector " + parent_name +
                            " (because it does NOT exist).\n");
            }
            else
            {
                wsgd_report_failure(
                            "Generic dissector did NOT succeed to find parent heuristic dissector " + parent_name +
                            " (because it is NOT an heuristic dissector).\n");
            }
        
            continue;
        }
      
#if WIRESHARK_VERSION_NUMBER >= 30400
        heur_dissector_add(parent_name.c_str(),
                            P_protocol_ws_data->P_heuristic_fct,
                            protocol_data.PROTONAME.c_str(),
                            protocol_data.PROTOABBREV.c_str(),
                            P_protocol_ws_data->proto_generic,
                            HEURISTIC_ENABLE);
#else
        heur_dissector_add(parent_name.c_str(),
                            P_protocol_ws_data->P_heuristic_fct,
                            protocol_data.PROTONAME.c_str(),
                            protocol_data.PROTOSHORTNAME.c_str(),
                            P_protocol_ws_data->proto_generic,
                            HEURISTIC_ENABLE);
#endif
    }

    // Declare decode as dissectors
    for (vector<string>::const_iterator
            decode_as_iter  = protocol_data.ADD_FOR_DECODE_AS_TABLES.begin();
            decode_as_iter != protocol_data.ADD_FOR_DECODE_AS_TABLES.end();
          ++decode_as_iter)
    {
        const string  & decode_as_table_name = *decode_as_iter;

        dissector_add_for_decode_as(decode_as_table_name.c_str(), P_protocol_ws_data->dissector_handle);
    }

    // Check subdissector name
    vector<string>  & vector_subdissector_name = protocol_data.type_definitions.vector_subdissector_name;
    for (vector<string>::const_iterator
                                subdissector_iter  = vector_subdissector_name.begin();
                                subdissector_iter != vector_subdissector_name.end();
                              ++subdissector_iter)
    {
        const string  & subdissector_name = *subdissector_iter;

        dissector_handle_t  dissector_handle = find_dissector(subdissector_name.c_str());
        if (dissector_handle == nullptr)
        {
            wsgd_report_failure(
                        "Generic dissector did NOT succeed to find subdissector " + subdissector_name +
                        " (because it does NOT exist).\n" 
                        "\n"
                        "Explicit calls to this subdissector will NOT work.\n");
        }
        if (false)  // comment savoir qu'il est désactivé ?
        {
            wsgd_report_failure(
                        "Generic dissector will NOT succeed to call subdissector " + subdissector_name +
                        " because it has been deactivated.\n" 
                        "\n"
                        "Explicit calls to this subdissector will NOT work.\n");
        }
    }

    // Statistics
    register_generic_stats_trees(protocol_data);

    M_TRACE_DEBUG ("Leave cpp_proto_reg_handoff_generic " << protocol_data.proto_idx);
}

//*****************************************************************************
// get_interpret_ostream
//*****************************************************************************

ostream &  get_interpret_ostream()
{
    if (get_debug())
    {
        return  get_state_ostream();
    }
    else
    {
        static ostringstream    oss;
        return  oss;
    }
}

//*****************************************************************************
// proto_init_routine
// Called when :
// - a new capture is started ?
// - a new capture file is loaded
// - end of wireshark
// NOT called when :
// - a new display filter is applied or cancelled
// - colorization is applied or cancelled
// !!!!!!!!!!!!!!! a verifier !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// voir pinfo->fd->num == 1
// voir reset callback of register_tap_listener
//*****************************************************************************

void    proto_init_routine(T_generic_protocol_data  & protocol_data)
{
    C_debug_set_temporary      debug_dissect_main(protocol_data.DEBUG);
    M_TRACE_ENTER ("proto_init_routine", protocol_data.PROTOABBREV);


    // Begin/load a new capture/file or ...
    interpret_builder_begin(protocol_data.type_definitions);


    // remove all global.*
    if (protocol_data.GLOBAL_DATA_TYPE != "")
    {
        // reset global data
        protocol_data.ws_data.global_data = T_generic_protocol_global_data();

        // init global data
        C_interpret_builder_set_temporary  interpret_builder_set_temporary(nullptr);
        T_interpret_data   & interpret_data = * protocol_data.ws_data.global_data.RCP_initialized_data;
        const T_byte       * in_out_P_bytes = nullptr;
        size_t               in_out_sizeof_bytes = 0;
        const string         str_interpret = "var " + protocol_data.GLOBAL_DATA_TYPE + " global = zero;";
        istringstream        iss(str_interpret.c_str());
        ostream            & os = get_interpret_ostream();
        bool    result = interpret_bytes (protocol_data.type_definitions,
                                                            in_out_P_bytes,
                                                            in_out_sizeof_bytes,
                                                            iss,
                                                            os,
                                                            os,
                                                            interpret_data);
        if (result == false)
        {
            // must report
            string    str_report = protocol_data.PROTOABBREV + " : Generic dissector is not able to interpret the global data";
            M_TRACE_FATAL (str_report);
            wsgd_report_failure(str_report.c_str());
        }
    }
}

//*****************************************************************************
// proto_init_routine
//*****************************************************************************

// ICIOA
extern vector<T_generic_protocol_data>  S_protocol_data;

void    proto_init_routine()
{
    for (uint   proto_idx = 0; proto_idx < S_protocol_data.size(); ++proto_idx)
    {
        T_generic_protocol_data  & protocol_data = S_protocol_data[proto_idx];
        if (protocol_data.is_proto_usable())
        {
            proto_init_routine(protocol_data);
        }
    }
}

//*****************************************************************************
// cpp_proto_reg_handoff_generic
//*****************************************************************************
extern "C"
void    cpp_proto_reg_handoff_generic()
{
    uint   proto_idx = 0;
    for (proto_idx = 0; proto_idx < S_protocol_data.size(); ++proto_idx)
    {
        if (S_protocol_data[proto_idx].is_proto_usable())
        {
            cpp_proto_reg_handoff_generic_proto(S_protocol_data[proto_idx]);
        }
    }

    T_generic_protocol_subdissector_data::data_handle = find_dissector("data");
    register_init_routine(proto_init_routine);
}

//*****************************************************************************
// dissect
//*****************************************************************************

/******************************************************************************
 * cpp_dissect_generic_add_tree
 *****************************************************************************/

proto_tree  * cpp_dissect_generic_add_tree(const int           proto_idx,
                                                 proto_item  * item)
{
    T_generic_protocol_data     & protocol_data = get_protocol_data(proto_idx);
    T_generic_protocol_ws_data  * P_protocol_ws_data = &protocol_data.ws_data;

    return  proto_item_add_subtree(item, P_protocol_ws_data->fields_data.ett_id[K_ANY_WSGD_FIELD_IDX]);
}

/******************************************************************************
 * cpp_dissect_generic_set_packet_id_str
 *****************************************************************************/

void    cpp_dissect_generic_set_packet_id_str(T_generic_protocol_data  & UNUSED(protocol_data),
                                                    tvbuff_t     * UNUSED(tvb),
                                                    packet_info  * pinfo,
                                                    proto_tree   * UNUSED(tree),
                                              const string       & packet_id_str)
{
    M_TRACE_ENTER("cpp_dissect_generic_set_packet_id_str", packet_id_str);

    // ICIOA : do not print the port's names as tcp dissector does
    // must use get_tcp_port (or ...) from epan/addr_resolv.h
    // I choose to NOT do it (for now) :
    // - perhaps I'm not on tcp (how to know it ?)
    // - always use generic dissector with NOT public protocol on NOT public ports
    // - is it normal to display the parent proto ports ?
    // - what will happen with a sub proto ?
#if 0
    col_add_fstr(pinfo->cinfo, COL_INFO,
                 "%d > %d %s",
                 pinfo->srcport, pinfo->destport, packet_id_str.c_str());
#else
    col_append_fstr(pinfo->cinfo, COL_INFO, " %s", packet_id_str.c_str());
#endif
}

/******************************************************************************
 * cpp_dissect_generic_set_packet_summary_str
 *****************************************************************************/

proto_item  * cpp_dissect_generic_set_packet_summary_str(T_generic_protocol_data  & protocol_data,
                                                               tvbuff_t     * tvb,
                                                               packet_info  * UNUSED(pinfo),
                                                               proto_tree   * tree,
                                                         const string  & packet_summary_str)
{
    T_generic_protocol_ws_data   * P_protocol_ws_data = &protocol_data.ws_data;

    proto_item  * generic_item = proto_tree_add_protocol_format(tree, P_protocol_ws_data->proto_generic, tvb, 0, -1,
            "%s, %s",
            protocol_data.PROTONAME.c_str(), packet_summary_str.c_str());
    return  generic_item;
}


//*****************************************************************************
// pinfo_address_to_string
//*****************************************************************************
#if 0
typedef enum {
  AT_NONE,		/* no link-layer address */
  AT_ETHER,		/* MAC (Ethernet, 802.x, FDDI) address */
  AT_IPv4,		/* IPv4 */
  AT_IPv6,		/* IPv6 */
  AT_IPX,		/* IPX */
  AT_SNA,		/* SNA */
  AT_ATALK,		/* Appletalk DDP */
  AT_VINES,		/* Banyan Vines */
  AT_OSI,		/* OSI NSAP */
  AT_ARCNET,	/* ARCNET */
  AT_FC,		/* Fibre Channel */
  AT_SS7PC,		/* SS7 Point Code */
  AT_STRINGZ,	/* null-terminated string */
  AT_EUI64,		/* IEEE EUI-64 */
  AT_URI,		/* URI/URL/URN */
  AT_TIPC,		/* TIPC Address Zone,Subnetwork,Processor */
  AT_USB		/* USB Device address 
             * (0xffffffff represents the host) */
} address_type;
#endif

string    pinfo_address_to_string(const address  & addr)
{
    string  str_address = "type=" + get_string(addr.type);
    str_address += "  ";
    for (int  idx_addr = 0; idx_addr < addr.len; ++idx_addr)
    {
        if (addr.type == AT_ETHER)
        {
            if (idx_addr > 0)
                str_address += ":";
            char    tmp[99+1];
            sprintf(tmp, "%02x", (unsigned int)((const unsigned char*)addr.data)[idx_addr]);
            str_address += tmp;
        }
        else
        {
            if (idx_addr > 0)
                str_address += ".";
            str_address += get_string((unsigned int)((const unsigned char*)addr.data)[idx_addr]);
        }
    }

    return  str_address;
}

//*****************************************************************************
// pinfo_nstime_to_string
//*****************************************************************************

string    pinfo_nstime_to_string(const nstime_t  & nstime)
{
    char    tmp[99+1];
    // long cast : permits to handle platforms where time_t is a long
  //  rather than an int (e.g., OS X)
    sprintf (tmp, "%ld.%09d", (long)nstime.secs, nstime.nsecs);
    return  tmp;
}

//*****************************************************************************
// add_pinfo
// pinfo is wireshark internal data.
// pinfo definition could change depending on wireshark version.
//*****************************************************************************

void    add_pinfo(const T_generic_protocol_data  & UNUSED(protocol_data),
                  const packet_info              * pinfo,
                        T_interpret_data         & interpret_data)
{
    interpret_data.pinfo_variable_group_begin();

#define M_ADD_PINFO(NAME)                                             \
    interpret_data.add_read_variable(#NAME, pinfo->NAME)

#define M_ADD_PINFO_STR(NAME)                                             \
    interpret_data.add_read_variable(#NAME, pinfo->NAME ? pinfo->NAME : "")

#define M_ADD_PINFO_ADDRESS(NAME)                                             \
    interpret_data.add_read_variable(#NAME, pinfo_address_to_string(pinfo->NAME).c_str())

#define M_ADD_PINFO_ADDRESS(NAME)                                             \
    interpret_data.add_read_variable(#NAME, pinfo_address_to_string(pinfo->NAME).c_str())

#define M_ADD_PINFO_FD(NAME)                                             \
    interpret_data.add_read_variable(#NAME, pinfo->fd->NAME)

#define M_ADD_PINFO_FD_NSTIME(NAME)                                             \
    interpret_data.add_read_variable(#NAME, pinfo_nstime_to_string(pinfo->fd->NAME));  \
    interpret_data.add_read_variable(#NAME ".secs", pinfo->fd->NAME.secs);  \
    interpret_data.add_read_variable(#NAME ".nsecs", pinfo->fd->NAME.nsecs)

    M_ADD_PINFO(current_proto);
//  M_ADD_PINFO();  // column_info *cinfo;		/* Column formatting information */
    if (pinfo->fd != nullptr)  // frame_data*
    {
        interpret_data.read_variable_group_begin("fd");
        M_ADD_PINFO_FD(num);         /* Frame number */
        M_ADD_PINFO_FD(pkt_len);     /* Packet length */
        M_ADD_PINFO_FD(cap_len);     /* Amount actually captured */
        M_ADD_PINFO_FD(cum_bytes);   /* Cumulative bytes into the capture */
        M_ADD_PINFO_FD_NSTIME(abs_ts);      /* Absolute timestamp */
        M_ADD_PINFO_FD(file_off);    /* File offset */
        interpret_data.read_variable_group_end();
    }
//  M_ADD_PINFO();  // union wtap_pseudo_header *pseudo_header;
//  M_ADD_PINFO();  // GSList *data_src;		/* Frame data sources */
    M_ADD_PINFO_ADDRESS(dl_src);		/* link-layer source address */
    M_ADD_PINFO_ADDRESS(dl_dst);		/* link-layer destination address */
    M_ADD_PINFO_ADDRESS(net_src);		/* network-layer source address */
    M_ADD_PINFO_ADDRESS(net_dst);		/* network-layer destination address */
    M_ADD_PINFO_ADDRESS(src);			/* source address (net if present, DL otherwise )*/
    M_ADD_PINFO_ADDRESS(dst);			/* destination address (net if present, DL otherwise )*/

    // In wireshark 1.1.z, there is a new field here.
    // So, all following fields are not at the same place.

    // wireshark version is "x.y.z<anything>"
    const char  * version_compil = VERSION;
    const char  * version_exec   = epan_get_version();
    if (strncmp(version_compil, version_exec, 4) == 0)
    {
        M_ADD_PINFO_STR(noreassembly_reason);  /* reason why reassembly wasn't done, if any */
        M_ADD_PINFO(fragmented);          /* TRUE if the protocol is only a fragment */
        M_ADD_PINFO(ptype);               /* type of the following two port numbers */
        M_ADD_PINFO(srcport);
        M_ADD_PINFO(destport);
        M_ADD_PINFO_STR(match_string);

        M_ADD_PINFO(can_desegment);
        M_ADD_PINFO(saved_can_desegment);
        M_ADD_PINFO(desegment_offset);
        M_ADD_PINFO(desegment_len);
        M_ADD_PINFO(want_pdu_tracking);
        M_ADD_PINFO(bytes_until_next_pdu);
        M_ADD_PINFO(p2p_dir);
    // layers
//	if (pinfo->layers != nullptr)
//	{
//		//interpret_data.read_variable_group_begin("layers");
//		wmem_list_frame_t * layer_current = wmem_list_head(pinfo->layers);
//		int layer_current_idx = 0;
//		while (layer_current != nullptr)
//		{
//			const void * layer_data = wmem_list_frame_data(layer_current);
//			if (layer_data != nullptr)
//			{
//				const int   proto_id = GPOINTER_TO_UINT(layer_data);
//				const char* proto_name = proto_get_protocol_filter_name(proto_id);
//#if 0
//				// Good display format but NOT possible to use it
//				const std::string  variable_name = "layers[" + get_string(layer_current_idx) + "]";
//#else
//				// Possible to use (but only with hard-coded value !!!)
//				const std::string  variable_name = "layers" + get_string(layer_current_idx) + "";
//				interpret_data.read_variable_group_begin(variable_name);
//#endif
//				interpret_data.add_read_variable(variable_name, "id", get_string(proto_id));
//				interpret_data.add_read_variable(variable_name, "name", ((proto_name != nullptr) ? proto_name : "?"));
//				interpret_data.read_variable_group_end();
//			}
//			layer_current = wmem_list_frame_next(layer_current);
//			++layer_current_idx;
//		}
//		//interpret_data.read_variable_group_end();
//	}
        M_ADD_PINFO(curr_layer_num);
        M_ADD_PINFO(link_number);
        M_ADD_PINFO(clnp_srcref);
        M_ADD_PINFO(clnp_dstref);
        M_ADD_PINFO(link_dir);
        //{
        //	interpret_data.read_variable_group_begin("proto_data");
        //	GSList*  next = pinfo->proto_data;
        //	while (next != nullptr)
        //	{
        //		gpointer data = next->data;
        //		if (data != nullptr)
        //		{
        //		}
        //		next = next->next;
        //	}
        //	interpret_data.read_variable_group_end();
        //}
    }

    interpret_data.pinfo_variable_group_end();
}

//*****************************************************************************
// update_pinfo_ports
//*****************************************************************************

void    update_pinfo_ports(const T_generic_protocol_data  & protocol_data,
                                 packet_info              * pinfo,
                                 T_interpret_data         & interpret_data)
{
    if ((protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_2 == "") ||
        (protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1 == ""))
        return;

    for (vector<T_generic_protocol_data::T_parent>::const_iterator
                                    parent_iter  = protocol_data.PARENTS.begin();
                                    parent_iter != protocol_data.PARENTS.end();
                                  ++parent_iter)
    {
        const T_generic_protocol_data::T_parent  & parent = * parent_iter;

        /* TCP/UDP */
        if (parent.PARENT_SUBFIELD == "ip.proto")
        {
            if (parent.PARENT_SUBFIELD_VALUES_int[0] == 0x6)
            {
                pinfo->ptype    = PT_TCP;
            }
            else if (parent.PARENT_SUBFIELD_VALUES_int[0] == 0x11)
            {
                pinfo->ptype    = PT_UDP;
            }
            else
            {
                continue;
            }

            longlong    srcport;
            interpret_data.get_int_value_of_read_variable(
                                        protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_1,
                                        srcport);
            pinfo->srcport  = static_cast<guint32>(srcport);

            longlong    destport;
            interpret_data.get_int_value_of_read_variable(
                                        protocol_data.SUBPROTO_SUBFIELD_FROM_REAL_2,
                                        destport);
            pinfo->destport = static_cast<guint32>(destport);

            return;
        }
    }
}

//*****************************************************************************
// cpp_dissect_generic
//*****************************************************************************

gint    cpp_dissect_generic(      T_generic_protocol_data  & protocol_data,
                                  tvbuff_t                 * tvb,
                            const void                     * ptr_raw_data,
                            const int                        length_raw_data,
                                  packet_info              * pinfo,
                                  proto_tree               * msg_root_tree,
                            const long                       msg_number_inside_packet)
{
    M_TRACE_ENTER ("cpp_dissect_generic", protocol_data.PROTOABBREV << " ("
        << pinfo->fd->num << "/"
        << msg_number_inside_packet << ", "
        << tvb << ", "
        << ptr_raw_data << ", "
        << length_raw_data << ", "
        << pinfo << ", "
        << msg_root_tree << ")");

    proto_tree   * tree = msg_root_tree;

    const int                  proto_idx = protocol_data.proto_idx;
    M_TRACE_DEBUG ("proto_idx = " << proto_idx);
    ostream                  & os = get_interpret_ostream();

    // It could be mandatory to interpret the entire msg (even if msg_root_tree is nullptr)
    bool      mandatory_to_interpret_the_entire_msg = (tree != nullptr);
    if (protocol_data.GLOBAL_DATA_TYPE != "")
    {
        mandatory_to_interpret_the_entire_msg = true;
    }
    else if (protocol_data.ws_data.tap_data.tap_is_needed)
    {
        mandatory_to_interpret_the_entire_msg = true;
    }

    // interpret data.
    // ATTENTION, we must NOT create a new interpret_data if it already exist into global data.
    //  because it could be referenced by its memory address into other global data.
    // This is the current implementation of msg type. Not a good idea, but ...
    T_RCP_interpret_data              RCP_interpret_data;

    // Global data.
    T_generic_protocol_saved_interpreted_data  * P_where_to_save_interpret_data = nullptr;
    T_RCP_interpret_data                         RCP_prev_global_interpret_data = nullptr;
    if (protocol_data.GLOBAL_DATA_TYPE != "")
    {
        // Search for previous msg
        T_RCP_interpret_data    RCP_prev_saved_interpret_data = nullptr;

        // Compute P_prev_saved_interpret_data
        // Compute P_where_to_save_interpret_data
        const long                packet_number = pinfo->fd->num;
        for (vector<T_generic_protocol_saved_interpreted_data>::reverse_iterator
                rev_iter  = protocol_data.ws_data.global_data.saved_interpreted_datas.rbegin();
                rev_iter != protocol_data.ws_data.global_data.saved_interpreted_datas.rend();
              ++rev_iter)
        {
            T_generic_protocol_saved_interpreted_data  & saved_interpreted_data = *rev_iter;

            if ((saved_interpreted_data.packet_number == packet_number) &&
                (saved_interpreted_data.msg_number_inside_packet == msg_number_inside_packet))
            {
                // Found
                P_where_to_save_interpret_data = & saved_interpreted_data;

                ++rev_iter;
                if (rev_iter != protocol_data.ws_data.global_data.saved_interpreted_datas.rend())
                {
                    RCP_prev_saved_interpret_data = rev_iter->RCP_interpret_data;
                }

                break;
            }
            else if (saved_interpreted_data.packet_number < packet_number)
            {
                // Not found
                RCP_prev_saved_interpret_data = rev_iter->RCP_interpret_data;
                break;
            }
            else if ((saved_interpreted_data.packet_number == packet_number) &&
                    (saved_interpreted_data.msg_number_inside_packet < msg_number_inside_packet))
            {
                // Not found
                RCP_prev_saved_interpret_data = rev_iter->RCP_interpret_data;
                break;
            }
        }

        if (P_where_to_save_interpret_data != nullptr)
        {
            RCP_interpret_data = P_where_to_save_interpret_data->RCP_interpret_data;
            RCP_interpret_data->reset();
        }
        else
        {
            RCP_interpret_data = new T_interpret_data;
            M_TRACE_DEBUG ("GLOBAL_DATA new T_interpret_data=" << RCP_interpret_data.get());
        }
        // Save global data pointer
        RCP_prev_global_interpret_data = RCP_prev_saved_interpret_data;
        if (RCP_prev_global_interpret_data.get() == nullptr)
        {
            RCP_prev_global_interpret_data = protocol_data.ws_data.global_data.RCP_initialized_data;
        }
        RCP_interpret_data->copy_global_values(*RCP_prev_global_interpret_data);
    }

    // interpret data.
    if (! RCP_interpret_data)
    {
        RCP_interpret_data = new T_interpret_data;
        M_TRACE_DEBUG ("new T_interpret_data=" << RCP_interpret_data.get());
    }
    T_interpret_data                & interpret_data = * RCP_interpret_data;

    // pinfo is wireshark internal data.
    if (protocol_data.MANAGE_WIRESHARK_PINFO == true)
    {
        add_pinfo(protocol_data, pinfo, interpret_data);
    }

    // create this_msg variable
    if (protocol_data.GLOBAL_DATA_TYPE != "")
    {
        interpret_data.add_this_msg();
    }

    // No wireshark output.
    C_interpret_builder_set_temporary  interpret_builder_set_temporary(nullptr);

    string    summary;
    string    MSG_ID_FIELD_NAME;

    // Read MSG_HEADER_TYPE.
    // Retrieve the MSG_ID value
    // -> set the packet_id_str
    // -> init summary
    {
        // Read MSG_HEADER_TYPE.
        const T_byte       * in_out_P_bytes = static_cast<const T_byte *>(ptr_raw_data);
        size_t               in_out_sizeof_bytes = length_raw_data;

        // NB: do not set a name to the variable
        //      because the name will be mandatory everywhere in the code and/or wsgd file
        //      for MSG_ID_FIELD_NAME, MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES and MSG_MAIN_TYPE
        const string         str_interpret = protocol_data.MSG_HEADER_TYPE + " " + "\"\" ;";
        istringstream        iss(str_interpret.c_str());

        C_byte_interpret_wsgd_builder_base  wsgd_builder(proto_idx, tvb, pinfo, tree, tree);
        C_interpret_builder_set_temporary   interpret_builder_set_temporary2(&wsgd_builder);

        if (protocol_data.PACKET_CONTAINS_ONLY_COMPLETE_MSG)
        {
        // All the input data is present.
        wsgd_builder.set_is_input_data_complete(true);
        }

        // returns false if NOT enough bytes -> DESEGMENT managed by wsgd_builder.
        // NB: returns true even if MSG_HEADER_TYPE is unknown !
        bool    result = interpret_bytes (protocol_data.type_definitions,
                                                        in_out_P_bytes,
                                                        in_out_sizeof_bytes,
                                                        iss,
                                                        os,
                                                        os,
                                                        interpret_data);
        if (result == false)
        {
            M_TRACE_WARNING ("Error during interpret_bytes for the header, could be not enough data");
            return  0;
        }

        const T_attribute_value *  P_attr_MSG_ID_FIELD_NAME = interpret_data.get_P_attribute_value_of_read_variable (protocol_data.MSG_ID_FIELD_NAME);

        if (P_attr_MSG_ID_FIELD_NAME == nullptr)
        {
            // The header has been entirely read, but the MSG_ID_FIELD_NAME has NOT been found !!!
            // ICIOA user NOT warned !!!
            M_TRACE_FATAL ("Did NOT find " << protocol_data.MSG_ID_FIELD_NAME << " (MSG_ID_FIELD_NAME)");
            return  0;
        }

        // Retrieve the MSG_ID value
        MSG_ID_FIELD_NAME = interpret_data.get_full_str_value_of_read_variable(protocol_data.MSG_ID_FIELD_NAME);
        M_TRACE_DEBUG ("MSG_ID_FIELD_NAME = " << MSG_ID_FIELD_NAME);

        string    msg_id;
        if (protocol_data.MSG_TITLE != "")
        {
            string    MSG_TITLE = interpret_data.get_full_str_value_of_read_variable(protocol_data.MSG_TITLE);
            M_TRACE_DEBUG ("MSG_TITLE = " << MSG_TITLE);
            msg_id = "[" + MSG_TITLE + "]";
        }
        else
        {
            msg_id = "[" + MSG_ID_FIELD_NAME + "]";
        }

        // -> set the packet_id_str
        cpp_dissect_generic_set_packet_id_str(protocol_data, tvb, pinfo, tree, msg_id);

        /* Update the pinfo ports */
        update_pinfo_ports(protocol_data, pinfo, interpret_data);


        if ((tree == nullptr) && (mandatory_to_interpret_the_entire_msg != true))
        {
            if (protocol_data.PACKET_CONTAINS_ONLY_COMPLETE_MSG &&
                protocol_data.PACKET_CONTAINS_ONLY_1_MSG)
                return length_raw_data;

            if (protocol_data.MSG_TOTAL_LENGTH != "")
            {
                C_value  msg_total_length = compute_expression_no_io(protocol_data.type_definitions, interpret_data, protocol_data.MSG_TOTAL_LENGTH);
                if (length_raw_data < msg_total_length.get_int())
                {
                    // Not an error, wait for the next segment.
                    pinfo->desegment_offset = 0;             /* Start at beginning next time */
                    pinfo->desegment_len = static_cast<guint32>(msg_total_length.get_int() - length_raw_data);
                    return  0;
                }
                else
                {
                    pinfo->desegment_len = 0;           // 2011/05/15
                    return  msg_total_length.get_int_int();
                }
            }

            // I do not know the size of the message.
            // I must interpret it completely to know where it ends
            //  (and so where start the next message) !
            mandatory_to_interpret_the_entire_msg = true;
        }

        // -> init summary
        summary += protocol_data.MSG_ID_FIELD_NAME;
        summary += ": ";
        summary += MSG_ID_FIELD_NAME;
        summary += "  ";
    }

    if (mandatory_to_interpret_the_entire_msg)
    {
        // Retrieve all the MSG_SUMMARY_SUBSIDIARY values and complete summary.
        for (vector<string>::const_iterator  iter  = protocol_data.MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES.begin();
                                             iter != protocol_data.MSG_SUMMARY_SUBSIDIARY_FIELD_NAMES.end();
                                           ++iter)
        {
            const string    MSG_SUMMARY_SUBSIDIARY_FIELD_NAME = interpret_data.get_full_str_value_of_read_variable(*iter);
            M_TRACE_DEBUG ("MSG_SUMMARY_SUBSIDIARY_FIELD_NAME = " << MSG_SUMMARY_SUBSIDIARY_FIELD_NAME);

            summary += *iter;
            summary += ": ";
            summary += MSG_SUMMARY_SUBSIDIARY_FIELD_NAME;
            summary += "  ";
        }

        // Set first item.
        proto_item  * proto_item = cpp_dissect_generic_set_packet_summary_str(protocol_data, tvb, pinfo, tree, summary);
        tree = cpp_dissect_generic_add_tree(proto_idx, proto_item);

        // Read MSG_MAIN_TYPE.
        const T_byte       * in_out_P_bytes = static_cast<const T_byte *>(ptr_raw_data);
        size_t               in_out_sizeof_bytes = length_raw_data;

        C_byte_interpret_wsgd_builder      wsgd_builder(proto_idx, tvb, pinfo, tree, msg_root_tree);
        C_interpret_builder_set_temporary  interpret_builder_set_temporary2(&wsgd_builder);

        // Check that the packet identifier has been found.
        if (MSG_ID_FIELD_NAME == "")
        {
            // Could happen if :
            // - bad MSG_HEADER_TYPE specified (checked at initialization)
            // - bad MSG_ID_FIELD_NAME specified
            // - too small packet (checked before IF MSG_HEADER_LENGTH has been set)
            M_TRACE_FATAL (protocol_data.MSG_ID_FIELD_NAME << " (MSG_ID_FIELD_NAME) NOT found");
            const string         str_interpret = "fatal  \"value of " + protocol_data.MSG_ID_FIELD_NAME + " (MSG_ID_FIELD_NAME) NOT found into " + protocol_data.MSG_HEADER_TYPE + " (MSG_HEADER_TYPE)\" ;";

            interpret_bytes (               protocol_data.type_definitions,
                                            in_out_P_bytes,
                                            in_out_sizeof_bytes,
                                            str_interpret,
                                            os,
                                            os,
                                            interpret_data);
//          return  0;
        }

        // Manage MSG_TOTAL_LENGTH
        // More or less tested.
        size_t    sizeof_bytes_NOT_given_to_interpretor = 0;
        if (protocol_data.MSG_TOTAL_LENGTH != "")
        {
            C_value  msg_total_length = compute_expression_no_io(protocol_data.type_definitions, interpret_data, protocol_data.MSG_TOTAL_LENGTH);
            if (length_raw_data < msg_total_length.get_int())
            {
                // Not an error, wait for the next segment.
                pinfo->desegment_offset = 0;             /* Start at beginning next time */
                pinfo->desegment_len = static_cast<guint32>(msg_total_length.get_int() - length_raw_data);
                return  0;
            }

            // Reduce the interpretor accessible data.
            in_out_sizeof_bytes = msg_total_length.get_int_size_t();
            sizeof_bytes_NOT_given_to_interpretor = length_raw_data - msg_total_length.get_int_size_t();

            // All the input data is present.
            wsgd_builder.set_is_input_data_complete(true);
        }

        if ((sizeof_bytes_NOT_given_to_interpretor > 0) && (protocol_data.PACKET_CONTAINS_ONLY_1_MSG))
        {
            // The packet seems to contains more than 1 msg or
            //  MSG_TOTAL_LENGTH is not good or
            //  message is not good.
        }

        if (protocol_data.PACKET_CONTAINS_ONLY_COMPLETE_MSG)
        {
            // All the input data is present.
            wsgd_builder.set_is_input_data_complete(true);
        }

        if (RCP_prev_global_interpret_data.get() != nullptr)
        {
            // global data could have been modified by header
            // Since we read again the header, must start with the original values
            interpret_data.copy_global_values(*RCP_prev_global_interpret_data);
        }

        // Intrepretation of the main type.
        string         str_interpret_main;
        if (protocol_data.MSG_MAIN_TYPE != "")
        {
            str_interpret_main = protocol_data.MSG_MAIN_TYPE;
        }
        else
        {
            // Must choose the main type depending on the source port and the dest port.
            // 2009/03/14 : these tests work only above a protocol which fills srcport and destport.
            // Will NOT work above my own proto.
            for (vector<T_generic_protocol_data::T_parent>::const_iterator
                                      parent_iter  = protocol_data.PARENTS.begin();
                                      parent_iter != protocol_data.PARENTS.end();
                                    ++parent_iter)
            {
                const T_generic_protocol_data::T_parent  & parent = * parent_iter;

                for (size_t   idx = 0; idx < parent.PARENT_SUBFIELD_VALUES_int.size(); ++idx)
                {
                    if (pinfo->srcport == parent.PARENT_SUBFIELD_VALUES_int[idx])
                    {
                        str_interpret_main = protocol_data.MSG_FROM_MAIN_TYPE;
                        break;
                    }
                    else if (pinfo->destport == parent.PARENT_SUBFIELD_VALUES_int[idx])
                    {
                        str_interpret_main = protocol_data.MSG_TO_MAIN_TYPE;
                        break;
                    }
                }
                if (str_interpret_main != "")
                    break;
            }
            if (str_interpret_main == "")
            {
                // Port NOT found, choose FROM by default.
                str_interpret_main = protocol_data.MSG_FROM_MAIN_TYPE;
                // This could happen if user use Decode as.
                // How to warn the user about the problem ?
                // ATTENTION, this will warn the user for each dissection !!!
                // -> static value to avoid another call (or too much call) ?
                // -> How to know if it is the same capture or Decode as operation ?
                wsgd_report_failure("Generic dissector is NOT able to choose between MSG_FROM_MAIN_TYPE and MSG_TO_MAIN_TYPE.\n"
                        "This could happen if you have choose Decode as for a packet with source and dest ports not into PARENT_SUBFIELD_VALUES.\n"
                        "\n"
                        "MSG_FROM_MAIN_TYPE has been taken.\n");
            }
        }
        str_interpret_main += " \"\" ;";
        istringstream        iss_main(str_interpret_main.c_str());

        bool    result = interpret_bytes (   protocol_data.type_definitions,
                                            in_out_P_bytes,
                                            in_out_sizeof_bytes,
                                            iss_main,
                                            os,
                                            os,
                                            interpret_data);

        const int   proto_item_len = length_raw_data - in_out_sizeof_bytes - sizeof_bytes_NOT_given_to_interpretor;
        proto_item_set_len(proto_item, proto_item_len);

        // Global data and Tap.
        if (result == true)
        {
            if ((P_where_to_save_interpret_data == nullptr) &&
                (protocol_data.GLOBAL_DATA_TYPE != ""))
            {
                M_TRACE_DEBUG ("GLOBAL_DATA saved_interpreted_datas.push_back");

                protocol_data.ws_data.global_data.saved_interpreted_datas.push_back(T_generic_protocol_saved_interpreted_data());
                T_generic_protocol_saved_interpreted_data  & saved_interpreted_data = protocol_data.ws_data.global_data.saved_interpreted_datas.back();
                saved_interpreted_data.packet_number = pinfo->fd->num;
                saved_interpreted_data.msg_number_inside_packet = msg_number_inside_packet;

                P_where_to_save_interpret_data = & saved_interpreted_data;
            }

            if (P_where_to_save_interpret_data != nullptr)
            {
                P_where_to_save_interpret_data->RCP_interpret_data = RCP_interpret_data;
            }

            protocol_data.ws_data.tap_data.RCP_last_msg_interpret_data = RCP_interpret_data;

            tap_queue_packet(protocol_data.ws_data.tap_data.proto_tap, pinfo, &protocol_data);
        }

        // Verify that the entire message has been dissect.
        // Only if interpretation result is ok.
        // If not, I suppose that an error is already displayed. 
        if ((result == true) && (in_out_sizeof_bytes > 0))
        {
            if ((protocol_data.PACKET_CONTAINS_ONLY_1_MSG) || (protocol_data.MSG_TOTAL_LENGTH != ""))
            {
                const string         str_interpret = "error  \"" + get_string(in_out_sizeof_bytes) + " bytes NOT read into message\" ;";
                istringstream        iss(str_interpret.c_str());

                interpret_bytes (                   protocol_data.type_definitions,
                                                    in_out_P_bytes,
                                                    in_out_sizeof_bytes,
                                                    iss,
                                                    os,
                                                    os,
                                                    interpret_data);
            }
        }

        // Verify that the entire packet has been dissect.
        // Only if interpretation result is ok.
        // If not, I suppose that an error is already displayed. 
        if ((result == true) && (sizeof_bytes_NOT_given_to_interpretor > 0))
        {
            if (protocol_data.PACKET_CONTAINS_ONLY_1_MSG)
            {
            const string         str_interpret = "error  \"" + get_string(sizeof_bytes_NOT_given_to_interpretor) + " bytes NOT read into packet\" ;";
            istringstream        iss(str_interpret.c_str());

            interpret_bytes (                   protocol_data.type_definitions,
                                                in_out_P_bytes,
                                                in_out_sizeof_bytes,
                                                iss,
                                                os,
                                                os,
                                                interpret_data);
            }
            // ICIOA ne fonctionne pas
            // Y a-t-il un moyen pour que wireshark re-appelle le dissector ???
            // je ne crois pas, il faut se rappeler soi-même

            // 2009/03/03 en testant MSG_TOTAL_LENGTH, il semble bien que le dissector est appelé plusieurs fois.
            // appele 1 fois de plus MAIS pas plus ???
            // -> voir offset de tvbbuf ???
            // tvb_reported_length & tvb_reported_length_remaining ???
            pinfo->desegment_offset = length_raw_data - sizeof_bytes_NOT_given_to_interpretor;
            pinfo->desegment_len = 0;           // 2011/05/15 
//          pinfo->desegment_len = 10;         // ICIOA essai avec une taille de msg en dur -> dissector non rappele
//          pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;    // ICIOA -> dissector non rappele
        }

        if (result == true)
        {
            // Msg is ended, some data are no more necessary
            if (protocol_data.ws_data.tap_data.tap_is_needed == false)
            {
                interpret_data.msg_is_ended();
            }
            // else generic_stats_tree_packet will do it
        }

        return  proto_item_len;
    }

    M_FATAL_COMMENT("Bug in the software");
    return  0;  // avoid compiler warning
}

//*****************************************************************************
// get_generic_pdu_len
//*****************************************************************************
#if 0
static guint
get_generic_pdu_len(packet_info  * pinfo _U_, tvbuff_t  * tvb, int  offset)  // ICIOA absolument pas termine
{
    guint    length = tvb_get_ntohs(tvb, offset+2);
//    guint    length = tvb_get_guint8(tvb, offset+2);
    length = 32;
    return length;
}
#endif

//*****************************************************************************
// dissect_generic_proto
//*****************************************************************************

gint    dissect_generic_proto(    T_generic_protocol_data  & protocol_data,
                                  tvbuff_t                 * tvb,
                            const void                     * ptr_raw_data,
                            const int                        length_raw_data,
                                  packet_info              * pinfo,
                                  proto_tree               * tree,
                            const long                       msg_number_inside_packet)
{
    M_TRACE_ENTER ("dissect_generic_proto", protocol_data.PROTOABBREV << " ("
                    << pinfo->fd->num << "/"
                    << msg_number_inside_packet << ", "
                    << tvb << ", "
                    << pinfo << ", "
                    << tree << ")");

    if ((protocol_data.MSG_HEADER_LENGTH > 0) &&
        (length_raw_data < protocol_data.MSG_HEADER_LENGTH))
    {
        // Not an error, wait for the next segment.
        M_TRACE_DEBUG("Not enougth data (" << length_raw_data << ") to read the header. Wait for the next segment.");
        pinfo->desegment_offset = 0;             /* Start at beginning next time */
        pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
        return  0;
    }

    gint  result = 0;

    M_TRACE_DEBUG ("wsgd_debug dissect+ " << protocol_data.PROTOABBREV << " ("
                    << pinfo->fd->num << "/"
                    << msg_number_inside_packet << ") "
                    << C_debug_object_counter::get_debug_string());

    try
    {
        result = cpp_dissect_generic(protocol_data,
                                    tvb,
                                    ptr_raw_data,
                                    length_raw_data,
                                    pinfo,
                                    tree,
                                    msg_number_inside_packet);
    }
    catch(std::exception& e)
    {
        M_TRACE_FATAL("Unexpected exception " << e.what());
        result = 0;
    }
    catch(...)
    {
        M_TRACE_FATAL("Unexpected unknow exception.");
        result = 0;
    }

    M_TRACE_DEBUG ("wsgd_debug dissect- " << protocol_data.PROTOABBREV << " ("
                    << pinfo->fd->num << "/"
                    << msg_number_inside_packet << ") "
                    << C_debug_object_counter::get_debug_string());
    return  result;
}

//*****************************************************************************
// dissect_generic_proto
//*****************************************************************************
gint
dissect_generic_proto(const int    proto_idx, tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    T_generic_protocol_data  & protocol_data = get_protocol_data(proto_idx);
    C_debug_set_temporary      debug_dissect_main(protocol_data.DEBUG);

    M_TRACE_ENTER ("dissect_generic_proto", protocol_data.PROTOABBREV << " ("
                    << proto_idx << ", "
                    << tvb << ", "
                    << pinfo << ", "
                    << tree << ")");

    col_set_str(pinfo->cinfo, COL_PROTOCOL, protocol_data.PROTOSHORTNAME.c_str());
    col_add_fstr(pinfo->cinfo, COL_INFO,
                "%d > %d",
                pinfo->srcport, pinfo->destport);


    gint    offset_where_dissection_stops = 0;
    long    msg_number_inside_packet = 0;
    do
    {
        // compute new tvb 
        tvbuff_t  * sub_tvb = tvb_new_subset_length_caplen(tvb, offset_where_dissection_stops, -1, -1);

        gint    sub_offset_where_dissection_stops = 
                                        dissect_generic_proto(protocol_data,
                                                            sub_tvb,
                                                            tvb_get_ptr(sub_tvb, 0, -1),
                                                            tvb_reported_length_remaining(sub_tvb, 0),
                                                            pinfo,
                                                            tree,
                                                            msg_number_inside_packet);
        if (pinfo->desegment_len != 0)
        {
            M_TRACE_DEBUG("Not enougth data to read the message. Wait for the next segment.");

            pinfo->desegment_offset += offset_where_dissection_stops;
        }
        else
        {
            M_TRACE_DEBUG("Message entirely read (no desegmentation required)");
        }

        offset_where_dissection_stops += sub_offset_where_dissection_stops;
        ++msg_number_inside_packet;

        // Stop the loop if 
        if ((pinfo->desegment_len != 0) ||                                               // Message not entirely read
            (offset_where_dissection_stops >= tvb_reported_length_remaining(tvb, 0)) ||  // No more data to read
            (protocol_data.PACKET_CONTAINS_ONLY_1_MSG) ||                                // Only 1 msg per packet
            (sub_offset_where_dissection_stops <= 0))                                    // Nothing has been read
        {
            break;
        }
    } while (true);



    if ((pinfo->desegment_len != 0) && (offset_where_dissection_stops == 0))
    {
        // to avoid tshark 1.12.0 rc2 crash (rejects return 0 when desegment asked)
//      return tvb_length(tvb);
        return  1;
    }
    return  offset_where_dissection_stops;
}

//*****************************************************************************
// heuristic_generic_proto
//*****************************************************************************

gboolean    heuristic_generic_proto(const int      proto_idx,
                                    tvbuff_t     * tvb,
                                    packet_info  * pinfo,
                                    proto_tree   * tree)
{
    T_generic_protocol_data  & protocol_data = get_protocol_data(proto_idx);
    C_debug_set_temporary      debug_dissect_main(protocol_data.DEBUG);

    M_TRACE_ENTER ("heuristic_generic_proto", protocol_data.PROTOABBREV << " ("
                    << proto_idx << ", "
                    << tvb << ", "
                    << pinfo << ", "
                    << tree << ")");

    if (protocol_data.HEURISTIC_FUNCTION != "")
    {
        // Check of tvb data to know if it is our protocol
        try
        {
            const void           * ptr_raw_data = tvb_get_ptr(tvb, 0, -1);
            const int              length_raw_data = tvb_reported_length_remaining(tvb, 0);
            if ((ptr_raw_data == nullptr) || (length_raw_data <= 0))
            {
                return  false;
            }
            T_frame_data           frame_data(ptr_raw_data, 0, length_raw_data * 8);

            const string           expression_str = protocol_data.HEURISTIC_FUNCTION + " ()";
            T_interpret_data       interpret_data;
            T_decode_stream_frame                             decode_stream_frame;
            C_decode_stream_frame_set_temporary_if_necessary  dsfstin(interpret_data, decode_stream_frame);
//          interpret_data.set_decode_stream_frame(&decode_stream_frame);
            const string           data_name;
            const string         & data_simple_name = data_name;
            ostringstream          os_out;
            ostringstream        & os_err = os_out;

            const C_value          HEURISTIC_FUNCTION_result = compute_expression(protocol_data.type_definitions, interpret_data, frame_data,
                                                                            expression_str,
                                                                            data_name, data_simple_name, os_out, os_err);

            if (HEURISTIC_FUNCTION_result.get_bool() != true)
            {
                return  false;
            }
        }
        catch(std::exception& e)
        {
            M_TRACE_FATAL("Unexpected exception " << e.what());
            return  false;
        }
        catch(...)
        {
            M_TRACE_FATAL("Unexpected unknow exception.");
            return  false;
        }
    }

    // Do not care of return value ?
    dissect_generic_proto(proto_idx, tvb, pinfo, tree);

    return  true;
}

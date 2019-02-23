/*
 * Copyright 2008-2019 Olivier Aveline <wsgd@free.fr>
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

#include "precomp.h"
#include "byte_interpret_builder.h"
#include "byte_interpret.h"                 // is_a_raw_field ...

#include "C_byte_interpret_builder.h"



//*****************************************************************************
// The builder.
//*****************************************************************************

static C_byte_interpret_builder  * S_P_builder = NULL;    // recursive call ok


//*****************************************************************************
// C_interpret_builder_set_temporary
//*****************************************************************************

C_interpret_builder_set_temporary::C_interpret_builder_set_temporary(C_byte_interpret_builder  * P_builder)
    :previous_value(S_P_builder),
     value_modified(false)
{
    set(P_builder);
}

C_interpret_builder_set_temporary::~C_interpret_builder_set_temporary()
{
    unset();
}

void
C_interpret_builder_set_temporary::set(C_byte_interpret_builder  * P_builder)
{
    S_P_builder = P_builder;
    value_modified = true;
}
void
C_interpret_builder_set_temporary::unset()
{
    if (value_modified)
    {
        S_P_builder = previous_value;
        value_modified = false;
    }
}
void
C_interpret_builder_set_temporary::forget()
{
    value_modified = false;
}


//*****************************************************************************
// M_TRACE_CHECK
//*****************************************************************************

#if 1
#define M_TRACE_CHECK()
#else
#define M_TRACE_CHECK()        							\
{        												\
    if (data_simple_name != field_type_name.name)       \
    {        											\
        M_TRACE_INFO("data_simple_name(" << data_simple_name << ") != field_type_name.name(" << field_type_name.name << ")");        \
    }        											\
}
#endif

//*****************************************************************************
// interpret_builder_begin
//*****************************************************************************

void    interpret_builder_begin(const T_type_definitions  & type_definitions)
{
    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_begin_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_begin_cb)(
                    NULL);
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_value
//*****************************************************************************

void    interpret_builder_value(    const T_type_definitions  & type_definitions,
                                    const T_frame_data        & in_out_frame_data,
                                    const T_field_type_name   & field_type_name,
                                    const string              & data_name,
                                    const string              & data_simple_name,
                                    const T_attribute_value   & attribute_value,
                                    const string              & data_value,
                                    const string              & final_type,
                                    const int                   type_bit_size,
                                    const bool                  is_little_endian,
                                    const bool                  error)
{
    M_TRACE_CHECK();

    if (S_P_builder != NULL)
    {
        S_P_builder->value (type_definitions, in_out_frame_data,
                             field_type_name, data_name, data_simple_name,
                             attribute_value, data_value,
                             final_type, type_bit_size,
                             is_little_endian,
                             error);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        const char  * P_error = NULL;
        if (attribute_value.has_error())
        {
            P_error = attribute_value.get_P_error()->c_str();
        }
        
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (attribute_value.get_value().get_type() == C_value::E_type_float)
            {
                if (plugin.byte_interpret_plugin_output_value_float_cb != NULL)
                {
                    M_TRACE_ENTER ("byte_interpret_plugin_output_value_float_cb", "");
                    (*plugin.byte_interpret_plugin_output_value_float_cb)(
                        NULL,
                        NULL,
                        field_type_name.type.c_str(),
                        data_name.c_str(),
                        field_type_name.name.c_str(),
                        final_type.c_str(),
                        attribute_value.get_value().get_flt(),
                        attribute_value.get_value().as_string().c_str(),
                        P_error);
                }
            }
            else if (attribute_value.get_value().get_type() == C_value::E_type_integer)
            {
                if (plugin.byte_interpret_plugin_output_value_integer_cb != NULL)
                {
                    M_TRACE_ENTER ("byte_interpret_plugin_output_value_integer_cb", "");
                    (*plugin.byte_interpret_plugin_output_value_integer_cb)(
                        NULL,
                        NULL,
                        field_type_name.type.c_str(),
                        data_name.c_str(),
                        field_type_name.name.c_str(),
                        final_type.c_str(),
                        attribute_value.get_value().get_int(),
                        attribute_value.get_value().as_string().c_str(),
                        P_error);
                }
            }
            else if (attribute_value.get_value().get_type() == C_value::E_type_string)
            {
                if (plugin.byte_interpret_plugin_output_value_string_cb != NULL)
                {
                    M_TRACE_ENTER ("byte_interpret_plugin_output_value_string_cb", "");
                    (*plugin.byte_interpret_plugin_output_value_string_cb)(
                        NULL,
                        NULL,
                        field_type_name.type.c_str(),
                        data_name.c_str(),
                        field_type_name.name.c_str(),
                        final_type.c_str(),
                        attribute_value.get_value().get_str().c_str(),
                        attribute_value.get_value().as_string().c_str(),
                        P_error);
                }
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_raw_data
//*****************************************************************************

void    interpret_builder_raw_data( const T_type_definitions  & type_definitions,
                                    const T_frame_data        & in_out_frame_data,
                                    const T_interpret_data    & interpret_data,
                                    const T_field_type_name   & field_type_name,
                                    const string              & data_name,
                                    const string              & data_simple_name,
                                    const int                   type_bit_size,
                                    const E_raw_data_type       raw_data_type)
{
    M_TRACE_CHECK();

    if (S_P_builder != NULL)
    {
        S_P_builder->raw_data (type_definitions, in_out_frame_data, interpret_data,
                             field_type_name, data_name, data_simple_name,
                             type_bit_size,
                             raw_data_type);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_raw_data_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_raw_data_cb)(
                        NULL,
                        NULL,
                        field_type_name.type.c_str(),
                        data_name.c_str(),
                        field_type_name.name.c_str(),
                        type_bit_size);
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_group_begin
//*****************************************************************************

void    interpret_builder_group_begin(const T_type_definitions  & type_definitions,
                                      const T_frame_data        & in_out_frame_data,
                                      const T_field_type_name   & field_type_name,
                                      const string              & data_name,
                                      const string              & data_simple_name)
{
    M_TRACE_CHECK();

    if (S_P_builder != NULL)
    {
        S_P_builder->group_begin (type_definitions, in_out_frame_data,
                                  field_type_name, data_name, data_simple_name);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_group_begin_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_group_begin_cb)(
                    NULL,
                    NULL,
                    field_type_name.type.c_str(),
                    data_name.c_str(),
                    field_type_name.name.c_str());
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_group_append_text
//*****************************************************************************

void    interpret_builder_group_append_text(const T_type_definitions  & type_definitions,
                                            const T_frame_data        & in_out_frame_data,
                                            const string              & data_name,
                                            const string              & data_simple_name,
                                            const string              & text)
{
    if (S_P_builder != NULL)
    {
        S_P_builder->group_append_text (type_definitions, in_out_frame_data,
                                        data_name, data_simple_name, text);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_group_append_text_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_group_append_text_cb)(
                    NULL,
                    NULL,
                    data_name.c_str(),
                    data_simple_name.c_str(),
                    text.c_str());
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_group_end
//*****************************************************************************

void    interpret_builder_group_end(const T_type_definitions  & type_definitions,
                                       const T_frame_data        & in_out_frame_data,
                                       const T_field_type_name   & field_type_name,
                                       const string              & data_name,
                                       const string              & data_simple_name,
                                       const int                   type_bit_size)
{
    M_TRACE_CHECK();

    if (S_P_builder != NULL)
    {
        S_P_builder->group_end (type_definitions, in_out_frame_data,
                             field_type_name, data_name, data_simple_name, type_bit_size);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_group_end_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_group_end_cb)(
                    NULL,
                    NULL,
                    field_type_name.type.c_str(),
                    data_name.c_str(),
                    field_type_name.name.c_str());
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_error
//*****************************************************************************

void    interpret_builder_error    (const T_type_definitions  & type_definitions,
                                    const T_frame_data        & in_out_frame_data,
                                    const T_field_type_name   & field_type_name,
                                    const string              & data_name,
                                    const string              & data_simple_name,
                                    const string              & error)
{
    M_TRACE_CHECK();

    if (S_P_builder != NULL)
    {
        S_P_builder->error (type_definitions, in_out_frame_data,
                            field_type_name, data_name, data_simple_name,
                            error);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_error_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_error_cb)(
                    NULL,
                    NULL,
                    data_name.c_str(),
                    field_type_name.name.c_str(),
                    error.c_str());
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_missing_data
//*****************************************************************************

void    interpret_builder_missing_data( const T_type_definitions  & type_definitions,
                                        const T_frame_data        & in_out_frame_data,
                                        const T_interpret_data    & interpret_data,
                                        const T_field_type_name   & field_type_name,
                                        const string              & data_name,
                                        const string              & data_simple_name,
                                        const string              & error)
{
    M_TRACE_CHECK();

    if (S_P_builder != NULL)
    {
        S_P_builder->missing_data (type_definitions, in_out_frame_data, interpret_data,
                            field_type_name, data_name, data_simple_name,
                            error);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_missing_data_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_missing_data_cb)(
                    NULL,
                    NULL,
                    field_type_name.type.c_str(),
                    data_name.c_str(),
                    field_type_name.name.c_str(),
                    error.c_str());
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_cmd_error
//*****************************************************************************

void    interpret_builder_cmd_error(const T_type_definitions  & type_definitions,
                                    const T_frame_data        & in_out_frame_data,
                                    const T_field_type_name   & field_type_name,
                                    const string              & data_name,
                                    const string              & text_to_print)
{
    if (S_P_builder != NULL)
    {
        S_P_builder->cmd_error (type_definitions, in_out_frame_data,
                             field_type_name, data_name, text_to_print);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_cmd_error_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_cmd_error_cb)(
                    NULL,
                    NULL,
                    data_name.c_str(),
                    field_type_name.name.c_str(),
                    text_to_print.c_str());
            }
        }
    }
}

//*****************************************************************************
// interpret_builder_cmd_print ************************************************
//*****************************************************************************

void    interpret_builder_cmd_print(const T_type_definitions  & type_definitions,
                                    const T_frame_data        & in_out_frame_data,
                                    const T_field_type_name   & field_type_name,
                                    const string              & data_name,
                                    const string              & text_to_print)
{
    if (S_P_builder != NULL)
    {
        S_P_builder->cmd_print (type_definitions, in_out_frame_data,
                                field_type_name, data_name, text_to_print);
    }

    if (type_definitions.vector_plugin_output_definition.empty() == false)
    {
        // Call all plugin_ouput
        for (T_vector_plugin_output_definition::const_iterator
                iter  = type_definitions.vector_plugin_output_definition.begin();
                iter != type_definitions.vector_plugin_output_definition.end();
              ++iter)
        {
            const T_plugin_output_definition  & plugin = *iter;

            if (plugin.byte_interpret_plugin_output_cmd_print_cb != NULL)
            {
                (*plugin.byte_interpret_plugin_output_cmd_print_cb)(
                    NULL,
                    NULL,
                    data_name.c_str(),
                    field_type_name.name.c_str(),
                    text_to_print.c_str());
            }
        }
    }
}

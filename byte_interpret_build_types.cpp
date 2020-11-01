/*
 * Copyright 2005-2020 Olivier Aveline <wsgd@free.fr>
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

// Necessary for windows pre-compiled headers.
// At the end, does NOT work : missing functions at link time
// After other includes, does NOT work : does NOT compile (on "ifstream   ifs") 
#include "precomp.h"

#include <cstdio>
#include <cerrno>
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
using namespace std;

#include "byte_interpret_common.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret_parse.h"
#include "F_common_print.h"
#include "byte_interpret_build_types.h"
#include "byte_interpret_build_types_context.h"
#include "byte_interpret_build_types_read_token.h"

// plugin needs gmodule/glib
// Linux 300X must find (cmake) g_module to enable it
// Undocumented feature, nobody will miss it
//#define BYTE_INTERPRET_USE_PLUGIN
#ifdef FRHED
// FRHED do not find gmodule lib
#undef  BYTE_INTERPRET_USE_PLUGIN
#endif


//*****************************************************************************
// byte_interpret_get_include_file_name
//*****************************************************************************

static string    S_basic_directory;

void      byte_interpret_set_include_directory(const string  & dir_name)
{
    S_basic_directory = dir_name;
}

void      byte_interpret_append_include_directory(const string  & sub_dir_name)
{
    S_basic_directory = byte_interpret_get_include_file_name(sub_dir_name);
}

string    byte_interpret_get_include_directory()
{
    return  S_basic_directory;
}

string    byte_interpret_get_include_file_name(const string  & file_name)
{
    if (S_basic_directory == "")
        return  file_name;

    return  S_basic_directory + "/" + file_name;      // "/" is ok for windows
}

class C_byte_interpret_append_include_directory
{
public:
    C_byte_interpret_append_include_directory()
        :include_directory_save(byte_interpret_get_include_directory())
    {
    }

    C_byte_interpret_append_include_directory(const string  & sub_dir_name)
        :include_directory_save(byte_interpret_get_include_directory())
    {
        byte_interpret_append_include_directory(sub_dir_name);
    }

    void    append_include_directory(const string  & sub_dir_name)
    {
        byte_interpret_append_include_directory(sub_dir_name);
    }

    ~C_byte_interpret_append_include_directory()
    {
        byte_interpret_set_include_directory(include_directory_save);
    }

private:
    string    include_directory_save;
};

//*****************************************************************************
// build_alias
// ----------------------------------------------------------------------------
// Format :
// alias  <type_alias_name>  <type_to_use_instead>
//*****************************************************************************
void    build_alias (const E_override            must_override,
                     const string              & key_word,
                           istream             & is,
                           T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_alias", "");

    M_ASSERT_EQ (key_word, "alias");

    string    alias_name;
    M_FATAL_IF_FALSE (read_token_type_simple (is, alias_name));

    build_types_context_type_begin(alias_name);

    string    type;
    M_FATAL_IF_FALSE (read_token_type_complex (is, type));

    type_definitions.add_alias(alias_name, type, must_override);

    read_token_end_of_statement(is);

    build_types_context_type_end(alias_name);
}

//*****************************************************************************
// string_to_numeric **********************************************************
//*****************************************************************************
// ICIOA
C_value    string_to_numeric(const T_type_definitions  & type_definitions,
                             const string              & str_to_convert_to_numeric,
                             const string              & field_name,
                             const char                * attr);

//*****************************************************************************
// check_field_name ***********************************************************
//*****************************************************************************
// Authorized : alphanum and _.
// I refuse - (accepted by wireshark) because used into expression.
//*****************************************************************************

void    check_field_name(      T_field_type_name  & field_type_name,
                         const char                 also_authorized = '\0')
{
    // Extract display/filter/extended names
    string    simple_name;
    string    str_parameter;
    while (decompose_type_sep_value_sep (field_type_name.name,
                                                      '{',
                                                      '}',
                                                      simple_name,
                                                      str_parameter) == E_rc_ok)
    {
        field_type_name.name = simple_name;
        M_TRACE_DEBUG(str_parameter);

        if (strncmp(str_parameter.c_str(), "name=", 5) == 0)         // display name
        {
            field_type_name.display_name = str_parameter.substr(5);
            remove_string_limits(field_type_name.display_name);
        }
        else if (strncmp(str_parameter.c_str(), "filter=", 7) == 0)    // filter name
        {
            field_type_name.filter_name = str_parameter.substr(7);
            if ((field_type_name.filter_name[0] == '\"') ||
                (field_type_name.filter_name[0] == '\''))
            {
                M_FATAL_COMMENT("Unexpected blank/space/'/\" inside field filter name (" << field_type_name.filter_name << ").");
            }
        }
        else if (strncmp(str_parameter.c_str(), "desc=", 5) == 0)  // extended name
        {
            field_type_name.extended_name = str_parameter.substr(5);
            remove_string_limits(field_type_name.extended_name);
        }
        else
        {
            M_FATAL_COMMENT("Unexpected parameter (" << str_parameter << ") inside field name.");
        }
    }

    const string &  field_name = field_type_name.name;
    if (is_a_valid_short_variable_name(field_name, also_authorized) == false)
    {
        M_FATAL_COMMENT("Unexpected field name (" << field_name << ") only alphanumeric and _ accepted. Must not start by a number.");
    }
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <simple_type>[<str_array_size>]
// NB: <str_array_size> is :
//     - an integer > 0 or
//     - the name of the integer variable which contains the size
//*****************************************************************************

bool    is_an_array (const string   & orig_type,
                           string   & simple_type,
                           string   & str_array_size)
{
    E_return_code  rc = decompose_type_sep_value_sep (orig_type,
                                                      '[',
                                                      ']',
                                                      simple_type,
                                                      str_array_size);
    if (rc != E_rc_ok)
    {
        return  false;
    }

    return  true;
}

//*****************************************************************************
// Find an isolated character into a string
//*****************************************************************************

string::size_type  find_isolated(const string  & str, char  to_find)
{
    string::size_type  idx_sep = str.find (to_find);

    while (idx_sep != string::npos)
    {
        if (str[idx_sep+1] != to_find)
        {
            break;
        }

        idx_sep += 2;
        while (str[idx_sep] == to_find)
            ++idx_sep;

        idx_sep = str.find (to_find, idx_sep);
    }
    return  idx_sep;
}

//*****************************************************************************
// post_build_field_base
// Manage Display, transform and constraints specifications.
//*****************************************************************************

enum E_field_scope
{
    E_field_scope_bitfield,
    E_field_scope_parameter,
    E_field_scope_other
};

void    post_build_field_base (
                 const T_type_definitions                & type_definitions,
                       T_field_type_name_base            & field_type_name,
                       E_field_scope                       field_scope)
{
    M_TRACE_ENTER ("post_build_field_base", field_type_name.type << "  " << field_type_name.name);

    if ((field_type_name.type == "padding_bits") ||
        (field_type_name.type == "msg"))
    {
        return;
    }

    // while true because of (e.g.) : string{decoder=decoder_utf8}(50){d=%32.32s}
    while (true)
    {
        string    simple_type;

        // Display, transform and constraints (integer/float)
        string    str_display_or_transform;
        while (decompose_type_sep_value_sep (field_type_name.type,
                                                          '{',
                                                          '}',
                                                          simple_type,
                                                          str_display_or_transform) == E_rc_ok)
        {
            field_type_name.type = type_definitions.get_final_type (simple_type);
            M_TRACE_DEBUG(str_display_or_transform);

            if ((strncmp(str_display_or_transform.c_str(), "q=", 2) == 0) ||
                (strncmp(str_display_or_transform.c_str(), "o=", 2) == 0))
            {
                if (strncmp(str_display_or_transform.c_str(), "q=", 2) == 0)
                {
                    const string::size_type  idx_sep = str_display_or_transform.find(":o=");
                    type_definitions.set_field_transform_quantum(field_type_name, str_display_or_transform.substr(2, idx_sep-2));

                    if (idx_sep != string::npos)
                    {
                        type_definitions.set_field_transform_offset(field_type_name, str_display_or_transform.c_str()+idx_sep+1+2);
                    }
                }
                else
                {
                    const string::size_type  idx_sep = str_display_or_transform.find(":q=");
                    type_definitions.set_field_transform_offset(field_type_name, str_display_or_transform.substr(2, idx_sep));

                    if (idx_sep != string::npos)
                    {
                        M_FATAL_COMMENT("Bad transform specification (" << str_display_or_transform << ") : do NOT accept anything after offset");
//                        M_FATAL_IF_NE(strncmp(str_display_or_transform.c_str()+idx_sep+1, "q=", 2), 0);
//                        type_definitions.set_field_transform_quantum(field_type_name, str_display_or_transform.c_str()+idx_sep+1+2);
                    }
                }
            }
            else if ((strncmp(str_display_or_transform.c_str(), "tei=", 4) == 0))
            {
                type_definitions.set_field_transform_expression_integer(field_type_name, str_display_or_transform.c_str()+4);
            }
            else if ((strncmp(str_display_or_transform.c_str(), "tef=", 4) == 0))
            {
                type_definitions.set_field_transform_expression_float(field_type_name, str_display_or_transform.c_str()+4);
            }
            else if ((strncmp(str_display_or_transform.c_str(), "min=", 4) == 0) ||
                     (strncmp(str_display_or_transform.c_str(), "max=", 4) == 0))
            {
                const string  & str_constraints = str_display_or_transform;
                if (strncmp(str_display_or_transform.c_str(), "min=", 4) == 0)
                {
                    const string::size_type  idx_sep = str_constraints.find(":max=");
                    const string             min_param = str_constraints.substr(4, idx_sep-4);
                    if (idx_sep == string::npos)
                    {
                        type_definitions.prepend_field_constraint_min(field_type_name, min_param);
                    }
                    else
                    {
                        type_definitions.prepend_field_constraint_min_max(field_type_name, min_param, str_constraints.c_str()+idx_sep+1+4);
                    }
                }
                else
                {
                    const string::size_type  idx_sep = str_constraints.find(":min=");
                    const string             max_param = str_constraints.substr(4, idx_sep-4);
                    if (idx_sep == string::npos)
                    {
                        type_definitions.prepend_field_constraint_max(field_type_name, max_param);
                    }
                    else
                    {
                        type_definitions.prepend_field_constraint_min_max(field_type_name, str_constraints.c_str()+idx_sep+1+4, max_param);
                    }
                }
            }
            else if (strncmp(str_display_or_transform.c_str(), "d=", 2) == 0)
            {
                type_definitions.set_field_display(field_type_name, str_display_or_transform.c_str()+2);
            }
            else if (strncmp(str_display_or_transform.c_str(), "de=", 3) == 0)
            {
                type_definitions.set_field_display_expression(field_type_name, str_display_or_transform.c_str()+3);
            }
            else if (strncmp(str_display_or_transform.c_str(), "ns=", 3) == 0)
            {
                type_definitions.set_field_no_statement(field_type_name, str_display_or_transform.c_str()+3);
            }
            else if (strncmp(str_display_or_transform.c_str(), "dissector=", 10) == 0)
            {
                type_definitions.set_field_subdissector(field_type_name, str_display_or_transform.c_str()+10);
            }
            else if (strncmp(str_display_or_transform.c_str(), "decoder=", 8) == 0)
            {
                type_definitions.set_field_decoder(field_type_name, str_display_or_transform.c_str()+8);
            }
            else if (strncmp(str_display_or_transform.c_str(), "byte_order=", 11) == 0)
            {
                type_definitions.set_field_byte_order(field_type_name, str_display_or_transform.c_str()+11);
            }
            else if (strncmp(str_display_or_transform.c_str(), "name=", 5) == 0)  // name display
            {
                field_type_name.display_name = str_display_or_transform.substr(5);
                remove_string_limits(field_type_name.display_name);
            }
            else if (strncmp(str_display_or_transform.c_str(), "filter=", 7) == 0)  // name filter
            {
                field_type_name.filter_name = str_display_or_transform.substr(7);
                if ((field_type_name.filter_name[0] == '\"') ||
                    (field_type_name.filter_name[0] == '\''))
                {
                    M_FATAL_COMMENT("Unexpected blank/space/'/\" inside field filter name (" << field_type_name.filter_name << ").");
                }
            }
            else if (strncmp(str_display_or_transform.c_str(), "desc=", 5) == 0)  // name extended
            {
                field_type_name.extended_name = str_display_or_transform.substr(5);
                remove_string_limits(field_type_name.extended_name);
            }
            else
            {
                M_FATAL_COMMENT("Unexpected " << str_display_or_transform);
            }
        }

        // string/raw size or switch parameter
        // Could be :
        // string(50), string{decoder=decoder_utf8}(50)
        // string(50){d=%32.32s}, subproto(50){dissector=...}
        // string{decoder=decoder_utf8}(50){d=%32.32s}
        {
            string    str_size_or_parameter;
            if (decompose_type_sep_value_sep (field_type_name.type,
                                                          '(',
                                                          ')',
                                                          simple_type,
                                                          str_size_or_parameter) == E_rc_ok)
            {
                M_TRACE_DEBUG(str_size_or_parameter);
                type_definitions.set_field_type(field_type_name, simple_type);
                type_definitions.set_field_type_size_or_parameter(field_type_name, str_size_or_parameter);
            }
            else
            {
                // (...) not found at the end, so no more specifications to read/extract
                break;
            }
        }
    }

    // check the type.
    if (type_definitions.is_a_type_name(field_type_name.type) == false)
    {
        bool    bad_type = true;

        if (field_scope == E_field_scope_parameter)
        {
            if ((field_type_name.type == "any") ||
                (field_type_name.type == "pointer"))
            {
                bad_type = false;
            }
        }
        else if (field_scope == E_field_scope_bitfield)
        {
            long    bit_size = 0;
            if ((strncmp(field_type_name.type.c_str(), "uint", 4) == 0) &&
                (get_number(field_type_name.type.c_str()+4, &bit_size) == true))
            {
                bad_type = false;
            }
        }

        if (bad_type)
        {
            M_FATAL_COMMENT("Unexpected field type " << field_type_name.type << " (" << field_type_name.orig_type << ")");
        }
    }

    // Check the str_size_or_parameter
    if ((field_type_name.type == "raw") ||
        (field_type_name.type == "subproto") ||
        (field_type_name.type == "insproto") ||
        (type_definitions.is_a_switch_value(field_type_name.type)))
    {
        if (field_type_name.str_size_or_parameter == "")
        {
            M_FATAL_COMMENT("Empty () for " << field_type_name.type);
        }
    }

    // Compute basic_type_bit_size.
#define M_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_NAME_SIZE,TYPE)                   \
    else if (field_type_name.type == TYPE_NAME)                             \
    {                                                                       \
        field_type_name.basic_type_bit_size = TYPE_NAME_SIZE;               \
    }

    const T_enum_definition_representation  * P_enum = type_definitions.get_P_enum(field_type_name.type);

    if (P_enum != NULL)
    {
        field_type_name.basic_type_bit_size = P_enum->bit_size;
    }
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
    M_READ_SIMPLE_TYPE ("spare",    8, unsigned char)
    M_READ_SIMPLE_TYPE ( "char",    8,          char)
    M_READ_SIMPLE_TYPE ("schar",    8,   signed char)
    M_READ_SIMPLE_TYPE ("uchar",    8, unsigned char)
    else
    {
        /* bitfield, bitstream, bsew uintXX intXX */
        long    bit_size = 0;
        if ((strncmp(field_type_name.type.c_str(), "uint", 4) == 0) &&
            (get_number(field_type_name.type.c_str()+4, &bit_size)))
        {
            field_type_name.basic_type_bit_size = bit_size;
        }
        else if ((strncmp(field_type_name.type.c_str(), "int", 3) == 0) &&
                 (get_number(field_type_name.type.c_str()+3, &bit_size)))
        {
            field_type_name.basic_type_bit_size = bit_size;
        }
    }
}

//*****************************************************************************
//*****************************************************************************

const char *  build_struct_fields (
                   istream                           & is,
             const T_type_definitions                & type_definitions,
                   T_struct_fields                   & struct_fields,
             const char                              * end_flag1,
             const char                              * end_flag2 = NULL_PTR,
             const char                              * end_flag3 = NULL_PTR,
                   E_field_scope                       field_scope = E_field_scope_other,
             const char                              * return_type = "void");

void    build_struct_base ( istream              & is,
                      const T_type_definitions   & type_definitions,
                            T_struct_definition  & struct_definition,
                      const char                 * return_type = "void");

string    build_struct_unnamed (
                            istream             & is,
                            T_type_definitions  & type_definitions);

void    build_bitfield_unnamed (
                        const string                 & key_word,
                              istream                & is,
                        const T_type_definitions     & type_definitions,
                              T_bitfield_definition  & def_rep);

void    build_switch_unnamed (
                        const string                 & key_word,
                              istream                & is,
                        const T_type_definitions     & type_definitions,
                              T_switch_definition    & switch_def,
                        const char                   * return_type);

//*****************************************************************************
// build_field
// ----------------------------------------------------------------------------
// Format :
// <type_name>  <field_name>
// while/if     (condition)
// { ... }
// else                          optional
// { ... }
// ... 
// Returns the last word read (which is NOT part of the field).
//*****************************************************************************
// type_definitions  must be const because
//  build_field is used into frame_to_main
//  where I do not want to declare new types.
// BUT then, I'm unable to call build_struct_unnamed or ...
//*****************************************************************************

#define  M_FINISH_build_field_check()                                      \
    if ((must_have_new_expression) &&                                      \
        (field_type_name.new_expression.is_defined() == false))            \
    {                                                                      \
        M_FATAL_COMMENT("var could not be use for a command ...");         \
    }

#define  M_FINISH_build_field_read_next()                                  \
    string    last_word_read;                                              \
    if (read_next_token == true)                                           \
    {                                                                      \
        read_token_left_any (is, last_word_read);                          \
    }                                                                      \
    return  last_word_read

#define  M_FINISH_build_field()                                            \
         M_FINISH_build_field_check();                                     \
        if (field_type_name.display_name  == "")  field_type_name.display_name  = field_type_name.name;  \
        if (field_type_name.filter_name   == "")  field_type_name.filter_name   = field_type_name.name;  \
        if (field_type_name.extended_name == "")  field_type_name.extended_name = field_type_name.name;  \
         read_token_end_of_statement(is);                                  \
         M_FINISH_build_field_read_next()


string    build_field (istream                           & is,
                 const T_type_definitions                & type_definitions,
                       string                              first_word,
                       T_field_type_name                 & field_type_name,
                       E_field_scope                       field_scope,
                 const char                              * return_type,
                       bool                                read_next_token = true)
{
    M_TRACE_ENTER ("build_field", first_word);

    M_FATAL_IF_EQ (first_word, "");

    // modifier: forget
    if (first_word == "forget")
    {
        field_type_name.must_forget = true;
        M_FATAL_IF_FALSE (read_token_left_any (is, first_word));
    }

    // modifier: hide
    if (first_word == "hide")
    {
        field_type_name.output_directive = T_field_type_name::E_output_directive_hide;
        M_FATAL_IF_FALSE (read_token_left_any (is, first_word));
    }
    else if (first_word == "show")
    {
        field_type_name.output_directive = T_field_type_name::E_output_directive_show;
        M_FATAL_IF_FALSE (read_token_left_any (is, first_word));
    }

    // modifier: const
    bool    is_const = false;
    if (first_word == "const")
    {
        is_const = true;
        first_word = "var";    // treated as var
    }

    // modifier: var
    bool    must_have_new_expression = false;
    if (first_word == "var")
    {
        field_type_name.A_is_a_variable = true;
        must_have_new_expression = true;
        M_FATAL_IF_FALSE (read_token_left_any (is, first_word));
    }

    field_type_name.type = type_definitions.get_final_type(first_word);


    // command ctrl: break & continue
    if ((field_type_name.type == "deep_break") ||
        (field_type_name.type == "deep_continue") ||
        (field_type_name.type == "break") ||
        (field_type_name.type == "continue"))
    {
        M_FINISH_build_field();
    }


    // command ctrl: return
    if (field_type_name.type == "return")
    {
        if (strcmp(return_type, "void") != 0)
        {
            // return value
            M_FATAL_IF_FALSE (read_token_expression_any (is, field_type_name.name));

            if (field_type_name.name == ";")
            {
                M_FATAL_COMMENT("No return value, expecting a " << return_type);
            }

            field_type_name.return_expression.build_expression(type_definitions, field_type_name.name);
        }

        M_FINISH_build_field();
    }


    // command: display
    if ((field_type_name.type == "print") ||
        (field_type_name.type == "debug_print") ||
        (field_type_name.type == "chat") ||
        (field_type_name.type == "note") ||
        (field_type_name.type == "warning") ||
        (field_type_name.type == "error") ||
        (field_type_name.type == "fatal"))
    {
        M_FATAL_IF_FALSE (read_token_expression_any (is, field_type_name.name));
        // ICIOA mettre les parametres dans une/des expressions

        M_FINISH_build_field();
    }


    // command: display ctrl
    if (field_type_name.type == "output")
    {
        read_token_key_word_specified (is, field_type_name.name, "--", "++");
        M_FINISH_build_field();
    }

    // command: data ctrl
    if (field_type_name.type == "byte_order")
    {
        read_token_key_word_specified (is, field_type_name.name, "big_endian", "little_endian", "as_host");
        M_FINISH_build_field();
    }

    // command: position
    if ((field_type_name.type == "save_position") ||
        (field_type_name.type == "goto_position") ||
        (field_type_name.type == "move_position_bytes") ||
        (field_type_name.type == "move_position_bits"))
    {
        M_FATAL_IF_FALSE (read_token_expression_any (is, field_type_name.name));
        // nothing to test for now
        // not sure it is a good idea
        M_FINISH_build_field();
    }
    if (field_type_name.type == "move_position")
    {
        M_FATAL_COMMENT("move_position is no more available. Replace it by move_position_bits or move_position_bytes");
    }

    if ((field_type_name.type == "check_eof_distance_bytes") ||
        (field_type_name.type == "check_eof_distance_bits"))
    {
        M_FATAL_IF_FALSE (read_token_expression_any (is, field_type_name.name));
        // nothing to test for now
        // not sure it is a good idea
        M_FINISH_build_field();
    }
    if (field_type_name.type == "check_eof_distance")
    {
        M_FATAL_COMMENT("check_eof_distance is no more available. Replace it by check_eof_distance_bytes");
    }

    // command: chrono
    if (field_type_name.type == "chrono")
    {
        // Read chrono argument.
        M_FATAL_IF_FALSE (read_token_expression_any(is, field_type_name.name));

        M_FINISH_build_field();
    }


    // command: call function
    if (field_type_name.type == "call")
    {
        // Read function name.
        M_FATAL_IF_FALSE (read_token_function_name(is, field_type_name.name));

        if (type_definitions.is_a_function(field_type_name.name) == false)
        {
            M_FATAL_COMMENT(field_type_name.name << " is not a known function");
        }

        // ICIOA tout mettre dans une expression ?

        // Read function parameters.
        vector<string>  fct_parameters_vector;
        M_FATAL_IF_FALSE(read_token_parameters_vector(is, fct_parameters_vector, E_parser_cfg_parameters));

        for (size_t  idx_parameters = 0; idx_parameters < fct_parameters_vector.size(); ++idx_parameters)
        {
            const string  & fct_parameter = fct_parameters_vector[idx_parameters];
            T_expression    expression;
            expression.build_expression(type_definitions, fct_parameter);

            field_type_name.fct_parameters.push_back(expression);
        }

        M_FINISH_build_field();
    }


    // command: set variable
    if (field_type_name.type == "set")
    {
        // Read variable name.
        M_FATAL_IF_FALSE (read_token_left_any(is, field_type_name.name));

        read_token_key_word_specified(is, "=");

        string    new_expression;
        M_FATAL_IF_FALSE (read_token_expression_any(is, new_expression));
        field_type_name.new_expression.build_expression(type_definitions, new_expression);

        M_FINISH_build_field();
    }


    // data: inline struct
    if (field_type_name.type == "struct")
    {
        field_type_name.P_sub_struct.reset(new T_struct_definition);
        build_struct_base (is, type_definitions, *field_type_name.P_sub_struct, return_type);

        M_FATAL_IF_FALSE (read_token_field_name (is, field_type_name.name));
        // NB : "" not authorized : non sense and not coded into generic (register)
        check_field_name(field_type_name);

        M_FINISH_build_field();
    }


    // data: inline bitfield
    if ((field_type_name.type == "bitfield8") ||
        (field_type_name.type == "bitfield16") ||
        (field_type_name.type == "bitfield24") ||
        (field_type_name.type == "bitfield32") ||
        (field_type_name.type == "bitstream8") ||
        (field_type_name.type == "bitstream16") ||
        (field_type_name.type == "bitstream24") ||
        (field_type_name.type == "bitstream32"))
    {
        field_type_name.P_bitfield_inline.reset(new T_bitfield_definition);
        build_bitfield_unnamed (field_type_name.type, is, type_definitions, *field_type_name.P_bitfield_inline);
        field_type_name.type = "bitfield";    // simplification (for register and dissect) 

        M_FATAL_IF_FALSE (read_token_field_name (is, field_type_name.name));

        if (field_type_name.name != "")
        {
            check_field_name(field_type_name);
        }

        M_FINISH_build_field();
    }


    // command: inline switch
    {
        bool    is_an_inline_switch = false;
        bool    is_switch_expr = false;
        string  inline_switch_parameter;
        if (field_type_name.type == "switch")
        {
            is_an_inline_switch = true;

            M_FATAL_IF_FALSE (read_token_right_any (is, inline_switch_parameter));
            if ((inline_switch_parameter == "{") ||
                (inline_switch_parameter[0] != '('))
            {
                M_FATAL_COMMENT("expecting (parameter) for inline switch " << field_type_name.type);
            }
            if ((inline_switch_parameter.size() < 3) ||
                (inline_switch_parameter[0] != '(') ||
                (inline_switch_parameter[inline_switch_parameter.size()-1] != ')'))
            {
                M_FATAL_COMMENT("invalid parameter for inline switch " << field_type_name.type);
            }
            inline_switch_parameter.erase(inline_switch_parameter.size()-1);
            inline_switch_parameter.erase(0, 1);
        }
        else if (strncmp(field_type_name.type.c_str(), "switch(", 7) == 0)
        {
            is_an_inline_switch = true;
            inline_switch_parameter = field_type_name.type.substr(7);
            if ((inline_switch_parameter.size() < 2) ||
                (inline_switch_parameter[inline_switch_parameter.size()-1] != ')'))
            {
                M_FATAL_COMMENT("invalid parameter for inline switch " << field_type_name.type);
            }
            inline_switch_parameter.erase(inline_switch_parameter.size()-1);
        }
        else if (strncmp(field_type_name.type.c_str(), "switch_expr", 7) == 0)
        {
            is_an_inline_switch = true;
            is_switch_expr = true;
            // no parameter
        }
        if (is_an_inline_switch)
        {
            field_type_name.str_size_or_parameter = inline_switch_parameter;
            field_type_name.type = "switch";
            
            field_type_name.P_switch_inline.reset(new T_switch_definition);
            field_type_name.P_switch_inline->is_switch_expr = is_switch_expr;
            build_switch_unnamed (field_type_name.type, is, type_definitions, *field_type_name.P_switch_inline, return_type);

            // Inline switch is a command control bloc (like if and loops)
            // -> no name
            // -> no ; after }
            M_FINISH_build_field_check();
            M_FINISH_build_field_read_next();
        }
    }


    // command: inline frame
    if (field_type_name.type == "frame_old")    // Not finished and deprecated
    {
        field_type_name.type = "frame_bytes";

        string  str_open_brace_or_size;
        M_FATAL_IF_FALSE (read_token_right_any (is, str_open_brace_or_size));

        if (str_open_brace_or_size.compare(0, 11, "size_bytes=") == 0)
        {
            str_open_brace_or_size.erase(0, 11);
            field_type_name.str_size_or_parameter = str_open_brace_or_size;

            M_FATAL_IF_FALSE (read_token_key_word (is, str_open_brace_or_size));
        }

        M_FATAL_IF_FALSE (str_open_brace_or_size == "{");

        field_type_name.P_sub_struct.reset(new T_struct_definition);
        build_struct_fields (is, type_definitions,
                             field_type_name.P_sub_struct->fields,
                             "}",
                             NULL_PTR,
                             NULL_PTR,
                             field_scope,
                             return_type);

        // Inline frame is a command control bloc (like if and loops)
        // -> no name
        // -> no ; after }
        M_FINISH_build_field_check();
        M_FINISH_build_field_read_next();
    }

    // command: decoder
    if (field_type_name.type == "decoder")
    {
        string  decoder_function_name;
        M_FATAL_IF_FALSE (read_token_function_name (is, decoder_function_name));

        // Check that the function exist
        if (decoder_function_name != "")
        {
            const T_function_definition  * P_fct_def = type_definitions.get_P_function(decoder_function_name);
            if (P_fct_def == NULL)
            {
                M_FATAL_COMMENT(decoder_function_name << " is not a function.");
            }
            if (P_fct_def->get_function_parameters().size() != 2)
            {
                M_FATAL_COMMENT(decoder_function_name << " must have 2 parameters.");
            }
            if (P_fct_def->return_type != "void")
            {
                M_FATAL_COMMENT(decoder_function_name << " must not return anything.");
            }
        }

        field_type_name.name = decoder_function_name;

        M_FINISH_build_field();
    }


    // command ctrl: condition & loop
    if ((field_type_name.type == "if") ||
        (field_type_name.type == "while") ||
        (field_type_name.type == "loop_size_bytes") ||
        (field_type_name.type == "loop_size_bits") ||
        (field_type_name.type == "loop_nb_times") ||
        (field_type_name.type == "do"))
    {
        if (field_type_name.type != "do")
        {
            M_FATAL_IF_FALSE (read_token_expression_parenthesis (is, field_type_name.name));
            field_type_name.condition_expression.build_expression(type_definitions, field_type_name.name);
        }

        read_token_key_word_specified (is, "{");

        field_type_name.P_sub_struct.reset(new T_struct_definition);
        build_struct_fields (is, type_definitions,
                             field_type_name.P_sub_struct->fields,
                             "}",
                             NULL_PTR,
                             NULL_PTR,
                             field_scope,
                             return_type);

        if (field_type_name.type == "if")
        {
            string    word_else;
            read_token_left_any (is, word_else);

            if (word_else != "else")
                return  word_else;

            read_token_key_word_specified (is, "{");

            build_struct_fields (is, type_definitions,
                               field_type_name.sub_struct_2,
                               "}",
                               NULL_PTR,
                               NULL_PTR,
                               field_scope,
                               return_type);
        }
        else if (field_type_name.type == "do")
        {
            read_token_key_word_specified (is, "while");

            field_type_name.type = "do_while";

            M_FATAL_IF_FALSE (read_token_expression_parenthesis (is, field_type_name.name));
            field_type_name.condition_expression.build_expression(type_definitions, field_type_name.name);

            read_token_end_of_statement(is);
        }
    }
    else if ((field_type_name.type == "loop_size"))
    {
        M_FATAL_COMMENT("loop_size is no more available. Replace it by loop_size_bytes");
    }

    else

    {
        M_FATAL_IF_FALSE (read_token_field_name (is, field_type_name.name));

        // Incomplete !!!
        // So a field/variable name could be identic to a type or a keyword.
        // type : Use is_a_type_name ? Is it interesting to forbid this ?
        if ((field_type_name.name == "}") ||
            (field_type_name.name == "{") ||
            (field_type_name.name == "struct") ||
            (field_type_name.name == "switch") ||
            (field_type_name.name == "alias") ||
            (field_type_name.name == "enum8") ||
            (field_type_name.name == "enum16") ||
            (field_type_name.name == "enum24") ||
            (field_type_name.name == "enum32") ||
            (field_type_name.name == "include"))
        {
            M_FATAL_COMMENT("Bad field name " << field_type_name.name << " for field type " << field_type_name.type);
        }

        field_type_name.orig_type = field_type_name.type;
        field_type_name.type = type_definitions.get_final_type(field_type_name.type);

        if (must_have_new_expression)
        {
            read_token_key_word_specified (is, "=");

            string    new_expression;
            M_FATAL_IF_FALSE (read_token_expression_any(is, new_expression));
            field_type_name.new_expression.build_expression(type_definitions, new_expression);
        }
        else if (field_type_name.type == "msg")
        {
            M_FATAL_COMMENT("msg must always be var (" << field_type_name.name << ")");
        }

        read_token_end_of_statement(is);

        string    simple_type;

        // Arrays
        {
            string    str_array_size;
            while (is_an_array (field_type_name.type, simple_type, str_array_size))
            {
                if ((str_array_size == "*") || (str_array_size == "+"))
                {
                    if (field_type_name.str_arrays.empty() == false)
                    {
                        M_FATAL_COMMENT("[*] or [+] are only accepted on the last array level");
                    }
                }
                field_type_name.type = type_definitions.get_final_type(simple_type);

                T_field_type_name::T_array  field_array;
                if (str_array_size == "*")
                {
                    field_array.size_type = T_field_type_name::T_array::E_size_unknow_any;
                }
                else if (str_array_size == "+")
                {
                    field_array.size_type = T_field_type_name::T_array::E_size_unknow_at_least_1;
                }
                else
                {
                    field_array.size_type = T_field_type_name::T_array::E_size_normal;
                    field_array.size_expression.build_expression(type_definitions, str_array_size);
                }

                field_type_name.str_arrays.insert(field_type_name.str_arrays.begin(), field_array);
            }
        }

        // int/float           : Display, transform, constraints and basic_type_bit_size
        // string/raw/subproto : size
        // switch              : parameter
        // decoder, byte_order, nameDisplay, nameFilter, nameExtended ...
        // Check that type is defined.
        // Must be called before check_field_name.
        post_build_field_base(type_definitions, field_type_name, field_scope);

        if (field_type_name.name != "")
        {
            if (is_const == true)
                check_field_name(field_type_name, ':');
            else
                check_field_name(field_type_name);
        }
    }


    M_FINISH_build_field_check();
    M_FINISH_build_field_read_next();
}


string    build_field (istream                           & is,
                 const T_type_definitions                & type_definitions,
                       string                              first_word,
                       T_field_type_name                 & field_type_name)
{
    return  build_field(is, type_definitions, first_word, field_type_name,
                        E_field_scope_other,
                        "void");
}

//*****************************************************************************
// build_struct_fields
// ----------------------------------------------------------------------------
// Format :
// <type_name>  <field_name>
//  ...
// <type_name>  <field_name>
// <end_flag>
//*****************************************************************************
const char *  build_struct_fields (
                   istream                           & is,
             const T_type_definitions                & type_definitions,
                   T_struct_fields                   & struct_fields,
             const char                              * end_flag1,
             const char                              * end_flag2,
             const char                              * end_flag3,
                   E_field_scope                       field_scope,
             const char                              * return_type)
{
    M_TRACE_ENTER ("build_struct_fields", "");

    if (end_flag2 == NULL_PTR)
        end_flag2 = end_flag1;

    if (end_flag3 == NULL_PTR)
        end_flag3 = end_flag1;

    string    last_word_read;
    M_FATAL_IF_FALSE (read_token_left_any (is, last_word_read));

    while (true)
    {
        if (last_word_read == end_flag1)
            return  end_flag1;
        if (last_word_read == end_flag2)
            return  end_flag2;
        if (last_word_read == end_flag3)
            return  end_flag3;

        T_field_type_name    field_type_name;

        last_word_read = build_field (is, type_definitions, last_word_read, field_type_name, field_scope, return_type);

        struct_fields.push_back (field_type_name);
    }
}

//*****************************************************************************
// build_struct_base
// ----------------------------------------------------------------------------
// Format :
//    [ print ("...", ..., ...) ]
// {
//    <type_name>  <field_name>
//    ...
//    <type_name>  <field_name>
// }
//*****************************************************************************
void    build_struct_base ( istream              & is,
                      const T_type_definitions   & type_definitions,
                            T_struct_definition  & struct_definition,
                      const char                 * return_type)
{
    M_TRACE_ENTER ("build_struct_base", "");

    string  open_brace_or_print;
    M_FATAL_IF_FALSE (read_token_key_word (is, open_brace_or_print));
    if (open_brace_or_print == "print")
    {
        M_FATAL_IF_FALSE (read_token_parameters (is, struct_definition.printf_args));
        // ICIOA mettre les parametres dans une/des expressions

        M_FATAL_IF_FALSE (read_token_key_word (is, open_brace_or_print));
    }
    M_ASSERT (open_brace_or_print == "{");

    build_struct_fields (is,
                       type_definitions,
                       struct_definition.fields,
                       "}",
                       NULL_PTR,
                       NULL_PTR,
                       E_field_scope_other,
                       return_type);

    // Search if it is a field_struct
    for (T_struct_fields::const_iterator  iter  = struct_definition.fields.begin();
                                          iter != struct_definition.fields.end();
                                        ++iter)
    {
        const T_field_type_name  & field_type_name = *iter;

        // No control
        if (field_type_name.name == "field_struct")
        {
            struct_definition.field_struct_idx = iter - struct_definition.fields.begin();
            break;
        }
    }
}

//*****************************************************************************
// M_CHECK_FORWARD
// If the next character is ; (ie end of statement),
//  it is a forward declaration (ie "<type> <type_name> ;").
//*****************************************************************************

#define M_CHECK_FORWARD(is,KIND,NAME)                                      \
    skip_blanks_and_comments(is);                                          \
    if (is.peek() == ';')                                                  \
    {                                                                      \
        read_token_end_of_statement(is);                                   \
        type_definitions.add_forward_declaration(KIND, NAME);              \
        return;                                                            \
    }

//*****************************************************************************
// build_struct
// ----------------------------------------------------------------------------
// Format :
// struct  <struct_name>   [ print ("...", ..., ...) ]
// {
//    <type_name>  <field_name>
//    ...
//    <type_name>  <field_name>
// }
//*****************************************************************************
void    build_struct (const E_override            must_override,
                      const string              & key_word,
                            istream             & is,
                            T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_struct", "");

    M_ASSERT_EQ (key_word, "struct");

    string  struct_name;
    M_FATAL_IF_FALSE (read_token_type_simple (is, struct_name));

    M_CHECK_FORWARD(is, type_definitions.E_type_struct, struct_name);

    build_types_context_type_begin(struct_name);

    // Check it is possible to define a new type with this name.
    // No returned value, FATAL is called if it is not possible.
    type_definitions.could_define_new_type(struct_name, T_type_definitions::E_type_struct, must_override);

    type_definitions.map_struct_definition[struct_name] = T_struct_definition();    // possible override ok

    build_struct_base (is, type_definitions, type_definitions.map_struct_definition[struct_name]);

    build_types_context_type_end(struct_name);
}

//*****************************************************************************
// build_const
// ----------------------------------------------------------------------------
// Format :
// const  <existing_type_name>  <const_name> = <expression>;
//*****************************************************************************
void    build_const (const E_override            must_override,
                     const string              & key_word,
                           istream             & is,
                           T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_const", "");

    M_ASSERT_EQ (key_word, "const");


    T_field_type_name    field_type_name;

    string  last_word_read = build_field (is, type_definitions,
                                          key_word,
                                          field_type_name,
                                          E_field_scope_other,
                                          "return_type_do_not_care",
                                          false  /* read_next_token */);

    build_types_context_type_begin(field_type_name.name);

    M_FATAL_IF_NE(last_word_read, "");

    M_FATAL_IF_NE(field_type_name.is_a_variable(), true);
    M_FATAL_IF_NE(field_type_name.is_an_array(), false);
    M_FATAL_IF_NE(field_type_name.has_post_treatment(), false);

    // Check it is possible to define a new const value with this name.
    // No returned value, FATAL is called if it is not possible.
    type_definitions.could_define_new_type(field_type_name.name, T_type_definitions::E_type_const, must_override);

    type_definitions.map_const_value[field_type_name.name] = field_type_name.get_var_expression().compute_expression_static(type_definitions);

    build_types_context_type_end(field_type_name.name);
}

//*****************************************************************************
// build_bitfield_unnamed
// ----------------------------------------------------------------------------
// Format :
// bitfieldXX  
// {
//    <type_name>  <var_name>
//    ...
//    <type_name>  <var_name>
// }
// NB: bitfieldXX : XX is the size (in bits) of the bitfield (could be 8, 16, 24 or 32)
//*****************************************************************************
void    build_bitfield_unnamed (
                        const string                 & key_word,
                              istream                & is,
                        const T_type_definitions     & type_definitions,
                              T_bitfield_definition  & def_rep)
{
    M_TRACE_ENTER ("build_bitfield_unnamed", "key_word=" << key_word);

    long    sizeof_bitfield_total = 0;
    if (strncmp(key_word.c_str (), "bitfield", 8) == 0)
    {
        def_rep.is_a_bitstream = false;
        M_FATAL_IF_FALSE (get_number (key_word.c_str () + strlen ("bitfield"),
                                      &sizeof_bitfield_total));
    }
    else
    {
        def_rep.is_a_bitstream = true;
        M_FATAL_IF_FALSE (get_number (key_word.c_str () + strlen ("bitstream"),
                                      &sizeof_bitfield_total));
    }

    read_token_key_word_specified (is, "{");

    def_rep.master_field.name = "last_bitfield_value";            // name used by the following fields
    def_rep.master_field.type = "uint" + get_string(sizeof_bitfield_total);
    def_rep.master_field.output_directive = T_field_type_name::E_output_directive_hide;
    def_rep.master_field.str_display = "hex";


    // lecture struct dans def_rep.fields_definition
    build_struct_fields (is, type_definitions,
                         def_rep.fields_definition,
                         "}",
                         NULL_PTR,
                         NULL_PTR,
                         E_field_scope_bitfield);

    // Transform each "normal" structure field 
    //  to a bitfield field.
    int    bitfield_position = 0;
    for (T_struct_fields::iterator    iter  = def_rep.fields_definition.begin();
                                      iter != def_rep.fields_definition.end();
                                    ++iter)
    {
        T_field_type_name  & field_type_name = *iter;

        if (field_type_name.is_a_variable())
            continue;                            
        if (field_type_name.type == "set")
            continue;

        // Could not initialize an array with 1 expression
        M_FATAL_IF_NE(field_type_name.is_an_array(), false);

        const T_enum_definition_representation  * P_enum = NULL;

        long    sizeof_bitfield_field = 0;
        if (strncmp(field_type_name.type.c_str(), "uint", 4) == 0)
        {
            M_FATAL_IF_FALSE (get_number (field_type_name.type.c_str() + strlen ("uint"),
                                          &sizeof_bitfield_field));
            field_type_name.type = "uint32";
        }
        else if ((P_enum = type_definitions.get_P_enum(field_type_name.type)) != NULL)
        {
            sizeof_bitfield_field = P_enum->bit_size;
        }
        else
        {
            M_FATAL_COMMENT("bitfield accepts only uint, enum, var and set");
        }

        if (sizeof_bitfield_field < 1)
        {
            M_FATAL_COMMENT("bitfield field size is < 1");
        }
        if (sizeof_bitfield_field + bitfield_position > sizeof_bitfield_total)
        {
            M_FATAL_COMMENT("bitfield total size averflow");
        }

        const int    mask = (1 << sizeof_bitfield_field) -1;
        const int    shift = def_rep.is_a_bitstream ? sizeof_bitfield_total-bitfield_position-sizeof_bitfield_field : bitfield_position;
        string       new_expression = "((last_bitfield_value >> " +
                                         get_string(shift) + ") & " +
                                         get_string(mask) +
                                         ")";
        field_type_name.new_expression.build_expression(type_definitions, new_expression);

        field_type_name.A_is_a_variable = true;

        bitfield_position += sizeof_bitfield_field;
    }

//    def_rep.bit_size = sizeof_bitfield;
//    def_rep.representation_type = "uint" + get_string (def_rep.bit_size);
}

//*****************************************************************************
// build_bitfield
// ----------------------------------------------------------------------------
// Format :
// bitfieldXX  <bitfield_name>
// {
//    <type_name>  <var_name>
//    ...
//    <type_name>  <var_name>
// }
// NB: bitfieldXX : XX is the size (in bits) of the bitfield (could be 8, 16, 24 or 32)
//*****************************************************************************
void    build_bitfield (const E_override            must_override,
                        const string              & key_word,
                              istream             & is,
                              T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_bitfield", "key_word=" << key_word);

    string  bitfield_name;
    M_FATAL_IF_FALSE (read_token_type_simple (is, bitfield_name));

    M_CHECK_FORWARD(is, type_definitions.E_type_bitfield, bitfield_name);

    build_types_context_type_begin(bitfield_name);

    // Check it is possible to define a new bitfield type with this name.
    // No returned value, FATAL is called if it is not possible.
    type_definitions.could_define_new_type(bitfield_name, T_type_definitions::E_type_bitfield, must_override);

    T_map_bitfield_definition  & map_bitfield_def = type_definitions.map_bitfield_definition;
    T_bitfield_definition      & def_rep = map_bitfield_def[bitfield_name];    // possible override ok
    def_rep = T_bitfield_definition();

    build_bitfield_unnamed (key_word, is, type_definitions, def_rep);

    build_types_context_type_end(bitfield_name);
}

//*****************************************************************************
// build_enum
// ----------------------------------------------------------------------------
// Format :
// enumXX  <enum_name>
// {
//    <symbolic_name>  <integer_value>
//    ...
//    <symbolic_name>  <integer_value>
// }
// NB: enumXX : XX is the size (in bits) of the enum (could be 8, 16, 24 or 32)
// NB: <integer_value> could be -. It means :
//     - previous <integer_value> + 1
//     - zero if there is no previous <integer_value>
//*****************************************************************************
void    build_enum (const E_override            must_override,
                    const string              & key_word,
                          istream             & is,
                          T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_enum", "key_word=" << key_word);

    T_map_enum_definition_representation  & map_enum_def_rep = type_definitions.map_enum_definition_representation;

    long    sizeof_enum = 0;
    if (key_word != "enum")
    {
        M_FATAL_IF_FALSE (get_number (key_word.c_str () + strlen ("enum"),
                                      &sizeof_enum));
    }
    M_FATAL_IF_LE(sizeof_enum,  0);
    M_FATAL_IF_GT(sizeof_enum, 32);

    string  enum_name;
    M_FATAL_IF_FALSE (read_token_type_simple (is, enum_name));

    M_CHECK_FORWARD(is, type_definitions.E_type_enum, enum_name);

    build_types_context_type_begin(enum_name);

    string  open_brace_or_as_or_expand;
    read_token_key_word_specified (is, open_brace_or_as_or_expand, "{", "as", "expand");


    // Check it is possible to define a new enum type with this name.
    // No returned value, FATAL is called if it is not possible.
    type_definitions.could_define_new_type(enum_name, T_type_definitions::E_type_enum, must_override);

    T_enum_definition_representation  & def_rep = map_enum_def_rep[enum_name];    // possible override ok

    if ((open_brace_or_as_or_expand == "as") ||
        (open_brace_or_as_or_expand == "expand"))
    {
        string  as_enum_name;
        M_FATAL_IF_FALSE (read_token_type_simple (is, as_enum_name));

        if (map_enum_def_rep.find (as_enum_name) ==
            map_enum_def_rep.end ())
        {
            M_FATAL_COMMENT ("enum " << as_enum_name << " NOT found.");
        }

        def_rep = map_enum_def_rep[as_enum_name];
    }
    else
    {
        def_rep = T_enum_definition_representation ();
    }

    if (open_brace_or_as_or_expand == "expand")
    {
        read_token_key_word_specified (is, "{");
    }

    if (open_brace_or_as_or_expand == "as")
    {
        read_token_end_of_statement(is);
    }
    else
    {
        long long    last_value = -1;
        long long    min_value = LONGLONG_MAX;
        long long    max_value = LONGLONG_MIN;

        while (true)
        {
            T_enum_name_val    enum_name_val;

            M_FATAL_IF_FALSE (read_token_enum_symbolic (is, enum_name_val.name));

            if (enum_name_val.name == "}")
                break;

            string    str_last_value;
            M_FATAL_IF_FALSE (read_token_enum_value (is, str_last_value));

            if (str_last_value == "-")
                ++last_value;
            else
            {
                if (get_number (str_last_value.c_str (), last_value) != true)
                {
                    M_FATAL_COMMENT(str_last_value << " is NOT a number for enum type " << enum_name);
                }
            }

            enum_name_val.value = last_value;

            min_value = min (min_value, last_value);
            max_value = max (max_value, last_value);

            def_rep.definition.push_back (enum_name_val);
        }

        def_rep.is_signed = (min_value < 0);
    }

    def_rep.bit_size = sizeof_enum;
    def_rep.representation_type =
                string (def_rep.is_signed ? "" : "u") +
                "int" +
                get_string (sizeof_enum);

    build_types_context_type_end(enum_name);
}

//*****************************************************************************
// build_switch_unnamed
// ----------------------------------------------------------------------------
// Format :
// switch(parameter)  <case_type_name>
// {
// case <a_valid_value_for case_type_name> :  <type_name>  <field_name>
// ...
// case <a_valid_value_for case_type_name> :
//   <type_name>  <field_name>
//   ...
//   <type_name>  <field_name>
// default       :
//   <type_name>  <field_name>
// }
//*****************************************************************************
void    build_switch_unnamed (
                      const string               & key_word,
                            istream              & is,
                      const T_type_definitions   & type_definitions,
                            T_switch_definition  & switch_def,
                      const char                 * return_type)
{
    M_TRACE_ENTER ("build_switch_unnamed", "");

    bool const  is_switch_expr = switch_def.is_switch_expr;
    M_ASSERT_EQ (key_word, "switch");

//    M_FATAL_IF_FALSE (read_token_type_simple (is, switch_def.case_type));
    M_FATAL_IF_FALSE (read_token_simple_word (is, switch_def.case_type));

    if (switch_def.case_type == "{")
    {
        // switch case_type is optional and deprecated.
        // It is not if you try to specify enum values without ::. You should not.
        switch_def.case_type = "";
    }
    else
    {
        read_token_key_word_specified (is, "{");
    }

    string  end_flag;
    M_FATAL_IF_FALSE (read_token_key_word (is, end_flag));

    C_value::E_type    previous_case_value_type = C_value::E_type_float;

    while (end_flag != "}")
    {
        T_switch_case    switch_case;

        if ((end_flag == "case") && (is_switch_expr == true))
        {
            string    str_case_expr;
            M_FATAL_IF_FALSE (read_token_expression_parenthesis (is, str_case_expr));

            T_expression  & case_expression = switch_case.case_expr;
            case_expression.build_expression(type_definitions, str_case_expr);
        }
        else if (end_flag == "case")
        {
            string    str_case_value;
            M_FATAL_IF_FALSE (read_token_case_value (is, str_case_value));

            // 20090606 new
            string::size_type  str_size_before = str_case_value.size();
            remove_string_limits(str_case_value);
            switch_case.case_value = str_case_value;

            if (str_case_value.size() != str_size_before)
            {
                // It is a string
            }
            else
            {
                // It is not a string (could be int or enum symbolic value)
                switch_case.case_value.promote();

                if (switch_case.case_value.get_type() == C_value::E_type_integer)
                {
                }
                else if (switch_case.case_value.get_type() == C_value::E_type_string)
                {
                    // Search ::, means <enum_type>::<symbolic_name>
                    if (switch_case.case_value.as_string().find("::") == string::npos)
                    {
                        // Must add <enum_type> to be able to retrieve the value.
                        switch_case.case_value = switch_def.case_type + "::" + switch_case.case_value.as_string();
                    }
                    switch_case.case_value =
                            string_to_numeric(type_definitions, switch_case.case_value.as_string(), "inline switch", "case value");
                }
                else
                {
                    M_FATAL_COMMENT("case value must be string, int or enum symbolic name");
                }
            }

            // Check that all case value have the same type.
            if ((previous_case_value_type != C_value::E_type_float) &&
                (switch_case.case_value.get_type() != previous_case_value_type))
            {
                M_FATAL_COMMENT("switch case values must have the same type");
            }

            previous_case_value_type = switch_case.case_value.get_type();
        }
        else if (end_flag == "case_")    // ICIOA in progress   does not work
        {
            string    str_case_value;
            M_FATAL_IF_FALSE (read_token_case_value (is, str_case_value));

            T_expression  & case_expression = switch_case.case_expr;
            case_expression.build_expression(type_definitions, str_case_value);

            switch_case.case_value = case_expression.compute_expression_static(type_definitions);

            // Check that all case value have the same type.
            if ((previous_case_value_type != C_value::E_type_float) &&
                (switch_case.case_value.get_type() != previous_case_value_type))
            {
                M_FATAL_COMMENT("switch case values must have the same type");
            }

            previous_case_value_type = switch_case.case_value.get_type();
        }
        else if (end_flag == "default")
        {
            switch_case.is_default_case = true;
        }
        else
        {
            M_FATAL_COMMENT ("Expected case, default or } instead of " << end_flag);
        }

        read_token_key_word_specified (is, ":");

        // lecture struct dans switch_case.definition
        end_flag = build_struct_fields (is, type_definitions,
                                      switch_case.fields,
                                      "case",
                                      "default",
                                      "}",
                                      E_field_scope_other,
                                      return_type);

        switch_def.switch_cases.push_back (switch_case);
    }
}

//*****************************************************************************
// build_switch
// ----------------------------------------------------------------------------
// Format :
// switch  <switch_name>  <case_type_name>
// {
// case <a_valid_value_for case_type_name> :  <type_name>  <field_name>
// ...
// case <a_valid_value_for case_type_name> :
//   <type_name>  <field_name>
//   ...
//   <type_name>  <field_name>
// default       :
//   <type_name>  <field_name>
// }
//*****************************************************************************
void    build_switch (const E_override            must_override,
                      const string              & key_word_param,
                            istream             & is,
                            T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_switch", "");

//    M_ASSERT_EQ (key_word, "switch");
    string    key_word = key_word_param;

    string  switch_name;
    M_FATAL_IF_FALSE (read_token_type_simple (is, switch_name));

    M_CHECK_FORWARD(is, type_definitions.E_type_switch, switch_name);

    build_types_context_type_begin(switch_name);

    T_switch_definition    switch_def;
    if (key_word == "switch_expr")
    {
        switch_def.is_switch_expr = true;
        key_word = "switch";
    }

    build_switch_unnamed(key_word, is, type_definitions, switch_def, "void");

    // Check it is possible to define a new type with this name.
    // No returned value, FATAL is called if it is not possible.
    type_definitions.could_define_new_type(switch_name, T_type_definitions::E_type_switch, must_override);

    type_definitions.map_switch_definition[switch_name] = switch_def;    // possible override ok

    build_types_context_type_end(switch_name);
}

//*****************************************************************************
// build_function_prototype
// ----------------------------------------------------------------------------
// Format :
// function  <type_name>  <function_name> ( <type_name>  <param_name>, ... )
//*****************************************************************************
void    build_function_prototype (
                        const string                           & key_word,
                              istream                          & is,
                              T_function_prototype_definition  & function_def,
                              string                           & function_name,
                        const T_type_definitions               & type_definitions)
{
    M_TRACE_ENTER ("build_function_prototype", "");

    M_ASSERT_EQ (key_word, "function");

    M_FATAL_IF_FALSE (read_token_type_simple (is, function_def.return_type));

    M_FATAL_IF_FALSE (read_token_type_simple (is, function_name));

    build_types_context_type_begin(function_name);

    vector<string>  parameters;
    M_FATAL_IF_FALSE(read_token_parameters_vector(is, parameters));
    // ICIOA mettre les parametres dans une/des expressions
    
    {
        // Get type and name.
        bool    is_parameter_default_value_mandatory = false;
        for (unsigned int   idx = 0; idx < parameters.size(); ++idx)
        {
            vector<string>    parameter_type_name;
            string_to_words(parameters[idx], parameter_type_name);

            if ((parameter_type_name.size() != 3) &&
                (parameter_type_name.size() != 5))
            {
                M_FATAL_COMMENT("Bad function parameters specification (" << parameters[idx] << ")");
            }
            if ((is_parameter_default_value_mandatory == true) &&
                (parameter_type_name.size() != 5))
            {
                M_FATAL_COMMENT("No default value for parameter (" << parameters[idx] << ")");
            }

            T_function_parameter    function_parameter;

            const string  & parameter_direction = parameter_type_name[0];
            if (parameter_direction == "in")
            {
                function_parameter.direction = E_parameter_in;
            }
            else if (parameter_direction == "out")
            {
                function_parameter.direction = E_parameter_out;
            }
            else if (parameter_direction == "in_out")
            {
                function_parameter.direction = E_parameter_in_out;
            }
            else
            {
                M_FATAL_COMMENT("Bad parameter direction " << parameter_direction);
            }

            function_parameter.type = type_definitions.get_final_type(parameter_type_name[1]);
            function_parameter.name = parameter_type_name[2];

            post_build_field_base(type_definitions, function_parameter, E_field_scope_parameter);

            if (parameter_type_name.size() > 3)
            {
                // parameter with default value.

                // all following parameters must have a default value.
                is_parameter_default_value_mandatory = true;

                if (function_parameter.direction != E_parameter_in)
                {
                    M_FATAL_COMMENT("Default value is possible only with in parameter (" << parameters[idx] << ")");
                }

                M_FATAL_IF_NE(parameter_type_name[3], "=");

                const string  & parameter_default_value_expression = parameter_type_name[4];
                C_value         parameter_default_value = compute_expression_static(type_definitions, parameter_default_value_expression);

                // Check value type.
                check_function_parameter_value(type_definitions, function_parameter, parameter_default_value);

                function_parameter.set_default_value(parameter_default_value);
            }

            function_def.add_function_parameter(function_parameter);
        }
    }

    build_types_context_type_end(function_name);
}

//*****************************************************************************
// build_function_after_prototype
// ----------------------------------------------------------------------------
// Format :
// {
//   fields
// }
//*****************************************************************************
void    build_function_after_prototype (
                        const E_override                         must_override,
                              istream                          & is,
                              T_function_definition            & function_def,
                        const string                           & function_name,
                              T_type_definitions               & type_definitions)
{
    M_TRACE_ENTER ("build_function_after_prototype", "");

    build_types_context_type_begin(function_name);

    read_token_key_word_specified (is, "{");

    build_struct_fields(is, type_definitions, function_def.fields,
                        "}", NULL_PTR, NULL_PTR,
                        E_field_scope_other,
                        function_def.return_type.c_str());

    // NB: too difficult to verify that there is a return (for a not void function). 

    // Check it is possible to define a new type with this name.
    // No returned value, FATAL is called if it is not possible.
    type_definitions.could_define_new_type(function_name, T_type_definitions::E_type_function, must_override);

    type_definitions.map_function_definition[function_name] = function_def;    // possible override ok

    build_types_context_type_end(function_name);
}

//*****************************************************************************
// build_function
// ----------------------------------------------------------------------------
// Format :
// function  <type_name>  <function_name> ( <type_name>  <param_name>, ... )
// {
//   fields
// }
//*****************************************************************************
void    build_function (const E_override            must_override,
                        const string              & key_word,
                              istream             & is,
                              T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_function", "");

    T_function_definition    function_def;
    string                   function_name;

    build_function_prototype (key_word, is, function_def, function_name, type_definitions);

    build_function_after_prototype (must_override, is, function_def, function_name, type_definitions);
}

//*****************************************************************************
// build_plugin_output
// ----------------------------------------------------------------------------
// Format :
// plugin{output}{user_data="..."}  <library_name>;
//*****************************************************************************
#ifdef BYTE_INTERPRET_USE_PLUGIN
#include <gmodule.h>

void    build_plugin_output (const E_override            UNUSED(must_override),
                             const string              & key_word,
                                   istream             & is,
                                   T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_plugin_output", "");

    M_ASSERT_EQ (key_word.compare(0, 14, "plugin{output}"), 0);


    T_plugin_output_definition  plugin_def;

    M_FATAL_IF_FALSE (read_token_type_simple (is, plugin_def.library_name));

    build_types_context_type_begin(plugin_def.library_name);

    {
        string    key_word_base = key_word;
        string    key_word_extend;
        if (decompose_type_sep_value_sep (key_word_base,
                                              '{',
                                              '}',
                                              key_word_base,
                                              key_word_extend) == E_rc_ok)
        {
            M_TRACE_DEBUG(key_word_extend);

            if (strncmp(key_word_extend.c_str(), "user_data=", 10) == 0)
            {
                key_word_extend = key_word_extend.substr(10);
                remove_string_limits(key_word_extend);
                plugin_def.user_data = key_word_extend;
                plugin_def.context.P_user_data = strdup(plugin_def.user_data.c_str());
            }
        }
    }


    const string  & filename = plugin_def.library_name;

    GModule   *handle = g_module_open(filename.c_str(), (GModuleFlags)G_MODULE_BIND_LAZY);

    if (handle == NULL)
    {
        const char  * error = g_module_error ();
        M_FATAL_COMMENT(filename << " not found or not a readable libray : " << error);
    }
    M_TRACE_DEBUG(filename << " found");

    gpointer       gp;

    if (g_module_symbol(handle, "byte_interpret_plugin_output_begin", &gp))
    {
        plugin_def.byte_interpret_plugin_output_begin_cb = (T_byte_interpret_plugin_output_begin_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_begin found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_value_integer", &gp))
    {
        plugin_def.byte_interpret_plugin_output_value_integer_cb = (T_byte_interpret_plugin_output_value_integer_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_value_integer found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_value_float", &gp))
    {
        plugin_def.byte_interpret_plugin_output_value_float_cb = (T_byte_interpret_plugin_output_value_float_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_value_float found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_value_string", &gp))
    {
        plugin_def.byte_interpret_plugin_output_value_string_cb = (T_byte_interpret_plugin_output_value_string_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_value_string found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_raw_data", &gp))
    {
        plugin_def.byte_interpret_plugin_output_raw_data_cb = (T_byte_interpret_plugin_output_raw_data_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_raw_data found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_group_begin", &gp))
    {
        plugin_def.byte_interpret_plugin_output_group_begin_cb = (T_byte_interpret_plugin_output_group_begin_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_group_begin found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_group_append_text", &gp))
    {
        plugin_def.byte_interpret_plugin_output_group_append_text_cb = (T_byte_interpret_plugin_output_group_append_text_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_group_append_text found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_group_end", &gp))
    {
        plugin_def.byte_interpret_plugin_output_group_end_cb = (T_byte_interpret_plugin_output_group_end_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_group_end found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_error", &gp))
    {
        plugin_def.byte_interpret_plugin_output_error_cb = (T_byte_interpret_plugin_output_error_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_error found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_missing_data", &gp))
    {
        plugin_def.byte_interpret_plugin_output_missing_data_cb = (T_byte_interpret_plugin_output_missing_data_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_missing_data found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_cmd_error", &gp))
    {
        plugin_def.byte_interpret_plugin_output_cmd_error_cb = (T_byte_interpret_plugin_output_cmd_error_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_cmd_error found");
    }

    if (g_module_symbol(handle, "byte_interpret_plugin_output_cmd_print", &gp))
    {
        plugin_def.byte_interpret_plugin_output_cmd_print_cb = (T_byte_interpret_plugin_output_cmd_print_cb)gp;
        M_TRACE_DEBUG("byte_interpret_plugin_output_cmd_print found");
    }



    type_definitions.vector_plugin_output_definition.push_back(plugin_def);

    build_types_context_type_end(plugin_def.library_name);

    skip_line(is);
}
#endif

//*****************************************************************************
// build_library
// ----------------------------------------------------------------------------
// Format :
// library  "library path;...;library path"    or const string with same format
// {
//    <function prototype>
//    ...
//    <function prototype>
// }
//*****************************************************************************
#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
#include <dynload.h>

void    build_library  (const E_override            must_override,
                        const string              & key_word,
                              istream             & is,
                              T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_library", "");

    M_ASSERT_EQ (key_word, "library");


    // Read library filenames list (1 string with ; delimiter)
    string   library_filenames_str;
    M_FATAL_IF_FALSE (read_token_right_any(is, library_filenames_str));

    {
        T_expression  expression;
        expression.build_expression(type_definitions, library_filenames_str);

        C_value  expression_result = expression.compute_expression_static(type_definitions);
        library_filenames_str = expression_result.get_str();
    }

    // Split string read on ;
    vector<string>  library_filenames;
    while (true)
    {
        string         library_filename;
        string         str_right;
        E_return_code  rc = get_before_separator_after (library_filenames_str, ';', library_filename, str_right);
        if (rc != E_rc_ok)
        {
            library_filenames.push_back(library_filenames_str);
            break;
        }

        library_filenames.push_back(library_filename);
        library_filenames_str = str_right;
    }


    // Load library from one of the library_filenames 
    T_library_definition  * P_library_def = NULL;
    string   libraries_not_found;
    string   libraries_not_loadable;

    for (vector<string>::const_iterator  iter  = library_filenames.begin();
                                         iter != library_filenames.end();
                                       ++iter)
    {
        const string  & library_filename = *iter;

        T_map_library_definition::iterator  iter_lib =
            type_definitions.map_library_definition.find(library_filename);
        if (iter_lib != type_definitions.map_library_definition.end())
        {
            // already found
            P_library_def = & iter_lib->second;
            break;
        }

        ifstream              ifs (library_filename.c_str());
        if (!ifs)
        {
            M_TRACE_DEBUG(library_filename << " not found");
            libraries_not_found += " >";
            libraries_not_found += library_filename;
            libraries_not_found += "<";
            continue;
        }

        void  * DLLib_handle = dlLoadLibrary (library_filename.c_str());
        if (DLLib_handle == NULL)
        {
            M_TRACE_DEBUG(library_filename << " not a loadable library");
            libraries_not_loadable += " >";
            libraries_not_loadable += library_filename;
            libraries_not_loadable += "<";
            continue;
        }

        // New library
        P_library_def = & type_definitions.map_library_definition[library_filename];
        P_library_def->DLLib_handle = DLLib_handle;
        P_library_def->full_name    = library_filename;
        break;
    }

    if (P_library_def == NULL)
    {
        ostringstream  oss;
        if (libraries_not_loadable != "")
        {
            oss << "Libraries" << libraries_not_loadable << " are not loadable." << endl << endl;
        }
        if (libraries_not_found != "")
        {
            oss << "Libraries" << libraries_not_found << " are not found." << endl << endl;
        }

        M_FATAL_COMMENT(oss.str());
    }

    M_TRACE_DEBUG(P_library_def->full_name << " is a loadable library");

    read_token_key_word_specified(is, "{");

    // Read function prototypes and get corresponding pointer into library
    while (true)
    {
        string  key_word;
        M_FATAL_IF_FALSE (read_token_simple_word_or_string (is, key_word));
        if (key_word == "}")
            break;

        T_function_definition            function_def;
        T_library_function_definition    library_function_def;
        string                         & function_name = library_function_def.name;

        build_function_prototype (key_word, is, function_def, function_name, type_definitions);
        read_token_key_word_specified(is, ";");

        library_function_def.funptr = dlFindSymbol((DLLib*)P_library_def->DLLib_handle , library_function_def.name.c_str());
        if (library_function_def.funptr == NULL)
        {
            M_FATAL_COMMENT(library_function_def.name << " not found into " << P_library_def->full_name << " library");
        }
        M_TRACE_DEBUG(library_function_def.name << " found into " << P_library_def->full_name << " library");

        function_def.idx_library_function_def = P_library_def->library_functions.size();
        function_def.P_library_def = P_library_def;

        istringstream    iss("{ \n"
                             "  fatal \"Bug in the software, this version is only a prototype\"; \n"
                             "}");
        build_function_after_prototype (must_override, iss, function_def, function_name, type_definitions);

        P_library_def->library_functions.push_back(library_function_def);
    }
}
#endif

//*****************************************************************************
// build_types_no_include
// - search for alias, struct, enum, switch definitions
// - add the definition into the associated map
// - stop when key word is NOT understand and return it
//   NB: NOT understand = NOT understand at this level (see calling functions)
//*****************************************************************************
string    build_types_no_include (istream             & is,
                                  T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_types_no_include", "");

    string    key_word;
    while (read_token_left_any (is, key_word))
    {
        E_override    must_override = E_override_no;
        if (key_word == "override")
        {
            must_override = E_override_yes;

            M_FATAL_IF_FALSE (read_token_left_any (is, key_word));
        }

        build_types_context_type_kind_begin(key_word);

        long    sizeof_enum = 0;
        if (key_word == "alias")
        {
            build_alias (must_override, key_word, is, type_definitions);
        }
        else if (key_word == "const")
        {
            build_const (must_override, key_word, is, type_definitions);
        }
        else if (key_word == "struct")
        {
            build_struct (must_override, key_word, is, type_definitions);
        }
        else if (/* (key_word == "enum") || */
                 (key_word == "enum8") ||
                 (key_word == "enum16") ||
                 (key_word == "enum24") ||
                 (key_word == "enum32") ||
                 ((strncmp(key_word.c_str(), "enum", 4) == 0) &&
                  (get_number (key_word.c_str () + strlen ("enum"), &sizeof_enum))))
        {
            build_enum (must_override, key_word, is, type_definitions);
        }
        else if ((key_word == "switch") ||
                 (key_word == "switch_expr"))
        {
            build_switch (must_override, key_word, is, type_definitions);
        }
        else if (key_word == "function")
        {
            build_function (must_override, key_word, is, type_definitions);
        }
        else if ((key_word == "bitfield8") ||
                 (key_word == "bitfield16") ||
                 (key_word == "bitfield24") ||
                 (key_word == "bitfield32") ||
                 (key_word == "bitstream8") ||
                 (key_word == "bitstream16") ||
                 (key_word == "bitstream24") ||
                 (key_word == "bitstream32"))
        {
            build_bitfield (must_override, key_word, is, type_definitions);
        }
#ifdef BYTE_INTERPRET_USE_PLUGIN
        else if (strncmp(key_word.c_str(), "plugin{output}", 14) == 0)
        {
            build_plugin_output (must_override, key_word, is, type_definitions);
        }
#endif
#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
        else if (strncmp(key_word.c_str(), "library", 7) == 0)
        {
            build_library (must_override, key_word, is, type_definitions);
        }
#endif
        else
        {
            build_types_context_type_kind_end(key_word);
            break;
        }

        build_types_context_type_kind_end(key_word);
    }

    return  key_word;
}

//*****************************************************************************
// build_types_no_include_str *************************************************
//*****************************************************************************

void    build_types_no_include_str (const string              & str,
                                          T_type_definitions  & type_definitions)
{
    istringstream           iss(str.c_str());

    const string    result = build_types_no_include (iss, type_definitions);
    M_FATAL_IF_NE(result, "");
}

//*****************************************************************************
// build_types_begin **********************************************************
//*****************************************************************************
C_value  string_count_cpp_to_fdesc(const string::size_type  cpp_count);

void    build_types_begin (T_type_definitions  & type_definitions)
{
    if (type_definitions.map_enum_definition_representation.empty() == false)
    {
        // Already initialized
        return;
    }

    M_TRACE_ENTER ("build_types_begin", "");

    // bool type definitions
    build_types_no_include_str("enum8     bool8   { false 0  true 1 }", type_definitions);
    build_types_no_include_str("enum16    bool16  as  bool8 ;", type_definitions);
    build_types_no_include_str("enum32    bool32  as  bool8 ;", type_definitions);
    build_types_no_include_str("enum1     bool1   as  bool8 ;", type_definitions);

    // string type definitions
    type_definitions.map_const_value["string::npos"] = string_count_cpp_to_fdesc(string::npos);

    // system
#if defined WIN32
    type_definitions.map_const_value["system::os"]     = "win";
#else
    type_definitions.map_const_value["system::os"]     = "linux";
#endif
    type_definitions.map_const_value["system::bits"] = sizeof(void*)*8;
    type_definitions.map_const_value["system::osbits"] =
    type_definitions.map_const_value["system::os"].as_string() +
    type_definitions.map_const_value["system::bits"].as_string();


#define M_DEFINE_BUILTIN_FUNCTION(PROTOTYPE)                                    \
    build_types_no_include_str("function " PROTOTYPE " \n"                      \
        "{ \n"                                                                  \
        "  fatal \"Bug in the software, this version is only a prototype\"; \n" \
        "}", type_definitions)

    // Built-in functions
    M_DEFINE_BUILTIN_FUNCTION("bool8    is_existing_field_or_variable (in string  var_name)");
    M_DEFINE_BUILTIN_FUNCTION("string   to_string (in any  value)");
    M_DEFINE_BUILTIN_FUNCTION("any      to_numeric (in string  value)");
    M_DEFINE_BUILTIN_FUNCTION("float64  to_float (in any  value)");
    M_DEFINE_BUILTIN_FUNCTION("int64    to_integer (in string  value, in int8  base = 0)");
    M_DEFINE_BUILTIN_FUNCTION("string   getenv (in string  env_name)");

    M_DEFINE_BUILTIN_FUNCTION("int64    string.length  (in string  str)");
    M_DEFINE_BUILTIN_FUNCTION("string   string.substr  (in string  str, in int64  idx, in int64  count = string::npos)");
    M_DEFINE_BUILTIN_FUNCTION("string   string.erase   (in string  str, in int64  idx, in int64  count = string::npos)");
    M_DEFINE_BUILTIN_FUNCTION("string   string.insert  (in string  str, in int64  idx, in string  str_to_insert)");
    M_DEFINE_BUILTIN_FUNCTION("string   string.replace (in string  str, in int64  idx, in int64  count, in string str_to_insert)");
    M_DEFINE_BUILTIN_FUNCTION("string   string.replace_all (in string  str, in string  old, in string new)");
    M_DEFINE_BUILTIN_FUNCTION("int64    string.find    (in string  str, in string  str_to_find)");

//    M_DEFINE_BUILTIN_FUNCTION("void     date.get_year_from_days          (in uint16{min=1583} ref_year, in uint32 number_of_days   , out uint16 year, out uint16 number_of_days_in_year)");
//    M_DEFINE_BUILTIN_FUNCTION("void     date.get_year_from_seconds       (in uint16{min=1583} ref_year, in uint40 number_of_seconds, out uint16 year, out uint32 number_of_seconds_in_year)");
//    M_DEFINE_BUILTIN_FUNCTION("void     date.get_month_from_year_days    (in uint16{min=1583} year, in uint16 number_of_days_in_year   , out uint8 month, out uint8 day)");
//    M_DEFINE_BUILTIN_FUNCTION("void     date.get_month_from_year_seconds (in uint16{min=1583} year, in uint32 number_of_seconds_in_year, out uint8 month, out uint8 day, out uint32 number_of_seconds_in_day)");
    M_DEFINE_BUILTIN_FUNCTION("string   date.get_string_from_days    (in uint16{min=1583} ref_year, in uint32 number_of_days)");
    M_DEFINE_BUILTIN_FUNCTION("string   date.get_string_from_seconds (in uint16{min=1583} ref_year, in uint40 number_of_seconds)");

    // Decoder definitions
    build_types_no_include_str("alias     frame   int64 ;", type_definitions);
    M_DEFINE_BUILTIN_FUNCTION("void  frame_append_data (in frame  frame, in any  byte)");
    M_DEFINE_BUILTIN_FUNCTION("void  frame_append_data_array (in frame  frame, in pointer  data, in int64  data_size)");
    M_DEFINE_BUILTIN_FUNCTION("void  frame_append_hexa_data (in frame  frame, in string  str_hexa)");

    // Decoder built-in
    M_DEFINE_BUILTIN_FUNCTION("void  decoder_aes     (in frame  frame, in uint32   nb_of_bits_needed_uint)");
    M_DEFINE_BUILTIN_FUNCTION("void  decoder_base64  (in frame  frame, in uint32   nb_of_bits_needed_uint)");
    M_DEFINE_BUILTIN_FUNCTION("void  decoder_utf16be (in frame  frame, in uint32   nb_of_bits_needed_uint)");
    M_DEFINE_BUILTIN_FUNCTION("void  decoder_utf16le (in frame  frame, in uint32   nb_of_bits_needed_uint)");
    M_DEFINE_BUILTIN_FUNCTION("void  decoder_utf8    (in frame  frame, in uint32   nb_of_bits_needed_uint)");

    // Library pointer function
    M_DEFINE_BUILTIN_FUNCTION("int64    get_pointer_len_bytes     (                        in uint32  length_bytes )");
    M_DEFINE_BUILTIN_FUNCTION("int64    get_pointer_len_bits      (                        in uint32  length_bits )");
    M_DEFINE_BUILTIN_FUNCTION("int64    get_pointer_pos_len_bytes ( in uint32  pos_bytes , in uint32  length_bytes )");
    M_DEFINE_BUILTIN_FUNCTION("int64    get_pointer_pos_len_bits  ( in uint32  pos_bits  , in uint32  length_bits  )");
}



//*****************************************************************************
// build_types_finalize struct_fields *****************************************
//*****************************************************************************
void    build_types_finalize(T_type_definitions  & type_definitions,
                             T_field_type_name   & field_type_name);

void    build_types_finalize(T_type_definitions  & type_definitions,
                             T_struct_fields     & fields)
{
    for (T_struct_fields::iterator
                             iter  = fields.begin();
                             iter != fields.end();
                           ++iter)
    {
        T_field_type_name   & field = *iter;

        build_types_finalize(type_definitions, field);
    }
}

//*****************************************************************************
// build_types_finalize struct_def ********************************************
//*****************************************************************************

void    build_types_finalize(T_type_definitions   & type_definitions,
                             T_struct_definition  & struct_def)
{
    build_types_finalize(type_definitions, struct_def.fields);
}

//*****************************************************************************
// build_types_finalize bitfield_def ******************************************
//*****************************************************************************

void    build_types_finalize(T_type_definitions     & type_definitions,
                             T_bitfield_definition  & bitfield_def)
{
    build_types_finalize(type_definitions, bitfield_def.master_field);
    build_types_finalize(type_definitions, bitfield_def.fields_definition);
}

//*****************************************************************************
// build_types_finalize switch_def ********************************************
//*****************************************************************************

void    build_types_finalize(T_type_definitions   & type_definitions,
                             T_switch_definition  & switch_def)
{
    for (T_switch_cases::iterator
                             iter  = switch_def.switch_cases.begin();
                             iter != switch_def.switch_cases.end();
                           ++iter)
    {
        T_switch_case   & case_val = *iter;

        build_types_finalize(type_definitions, case_val.fields);
    }
}

//*****************************************************************************
// build_types_finalize field *************************************************
//*****************************************************************************
void    build_types_finalize_itself(T_type_definitions  & type_definitions,
                                    T_field_type_name   & field_type_name);

void    build_types_finalize(T_type_definitions  & type_definitions,
                             T_field_type_name   & field_type_name)
{
    if (field_type_name.P_sub_struct)
    {
        build_types_finalize(type_definitions, *field_type_name.P_sub_struct);
    }

    build_types_finalize(type_definitions, field_type_name.sub_struct_2);

    if (field_type_name.P_bitfield_inline)
    {
        build_types_finalize(type_definitions, *field_type_name.P_bitfield_inline);
    }

    if (field_type_name.P_switch_inline)
    {
        build_types_finalize(type_definitions, *field_type_name.P_switch_inline);
    }

    build_types_finalize_itself(type_definitions, field_type_name);
}



//*****************************************************************************
// build_types_finalize *******************************************************
//*****************************************************************************

void    build_types_finalize(T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER("build_types_finalize", "");

  {
    T_map_struct_definition  & map_struct_definition = type_definitions.map_struct_definition;
    for (T_map_struct_definition::iterator
                             iter  = map_struct_definition.begin();
                             iter != map_struct_definition.end();
                           ++iter)
    {
      T_struct_definition  & struct_definition = iter->second;

      build_types_finalize(type_definitions, struct_definition);
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

      build_types_finalize(type_definitions, bitfield_definition);
    }
  }
#if 0
  // Nothing to do
  {
    T_map_enum_definition_representation  & map_enum_definition_representation = type_definitions.map_enum_definition_representation;
    for (T_map_enum_definition_representation::iterator
                             iter  = map_enum_definition_representation.begin();
                             iter != map_enum_definition_representation.end();
                           ++iter)
    {
      T_enum_definition_representation  & enum_definition = iter->second;

      build_types_finalize(type_definitions, enum_definition);
    }
  }
#endif
  {
    T_map_switch_definition  & map_switch_definition = type_definitions.map_switch_definition;
    for (T_map_switch_definition::iterator
                             iter  = map_switch_definition.begin();
                             iter != map_switch_definition.end();
                           ++iter)
    {
      T_switch_definition  & switch_definition = iter->second;

      build_types_finalize(type_definitions, switch_definition);
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

      build_types_finalize(type_definitions, function_definition.fields);
    }
  }
}

//*****************************************************************************
// build_types ****************************************************************
//*****************************************************************************
// Read type definitions until :
// - end of stream        : returns ""
// - NOT understand word  : returns the NOT understand word
//*****************************************************************************

string    build_types2 (istream             & is,
                       T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_types", "");

    build_types_begin (type_definitions);

    while (true)
    {
        const string    last_extracted_word = build_types_no_include (is,
                                                           type_definitions);

        // If there is nothing else to read.
        if (last_extracted_word == "")
        {
            M_ASSERT (is.eof ());
            return  last_extracted_word;
        }

        if (last_extracted_word == "include")
        {
            // include a file.
            string    file_name;
            M_FATAL_IF_FALSE (read_token_include_name (is, file_name));

            read_token_end_of_statement(is);

            const string    full_file_name = byte_interpret_get_include_file_name(file_name);
            ifstream   ifs (full_file_name.c_str ());
            if (!ifs)
            {
                M_FATAL_COMMENT("Impossible to open file " << full_file_name);
            }

            M_TRACE_INFO ("include " << file_name);

            // Save include_directory (and restore it in destructor).
            C_byte_interpret_append_include_directory  biaid;

            // Retrieve (sub-)directory from file name.
            {
                const string::size_type  idx_last_dir = file_name.find_last_of("/\\");
                if (idx_last_dir != string::npos)
                {
                    biaid.append_include_directory(file_name.substr(0, idx_last_dir));
                }
            }

            build_types_context_include_file_open(file_name, ifs);

            const string    result = build_types (ifs, type_definitions);
            if (result != "")
            {
                M_FATAL_COMMENT("Expecting keyword instead of " << result);
            }

            build_types_context_include_file_close(file_name);
            continue;
        }

        // Unknow word, return it.
        return  last_extracted_word;
    }
}

string    build_types (istream             & is,
                       T_type_definitions  & type_definitions)
{
    // save the current locale for LC_NUMERIC (used for numeric input/output, e.g. strtoll)
    // change the locale for LC_NUMERIC (so 0.236 is a valid number)
    const char  * locale_save = setlocale(LC_NUMERIC, "C");

    const string  NOT_understood_word = build_types2 (is, type_definitions);

    // restore the saved locale
    setlocale(LC_NUMERIC, locale_save);

    build_types_finalize(type_definitions);


    return  NOT_understood_word;
}

//****************************************************************************
// build_types
//****************************************************************************

void      build_types (const string              & file_name,
                             T_type_definitions  & type_definitions)
{
    M_TRACE_ENTER ("build_types", file_name);

    // Open the types definitions file.
    ifstream    ifs(file_name.c_str());
    if (!ifs)
    {
        M_FATAL_COMMENT("Unable to open " << file_name);
    }

    // ICIOA must manage sub-directory

    // Build the types.
    const string  NOT_understood_word = build_types (ifs, type_definitions);
    if (NOT_understood_word != "")
    {
        M_FATAL_COMMENT("Not understood word (" << NOT_understood_word << ") into " << file_name);
    }
}


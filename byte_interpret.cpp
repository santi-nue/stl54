/*
 * Copyright 2005-2013 Olivier Aveline <wsgd@free.fr>
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
// After other includes, does NOT work : does NOT compile
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
#include <strstream>
#include <algorithm>
using namespace std;

#include "C_value.h"
#include "byte_interpret.h"
#include "byte_interpret_common.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_build_types.h"
#include "byte_interpret_build_types_read_token.h"
#include "byte_interpret_builder.h"
#include "F_common_print.h"
#include "T_interpret_data.h"
#include "T_frame_data_write.h"
#include "T_expression.h"


//*****************************************************************************
//*****************************************************************************

#define  K_ATTRIBUTE_SEPARATOR  "."


//****************************************************************************
// Exceptions
//****************************************************************************

class C_byte_interpret_exception_normal : public C_byte_interpret_exception
{
public:
	C_byte_interpret_exception_normal(
						 const char                        * file_name,
							   int                           file_line,
							   E_byte_interpret_exception    bie,
                         const std::string                 & str)
		:C_byte_interpret_exception(file_name, file_line, bie, str)
	{
	}
	C_byte_interpret_exception_normal()
		:C_byte_interpret_exception(M_WHERE, E_byte_interpret_exception_fatal, "NOT initialized exception explanation")
	{
	}
};

class C_byte_interpret_exception_loop : public C_byte_interpret_exception_normal
{
public:
	C_byte_interpret_exception_loop(
						 const char                        * file_name,
							   int                           file_line,
							   E_byte_interpret_exception    bie,
                         const std::string                 & str)
		:C_byte_interpret_exception_normal(file_name, file_line, bie, str)
	{
	}
	C_byte_interpret_exception_loop()
		:C_byte_interpret_exception_normal(M_WHERE, E_byte_interpret_exception_fatal, "NOT initialized exception explanation")
	{
	}
};

class C_byte_interpret_exception_return : public C_byte_interpret_exception_normal
{
public:
	C_byte_interpret_exception_return(
						 const char                        * file_name,
							   int                           file_line,
							   E_byte_interpret_exception    bie,
                         const std::string                 & str,
						 const C_value                     & returned_value_param)
		:C_byte_interpret_exception_normal(file_name, file_line, bie, str),
		 is_returned_value_set(true),
		 returned_value(returned_value_param)
	{
	}
	C_byte_interpret_exception_return(
						 const char                        * file_name,
							   int                           file_line,
							   E_byte_interpret_exception    bie,
                         const std::string                 & str)
		:C_byte_interpret_exception_normal(file_name, file_line, bie, str),
		 is_returned_value_set(false)
	{
	}
	C_byte_interpret_exception_return()
		:C_byte_interpret_exception_normal(M_WHERE, E_byte_interpret_exception_fatal, "NOT initialized exception explanation"),
		 is_returned_value_set(false)
	{
	}

	bool       is_returned_value_set;
	C_value    returned_value;
};


//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <value>
// <value> could be :
//   <int_value>
//   <enum_value>                           NOT IMPLEMENTED
//   <variable_value>
//*****************************************************************************

E_return_code    get_complex_variable_integer_value (
										const T_interpret_data  & interpret_data,
                                        const string     & value_str,
                                              long long  & value_int)
{
    if (get_number (value_str.c_str (), value_int))
    {
    }
    else if (interpret_data.get_int_value_of_read_variable (
                                           value_str, value_int) == false)
    {
        return  E_rc_not_integer;
    }

    return  E_rc_ok;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <left_part><separator_left><value_str><separator_right>
//*****************************************************************************

E_return_code    decompose_type_sep_value_sep (
						const T_type_definitions  & type_definitions,
							  T_interpret_data    & interpret_data,
                        const string              & orig_type,
                        const char                  separator_left,
                        const char                  separator_right,
                              string              & left_part,
                              string              & value_str,
                              long long           & value_int)
{
    E_return_code  rc = decompose_type_sep_value_sep (orig_type,
                                                      separator_left,
                                                      separator_right,
                                                      left_part,
                                                      value_str);
    if (rc != E_rc_ok)
        return  rc;

    C_value    value = compute_expression_no_io(type_definitions, interpret_data, value_str);

    if (value.get_type () != C_value::E_type_integer)
        return  E_rc_not_integer;

    value_int = value.get_int ();
    return  E_rc_ok;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <simple_type>[<str_array_size>]
// NB: <str_array_size> is :
//     - an integer > 0 or
//     - the name of the integer variable which contains the size
//*****************************************************************************
#if 0
bool    is_an_array (const T_type_definitions  & type_definitions,
						   T_interpret_data    & interpret_data,
                     const string              & orig_type,
                           string              & simple_type,
                           string              & str_array_size,
                           long                & array_size)
{
    array_size = -1;
	bool    result = is_an_array (orig_type, simple_type, str_array_size);

	if (result != true)
    {
        return  false;
    }

    // Verify special cases.
    if ((str_array_size == "*") || (str_array_size == "+"))
    {
        array_size = LONG_MAX;
        return  true;
    }

    // Compute the array size.
    C_value    value = compute_expression_no_io(type_definitions, interpret_data, str_array_size);

    if (value.get_type () != C_value::E_type_integer)
    {
		// Error trace/display done by caller.
        return  true;
    }

    array_size = static_cast<long>(value.get_int ());

    return  true;
}
#endif

//*****************************************************************************
// ----------------------------------------------------------------------------
// Returns :
// <array_name>[<idx>]
// OR if <array_name> is <begin_array_name>[...][...][...]
//  returns :
// <begin_array_name>[<idx>][...][...][...] 
//*****************************************************************************

string  get_array_idx_name (const string  & array_name,
                            const int       idx)
{
	if ((array_name.empty ()) ||
		(array_name[array_name.size () - 1] != ']'))
        return  array_name + "[" + get_string (idx) + "]";

	// array_name already contains one or more sequence [ ]
	// Must put [<idx>] at the 1st place of this sequence.

	// Search the beginning of the sequence [ ].
    const char  * P_begin = array_name.c_str ();
    const char  * P_current = P_begin + array_name.size ();

    while ((P_current > P_begin) && (*--P_current == ']'))
    {
        while ((P_current > P_begin) && (*--P_current != '['))
        {
        }
        M_ASSERT (*P_current == '[');
    }

	// Put P_current on the 1st [ of the sequence [ ].
    ++P_current;

    return  array_name.substr (0, P_current - P_begin) +
            "[" + get_string (idx) + "]" +
            string (P_current);
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <key_word>               if could_have_no_size        or
// <key_word>()             if could_have_no_size        or
// <key_word>(<str_size>)
//*****************************************************************************
#if 0
bool    is_a_key_word_op_size_cp (const string   & key_word,
								  const bool       could_have_no_size,
                                  const string   & orig_type,
                                        string   & str_size)
{
    str_size = "";

	if (could_have_no_size)
	{
		if ((orig_type == key_word) ||
			(orig_type == key_word+"()"))
			return  true;
	}

    if (strncmp (orig_type.c_str (), key_word.c_str(), key_word.size()) != 0)
        return  false;
    if (orig_type[key_word.size()] != '(')
        return  false;

    string       left_part;
    if (decompose_type_sep_value_sep (orig_type, '(', ')',
                                      left_part,
                                      str_size) != E_rc_ok)
        return  false;

    if (left_part != key_word)
        return  false;

    return  true;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <key_word> or <key_word>()                      if could_have_no_size
// NB: unknow size, must search the "end" into the data
// ----------------------------------------------------------------------------
// Format :
// <key_word>(<str_size>)
// NB: <str_size> is :
//     - an integer > 0 or
//     - the name of the integer variable which contains the size
//*****************************************************************************

bool    is_a_key_word_op_size_cp (const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
								  const string              & key_word,
								  const bool                  could_have_no_size,
                                  const char                * special_size,
                                  const string              & orig_type,
                                        string              & str_size,
                                        long                & size)
{
	size = 0;
	bool    result = is_a_key_word_op_size_cp (key_word, could_have_no_size, orig_type, str_size);

	if (result != true)
    {
        return  false;
    }

    if (str_size == "")
        return  true;

	if (special_size)
	{
		if (str_size == special_size)
			return  true;
	}

    // Compute the string size.
    C_value    value = compute_expression_no_io(type_definitions, interpret_data, str_size);

    if (value.get_type () != C_value::E_type_integer)
    {
		// Error trace/display done by caller.
        return  true;
    }

    size = static_cast<long>(value.get_int ());

    return  true;
}
#endif

//*****************************************************************************
// size_expression_to_int
//*****************************************************************************

long    size_expression_to_int(   const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
								  const string              & str_size)
{
	if (str_size == "")    // forbidden for raw/subproto/insproto
		return  0;

	if (str_size == "*")   // forbidden for string...
		return  0;

    // Compute the string size.
    C_value    value = compute_expression_no_io(type_definitions, interpret_data, str_size);

    if (value.get_type () != C_value::E_type_integer)
    {
		// Error trace/display done by caller.
        return  -1;
    }

    return  static_cast<long>(value.get_int ());
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <final_simple_type>(<discriminant>)
// NB: <discriminant> is :
//     - an integer or
//     - a string or
//     - the name of the variable which contains the data
//*****************************************************************************

bool    is_a_switch (const T_type_definitions  & type_definitions,
					 const string              & orig_type,
                           string              & final_simple_type,
                           string              & discriminant)
{
    if (decompose_type_sep_value_sep (orig_type, '(', ')',
                                      final_simple_type,
                                      discriminant) != E_rc_ok)

  // reproductible fails without this line
    final_simple_type = type_definitions.get_final_type (final_simple_type);

    return  type_definitions.map_switch_definition.find (final_simple_type) !=
            type_definitions.map_switch_definition.end ();
}

//*****************************************************************************
// return the symbolic_name of an <enum_value> of <orig_type> enum type.
// return "" if :
// - <orig_type> is NOT an enum
// - <enum_value> is NOT found
// NB: <enum_value> could already be a symbolic value
// NB: take care of aliases
//*****************************************************************************

string  get_enum_symbolic_name (const T_type_definitions  & type_definitions,
					            const string              & orig_type,
                                const string              & enum_value)
{
	const T_enum_definition_representation  * P_enum = type_definitions.get_P_enum(orig_type);
	if (P_enum == NULL_PTR)
        return  "";

    const T_enum_definition  & enum_def = P_enum->definition;

	for (T_enum_definition::const_iterator  iter  = enum_def.begin ();
                                            iter != enum_def.end ();
                                          ++iter)
    {
        if ((enum_value == iter->name) ||
            (enum_value == get_string (iter->value)))
            return  iter->name;
    }

    return  "";
}
string  get_enum_symbolic_name (const T_type_definitions  & type_definitions,
					            const string              & orig_type,
                                const long long             enum_value)
{
    return  get_enum_symbolic_name (type_definitions,
                                    orig_type,
                                    get_string (enum_value));
}

// ICIOA a revoir ?
void    promote_enum_symbolic_name (const T_type_definitions  & type_definitions,
					                const string              & orig_type,
                                          C_value             & enum_value)
{
	const T_enum_definition_representation  * P_enum_def_rep = type_definitions.get_P_enum(orig_type);
	if (P_enum_def_rep == NULL)
		return;

    const T_enum_definition  & enum_def = P_enum_def_rep->definition;

	if (enum_value.get_type() == C_value::E_type_string)
	{
		for (T_enum_definition::const_iterator  iter  = enum_def.begin ();
												iter != enum_def.end ();
											  ++iter)
		{
			if ((enum_value.as_string() == iter->name) ||
				(enum_value.as_string() == get_string (iter->value)))
			{
				enum_value = C_value(iter->value);
				enum_value.set_str(iter->name);
				return;
			}
		}
	}
	else if (enum_value.get_type() == C_value::E_type_integer)
	{
		for (T_enum_definition::const_iterator  iter  = enum_def.begin ();
												iter != enum_def.end ();
											  ++iter)
		{
			if (enum_value.get_int() == iter->value)
			{
				enum_value = C_value(iter->value);
				enum_value.set_str(iter->name);
				return;
			}
		}
	}

    return;
}
#if 0
//*****************************************************************************
// printf_args_to_string ******************************************************
//*****************************************************************************

C_value    printf_args_to_string (const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
								  const string              & print_args,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err,
							   bool                    & could_be_and_is_multiple_value)
{
	const bool  could_be_multiple_value = could_be_and_is_multiple_value;
	could_be_and_is_multiple_value = false;

	string    printf_result = print_args;

	remove_string_limits(printf_result);

	if (printf_result != print_args)
	{
		// There was "...", so it is a simple string.
		// Nothing more to do.
	}
	else if ((printf_result != "") &&
			 (printf_result[0] == '(') &&
			 (printf_result[printf_result.size()-1] == ')'))
	{
		// There is (...), so it is a printf.

		// Remove ( and ).
		printf_result.erase(0, 1);
		printf_result.erase(printf_result.size()-1);

		// Split on ,.
		vector<string>    words;
		string_to_words(printf_result, words, K_parser_cfg_parameters);

		//
		remove_string_limits(words[0]);
		promote_printf_string_to_64bits(words[0]);

		// Compute values to print.
		vector<C_value>    values_to_print;
		for (unsigned int   idx = 1; idx < words.size(); ++idx)
		{
			values_to_print.push_back(compute_expression(type_definitions, interpret_data, in_out_frame_data,
														 words[idx],
														 data_name, data_simple_name, os_out, os_err));
		}

		// printf.
		return  C_value::sprintf_values(words[0], values_to_print);
	}
	else
	{
		// There is no "", so it is a value.
		if (( could_be_multiple_value) &&
			(printf_result.find('*') != string::npos))
		{
			could_be_and_is_multiple_value = true;
		}

		{
			C_value       value;
			if (get_complex_value (type_definitions, interpret_data, printf_result, value) == E_rc_ok)
			{
				printf_result += " -> ";
				printf_result += value.as_string();
			}
		}
	}

	return  printf_result;
}

//*****************************************************************************
// printf_args_to_string ******************************************************
//*****************************************************************************

C_value    printf_args_to_string (const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
								  const string              & print_args,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err)
{
	bool    could_be_multiple_value = false;
	return  printf_args_to_string (type_definitions,
								   interpret_data,
								   in_out_frame_data,
								   print_args,
								   data_name,
								   data_simple_name,
								   os_out,
								   os_err,
								   could_be_multiple_value);
}
#else
//*****************************************************************************
// printf_args_to_string ******************************************************
//*****************************************************************************

C_value    printf_args_to_string (const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
								  const string              & print_args,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err)
{
	string    printf_result = print_args;

	remove_string_limits(printf_result);

	if (printf_result != print_args)
	{
		// There was "...", so it is a simple string.
		// Nothing more to do.
	}
	else if ((printf_result != "") &&
			 (printf_result[0] == '(') &&
			 (printf_result[printf_result.size()-1] == ')'))
	{
		// There is (...), so it is a printf.

		// Remove ( and ).
		printf_result.erase(0, 1);
		printf_result.erase(printf_result.size()-1);

		// Split on ,.
		vector<string>    words;
		string_to_words(printf_result, words, K_parser_cfg_parameters);

		//
		remove_string_limits(words[0]);
		promote_printf_string_to_64bits(words[0]);

		// Compute values to print.
		vector<C_value>    values_to_print;
		for (unsigned int   idx = 1; idx < words.size(); ++idx)
		{
			values_to_print.push_back(compute_expression(type_definitions, interpret_data, in_out_frame_data,
														 words[idx],
														 data_name, data_simple_name, os_out, os_err));
		}

		// printf.
		return  C_value::sprintf_values(words[0], values_to_print);
	}
	else
	{
		// There is no "", so it is a value.
		C_value       value;
		if (get_complex_value (type_definitions, interpret_data, printf_result, value) == E_rc_ok)
		{
			printf_result += " -> ";
			printf_result += value.as_string();
		}
	}

	return  printf_result;
}

//*****************************************************************************
// printf_args_to_string ******************************************************
//*****************************************************************************

C_value    printf_args_to_string (const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
								  const string              & print_args,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err,
							   T_interpret_read_values::T_var_name_P_values  & var_name_P_values)
{
	string    printf_result = print_args;

	remove_string_limits(printf_result);

	if (printf_result != print_args)
	{
		// There was "...", so it is a simple string.
		// Nothing more to do.
	}
	else if ((printf_result != "") &&
			 (printf_result[0] == '(') &&
			 (printf_result[printf_result.size()-1] == ')'))
	{
		// There is (...), so it is a printf.

		// Remove ( and ).
		printf_result.erase(0, 1);
		printf_result.erase(printf_result.size()-1);

		// Split on ,.
		vector<string>    words;
		string_to_words(printf_result, words, K_parser_cfg_parameters);

		//
		remove_string_limits(words[0]);
		promote_printf_string_to_64bits(words[0]);

		// Compute values to print.
		vector<C_value>    values_to_print;
		for (unsigned int   idx = 1; idx < words.size(); ++idx)
		{
			values_to_print.push_back(compute_expression(type_definitions, interpret_data, in_out_frame_data,
														 words[idx],
														 data_name, data_simple_name, os_out, os_err));
		}

		// printf.
		return  C_value::sprintf_values(words[0], values_to_print);
	}
	else
	{
		// There is no "", so it is a value.
		C_value        value;
		E_return_code  return_code = get_complex_value (type_definitions, interpret_data, printf_result, value);
		if (return_code == E_rc_ok)
		{
#if 1
			T_interpret_read_values::T_var_name_P_value    var_name_P_value;
			var_name_P_value.var_name = printf_result;
			var_name_P_value.P_value = & interpret_data.get_attribute_value_of_read_variable(printf_result);
			var_name_P_values.push_back(var_name_P_value);
#else
			printf_result += " -> ";
			printf_result += value.as_string();
#endif
		}
		else if (return_code == E_rc_multiple_value)
		{
			interpret_data.get_multiple_P_attribute_value_of_read_variable(printf_result, var_name_P_values);
		}
	}

	return  printf_result;
}
#endif
//*****************************************************************************
// 
//*****************************************************************************

#define M_FATAL_MISSING_DATA()                                         \
	throw  C_byte_interpret_exception(M_WHERE, E_byte_interpret_exception_missing_data, "stop on error because nothing more to read")

//*****************************************************************************
// read the specified bit size of data
//*****************************************************************************

bool    read_data_bits (      T_frame_data  & in_out_frame_data,
                         void          * P_value_param,
                   const char          * TYPE_NAME,
                   const size_t          TYPE_BIT_SIZE,
                   const char          * TYPE_IMPL_STR,
                   const size_t          TYPE_IMPL_BIT_SIZE,
				   const bool            must_invert_bytes,
				   const bool            is_signed_integer)
{
	const size_t    TYPE_SIZE = (TYPE_BIT_SIZE / 8) + ((TYPE_BIT_SIZE % 8) ? 1 : 0);

	T_byte  * P_value_orig = (T_byte *)P_value_param;
	T_byte  * P_value = P_value_orig;
	if (TYPE_IMPL_BIT_SIZE > 8)
	{
		if (is_host_byte_order_inverted() == false)
		{
			P_value += (TYPE_IMPL_BIT_SIZE / 8) - TYPE_SIZE;
		}
	}

	if (TYPE_BIT_SIZE < 8)
	{
		*P_value = in_out_frame_data.read_less_1_byte(TYPE_BIT_SIZE);
#if 0
		if (is_signed_integer == false)
			return  true;

		M_FATAL_IF_LE(TYPE_BIT_SIZE, 1);

		// The bit sign is the higher bit.
		const T_byte    sign_mask = 1 << (TYPE_BIT_SIZE-1);
		if (*P_value & sign_mask)
		{
			// The value is signed.
			// I must set to 1 all the bits higher to the sign bit.
			// If sign is on the 4th bit :
			// sign_mask   is    00001000
			// sign_mask-1 gives 00000111
			// ^ 0xff      gives 11111000
			const T_byte    move_sign_mask = (sign_mask-1) ^ 0xff;
			*P_value |= move_sign_mask;
		}
#endif
	}
	else
	{
		in_out_frame_data.read_n_bits(TYPE_BIT_SIZE, P_value, TYPE_SIZE);

		if (must_invert_bytes)
		{
			T_byte  * P_swap = (T_byte *)P_value;
			for (size_t  idx = 0; idx < (TYPE_SIZE/2); ++idx)
				swap (P_swap[idx], P_swap[TYPE_SIZE-1-idx]);
		}
	}

	if (is_signed_integer == false)
		return  true;

	M_FATAL_IF_LE(TYPE_BIT_SIZE, 1);

#define M_READ_DATA_MOVE_SIGN(TYPE_IMPL)                                      \
		{                                                                     \
			TYPE_IMPL  * P_value_sign = (TYPE_IMPL *)P_value_param;           \
			/* The bit sign is the higher bit. */                             \
			const TYPE_IMPL    sign_mask = 1 << (TYPE_BIT_SIZE-1);            \
			if (*P_value_sign & sign_mask)                                    \
			{                                                                 \
				/* The value is signed.                                       \
				 * I must set to 1 all the bits higher to the sign bit.       \
				 * If sign is on the 4th bit :                                \
				 * sign_mask   is    00001000                                 \
				 * sign_mask-1 gives 00000111                                 \
				 * ^ 0xff      gives 11111000                                 \
				 */                                                           \
				const TYPE_IMPL    move_sign_mask = (sign_mask-1) ^ 0xffffffffffffffffLL;     \
				*P_value_sign |= move_sign_mask;                              \
			}                                                                 \
		}

#define M_READ_DATA_MOVE_SIGN_SIZE(TYPE_IMPL_BIT_SIZE_to_check,TYPE_IMPL)     \
	if (TYPE_IMPL_BIT_SIZE == TYPE_IMPL_BIT_SIZE_to_check)                    \
	{                                                                         \
		M_READ_DATA_MOVE_SIGN(TYPE_IMPL)                                      \
	}


	M_READ_DATA_MOVE_SIGN_SIZE(8, signed char)
	else M_READ_DATA_MOVE_SIGN_SIZE(16, signed short)
	else M_READ_DATA_MOVE_SIGN_SIZE(32, signed int)
	else M_READ_DATA_MOVE_SIGN_SIZE(64, signed long long)
	else
	{
		M_FATAL_COMMENT("Unexpected implementation bit size " << TYPE_IMPL_BIT_SIZE);
	}

	return  true;
}

//*****************************************************************************
// read the specified size of data
// - throw if there is NOT enough data to read
// - invert bytes readed if specified
//   Consequently, this function must be used only for integers or float.
// Returns true if ok.
// Returns false if NOT enough data.
//*****************************************************************************

bool    read_data (      T_frame_data  & in_out_frame_data,
                         void          * P_value_param,
                   const char          * TYPE_NAME,
                   const size_t          TYPE_BIT_SIZE,
                   const char          * TYPE_IMPL_STR,
                   const size_t          TYPE_IMPL_BIT_SIZE,
				   const bool            must_invert_bytes,
				   const bool            is_signed_integer)
{
    if (in_out_frame_data.can_move_bit_forward(TYPE_BIT_SIZE) != true)
    {
        M_STATE_FATAL ("Not enough bits ("
               << in_out_frame_data.get_remaining_bits() << " instead of " << TYPE_BIT_SIZE
               << ") for read " << TYPE_NAME
               << " (" << TYPE_IMPL_STR << ") !");
		return  false;
    }

	M_FATAL_IF_NE(TYPE_IMPL_BIT_SIZE % 8, 0);

	if ((TYPE_BIT_SIZE % 8) != 0)
	{
		return  read_data_bits(in_out_frame_data,
                   P_value_param, TYPE_NAME, TYPE_BIT_SIZE,
                   TYPE_IMPL_STR, TYPE_IMPL_BIT_SIZE,
				   must_invert_bytes,
				   is_signed_integer);
	}

	const size_t    TYPE_IMPL_SIZE = TYPE_IMPL_BIT_SIZE / 8;
	const size_t    TYPE_SIZE = TYPE_BIT_SIZE / 8;

	T_byte  * P_value_orig = (T_byte *)P_value_param;
	T_byte  * P_value = P_value_orig;
	if (TYPE_BIT_SIZE < TYPE_IMPL_BIT_SIZE)
	{
		if (is_host_byte_order_inverted() == false)
		{
			P_value += TYPE_IMPL_SIZE - TYPE_SIZE;
		}
	}

	in_out_frame_data.read_n_bytes(TYPE_SIZE, P_value);

    if (must_invert_bytes)
    {
        T_byte  * P_swap = (T_byte *)P_value;
        for (size_t  idx = 0; idx < (TYPE_SIZE/2); ++idx)
            swap (P_swap[idx], P_swap[TYPE_SIZE-1-idx]);
    }

	if (TYPE_BIT_SIZE >= TYPE_IMPL_BIT_SIZE)
		return  true;

	// only (u)int24 at this time

	if (is_signed_integer == false)
		return  true;

	// only int24 at this time

	T_byte  * P_where_sign_bit_is      = NULL;
	T_byte  * P_ff_begin = NULL;
	T_byte  * P_ff_end = NULL;

	// hope it is ok !
	if (is_host_byte_order_inverted())
	{
		P_where_sign_bit_is      = P_value      + (TYPE_SIZE - 1);
		P_ff_begin = P_value_orig + TYPE_SIZE;
		P_ff_end   = P_value_orig + TYPE_IMPL_SIZE;
	}
	else
	{
		P_where_sign_bit_is      = P_value;
		P_ff_begin = P_value_orig;
		P_ff_end   = P_value;
	}

	int    is_bit_sign_set = *P_where_sign_bit_is & 0x80;
	if (is_bit_sign_set)
	{
		for (T_byte  * P_ff = P_ff_begin; P_ff < P_ff_end; ++P_ff)
		{
			*P_ff = 0xff;
		}
	}

	return  true;
}

//*****************************************************************************
// read_data
//*****************************************************************************

void    read_data (const T_type_definitions  & type_definitions,
					     T_frame_data        & in_out_frame_data,
				   const T_interpret_data    & interpret_data,
				   const T_field_type_name   & field_type_name,
				   const string              & data_name,
				   const string              & data_simple_name,
                         void          * P_value_param,
                   const char          * TYPE_NAME,
                   const size_t          TYPE_BIT_SIZE,
                   const char          * TYPE_IMPL_STR,
                   const size_t          TYPE_IMPL_BIT_SIZE,
				   const bool            must_invert_bytes,
				   const bool            is_signed_integer)
{
	if (read_data (in_out_frame_data,
                   P_value_param, TYPE_NAME, TYPE_BIT_SIZE,
                   TYPE_IMPL_STR, TYPE_IMPL_BIT_SIZE,
				   must_invert_bytes,
				   is_signed_integer) != true)
	{
        interpret_builder_missing_data(type_definitions, in_out_frame_data, interpret_data,
			                    field_type_name, data_name, data_simple_name,
								"not enough data to read " + data_simple_name);
		M_FATAL_MISSING_DATA();
	}
}


//*****************************************************************************
// frame_to_function_base *****************************************************
//*****************************************************************************

enum E_return_value_indicator
{
	E_return_value_mandatory,
	E_return_value_forbidden,
	E_return_value_do_not_care
};

bool    frame_to_function_base (const T_type_definitions    & type_definitions,
					           T_interpret_data        & in_out_interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
						 const vector<T_expression>    & fct_parameters,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err,
							   E_return_value_indicator  return_value_indicator,
							   C_value                 & returned_value);


//*****************************************************************************
// frame_append_byte **********************************************************
//*****************************************************************************

void    frame_append_byte(T_decode_stream_frame  * P_decode_stream_frame,
						  unsigned char            byte)
{
	if (P_decode_stream_frame == NULL)
	{
		M_FATAL_COMMENT("frame_append_byte no data");
	}

	P_decode_stream_frame->write_1_byte(byte);
}

//*****************************************************************************
// frame_append_data **********************************************************
//*****************************************************************************

void    frame_append_data(T_decode_stream_frame  * P_decode_stream_frame,
						  long long                data,
						  int                      data_bit_size)
{
	M_STATE_ENTER ("frame_append_data", "data=" << data << "  data_bit_size=" << data_bit_size);

	if (P_decode_stream_frame == NULL)
	{
		M_FATAL_COMMENT("frame_append_data no data");
	}

	P_decode_stream_frame->write_less_1_byte(data, data_bit_size);		// ICIOA signed !
}

//*****************************************************************************
// frame_append_data **********************************************************
//*****************************************************************************

bool    frame_append_data (const T_type_definitions    & type_definitions,
					           T_frame_data            & in_out_frame_data,
					           T_interpret_data        & interpret_data,
                         const T_function_definition   & fct_def,
						 const T_field_type_name       & field_type_name,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER ("frame_append_data", "");

	if (field_type_name.fct_parameters.size() != 2)
	{
		M_FATAL_COMMENT("frame_append_data expect 2 parameters");
		return  false;
	}

	// Compute frame address.
	T_decode_stream_frame  * P_decode_stream_frame = NULL;
	{
		const C_value	 & frame_value = field_type_name.fct_parameters[0].compute_expression(
									   type_definitions, interpret_data, in_out_frame_data,
									   data_name, data_simple_name, os_out, os_err);
		if (frame_value.get_type() == C_value::E_type_integer)
		{
			P_decode_stream_frame = (T_decode_stream_frame*)frame_value.get_int();
		}
		else
		{
			M_FATAL_COMMENT("frame_append_data expect param1 = frame (= int)");
			return  false;
		}
	}

	// Compute value of byte.
	const C_value  & obj_value = field_type_name.fct_parameters[1].compute_expression(
								   type_definitions, interpret_data, in_out_frame_data,
								   data_name, data_simple_name, os_out, os_err);
	if (obj_value.get_type() == C_value::E_type_integer)
	{
//		M_STATE_DEBUG("obj_value.external_type=" << obj_value.get_external_type());
		M_FATAL_IF_EQ(obj_value.get_external_type_bit_size(), 0);

		frame_append_data(P_decode_stream_frame,
							obj_value.get_int(),
							obj_value.get_external_type_bit_size());  // ICIOA signe ???
	}
	else if ((obj_value.get_type() == C_value::E_type_string) &&
			 (obj_value.get_str().size() == 1))
	{
		frame_append_byte(P_decode_stream_frame, obj_value.get_str()[0]);
	}
	else
	{
		M_FATAL_COMMENT("frame_append_byte expect param2 = int (or string(1))");
		return  false;
	}

	return  true;
}

//*****************************************************************************
// frame_append_hexa_data *****************************************************
//*****************************************************************************

bool    frame_append_hexa_data (
						 const T_type_definitions      & type_definitions,
					           T_frame_data            & in_out_frame_data,
					           T_interpret_data        & interpret_data,
                         const T_function_definition   & fct_def,
						 const T_field_type_name       & field_type_name,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER ("frame_append_hexa_data", "");

	if (field_type_name.fct_parameters.size() != 2)
	{
		M_FATAL_COMMENT("frame_append_hexa_data expect 2 parameters");
		return  false;
	}

	// Compute frame address.
	T_decode_stream_frame  * P_decode_stream_frame = NULL;
	{
		const C_value  & frame_value = field_type_name.fct_parameters[0].compute_expression(
									   type_definitions, interpret_data, in_out_frame_data,
									   data_name, data_simple_name, os_out, os_err);
		if (frame_value.get_type() == C_value::E_type_integer)
		{
			P_decode_stream_frame = (T_decode_stream_frame*)frame_value.get_int();
		}
		else
		{
			M_FATAL_COMMENT("frame_append_hexa_data expect param1 = frame (= int)");
			return  false;
		}
	}

	// Compute value of byte.
	const C_value  & obj_value = field_type_name.fct_parameters[1].compute_expression(
								   type_definitions, interpret_data, in_out_frame_data,
								   data_name, data_simple_name, os_out, os_err);
	if (obj_value.get_type() == C_value::E_type_string)
	{
		string    hexa_str = obj_value.get_str();
		if (hexa_str.find(' ') != string::npos)
		{
			mod_replace_all(hexa_str, " ", "");
		}

		T_byte_vector    byte_vector;
		string_hexa_to_frame(hexa_str, byte_vector);

		for (T_byte_vector::const_iterator  iter  = byte_vector.begin();
											iter != byte_vector.end();
										  ++iter)
		{
			frame_append_byte(P_decode_stream_frame, *iter);
		}
	}
	else
	{
		M_FATAL_COMMENT("frame_append_hexa_data expect param2 = string with hexadecimal data");
		return  false;
	}

	return  true;
}

//*****************************************************************************
// decode_data_size
//*****************************************************************************

void    decode_data_size (
				   const T_type_definitions     & type_definitions,
					     T_frame_data           & in_out_frame_data,
						 T_interpret_data       & interpret_data,
				   const T_field_type_name      & field_type_name,
				   const string                 & data_name,
				   const string                 & data_simple_name,
                         ostream                & os_out,
                         ostream                & os_err,
                         T_decode_stream_frame  & decode_stream_frame,
                   const char                   * TYPE_NAME,
                   const int                      TYPE_BIT_SIZE)  // could be decorelated of TYPE_NAME
{
	M_STATE_ENTER ("decode_data_size", interpret_data.get_decode_function() << " " << TYPE_BIT_SIZE);

	C_interpret_decode_in_progress  idip(interpret_data);

	const long    bit_offset_into_initial_frame = in_out_frame_data.get_bit_offset_into_initial_frame();
	const long    remaining_bits = decode_stream_frame.frame_data.get_remaining_bits();

	const T_function_definition  & fct_def = type_definitions.get_function(interpret_data.get_decode_function());

	vector<T_expression>     fct_parameters(2);
	// Memory address of the frame (cast to avoid hexa number).
	fct_parameters[0].build_expression(C_value((long long)&decode_stream_frame));
	// Bit size requested.
	fct_parameters[1].build_expression(C_value(TYPE_BIT_SIZE));

	C_value    return_value_do_not_care;
	bool  result = frame_to_function_base (type_definitions,
							   interpret_data,
							   in_out_frame_data,
							   fct_def,
							   fct_parameters,
							   data_name,
							   data_simple_name,
							   os_out,
							   os_err,
							   E_return_value_do_not_care,
							   return_value_do_not_care);
	if (result == false)
	{
		M_FATAL_COMMENT("frame_to_function_base");
	}

	// Check that the decoded size is >= at the asked size
	const long	decoded_bits = decode_stream_frame.frame_data.get_remaining_bits() - remaining_bits;
	if (decoded_bits < TYPE_BIT_SIZE)
	{
		M_FATAL_COMMENT("decoded_bits(" <<
			decode_stream_frame.decoded_data_bit_size << ") < TYPE_BIT_SIZE(" <<
			TYPE_BIT_SIZE << ")");
	}

	decode_stream_frame.frame_data.set_initial_frame_starting_bit_offset(bit_offset_into_initial_frame);
}

//*****************************************************************************
// decode_data_bytes_until
//*****************************************************************************

void    decode_data_bytes_until (
				   const T_type_definitions     & type_definitions,
					     T_frame_data           & in_out_frame_data,
						 T_interpret_data       & interpret_data,
				   const T_field_type_name      & field_type_name,
				   const string                 & data_name,
				   const string                 & data_simple_name,
                         ostream                & os_out,
                         ostream                & os_err,
                         T_decode_stream_frame  & decode_stream_frame,
                   const char                   * TYPE_NAME,
                   const char                   * p_ending_char_1,
				   const char                   * p_ending_char_2)
{
	M_STATE_ENTER ("decode_data_bytes_until", interpret_data.get_decode_function());

	C_interpret_decode_in_progress  idip(interpret_data);

	long    bit_offset_into_initial_frame = in_out_frame_data.get_bit_offset_into_initial_frame();

	const T_function_definition  & fct_def = type_definitions.get_function(interpret_data.get_decode_function());

	vector<T_expression>     fct_parameters(2);
	// Memory address of the frame (cast to avoid hexa number).
	fct_parameters[0].build_expression(C_value((long long)&decode_stream_frame));
	// Bit size requested.
	fct_parameters[1].build_expression(C_value(8));

	C_value    return_value_do_not_care;

	while (in_out_frame_data.get_remaining_bits() >= 8)
	{
		const long  current_decoded_data_bit_size = decode_stream_frame.decoded_data_bit_size;

		bool  result = frame_to_function_base (type_definitions,
								   interpret_data,
								   in_out_frame_data,
								   fct_def,
								   fct_parameters,
								   data_name,
								   data_simple_name,
								   os_out,
								   os_err,
								   E_return_value_do_not_care,
								   return_value_do_not_care);
		if (result == false)
		{
			M_FATAL_COMMENT("frame_to_function_base");
		}

		// Check that the decoded size is >= at the asked size
		const long	decoded_bits = decode_stream_frame.decoded_data_bit_size - current_decoded_data_bit_size;
		if (decoded_bits < 8)
		{
			M_FATAL_COMMENT("decoded_bits(" <<
				decoded_bits << ") < 8");
		}

		if (p_ending_char_1 != NULL)
		{
			if (decode_stream_frame.decoded_data[(decode_stream_frame.decoded_data_bit_size / 8) - 1] == *p_ending_char_1)
			{
				// Final character is found.
				break;
			}
		}
		if (p_ending_char_2 != NULL)
		{
			if (decode_stream_frame.decoded_data[(decode_stream_frame.decoded_data_bit_size / 8) - 1] == *p_ending_char_2)
			{
				// Final character is found.
				break;
			}
		}
	}

	decode_stream_frame.frame_data.set_initial_frame_starting_bit_offset(bit_offset_into_initial_frame);
}

//*****************************************************************************
// read_decode_data
//*****************************************************************************

void    read_decode_data (
				   const T_type_definitions  & type_definitions,
					     T_frame_data        & in_out_frame_data_param,
				         T_interpret_data    & interpret_data,
				   const T_field_type_name   & field_type_name,
				   const string              & data_name,
				   const string              & data_simple_name,
                         ostream                 & os_out,
                         ostream                 & os_err,
                         void          * P_value_param,
                   const char          * TYPE_NAME,
                   const size_t          TYPE_BIT_SIZE,
                   const char          * TYPE_IMPL_STR,
                   const size_t          TYPE_IMPL_BIT_SIZE,
				   const bool            must_invert_bytes,
				   const bool            is_signed_integer)
{
	T_frame_data           * P_in_out_frame_data = & in_out_frame_data_param;

	if (interpret_data.is_decode_in_progress() == false)
	{
		T_decode_stream_frame  & decode_stream_frame = interpret_data.get_decode_stream_frame();
		T_frame_data           & inside_frame = decode_stream_frame.frame_data;
		if (inside_frame.get_remaining_bits() >= TYPE_BIT_SIZE)
		{
			// Enough data into inside_frame, so simply use it (decoder is useless for now)
			P_in_out_frame_data = & inside_frame;
		}
		else if (interpret_data.must_decode_now() == false)
		{
			if (inside_frame.get_remaining_bits() > 0)
			{
				// For code simplification, do not manage this case.
				// And seems non-sense : 1st part of data is decoded and 2nd part no.
				M_FATAL_COMMENT("Not enough data into inside_frame (" << inside_frame.get_remaining_bits() << " bits < " <<
								TYPE_BIT_SIZE << ") and no decoder to fill it");
			}
			else
			{
				// No data into inside_frame and no decoder, so simply use normal frame
			}
		}
		else
		{
			// Not enough data into inside_frame, so use decoder to fill it
			M_STATE_ENTER ("read_decode_data must_decode_stream", TYPE_BIT_SIZE);

			decode_data_size (type_definitions, in_out_frame_data_param, interpret_data,
							  field_type_name, data_name, data_simple_name,
							  os_out, os_err,
							  decode_stream_frame,
							  TYPE_NAME, TYPE_BIT_SIZE - inside_frame.get_remaining_bits());

			P_in_out_frame_data = & inside_frame;
		}
	}
	else
	{
		// inside_frame and decoder not used during decoding
	}


	T_frame_data          & in_out_frame_data = * P_in_out_frame_data;

	read_data (    type_definitions, in_out_frame_data, interpret_data,
                   field_type_name, data_name, data_simple_name,
                   P_value_param, TYPE_NAME, TYPE_BIT_SIZE,
                   TYPE_IMPL_STR, TYPE_IMPL_BIT_SIZE,
				   must_invert_bytes,
				   is_signed_integer);

	interpret_data.get_decode_stream_frame().synchronize();  // mandatory each time something has been read into its frame_data
}

//*****************************************************************************
// Forward declarations.
//*****************************************************************************

bool    frame_to_any (const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err);

bool    frame_to_fields (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_struct_fields       & struct_fields,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err);

bool    frame_to_struct_inline (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_struct_definition   & struct_definition,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err);

bool    frame_to_bitfield_inline (const T_type_definitions    & type_definitions,
					           T_frame_data            & in_out_frame_data,
					           T_interpret_data        & interpret_data,
                         const T_bitfield_definition   & bitfield_definition,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err);

bool    frame_to_switch (const T_type_definitions      & type_definitions,
					           T_frame_data            & in_out_frame_data,
					           T_interpret_data        & interpret_data,
                         const T_switch_definition     & switch_def,
						 const string                  & discriminant_str,
						 const T_field_type_name       & field_type_name,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err);

//*****************************************************************************
// frame_to_field *************************************************************
//*****************************************************************************

void    build_types_finalize_itself(const T_type_definitions  & type_definitions,
							        const T_field_type_name   & field_type_name);

//*****************************************************************************
// frame_to_field_deep_break **************************************************
//*****************************************************************************

bool    frame_to_field_deep_break(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_deep_break", "");

	throw  C_byte_interpret_exception_loop(M_WHERE, E_byte_interpret_exception_loop_deep_break, "deep_break called outside any loop");

	return  true;
}

//*****************************************************************************
// frame_to_field_deep_continue ***********************************************
//*****************************************************************************

bool    frame_to_field_deep_continue(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_deep_continue", "");

	throw  C_byte_interpret_exception_loop(M_WHERE, E_byte_interpret_exception_loop_deep_continue, "deep_continue called outside any loop");

	return  true;
}

//*****************************************************************************
// frame_to_field_break *******************************************************
//*****************************************************************************

bool    frame_to_field_break(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_break", "");

	throw  C_byte_interpret_exception_loop(M_WHERE, E_byte_interpret_exception_loop_break, "break called outside a loop");

	return  true;
}

//*****************************************************************************
// frame_to_field_continue ****************************************************
//*****************************************************************************

bool    frame_to_field_continue(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_continue", "");

	throw  C_byte_interpret_exception_loop(M_WHERE, E_byte_interpret_exception_loop_continue, "continue called outside a loop");

	return  true;
}

//*****************************************************************************
// frame_to_field_return ******************************************************
//*****************************************************************************

bool    frame_to_field_return(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_return", "data_name=" << data_name);

	const T_expression  & return_expression = field_type_name.get_return_expression();

	if (return_expression.is_defined() == false)
	{
		// No return value.
		throw  C_byte_interpret_exception_return(M_WHERE, E_byte_interpret_exception_return, "return called outside a struct");
	}

	// Return value.
	throw  C_byte_interpret_exception_return(M_WHERE,
											 E_byte_interpret_exception_return,
											 "return called outside a struct",
											 return_expression.compute_expression(type_definitions, interpret_data, in_out_frame_data,
																data_name, data_simple_name, os_out, os_err));


	return  true;
}

//*****************************************************************************
// frame_to_field_while *******************************************************
//*****************************************************************************

bool    frame_to_field_while(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_while", "data_name=" << data_name);

    const string  & type = field_type_name.type;

	const T_expression  & condition_expression = field_type_name.get_condition_expression();

	string    new_data_name = data_name;
	if (new_data_name != "")
        new_data_name += K_ATTRIBUTE_SEPARATOR;

	bool      must_not_test_1st_time = (type == "do_while");
    size_t    idx_while = 0;
    while (must_not_test_1st_time ||
		   condition_expression.compute_expression(type_definitions, interpret_data, in_out_frame_data,
							  data_name, data_simple_name, os_out, os_err).get_bool ())
    {
		must_not_test_1st_time = false;

		try {
			if (frame_to_struct_inline (type_definitions,
								 in_out_frame_data,
								 interpret_data,
								*field_type_name.P_sub_struct,
								 get_array_idx_name (new_data_name,
													 idx_while),
								 "",
								 os_out,
								 os_err) != true)
			{
				os_err << "Error field data= " << data_name << endl;
			}
		}
		catch(C_byte_interpret_exception_loop  & val)
		{
			if ((val.get_cause() == E_byte_interpret_exception_loop_break) ||
				(val.get_cause() == E_byte_interpret_exception_loop_deep_break))
				break;
			// nothing to do on continue
		}
        ++idx_while;
    }

	return  true;
}

//*****************************************************************************
// frame_to_field_loop_size ***************************************************
//*****************************************************************************

bool    frame_to_field_loop_size(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_loop_size", "data_name=" << data_name);

    const string  & type = field_type_name.type;

	const T_expression  & condition_expression = field_type_name.get_condition_expression();

	string    new_data_name = data_name;
	if (new_data_name != "")
        new_data_name += K_ATTRIBUTE_SEPARATOR;

	long long   bit_size_to_reach = condition_expression.compute_expression(type_definitions, interpret_data,
													   in_out_frame_data,
													   data_name, data_simple_name,
													   os_out, os_err).get_int ();
	if (type == "loop_size_bytes")
	{
		bit_size_to_reach *= 8;
	}

	T_frame_data    orig_frame_data = in_out_frame_data;
    size_t          idx_while = 0;
    while ((in_out_frame_data.get_bit_offset() - orig_frame_data.get_bit_offset()) < bit_size_to_reach)
    {
		try {
			if (frame_to_struct_inline (type_definitions,
								 in_out_frame_data,
								 interpret_data,
								*field_type_name.P_sub_struct,
								 get_array_idx_name (new_data_name,
													 idx_while),
								 "",
								 os_out,
								 os_err) != true)
			{
				os_err << "Error field data= " << data_name << endl;
			}
		}
		catch(C_byte_interpret_exception_loop  & val)
		{
			if ((val.get_cause() == E_byte_interpret_exception_loop_break) ||
				(val.get_cause() == E_byte_interpret_exception_loop_deep_break))
				break;
			// nothing to do on continue
		}
        ++idx_while;
    }


	return  true;
}

//*****************************************************************************
// frame_to_field_loop_nb_times ***********************************************
//*****************************************************************************

bool    frame_to_field_loop_nb_times(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_loop_nb_times", "data_name=" << data_name);

	const T_expression  & condition_expression = field_type_name.get_condition_expression();

	string    new_data_name = data_name;
	if (new_data_name != "")
		new_data_name += K_ATTRIBUTE_SEPARATOR;

	long long       nb_times = condition_expression.compute_expression(type_definitions, interpret_data, in_out_frame_data,
												  data_name, data_simple_name, os_out, os_err).get_int ();
	size_t          idx_while = 0;
	while (idx_while < nb_times)
	{
		try {
			if (frame_to_struct_inline (type_definitions,
								 in_out_frame_data,
								 interpret_data,
								*field_type_name.P_sub_struct,
								 get_array_idx_name (new_data_name,
													 idx_while),
								 "",
								 os_out,
								 os_err) != true)
			{
				os_err << "Error field data= " << data_name << endl;
			}
		}
		catch(C_byte_interpret_exception_loop  & val)
		{
			if ((val.get_cause() == E_byte_interpret_exception_loop_break) ||
				(val.get_cause() == E_byte_interpret_exception_loop_deep_break))
				break;
			// nothing to do on continue
		}
		++idx_while;
	}

	return  true;
}

//*****************************************************************************
// frame_to_field_if **********************************************************
//*****************************************************************************

bool    frame_to_field_if(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_if", "data_name=" << data_name);

	const T_expression  & condition_expression = field_type_name.get_condition_expression();

    if (condition_expression.compute_expression(type_definitions, interpret_data, in_out_frame_data,
						   data_name, data_simple_name, os_out, os_err).get_bool ())
    {
        if (frame_to_struct_inline (type_definitions,
                             in_out_frame_data,
							 interpret_data,
                            *field_type_name.P_sub_struct,
                             data_name,
                             "",
                             os_out,
                             os_err) != true)
        {
            os_err << "Error field data= " << data_name << endl;
        }
    }
    else if (field_type_name.sub_struct_2.empty () == false)
    {
        if (frame_to_fields (type_definitions,
                             in_out_frame_data,
							 interpret_data,
                             field_type_name.sub_struct_2,
                             data_name,
                             "",
                             os_out,
                             os_err) != true)
        {
            os_err << "Error field data= " << data_name << endl;
        }
    }


	return  true;
}

//*****************************************************************************
// frame_to_field_other *******************************************************
//*****************************************************************************

bool    frame_to_field_other(
						 const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field_other", "data_name=" << data_name);

    const string  & type = field_type_name.type;
    const string  & name = field_type_name.name;

	M_STATE_DEBUG ("type=" << type << "  name=" << name);

	string    new_data_name = data_name;

	{
        if (name != "")
        {
            if (new_data_name != "")
                new_data_name += K_ATTRIBUTE_SEPARATOR;
            new_data_name += name;
        }

		// Compute all arrays size and total size.
#define K_ARRAY_MAGIC_NUMBER  50
		long    array_sizes[K_ARRAY_MAGIC_NUMBER];
		long    array_indexes[K_ARRAY_MAGIC_NUMBER];
		long    number_of_elements = 1;
		long    number_of_elements_before_LONG_MAX = 1;
		bool    LONG_MAX_is_plus = false;

		const bool    is_an_array = field_type_name.is_an_array();
		const int     number_of_arrays = field_type_name.str_arrays.size();

		for (int   array_idx = 0; array_idx < number_of_arrays; ++array_idx)
		{
			const T_field_type_name::T_array  & field_array = field_type_name.str_arrays[array_idx];
	        long                                array_size = -1;

			if (field_array.size_type != T_field_type_name::T_array::E_size_normal)
			{
				if (array_idx != (number_of_arrays - 1))
				{
					// NB: already tested into build_field
					M_FATAL_COMMENT("[*] or [+] are only accepted on the last array level");
				}

				array_size = LONG_MAX;
				if (field_array.size_type == T_field_type_name::T_array::E_size_unknow_at_least_1)
				{
					LONG_MAX_is_plus = true;
				}
			}
			else
			{
				// Compute the array size.
				const C_value  & value = field_array.size_expression.compute_expression(
					                                  type_definitions, interpret_data, in_out_frame_data,
													  data_name, data_simple_name, os_out, os_err);

				if (value.get_type () == C_value::E_type_integer)
				{
					array_size = static_cast<long>(value.get_int ());
				}

				if (array_size < 0)  /* taille variable == 0 OK */
				{
					const string  & str_array_size = field_array.size_expression.get_original_string_expression();
					os_err << "Error array size= " << str_array_size
						   << " data= " << data_name << endl;
					interpret_builder_error(type_definitions, in_out_frame_data,
											field_type_name, data_name, data_simple_name,
											"Error array size= " + str_array_size + " for " + data_simple_name);
					return  false;
				}
			}

			array_sizes[array_idx] = array_size;
			array_indexes[array_idx] = 0;
			if (array_size == LONG_MAX)
			{
				if (number_of_elements == LONG_MAX)
				{
					// NB: already tested into build_field and before in this loop
					M_FATAL_COMMENT("Could not have 2 [*] or [+] in multiple array");
				}
				number_of_elements_before_LONG_MAX = number_of_elements;
				number_of_elements = LONG_MAX;
			}
			else
			{
				number_of_elements *= array_size;
				number_of_elements_before_LONG_MAX = number_of_elements;
			}

			M_STATE_DEBUG("[" << array_idx << "]  " <<
						  "array_size=" << array_sizes[array_idx] << "  " <<
						  "number_of_elements=" << number_of_elements);

			if (array_size == 0)
			{
				number_of_elements = 0;  // in case it was LONG_MAX
				break;
			}
		}

		// Read all elements of all arrays (1 elt if no array).
		for (long  element_idx = 0; element_idx < number_of_elements; ++element_idx)
		{
			const long    sizeof_bits = in_out_frame_data.get_remaining_bits();

			// Compute data_simple_array_name
			string  new_data_array_name    = new_data_name;
			string  data_simple_array_name = field_type_name.name;
			for (int   array_idx = 0; array_idx < number_of_arrays; ++array_idx)
			{
				new_data_array_name += "[";
				new_data_array_name += get_string(array_indexes[array_idx]);
				new_data_array_name += "]";
				data_simple_array_name += "[";
				data_simple_array_name += get_string(array_indexes[array_idx]);
				data_simple_array_name += "]";
			}

			// Check remaining data size
			// Only for array and not for variable.
			// Only if no decode or decode in progress.   2011/03/31
			if ((sizeof_bits <= 0) &&
				(is_an_array == true) &&
				(field_type_name.is_a_variable() == false) &&
				((interpret_data.must_decode() == false) ||
				 (interpret_data.is_decode_in_progress() == true)))
			{
				// Error except if * or +
				if ((number_of_elements == LONG_MAX) &&
					((element_idx % number_of_elements_before_LONG_MAX) == 0))
				{
					if ((LONG_MAX_is_plus == false) || (element_idx > 0))
					{
						break;
					}
				}

				// Triggers invalid error on array of struct containing only variables.  2013/10/06
#if 0
				os_err << "Error array"
                       << " data= " << data_simple_array_name
                       << " : no more data to read" << endl;
				interpret_builder_missing_data(type_definitions, in_out_frame_data, interpret_data,
										field_type_name, new_data_array_name, data_simple_array_name,
										"End array : no more data to read " + data_simple_array_name);
				M_FATAL_MISSING_DATA();
                return  false;
#endif
			}

// ICIOA il faut supprimer les tests fait dans C_byte_interpret_wsgd_builder::group_begin ?
			bool  is_a_group_and_not_inline = true;
			if ((field_type_name.basic_type_bit_size >= 0) ||        // known size = not a group

				// Blocks which are not VISUALLY groups
				(field_type_name.name == "") ||                      // inline
				(field_type_name.type == "call") ||                  // function (ie inline)
//				(field_type_name.type == "if") ||
//				(field_type_name.type == "while") ||
//				(field_type_name.type == "do") ||
//				(field_type_name.type == "do_while") ||
//				(field_type_name.type == "loop_size_bytes") ||
//				(field_type_name.type == "loop_size_bits") ||
//				(field_type_name.type == "loop_nb_items") ||

				(field_type_name.type == "msg") ||

				// Commands
				(field_type_name.type == "set") ||
				(field_type_name.type == "save_position") ||
				(field_type_name.type == "goto_position") ||
				(field_type_name.type == "move_position_bytes") ||
				(field_type_name.type == "move_position_bits") ||
				(field_type_name.type == "chrono") ||
				(field_type_name.type == "byte_order") ||
				(field_type_name.type == "debug_print") ||
				(field_type_name.type == "print") ||
				(field_type_name.type == "chat") ||
				(field_type_name.type == "note") ||
				(field_type_name.type == "warning") ||
				(field_type_name.type == "error") ||
				(field_type_name.type == "fatal") ||
				(field_type_name.type == "check_eof_distance_bytes") ||
				(field_type_name.type == "check_eof_distance_bits") ||
				(field_type_name.type == "output") ||
				
				// Data (without known size) which are not group
				(field_type_name.type == "padding_bits") ||
				(field_type_name.type == "raw") ||
				(field_type_name.type == "subproto") ||
				(field_type_name.type == "insproto") ||
				(field_type_name.type == "string") ||
				(field_type_name.type == "string_nl"))
// ICIOA systeme A REVOIR
			{
				is_a_group_and_not_inline = false;
			}

			if (is_a_group_and_not_inline)
			{
				interpret_data.read_variable_group_begin(data_simple_array_name);    // ivvector

				if (interpret_data.must_output ())
				{
					interpret_builder_group_begin (type_definitions,
												in_out_frame_data,
												field_type_name,
												new_data_array_name /*data_name*/,
												data_simple_array_name /*field_type_name.name*/ /*data_simple_name*/);
				}
			}

			bool                               is_exception_loop_catched = false;
			C_byte_interpret_exception_loop       exception_loop_catched;
			bool                               is_exception_return_catched = false;
			C_byte_interpret_exception_return     exception_return_catched;

			try
			{
				if (type == "struct")
				{
					// Inline struct
					if (frame_to_struct_inline (type_definitions,
										 in_out_frame_data,
										 interpret_data,
										*field_type_name.P_sub_struct,
										 new_data_array_name /*new_data_name*/,
										 data_simple_array_name /*field_type_name.name*/,
										 os_out,
										 os_err) != true)
					{
						os_err << "Error field data= " << data_name << endl;
					}
				}
				else if (type == "frame_bytes")
				{
					if (in_out_frame_data.is_physically_at_beginning_of_byte() != true)
					{
						M_FATAL_COMMENT(type << " managed only a entire byte position");
						// must change T_frame_data interface
					}

					// Remaining size.
					long    size_bytes = in_out_frame_data.get_remaining_entire_bytes();
					if (field_type_name.str_size_or_parameter != "")
					{
						// Size specified.
						C_value    value = compute_expression(type_definitions, interpret_data, in_out_frame_data,
															  field_type_name.str_size_or_parameter,
															  data_name, data_simple_name, os_out, os_err);
						size_bytes = value.get_int();
					}

					T_frame_data    frame_data(in_out_frame_data.get_P_bytes(), 0, size_bytes * 8);
					frame_data.set_initial_frame_starting_bit_offset(in_out_frame_data.get_bit_offset_into_initial_frame());

					// Inline frame_bytes
					if (frame_to_struct_inline (type_definitions,
										 frame_data,
										 interpret_data,
										*field_type_name.P_sub_struct,
										 new_data_array_name /*new_data_name*/,
										 data_simple_array_name /*field_type_name.name*/,
										 os_out,
										 os_err) != true)
					{
						os_err << "Error field data= " << data_name << endl;
					}

					// Bit size effectively read.
					long    size_bits = frame_data.get_bit_offset();
					if (field_type_name.str_size_or_parameter != "")
					{
						// Size specified.
						size_bits = size_bytes * 8;
					}

					in_out_frame_data.move_bit_forward(size_bits);
				}
				else if (type == "bitfield")
				{
					// Inline bitfield
					if (frame_to_bitfield_inline (type_definitions,
										 in_out_frame_data,
										 interpret_data,
										*field_type_name.P_bitfield_inline,
										 new_data_array_name /*new_data_name*/,
										 data_simple_array_name /*field_type_name.name*/,
										 os_out,
										 os_err) != true)
					{
						os_err << "Error field data= " << data_name << endl;
					}
				}
				else if (type == "switch")
				{
					// Inline switch
//					if (frame_to_switch_inline (type_definitions,
					if (frame_to_switch (type_definitions,
										 in_out_frame_data,
										 interpret_data,
										*field_type_name.P_switch_inline,
										 field_type_name.str_size_or_parameter,
										 field_type_name,
										 new_data_array_name /*new_data_name*/,
										 data_simple_array_name /*field_type_name.name*/,
										 os_out,
										 os_err) != true)
					{
						os_err << "Error field data= " << data_name << endl;
					}
				}
				else if (frame_to_any (type_definitions,
								  in_out_frame_data,
								  interpret_data,
								  field_type_name,
								  new_data_array_name /*new_data_name*/,
								  data_simple_array_name /*field_type_name.name*/,
								  os_out,
								  os_err) != true)
				{
					os_err << "Error field data= " << data_name << endl;
				}
			}
			catch(C_byte_interpret_exception_loop  & val)
			{
				is_exception_loop_catched = true;
				exception_loop_catched = val;
				// nothing to do here
			}
			catch(C_byte_interpret_exception_return  & val)
			{
				is_exception_return_catched = true;
				exception_return_catched = val;
				// nothing to do here
			}

			if (is_a_group_and_not_inline)
			{
				interpret_data.read_variable_group_end();    // ivvector

				if (interpret_data.must_output ())
				{
					interpret_builder_group_end (type_definitions,
												in_out_frame_data,
												field_type_name,
												new_data_array_name /*data_name*/,
												data_simple_array_name /*field_type_name.name*/ /*data_simple_name*/,
												sizeof_bits - in_out_frame_data.get_remaining_bits());
				}
			}

			if (is_exception_loop_catched)
				throw  exception_loop_catched;

			if (is_exception_return_catched)
				throw  exception_return_catched;

			// Compute new indexes for next element.
			for (int   array_idx = 0; array_idx < number_of_arrays; ++array_idx)
			{
				++array_indexes[array_idx];
				if ((array_indexes[array_idx] % array_sizes[array_idx]) != 0)
				{
					break;
				}
				array_indexes[array_idx] = 0;
			}
		}
    }

    return  true;
}

//*****************************************************************************
// frame_to_field *************************************************************
//*****************************************************************************

bool    frame_to_field  (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_field_type_name     & field_type_name,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_field", "data_name=" << data_name);

	if (field_type_name.pf_frame_to_field == NULL)
	{
		// Normally set at initialisation time for all types into type_definitions.
		// But some field_type_name could be created on the fly.
		build_types_finalize_itself(type_definitions, field_type_name);

		if (field_type_name.pf_frame_to_field == NULL)
		{
			//-------------------------------------------------------------------------
			// Error
			//-------------------------------------------------------------------------
			interpret_builder_error(type_definitions, in_out_frame_data,
									field_type_name, data_name, data_simple_name,
									"Not valid type " + field_type_name.type);
			return  false;
		}
	}

	return  field_type_name.pf_frame_to_field(type_definitions,
											   in_out_frame_data,
											   interpret_data,
											   field_type_name,
											   data_name,
											   data_simple_name,
											   os_out, os_err);
}

//*****************************************************************************
// frame_to_fields ************************************************************
//*****************************************************************************

bool    frame_to_fields (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
							   T_interpret_data      & interpret_data,
                         const T_struct_fields       & struct_fields,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_fields", "");

	for (T_struct_fields::size_type  idx = 0;
                                     idx < struct_fields.size ();
                                   ++idx)
    {
		if (frame_to_field (type_definitions,
                            in_out_frame_data,
							interpret_data,
							struct_fields[idx],
							data_name,
							data_simple_name,
							os_out,
							os_err) != true)
		{
            os_err << "Error struct idx= " << idx
                   << " data= " << data_name << endl;
		}
	}

    return  true;
}

//*****************************************************************************
// frame_to_struct_base *******************************************************
//*****************************************************************************

bool    frame_to_struct_base (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
					           T_interpret_data      & interpret_data,
                         const T_struct_definition   & struct_definition,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err,
							   bool                    must_catch_return)
{
	M_STATE_ENTER ("frame_to_struct_base", "must_catch_return=" << must_catch_return);

	bool    result = true;

	try
	{
		result = frame_to_fields(type_definitions, in_out_frame_data, interpret_data,
							struct_definition.fields,
							data_name, data_simple_name,
                            os_out, os_err);
	}
	catch (C_byte_interpret_exception_normal  & val)
	{
//		catched_exception = val;        // slicing pb
//		P_catched_exception = &val;     // scope pb

#if 1
		// Even with a normal exception, I try to print the text.
		// But it is possible (because of break, continue, return ...)
		//  that the data to print is not available
		// -> fatal
		// It is not my problem (at least for now) : the user must manage it.
		// Other possibilities are :
		// - do not print the text (but I do not see why)
		// - print and catch possible exception (and warn the user)
		if ((struct_definition.printf_args != "") && (interpret_data.must_output ()))
		{
			const C_value   printf_result_value = printf_args_to_string(type_definitions, interpret_data, in_out_frame_data,
																		struct_definition.printf_args,
																	    data_name, data_simple_name, os_out, os_err);
			const string    printf_result = printf_result_value.as_string();

			interpret_builder_group_append_text(type_definitions, in_out_frame_data,
												data_name, data_simple_name,
												printf_result);
		}
#endif
		if ((must_catch_return != true) ||                 // inline struct
			(val.get_cause() == E_byte_interpret_exception_loop_deep_break) ||
			(val.get_cause() == E_byte_interpret_exception_loop_deep_continue))
		{
			throw;    // re-throw the exception
		}

		// We are inside a normal struct, ie not inline,
		//  so ...
		if (val.get_cause() == E_byte_interpret_exception_return)
		{
			// return exception stops here : ok
			return  true;
		}
		if ((val.get_cause() == E_byte_interpret_exception_loop_break) ||
			(val.get_cause() == E_byte_interpret_exception_loop_continue))
		{
			// loop_break and loop_continue exceptions stop here : NOT ok
			//  supposed to be catched before -> fatal
			M_FATAL_COMMENT(val.get_explanation());
		}

		throw;    // re-throw the other exceptions
	}

	if ((struct_definition.printf_args != "") && (interpret_data.must_output ()))
	{
		const C_value   printf_result_value = printf_args_to_string(type_definitions, interpret_data, in_out_frame_data,
																	struct_definition.printf_args,
																	data_name, data_simple_name, os_out, os_err);
		const string    printf_result = printf_result_value.as_string();

		interpret_builder_group_append_text(type_definitions, in_out_frame_data,
											data_name, data_simple_name,
											printf_result);
	}

	return  result;
}

//*****************************************************************************
// frame_to_struct_inline *****************************************************
//*****************************************************************************

bool    frame_to_struct_inline (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
							   T_interpret_data      & interpret_data,
                         const T_struct_definition   & struct_definition,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	return  frame_to_struct_base(type_definitions,
								   in_out_frame_data,
								   interpret_data,
								   struct_definition,
								   data_name, data_simple_name,
								   os_out, os_err,
								   false /* must_catch_return */);
}

//*****************************************************************************
// frame_to_struct ************************************************************
//*****************************************************************************

bool    frame_to_struct (const T_type_definitions    & type_definitions,
					           T_frame_data          & in_out_frame_data,
							   T_interpret_data      & interpret_data,
                         const T_struct_definition   & struct_definition,
                         const string                & data_name,
                         const string                & data_simple_name,
                               ostream               & os_out,
                               ostream               & os_err)
{
	return  frame_to_struct_base(type_definitions,
								   in_out_frame_data,
								   interpret_data,
								   struct_definition,
								   data_name, data_simple_name,
								   os_out, os_err,
								   true /* must_catch_return */);
}

//*****************************************************************************
// frame_to_bitfield_inline ***************************************************
//*****************************************************************************

bool    frame_to_bitfield_inline (const T_type_definitions    & type_definitions,
					           T_frame_data            & in_out_frame_data,
					           T_interpret_data        & interpret_data,
                         const T_bitfield_definition   & bitfield_definition,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER ("frame_to_bitfield_inline", "");

	bool                is_little_endian = interpret_data.is_little_endian();

	if (bitfield_definition.is_a_bitstream && is_little_endian)
	{
		interpret_data.set_big_endian();
	}

    if (frame_to_any (type_definitions,
		                 in_out_frame_data,
						 interpret_data,
						 bitfield_definition.master_field,
                         data_name,
                       /*  data_simple_name, */
						 bitfield_definition.master_field.name,
                         os_out, os_err) != true)
    {
        os_err << "Error bitfield master_field"
               << " data= " << data_name << endl;
        return  false;
    }

	if (bitfield_definition.is_a_bitstream && is_little_endian)
	{
		interpret_data.set_little_endian();
	}

    if (frame_to_fields (type_definitions,
		                 in_out_frame_data,
						 interpret_data,
						 bitfield_definition.fields_definition,
                         data_name,
                         data_simple_name,
                         os_out, os_err) != true)
    {
        os_err << "Error bitfield fields_definition"
               << " data= " << data_name << endl;
        return  false;
    }

	if (interpret_data.must_output ())
	{
		const C_value   printf_result_value = printf_args_to_string(type_definitions, interpret_data, in_out_frame_data,
																	"(\"  %s\", last_bitfield_value)",
																	data_name, data_simple_name, os_out, os_err);
		const string    printf_result = printf_result_value.as_string();

		interpret_builder_group_append_text(type_definitions, in_out_frame_data,
											data_name, data_simple_name,
											printf_result);
	}

	return  true;
}

//*****************************************************************************
// frame_to_bitfield **********************************************************
//*****************************************************************************

bool    frame_to_bitfield (const T_type_definitions    & type_definitions,
					           T_frame_data            & in_out_frame_data,
							   T_interpret_data        & interpret_data,
                         const T_bitfield_definition   & bitfield_definition,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	return  frame_to_bitfield_inline(type_definitions,
								   in_out_frame_data,
								   interpret_data,
								   bitfield_definition,
								   data_name, data_simple_name,
								   os_out, os_err);
}

//*****************************************************************************
// frame_to_switch ************************************************************
//*****************************************************************************

bool    frame_to_switch (const T_type_definitions      & type_definitions,
					           T_frame_data            & in_out_frame_data,
					           T_interpret_data        & interpret_data,
                         const T_switch_definition     & switch_def,
						 const string                  & discriminant_str,
						 const T_field_type_name       & field_type_name,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER ("frame_to_switch", "");

	const T_switch_case        & switch_case = switch_def.switch_case;

	C_value    discriminant_value = compute_expression_no_io (type_definitions,
															  interpret_data,
															  discriminant_str);

	const bool    is_discriminant_value_int = discriminant_value.get_type() == C_value::E_type_integer;
#if 0
	if (type_definitions.is_an_enum(switch_def.case_type))
    {
        promote_enum_symbolic_name (type_definitions,
			                        switch_def.case_type,
                                    discriminant_value);
    }
#endif
	for (T_switch_case::size_type  idx = 0; idx < switch_case.size (); ++idx)
    {
		bool    case_found = false;

		if (switch_case[idx].is_default_case)
		{
			case_found = true;
		}
		else
		{
			M_FATAL_IF_NE(discriminant_value.get_type(), switch_case[idx].case_value.get_type());

			if (is_discriminant_value_int)
			{
				if (switch_case[idx].case_value.get_int() == discriminant_value.get_int())
		        {
					case_found = true;
				}
			}
			else
			{
				if (switch_case[idx].case_value.get_str() == discriminant_value.get_str())
		        {
					case_found = true;
				}
			}
		}

		if (case_found)
		{
            if (frame_to_fields (type_definitions,
                                 in_out_frame_data,
								 interpret_data,
								 switch_case[idx].fields,
                                 data_name,
                                 data_simple_name,
                                 os_out, os_err) != true)
            {
                os_err << "Error switch idx= " << idx
                       << " data= " << data_name << endl;
            }

            return  true;
        }
    }

	os_err << "Error switch bad value " << discriminant_value.as_string()
           << " data= " << data_name << endl;
	interpret_builder_error(type_definitions, in_out_frame_data,
							field_type_name, data_name, data_simple_name,
							"Error switch bad value " + discriminant_value.as_string() + " data= " + data_simple_name);

	return  false;
}

//*****************************************************************************
// post_treatment_value_transform *********************************************
//*****************************************************************************

void    post_treatment_value_transform (
						 const T_type_definitions      & type_definitions,
						       T_interpret_data        & interpret_data,
						 const T_field_type_name_base  & field_type_name,
							   C_value                 & value)
{
	// Transform
	if (field_type_name.transform_expression.is_defined())
	{
		// Add this value into the interpret_data (to be accessible inside compute_expression).
		T_interpret_read_values::T_id  this_id = interpret_data.add_read_variable("this", "this", value);

		// Compute specified expression.
		value = field_type_name.transform_expression.compute_expression_no_io(type_definitions, interpret_data);

		// Suppress this variable
		interpret_data.sup_read_variable(this_id);
	}

	if (field_type_name.transform_quantum.as_string() != "")
	{
		value *= field_type_name.transform_quantum;
	}

	if (field_type_name.transform_offset.as_string() != "")
	{
		value += field_type_name.transform_offset;
	}
}

//*****************************************************************************
// post_treatment_value_display ***********************************************
//*****************************************************************************

void    post_treatment_value_display (
						 const T_type_definitions      & type_definitions,
						       T_interpret_data        & interpret_data,
						 const T_field_type_name_base  & field_type_name,
							   C_value                 & value)
{
	// Display
	if (field_type_name.str_display != "")
	{
		value.format(field_type_name.str_display);
	}
	else if (field_type_name.str_display_expression != "")
	{
//		M_STATE_ENTER ("post_treatment_value", "display expression");

		// Add this value into the interpret_data (to be accessible inside compute_expression).
		T_interpret_read_values::T_id  this_id = interpret_data.add_read_variable("this", "this", value);

		// Compute specified expression.
		C_value  str_value = compute_expression_no_io(type_definitions, interpret_data, field_type_name.str_display_expression);

		// The computed value must be a string.
		value.set_str(str_value.get_str());

		// Suppress this variable
		interpret_data.sup_read_variable(this_id);
	}
}

//*****************************************************************************
// post_treatment_value_check *************************************************
//*****************************************************************************

bool    post_treatment_value_check (
						 const T_type_definitions      & type_definitions,
						       T_interpret_data        & interpret_data,
						 const T_field_type_name_base  & field_type_name,
							   C_value                 & value,
							   std::string             & error)
{
	// Check constraints (the order is revelant)
	for (vector<T_field_constraint>::const_iterator  iter  = field_type_name.constraints.begin();
													 iter != field_type_name.constraints.end();
												   ++iter)
	{
		const T_field_constraint  & constraint = *iter;

		if (constraint.equal.as_string() != "")
		{
			if (value == constraint.equal)
			{
				error = "";
				break;
			}

			if (value < constraint.equal)
			{
				if (error == "")
				{
					error = "ERROR";
				}
				else
				{
					error += " and";
				}
				error += " is != to " + constraint.equal.as_string();
				return  false;
			}
			else
			{
				error = "ERROR is != to " + constraint.equal.as_string();
			}
		}

		if (constraint.min.as_string() != "")
		{
			if (value < constraint.min)
			{
				if (error == "")
				{
					error = "ERROR";
				}
				else
				{
					error += " and";
				}
				error += " is < to " + constraint.min.as_string();
				return  false;
			}
			error = "";
		}

		if (constraint.max.as_string() != "")
		{
			if (value > constraint.max)
			{
				error = "ERROR is > to " + constraint.max.as_string();
			}
			else
			{
				break;
			}
		}
	}
	if (error != "")
	{
		return  false;
	}

	// Check string length
	if ((value.get_type() == C_value::E_type_string) &&
		(field_type_name.str_size_or_parameter != ""))
	{
		// Compute specified expression.
		C_value  size_value = compute_expression_no_io(type_definitions, interpret_data, field_type_name.str_size_or_parameter);

		// The computed value must be a integer.
		if (value.get_str().size() > size_value.get_int())
		{
			error = "ERROR length > to " + field_type_name.str_size_or_parameter;
            return  false;
		}
	}

    return  true;
}

//*****************************************************************************
// post_treatment_value *******************************************************
//*****************************************************************************

bool    post_treatment_value (
						 const T_type_definitions      & type_definitions,
						       T_interpret_data        & interpret_data,
						 const T_field_type_name_base  & field_type_name,
							   C_value                 & value,
							   std::string             & error)
{
	// No statement value
	if (field_type_name.no_statement.as_string() != "")
	{
		if (value == field_type_name.no_statement)
		{
			value.set_str("No_Statement");
			return  true;
		}
	}

	// Transform
	post_treatment_value_transform(type_definitions, interpret_data, field_type_name, value);

	// Display
	post_treatment_value_display(type_definitions, interpret_data, field_type_name, value);

	// Check
	return  post_treatment_value_check(type_definitions, interpret_data, field_type_name, value, error);
}

//*****************************************************************************
//
//*****************************************************************************
#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
#include <dyncall.h>

DC_API void  dcArgUChar (DCCallVM* vm, DCuchar     value) { return  dcArgChar (vm, (DCchar) value); }
DC_API void  dcArgUShort(DCCallVM* vm, DCushort    value) { return  dcArgShort(vm, (DCshort)value); }
DC_API void  dcArgUInt  (DCCallVM* vm, DCuint      value) { return  dcArgInt  (vm, (DCint)  value); }

DC_API DCuchar   dcCallUChar (DCCallVM* vm, DCpointer funcptr) { return  dcCallChar (vm, funcptr); }
DC_API DCushort  dcCallUShort(DCCallVM* vm, DCpointer funcptr) { return  dcCallShort(vm, funcptr); }
DC_API DCuint    dcCallUInt  (DCCallVM* vm, DCpointer funcptr) { return  dcCallInt  (vm, funcptr); }
#endif

//*****************************************************************************
// T_expression_frame_to_function_base2 ***************************************
//*****************************************************************************

bool    T_expression_frame_to_function_base2 (const T_type_definitions    & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
						 const vector<T_expression>    & fct_parameters,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err,
							   E_return_value_indicator  return_value_indicator,
							   C_value                 & returned_value,
							   bool                      pre_compute,
							   bool                    & pre_compute_result)
{
	M_STATE_ENTER ("frame_to_function_base", "");

	// Check return_value_indicator.
	if (return_value_indicator == E_return_value_mandatory)
	{
		if (fct_def.return_type == "void")
		{
			M_FATAL_COMMENT("void function can not return a value.");
		}
	}
	else if (return_value_indicator == E_return_value_forbidden)
	{
		if (fct_def.return_type != "void")
		{
			M_FATAL_COMMENT("not void function return a value.");
		}
	}

	// Check the number of parameters
	if (fct_parameters.size() < fct_def.get_nb_of_mandatory_parameters())
	{
		M_FATAL_COMMENT("Too few parameters for function " << data_simple_name);
	}
	if (fct_parameters.size() > fct_def.get_function_parameters().size())
	{
		M_FATAL_COMMENT("Too many parameters for function " << data_simple_name);
	}


	// magic number
#define K_NBMAX_PARAMETERS  50

#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
	// dyncall
	const bool    is_dyncall = (fct_def.P_library_def != NULL);
	DCCallVM    * vm = NULL;
	struct T_dyncall_parameter
	{
		char                 int8;
		unsigned char       uint8;
		short                int16;
		unsigned short      uint16;
		int                  int32;
		unsigned int        uint32;
		long long            int64;
		unsigned long long  uint64;
		float               float32;
		double              float64;
		string              str;
		void              * ptr;
	} dyncall_parameters[K_NBMAX_PARAMETERS];

	if ((pre_compute == false) && (is_dyncall == true))
	{
		const int   CallingConvention = DC_CALL_C_DEFAULT;
		vm = dcNewCallVM (4096);
		dcMode (vm, CallingConvention);
		dcReset (vm);
	}
#endif

	// Compute values.
	T_interpret_read_values::T_id    parameters_id[K_NBMAX_PARAMETERS];

	for (unsigned int   idx = 0; idx < fct_def.get_function_parameters().size(); ++idx)
	{
		const T_function_parameter  & function_parameter = fct_def.get_function_parameters()[idx];

		if (function_parameter.direction != E_parameter_in)
		{
			if (fct_parameters[idx].is_a_variable() == false)
			{
				M_FATAL_COMMENT("out or in_out parameter[" << idx << "] must be a variable");
			}
		}
		
		if ((idx < fct_parameters.size()) && (fct_parameters[idx].is_a_variable() == true))
		{
			if (pre_compute == true)
			{
				pre_compute_result = false;
				continue;
			}
		}

		// Compute value.
		bool       pre_compute_result_local = true;
		C_value	   obj_value = (idx >= fct_parameters.size()) ?
			function_parameter.get_default_value() :
			fct_parameters[idx].compute_expression(
									type_definitions, interpret_data, in_out_frame_data,
									data_name, data_simple_name, os_out, os_err,
									pre_compute, pre_compute_result_local);

		pre_compute_result = pre_compute_result && pre_compute_result_local;
		if ((pre_compute == true) && (pre_compute_result_local == false))
		{
			continue;
		}

		// Check value type.
		check_function_parameter_value(type_definitions, function_parameter, obj_value);

		// quantum/offset/min/max/display
		string     error_on_value;
		if (post_treatment_value (type_definitions, interpret_data,
								  function_parameter,
								  obj_value, error_on_value) != true)
		{
			M_FATAL_COMMENT("Parameter " << function_parameter.name << " : " << error_on_value);
		}



#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
		if (is_dyncall == false)
#endif
		{
			if (function_parameter.direction == E_parameter_in)
			{
				parameters_id[idx] = interpret_data.add_read_variable(function_parameter.name, function_parameter.name, obj_value);
			}
			else
			{
				parameters_id[idx] = interpret_data.add_ref_variable(function_parameter.name, fct_parameters[idx].get_variable_name());
			}
		}
#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
		else if (vm != NULL)
		{
			T_dyncall_parameter  & dyncall_parameter = dyncall_parameters[idx];

#undef  M_FCT_READ_SIMPLE_TYPE
#define M_FCT_READ_SIMPLE_TYPE(TYPE_NAME,DC_FCT,FCT)                               \
			else if (function_parameter.type == #TYPE_NAME)                        \
			{                                                                      \
				dyncall_parameter. TYPE_NAME = obj_value. FCT ();                  \
				if (function_parameter.direction == E_parameter_in)                \
					DC_FCT(vm , dyncall_parameter. TYPE_NAME);                     \
				else                                                               \
					dcArgPointer(vm , &dyncall_parameter. TYPE_NAME );             \
			}

#undef  M_FCT_READ_SIMPLE_TYPE_INT
#define M_FCT_READ_SIMPLE_TYPE_INT(TYPE_NAME,TYPE_DC)                              \
			M_FCT_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_DC,get_int)

#undef  M_FCT_READ_SIMPLE_TYPE_FLT
#define M_FCT_READ_SIMPLE_TYPE_FLT(TYPE_NAME,TYPE_DC)                              \
			M_FCT_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_DC,get_flt)


			if (function_parameter.type == "string")
			{
				// string implementation could be shared,
				//  so copy does not duplicate data and keeps the original pointer.
				// Which means, if out parameter, that the original string will be modified !
//				dyncall_parameter.str = obj_value.get_str();
				// Using c_str() avoid share. Except if share implementation method works on raw pointer !
				dyncall_parameter.str = obj_value.get_str().c_str();
				// Called function will directly write on raw pointer of string object !
				// Should NOT write more than the size of the input string !
				dcArgPointer(vm , (DCpointer)dyncall_parameter.str.c_str());
			}
			M_FCT_READ_SIMPLE_TYPE_INT( int8 ,dcArgChar)
			M_FCT_READ_SIMPLE_TYPE_INT(uint8 ,dcArgUChar)
			M_FCT_READ_SIMPLE_TYPE_INT( int16,dcArgShort)
			M_FCT_READ_SIMPLE_TYPE_INT(uint16,dcArgUShort)
			M_FCT_READ_SIMPLE_TYPE_INT( int32,dcArgInt)
			M_FCT_READ_SIMPLE_TYPE_INT(uint32,dcArgUInt)
			M_FCT_READ_SIMPLE_TYPE_INT( int64,dcArgLongLong)
			M_FCT_READ_SIMPLE_TYPE_FLT(float32,dcArgFloat)
			M_FCT_READ_SIMPLE_TYPE_FLT(float64,dcArgDouble)
			else if (function_parameter.type == "pointer")
			{
				const long long    position_bits  = obj_value.get_int();
				const long long    position_bytes = position_bits / 8;
				const T_byte     * pointer = in_out_frame_data.get_initial_P_bytes() + position_bytes;

				dyncall_parameter.ptr = (void*)pointer;
				dcArgPointer(vm , (DCpointer)dyncall_parameter.ptr);
			}
			else
			{
				M_FATAL_COMMENT("unknow type (" << function_parameter.type << ") for dyncall parameter");
			}
		}
#endif
	}

	if ((pre_compute == true) && (pre_compute_result == false))
	{
		// pre_compute of parameters does not succeed
		return  true;
	}

	if (pre_compute == true) { pre_compute_result = false; return  true; }   /* ICIOA temporaire */


#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
	if (is_dyncall == true)
	{
		const T_library_definition           & library_def = * fct_def.P_library_def;
		const T_library_function_definition  & library_function_def = library_def.library_functions[fct_def.idx_library_function_def];

		if (fct_def.return_type == "void")
		{
			dcCallVoid(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "int8")
		{
			returned_value = dcCallChar(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "uint8")
		{
			returned_value = dcCallUChar(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "int16")
		{
			returned_value = dcCallShort(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "uint16")
		{
			returned_value = dcCallUShort(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "int32")
		{
			returned_value = dcCallInt(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "uint32")
		{
			returned_value = dcCallUInt(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "int64")
		{
			returned_value = dcCallLongLong(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "float32")
		{
			returned_value = dcCallFloat(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "float64")
		{
			returned_value = dcCallDouble(vm, library_function_def.funptr);
		}
		else if (fct_def.return_type == "string")
		{
			returned_value = (char*)dcCallPointer(vm, library_function_def.funptr);
		}
		else
		{
			M_FATAL_COMMENT("unknow type (" << fct_def.return_type << ") for dyncall return value");
		}

		dcFree ( vm );

		for (unsigned int   idx = 0; idx < fct_def.get_function_parameters().size(); ++idx)
		{
			const T_function_parameter  & function_parameter = fct_def.get_function_parameters()[idx];

			if (function_parameter.direction == E_parameter_in)
				continue;

			const T_expression   & fct_parameter = fct_parameters[idx];
			T_dyncall_parameter  & dyncall_parameter = dyncall_parameters[idx];

			if (function_parameter.type == "int8")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.int8);
			}
			else if (function_parameter.type == "uint8")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.uint8);
			}
			else if (function_parameter.type == "int16")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.int16);
			}
			else if (function_parameter.type == "uint16")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.uint16);
			}
			else if (function_parameter.type == "int32")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.int32);
			}
			else if (function_parameter.type == "uint32")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.uint32);
			}
			else if (function_parameter.type == "int64")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.int64);
			}
			else if (function_parameter.type == "float32")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.float32);
			}
			else if (function_parameter.type == "float64")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.float64);
			}
			else if (function_parameter.type == "string")
			{
				interpret_data.set_read_variable(fct_parameter.get_variable_name(), dyncall_parameter.str.c_str());
			}
			else
			{
				M_FATAL_COMMENT("unknow type (" << function_parameter.type << ") for dyncall parameter");
			}
		}

		return  true;
	}
#endif

	bool    result = true;

	try
	{
		result = frame_to_fields(type_definitions, in_out_frame_data, interpret_data, fct_def.fields,
							data_name, data_simple_name,
                            os_out, os_err);

		if (fct_def.return_type != "void")
		{
			M_FATAL_COMMENT("No return in a not void function");
		}
	}
	catch (C_byte_interpret_exception_return  & val)
	{
		if (fct_def.return_type == "void")
		{
			if (val.is_returned_value_set == true)
			{
				M_FATAL_COMMENT("A value is returned from a void function (internal bug)");
			}
		}
		else
		{
			if (val.is_returned_value_set == false)
			{
				M_FATAL_COMMENT("No value is returned from a not void function (internal bug)");
			}

			// ICIOA type verification ... (factoriser avec le code des parameters)
			returned_value = val.returned_value;
		}
	}

	// rollback on parameters
	interpret_data.sup_read_variables(fct_parameters.size(), parameters_id);

	return  result;
}

//*****************************************************************************
// T_expression_frame_to_function_base ****************************************
//*****************************************************************************

bool    T_expression_frame_to_function_base (const T_type_definitions    & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
							   vector<T_expression>    & fct_parameters,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err,
							   E_return_value_indicator  return_value_indicator,
							   C_value                 & returned_value,
							   bool                      pre_compute,
							   bool                    & pre_compute_result)
{
	T_interpret_read_values::T_id    last_data_id = interpret_data.get_id_of_last_read_variable ();

	bool	result = T_expression_frame_to_function_base2 (type_definitions,
								   interpret_data,
								   in_out_frame_data,
								   fct_def,
								   fct_parameters,
								   data_name,
								   data_simple_name,
								   os_out,
								   os_err,
								   return_value_indicator,
								   returned_value,
								   pre_compute,
								   pre_compute_result);

	// 2010/09/25 function behavior modification
	// Remove ALL datas created inside function !!!
	interpret_data.sup_all_read_variables_after(last_data_id);


	return  result;
}

//*****************************************************************************
// T_expression_compute_function **********************************************
//*****************************************************************************

C_value    T_expression_compute_function (const T_type_definitions    & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
							   vector<T_expression>    & fct_parameters,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err,
							   bool                      pre_compute,
							   bool                    & pre_compute_result)
{
	M_STATE_ENTER ("T_expression_compute_function", "");

	C_value    return_value;

	bool       result = T_expression_frame_to_function_base (type_definitions,
							   interpret_data,
					           in_out_frame_data,
							   fct_def,
							   fct_parameters,
							   data_name,
							   data_simple_name,
							   os_out,
							   os_err,
							   E_return_value_mandatory,
							   return_value,
							   pre_compute,
							   pre_compute_result);
	if (result == false)
	{
		M_FATAL_COMMENT("Error when computing function.");
	}

	return  return_value;
}

//*****************************************************************************
// frame_to_function_base *****************************************************
//*****************************************************************************

bool    frame_to_function_base (const T_type_definitions    & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
						 const vector<T_expression>    & fct_parameters,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err,
							   E_return_value_indicator  return_value_indicator,
							   C_value                 & returned_value)
{
	T_interpret_read_values::T_id    last_data_id = interpret_data.get_id_of_last_read_variable ();

	bool  pre_compute_result = false;
	bool  result = T_expression_frame_to_function_base2 (type_definitions,
													interpret_data,
													in_out_frame_data,
													fct_def,
													fct_parameters,
													data_name,
													data_simple_name,
													os_out,
													os_err,
													return_value_indicator,
													returned_value,
													false,
													pre_compute_result);

	// 2010/09/25 function behavior modification
	// Remove ALL datas created inside function !!!
	interpret_data.sup_all_read_variables_after(last_data_id);


	return  result;
}

//*****************************************************************************
// frame_to_function **********************************************************
//*****************************************************************************

bool    frame_to_function (const T_type_definitions    & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
						 const T_field_type_name       & field_type_name,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER ("frame_to_function", "");

	bool  result = false;
	if (field_type_name.name == "frame_append_data")
	{
		result = frame_append_data (type_definitions, in_out_frame_data, interpret_data,
							   fct_def,
							   field_type_name, data_name, data_simple_name,
							   os_out, os_err);
	}
	else if (field_type_name.name == "frame_append_hexa_data")
	{
		result = frame_append_hexa_data (type_definitions, in_out_frame_data, interpret_data,
							   fct_def,
							   field_type_name, data_name, data_simple_name,
							   os_out, os_err);
	}
	else
	{
		C_value    return_value_do_not_care;
		result = frame_to_function_base (type_definitions,
								   interpret_data,
								   in_out_frame_data,
								   fct_def,
								   field_type_name.fct_parameters,
								   data_name,
								   data_simple_name,
								   os_out,
								   os_err,
								   E_return_value_do_not_care,
								   return_value_do_not_care);
	}

	return  result;
}

//*****************************************************************************
// print_syntax ***************************************************************
//*****************************************************************************

void    print_syntax (ostream       & os_out)
{
    os_out <<
"\n\
This help could be uncomplete or not up-to-date : please refer to http://wsgd.free.fr/fdesc_format.html\n\
\n\
*** Rules\n\
\n\
- Define a type before using it\n\
- For type name and field name :\n\
--- Do not use keywords (int8, string, void, any, this, struct, enum, if, while ...)\n\
--- Use only alphanumerics and _\n\
--- Do not start with number\n\
\n\
*** Basic types\n\
\n\
- spare                                     not displayed byte\n\
- char, schar, uchar\n\
- bool1, bool8, bool16, bool32              take care of byte order\n\
-  int2 to int32,  int64                    take care of byte order\n\
- uint1 to uint32                           take care of byte order\n\
- float32, float64                          take care of byte order\n\
- string, string(<nb_bytes>)                manage zero as end of string\n\
- string_nl, string_nl(<nb_bytes>)          idem string + manage \"\\n\" or \"\\r\\n\" as end of string\n\
- raw(<nb_bytes>)                           dump hexa (nb_bytes could be * in some cases)\n\
                                            must start on an entire byte position\n\
- padding_bits                              permits to move to the next entire byte position\n\
\n\
*** No Statement value\n\
\n\
<int_or_float_type_name>{ns=<No Statement value>}       <field_name> ;\n\
If the read value is equal to the specified value :\n\
- No_Statement will be displayed.\n\
- Transform, Display and Constrains specifications are ignored\n\
NB: must be specified before the Transform, Display and Constrains specifications.\n\
\n\
*** Transform specifications\n\
\n\
<int_or_float_type_name>{q=<quantum>:o=<offset>}        <field_name> ;\n\
<int_or_float_type_name>{q=<quantum>}                   <field_name> ;\n\
<int_or_float_type_name>{o=<offset>}                    <field_name> ;\n\
The resulting value = read_value * quantum + offset.\n\
Quantum and offset values could be specified with an expression, e.g : 3.1415927/180.0\n\
\n\
<type_name>{tei=integer expression (use \"this\")}        <field_name> ;\n\
<type_name>{tef=float   expression (use \"this\")}        <field_name> ;\n\
The specified expression must return the appropriate type.\n\
E.g:\n\
uint16{tei=2*this-47}          <field_name> ;    # since this is an integer, the expression gives an integer\n\
uint16{tef=2*this-47.0}        <field_name> ;    # because of 47.0, the expression gives a float\n\
float32{tef=2*this-47}         <field_name> ;    # since this is a float, the expression gives a float\n\
\n\
<type_name>{tei=a_previous_field > 0 ? this/another_field : 2*this-47}    <field_name> ;\n\
<type_name>{tef=a_function_which_returns_a_float(this)}                   <field_name> ;\n\
\n\
*** Display specifications\n\
\n\
<int_type_name>{d=hex}                                  <field_name> ;\n\
<int_type_name>{d=oct}                                  <field_name> ;\n\
<int_type_name>{d=bin}                                  <field_name> ;\n\
<any_type_name>{d=printf format %22.32s}                <field_name> ;\n\
<any_type_name>{de=string expression (use \"this\")}      <field_name> ;\n\
NB: display specifications must always appears AFTER transform specifications (if any)\n\
\n\
*** Constraint specifications\n\
\n\
An error is displayed if the value does NOT match.\n\
For int or float only.\n\
\n\
<int_or_float_type_name>{min=<val_min>:max=<val_max>}   <field_name> ;\n\
<int_or_float_type_name>{min=<val_min>}                 <field_name> ;\n\
<int_or_float_type_name>{max=<val_max>}                 <field_name> ;\n\
NB: constraints specifications must always appears AFTER transform and display specifications (if any)\n\
Min and max values could be specified with an expression, e.g : -(3.0/4)*3.1415927\n\
\n\
Could use many constaints (specified in the good order), e.g:\n\
uint6{min=2:max=9}{min=12:max=19}\n\
\n\
*** Local byte order specification\n\
\n\
The global byte order is specified with byte_order command.\n\
This byte order specification apply only to the specified field.\n\
\n\
<type_name>{byte_order=big_endian}      <field_name> ;\n\
<type_name>{byte_order=little_endian}   <field_name> ;\n\
\n\
*** Arrays\n\
\n\
<type_name>[12]                                <array_field_name> ;\n\
<type_name>[<field_name>]                      <array_field_name> ;\n\
<type_name>[<field_name> - 12]                 <array_field_name> ;\n\
<type_name>[<field_name> < 36 ? 0 : 16]        <array_field_name> ;\n\
\n\
Only at the end of the message AND if Generic Dissector knows the size of the message :\n\
<type_name>[*]                                 <array_field_name> ;          * means any number of element\n\
<type_name>[+]                                 <array_field_name> ;          + means any number of element, at least 1\n\
If the array is not at the end of the message, look at loop_size.\n\
\n\
*** Alias\n\
\n\
alias  <new_type_name>    <already_existent_type_name> ;\n\
alias  <new_type_name>    <already_existent_type_name>{q=<quantum>}{min=<val_min>:max=<val_max>} ;\n\
alias  <new_type_name>    <already_existent_type_name>{d=hex}{min=<val_min>:max=<val_max>} ;\n\
\n\
*** Enum\n\
\n\
Take care of byte order\n\
\n\
enum<nb_bits 1 to 32>  <enum_type_name>\n\
{\n\
    <symbolic_value_name>  <integer_value or - >                   # - means last value + 1 (zero if first value)\n\
    ...\n\
}\n\
\n\
# To define an identic enum with a different size.\n\
enum<nb_bits 1 to 32>  <enum_type_name>  as      <already_existent_enum_type_name> ;\n\
\n\
# To define a new enum starting from an existent one.\n\
enum<nb_bits 1 to 32>  <enum_type_name>  expand  <already_existent_enum_type_name>\n\
{\n\
    <other_symbolic_value_name>  <integer_value>\n\
    ...\n\
}\n\
\n\
# Could use <enum_type_name>::<symbolic_value_name> in any expression/condition.\n\
\n\
*** Bit field\n\
\n\
bitfield must be understood like a C bitfield (even there is no standard about its implementation).\n\
Take care of byte order.\n\
Fields could be unsigned integers, bool1 or enum (without signed values).\n\
\n\
bitfield<nb_bits 8 16 24 or 32>  <bitfield_type_name>\n\
{\n\
  uint3{d=bin}                             field1 ;             # lower level bits\n\
  hide uint2                               field2_unused ;\n\
  uint15{q=2.34:o=-117.35}{min=-105.17}    field3 ;\n\
  <enum_type>                              field4;\n\
  bool1                                    field5 ;\n\
  ...                                                           # higher level bits\n\
}\n\
\n\
Could also use var and set inside Bit field.\n\
More info at http://wsgd.free.fr/bitfield_more.txt\n\
\n\
*** Bit stream (Deprecated)\n\
\n\
Absolutely identic to bitfield, except :\n\
- use bitstream keyword instead of bitfield !\n\
- read bits from left to right (does not take care of byte order)\n\
\n\
Deprecated: use struct instead.\n\
\n\
*** Struct\n\
\n\
struct  <struct_type_name>\n\
{\n\
    <type_name>     <field_name> ;\n\
    <command_name>  <command_parameter> ;\n\
    if              ((<field_name> + 20 < 572) && (...) || (...))\n\
    {\n\
        <anything that could be specified in a struct>\n\
    }\n\
    else\n\
    {\n\
        <anything that could be specified in a struct>\n\
    }\n\
    while           ((<field_name> % 20 < 2**3) && (...) || (...))\n\
    {\n\
        <anything that could be specified in a struct>\n\
        continue ;\n\
        break ;\n\
    }\n\
    do\n\
    {\n\
        <anything that could be specified in a struct>\n\
        continue ;\n\
        break ;\n\
    } while           ((to_string(<field_name>) + \"20\" != print(\"%d\", 572)) && (...) || (...)) ;\n\
\n\
    # repeat until the given size have been effectively read    \n\
    # Use loop_size_bits if bit size is needed\n\
    loop_size_bytes    <field_name>+20\n\
    {\n\
        <anything that could be specified in a struct>\n\
        continue ;\n\
        break ;\n\
    }  \n\
    \n\
    # Inline struct and bitfield\n\
    struct\n\
    {\n\
        <anything that could be specified in a struct>\n\
    }   <field_name> ;\n\
\n\
    bitfield<nb_bits 8 16 24 or 32>\n\
    {\n\
        <anything that could be specified in a bitfield>\n\
    }   <field_name> ;\n\
    ...\n\
}\n\
\n\
Optionaly, you can put a print specification :\n\
struct  <struct_type_name>  print (<printf format and arguments>)\n\
{\n\
    ...\n\
}\n\
The printf arguments could be fields specified inside the struct.\n\
\n\
*** Switch\n\
\n\
switch  <switch_type_name>  <switched_type_name>\n\
{\n\
    case <value 1> :\n\
    <anything that could be specified in a struct>\n\
    ...\n\
    case <value n> :\n\
    <anything that could be specified in a struct>\n\
    default :\n\
    <anything that could be specified in a struct>\n\
}\n\
\n\
# Example :\n\
enum16 T_Operation\n\
{\n\
  CREATION      0\n\
  MODIFICATION  1\n\
  DELETION      2\n\
}\n\
switch T_Operation_switch  T_Operation\n\
{\n\
case  T_Operation::MODIFICATION  : print (\"MODIFICATION value\");\n\
case  T_Operation::DELETION      : print (\"DELETION value\");\n\
case  T_Operation::CREATION      : print (\"CREATION value\");\n\
default                          : print (\"default value\");\n\
}\n\
struct  xxx\n\
{\n\
  ...\n\
  T_Operation                   operation;\n\
  ...\n\
  T_Operation_switch(operation)  oper_switch;\n\
  # or\n\
  T_Operation_switch(operation)  \"\";\n\
  # or directly inline without previous definition/declaration\n\
  switch(operation)\n\
  {\n\
  case  T_Operation::MODIFICATION  : print (\"MODIFICATION value\");\n\
  case  T_Operation::DELETION      : print (\"DELETION value\");\n\
  case  T_Operation::CREATION      : print (\"CREATION value\");\n\
  default                          : print (\"default value\");\n\
  }\n\
}\n\
\n\
*** Forward declarations\n\
\n\
- enum<nb_bits 1 to 32>  <enum_type_name>;\n\
- bitfield<nb_bits 8 16 24 or 32>  <bitfield_type_name>;\n\
- bitstream<nb_bits 8 16 24 or 32>  <bitfield_type_name>;\n\
- struct  <struct_type_name>;\n\
- switch  <switch_type_name>;\n\
\n\
"
      << endl;  // avoir too long string for compiler
      os_out <<
"*** Commands\n\
\n\
- include     <file_name> ;\n\
  include the specified description file\n\
  Not available inside a struct, switch ...\n\
- byte_order  (little_endian | big_endian | as_host) ;\n\
  specify the data byte order for int (also enum and bitfield) and float\n\
  - big endian (also known as network, motorola) : the bytes are not inverted\n\
  - little endian (also known as intel) : the bytes are inverted\n\
\n\
*** Var command\n\
\n\
var  <type>  <variable name> = <value or expression> ;\n\
 permits to declare, initialize and display a variable.\n\
 variable = field except it is initialized from expression and not from the captured packet.\n\
 <type> could be any int, float, string and enum\n\
E.g:\n\
var string                          str = \"Hello world!\";\n\
var string[2]                       str_array = \"Hello both world!\";\n\
var uint32{q=3}{d=the int=0x%04x}   integer = 23;\n\
var float32                         flt = 136.234;\n\
var float32[2]                      flt_array = 136.136;\n\
var string                          str2 = print(\"flt = %3f and integer = \", flt) + to_string(integer);    \n\
var <an enum type>                  oper = <an enum type>::<symbolic name>;\n\
var uint8{d=bin}                    integer_bit_1     =  integer & 0x01;\n\
var uint8{d=bin}                    integer_bit_2_3   = (integer & 0x06) >> 1;\n\
\n\
The variable must NOT already exist.\n\
\n\
Set command\n\
\n\
set  <variable name> = <expression> ;\n\
E.g:\n\
set    Size = 0xa * 3 + 3 ;\n\
set    Size = Size-10 ;\n\
set    Size_bit_part = (Size & 0xc) >> 2;\n\
\n\
The variable must already exist.\n\
The variable could be a field (but does NOT change the value seen by wireshark).\n\
\n\
*** Const command\n\
\n\
const  <type>  <variable name> = <value or expression> ;\n\
 permits to declare and initialize a constant.\n\
 <type> could be any int, float, string and enum\n\
Constants must be named with \"::\".\n\
Constants could NOT have/be :\n\
- array,\n\
- specifications no_statement, transform, display, constrains ...\n\
- struct, switch, ...\n\
E.g:\n\
const uint16           konst::int = 2;\n\
const string           konst::str = \"abcdefgh - abcdefgh\";\n\
const T_Type_message   konst::enum = T_Type_message::cs_start;\n\
const float64          Math::PI = 3.1415927;\n\
const float64          Math::deg_to_rad = Math::PI / 180;\n\
const string           konst::str_fct = string.replace_all (konst::str, \"bc\", \"xyz\");\n\
\n\
The constant must NOT already exist.\n\
\n\
*** Variable/field name usage\n\
\n\
When you want to use a variable or field inside an expression or set command, you simply use its name.\n\
But it could happen that there is many variables/fields with the same name.\n\
In these cases, you can use the full name or the partial ending name.\n\
E.g. :\n\
\n\
struct time\n\
{\n\
  uint32  value;\n\
  var uin16 sec = ...;\n\
  var uin16 min = ...;\n\
  ...\n\
}\n\
struct T_my_msg\n\
{\n\
  ...\n\
  struct {\n\
    ...\n\
    time    expected_time;\n\
  }  expected_data ;\n\
  struct {\n\
    ...\n\
    time    time;\n\
  }  measured_data ;\n\
  # \"value\" means the last variable/field called \"value\", so it is \"measured_data.time.value\"\n\
  if (value != expected_data.expected_time.value)  { ... }    # full name\n\
  if (value !=               expected_time.value)  { ... }    # partial ending name\n\
}\n\
\n\
*** Functions\n\
\n\
function  <return_type>  <function_name> (<direction> <type>  <name> [ = <default_value> ], ...)\n\
{\n\
  <anything that could be specified in a struct>\n\
  ...\n\
  return  <expression>;\n\
}\n\
  \n\
<direction>      = in, out, in_out\n\
<type>           = any existing simple type (numeric or enum or string) or any \n\
<return_type>    = <type> or void\n\
<default_value>  = default value used if parameter is not specified\n\
NB: if a default value is specified on a parameter, then all following parameters must have a default value.\n\
\n\
E.g.:\n\
function string  sec_to_dhms (in int64{min=0}  value)\n\
{\n\
  hide var int64    value_work = value;\n\
  hide var uint16   sec = value_work % 60;\n\
       set          value_work = (value_work - sec) / 60;\n\
  hide var uint16   min = value_work % 60;\n\
       set          value_work = (value_work - min) / 60;\n\
  hide var uint16   hour = value_work % 24;\n\
       set          value_work = (value_work - hour) / 24;\n\
  hide var uint16   days = value_work;\n\
  return  print(\"%d days %02d:%02d:%02d\", days, hour, min, sec);\n\
}\n\
function string   ms_to_dhms (in int64{min=0}  value)\n\
{\n\
  hide var int64    value_work = value;\n\
  hide var uint16   ms = value_work % 1000;\n\
       set          value_work = (value_work - sec) / 1000;\n\
  hide var string   str = sec_to_dhms(value_work) + print(\".%03d\", ms);\n\
  return  str;\n\
}\n\
...\n\
  int64          milli_sec_time;\n\
  var string     milli_sec_time_str = ms_to_dhms(milli_sec_time);\n\
  # or\n\
  int64{de=ms_to_dhms(this)}       milli_sec_time;\n\
\n\
NB: a function which returns void (ie nothing) must be called like this :\n\
  call  <function_name> (<the parameters>);\n\
\n\
*** Built-in functions\n\
\n\
- string          to_string (<field_variable_value_expression>) ;\n\
- <int_or_float>  to_numeric (in string  str_source) ;\n\
  fatal if result is not numeric\n\
- <float>         to_float (in string  str_source) ;\n\
  fatal if result is not numeric\n\
- <int>           to_integer (in string  str_source) ;\n\
  fatal if result is not numeric\n\
- string          getenv (in string  env_variable_name) ;\n\
- uint            string.length (in string  str_source);\n\
-  int64          string.find   (in string  str_source, in string  str_to_find);\n\
- string          string.substr (in string  str_source, in uint  index, in int64  count = string::npos);\n\
- string          string.erase  (in string  str_source, in uint  index, in int64  count = string::npos);\n\
- string          string.insert (in string  str_source, in uint  index, in string  str_to_insert);\n\
- string          string.replace(in string  str_source, in uint  index, in int64  count, in string  str_to_insert);\n\
- string          string.replace_all(in string  str_source, in string  str_old, in string  str_new);\n\
\n\
*** Decoder\n\
\n\
Sometimes, the input data is NOT directly usable (using int16, string ...) because it is encoded in a way that Generic dissector does NOT understand.\n\
It is necessary to decode the input data before use it.\n\
With decoder command, you can provide a function which decode the data following your rules.\n\
Explanations at http://wsgd.free.fr/decoder_more.txt\n\
\n\
*** Specific commands\n\
\n\
- output            -- or ++ ;\n\
  modify the output level\n\
\n\
- save_position          <position_name> ;\n\
  save the current data packet position\n\
- goto_position          <previously_saved_position_name> ;\n\
- move_position_bytes    <position_offset> ;\n\
- move_position_bits     <position_offset> ;\n\
\n\
- [debug_]print  \"<string>\" or <field name> or (\"<printf format string>\", <expression>, <expression> ...) ;\n\
- chat           \"<string>\" or <field name> or (\"<printf format string>\", <expression>, <expression> ...) ;\n\
- note           \"<string>\" or <field name> or (\"<printf format string>\", <expression>, <expression> ...) ;\n\
- warning        \"<string>\" or <field name> or (\"<printf format string>\", <expression>, <expression> ...) ;\n\
- error          \"<string>\" or <field name> or (\"<printf format string>\", <expression>, <expression> ...) ;\n\
- fatal          \"<string>\" or <field name> or (\"<printf format string>\", <expression>, <expression> ...) ;\n\
  print the specified string or the specified field value or printf\n\
  chat, note and warning will appear on gray, blue and yellow lines\n\
  error and fatal will declare an error and appear on red line\n\
  colored lines will appear into \"Analyze/Expert info composite\" menu\n\
  fatal is supposed to stop the dissection\n\
  \n\
- [debug_]print  (byte_order|output|position) ;\n\
  print specified internal data\n\
\n\
*** Others commands\n\
\n\
WITHOUT any effect on wireshark at this time.\n\
\n\
- check_eof_distance_bytes  <number of bytes before end of data> ;\n\
- check_eof_distance_bits   <number of bits  before end of data> ;\n\
  verify the distance between the current position and the end of packet\n\
- [debug_]print  (alias|struct|enum|switch) ;\n\
  print specified internal data\n\
- [debug_]print  all ;\n\
  print all internal data specified above\n\
- [debug_]print  (help | syntax) ;\n\
\n\
*** Modifiers\n\
\n\
- hide   <any_type>       <any_field> ;\n\
  permits to hide the field/variable (NOT displayed and could NOT filter on it)\n\
"
      << endl;
}

//*****************************************************************************
// print_help *****************************************************************
//*****************************************************************************

void    print_help (ostream       & os_out)
{
    print_syntax (os_out);
}

//*****************************************************************************
// frame_to_print_any *********************************************************
//*****************************************************************************

bool    frame_to_print_any (
                  const T_type_definitions  & type_definitions,
					    T_frame_data        & in_out_frame_data,
			            T_interpret_data    & interpret_data,
                  const T_field_type_name   & field_type_name,
                  const string              & final_type,
                  const string              & data_name,
                  const string              & data_simple_name,
                        ostream             & os_out,
                        ostream             & os_err,
						T_interpret_builder_cmd_print_cb    interpret_builder_cb)
{
	// NB: I'm supposed to ouput on os_err in case of error, but I do NOT really care.

	T_interpret_read_values::T_var_name_P_values    var_name_P_values;
	const C_value   printf_result_value = printf_args_to_string(type_definitions, interpret_data, in_out_frame_data,
																data_simple_name,
																data_name, data_simple_name, os_out, os_err,
																var_name_P_values);

	if (var_name_P_values.empty())
	{
		string    printf_result = printf_result_value.as_string();

		if (final_type != "print")
		{
			os_out << final_type << " : ";
		}
// useless because printf_result_value is a simple string
//		if (final_type == "debug_print")
//		{
//			printf_result = get_string(printf_result_value);
//		}

		os_out << printf_result << endl;
		interpret_builder_cb(type_definitions, in_out_frame_data,
							 field_type_name, data_name, printf_result);
	}
	else
	{
		for (T_interpret_read_values::T_var_name_P_values::const_iterator
										iter  = var_name_P_values.begin();
										iter != var_name_P_values.end();
									  ++iter)
		{
			string    printf_result = iter->var_name + " -> " + iter->P_value->transformed.as_string();

			if (final_type != "print")
			{
				os_out << final_type << " : ";
			}
			if (final_type == "debug_print")
			{
				printf_result += "  ";
				printf_result += get_string(iter->P_value->transformed);
			}

			os_out << printf_result << endl;
			interpret_builder_cb(type_definitions, in_out_frame_data,
								 field_type_name, data_name, printf_result);
		}
	}

	return  true;
}

//*****************************************************************************
// frame_to_print *************************************************************
//*****************************************************************************

bool    frame_to_print (
                  const T_type_definitions  & type_definitions,
					    T_frame_data        & in_out_frame_data,
			            T_interpret_data    & interpret_data,
                  const T_field_type_name   & field_type_name,
                  const string        & final_type,
                  const string        & data_name,
                  const string        & data_simple_name,
                        ostream       & os_out,
                        ostream       & os_err)
{
	// Verify debug_print is available.
    if ((final_type == "debug_print") &&
        (interpret_data.must_output_debug () == false))
    {
        return  true;
    }

    // Verify print is available.
    if ((final_type == "print") &&
        (interpret_data.must_output () == false))
    {
        return  true;
    }

    if (data_simple_name == "alias")
    {
        // gcc 4.4.1 does not accept my templates on map
#ifndef  __GNUC__
        print (os_out, type_definitions.map_alias_type, "alias ");
#endif
    }
    else if (data_simple_name == "struct")
    {
        print (os_out, type_definitions.map_struct_definition, "struct ");
    }
    else if (data_simple_name == "enum")
    {
        print (os_out, type_definitions.map_enum_definition_representation, "enum ");
    }
    else if (data_simple_name == "switch")
    {
        os_out << final_type << " " << data_simple_name
               << " NOT implemented." << endl;
    }
    else if (data_simple_name == "byte_order")
    {
        string    host_byte_order = interpret_data.get_host_order();
        string    data_byte_order = interpret_data.get_data_order();

		string  str;
		str += "data is " + data_byte_order + " and ";
		str += "host is " + host_byte_order + " so ";
		str += "integer or float read are ";
		str	+= interpret_data.must_invert_bytes() ? "" : "not ";
		str += "inverted.";

		os_out << str << endl;
		interpret_builder_cmd_print(type_definitions, in_out_frame_data,
								    field_type_name, data_name, str);
    }
    else if (data_simple_name == "output")
    {
		string  str = "output level = " + get_string(interpret_data.get_output_level());

		os_out << str << endl;
		interpret_builder_cmd_print(type_definitions, in_out_frame_data,
								    field_type_name, data_name, str);
    }
    else if (data_simple_name == "position")
    {
        os_out << "Ptr = " << (unsigned long)in_out_frame_data.get_P_bytes() << endl;
        os_out << "Nb bits to read = " << in_out_frame_data.get_remaining_bits() << endl;

		string  str;
		str += "Ptr = " + get_string((unsigned long)in_out_frame_data.get_P_bytes());
		str += "  " ;
		str += "Nb bits to read = " + get_string(in_out_frame_data.get_remaining_bits());
		interpret_builder_cmd_print(type_definitions, in_out_frame_data,
								    field_type_name, data_name, str);
    }
    else if (data_simple_name == "all")
    {
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "alias", os_out,os_err);
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "struct", os_out,os_err);
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "enum", os_out,os_err);
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "switch", os_out,os_err);
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "byte_order", os_out,os_err);
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "output", os_out,os_err);
        frame_to_print (type_definitions,
                      in_out_frame_data,
					  interpret_data,
					  field_type_name,
                      final_type, data_name, "position", os_out,os_err);
    }
    else if (data_simple_name == "help")
    {
        print_help (os_out);
    }
    else if (data_simple_name == "syntax")
    {
        print_syntax (os_out);
    }
    else
    {
		frame_to_print_any(type_definitions, in_out_frame_data, interpret_data,
						   field_type_name, final_type, data_name, data_simple_name,
						   os_out, os_err,
						   interpret_builder_cmd_print);
    }
    
    return  true;
}

//*****************************************************************************
// simple_value_to_attribute_value_main ***************************************
//*****************************************************************************

string    simple_value_to_attribute_value_main (
						 const T_type_definitions  & type_definitions,
						       T_interpret_data    & interpret_data,
                                  const C_value            & value,
                                  const string             & final_type,
								  const T_field_type_name  & field_type_name,
								        T_attribute_value  & attribute_value,
								        bool               & no_error)
{
    attribute_value.transformed    = value;
	// normalize the string original
	attribute_value.value_is_original_format_reset();

	string  error_str;
    no_error = post_treatment_value(type_definitions, interpret_data,
									field_type_name,
									attribute_value.transformed,
								    error_str);
	if (no_error == false)
	{
		attribute_value.set_error(error_str);
	}


    return  attribute_value_to_string(attribute_value);
}

//*****************************************************************************
// enum_value_to_attribute_value
//*****************************************************************************

string    enum_value_to_attribute_value (
						 const T_type_definitions  & type_definitions,
						       T_interpret_data    & interpret_data,
						 const C_value             & obj_value,
                         const T_enum_definition   & enum_definition,
						 const T_field_type_name   & field_type_name,
							   T_attribute_value   & attribute_value,
						       bool                & no_error)
{
	no_error = false;
	attribute_value.transformed = obj_value;
	attribute_value.value_is_original();

	post_treatment_value_transform(type_definitions, interpret_data,
									field_type_name,
									attribute_value.transformed);

	// Search symbolic value
	const long long    value = attribute_value.transformed.get_int();

    for (uint  idx = 0; idx < enum_definition.size (); ++idx)
    {
        if (enum_definition[idx].value == value)
        {
			no_error = true;
			// NB: transformed is still an integer
			attribute_value.transformed.set_str(enum_definition[idx].name);
		    break;
        }
    }

	if (no_error == false)
	{
		attribute_value.set_error("ERROR unknown enum value");
	}
	else
	{
		// apply display expressions ...
		post_treatment_value_display(type_definitions, interpret_data,
										field_type_name,
										attribute_value.transformed);

		// apply checks ...
		string  error_str;
		no_error = post_treatment_value_check(type_definitions, interpret_data,
										field_type_name,
										attribute_value.transformed,
										error_str);
		if (no_error == false)
		{
			attribute_value.set_error(error_str);
		}
	}

    return  attribute_value_to_string(attribute_value);
}

#if 0
//*****************************************************************************
// read_simple_type ***********************************************************
//*****************************************************************************

template 
void read_simple_type(const string  & TYPE_NAME,
					  const int       TYPE_BIT_SIZE,
					  TYPE_IMPL
					  const int       TYPE_IMPL_BIT_SIZE,
					  const string  & TYPE_IMPL_STR)
{
	#define M_READ_SIMPLE_TYPE_BASE(,,,,)    \
    else if (final_type == TYPE_NAME)                                         \
    {                                                                         \
        M_FATAL_IF_GT (TYPE_BIT_SIZE, TYPE_IMPL_BIT_SIZE);                    \
                                                                              \
	    int   type_bit_size_for_builder = TYPE_BIT_SIZE;                      \
		T_frame_data    in_out_frame_data_for_builder = in_out_frame_data;    \
                                                                              \
		C_value      obj_value;                                               \
                                                                              \
	    if (field_type_name.is_a_variable() == false)                         \
		{                                                                     \
			const int   bit_position_offset_into_initial_frame = in_out_frame_data.get_bit_offset_into_initial_frame();   \
		    TYPE_IMPL    value = 0;                                           \
			read_decode_data (type_definitions, in_out_frame_data,                   \
					   field_type_name, data_name, data_simple_name,          \
					   &value,                                                \
					   TYPE_NAME, TYPE_BIT_SIZE,                              \
					   TYPE_IMPL_STR, TYPE_IMPL_BIT_SIZE,                     \
					   interpret_data.must_invert_bytes(),                    \
					   final_type[0] == 'i' /* != 'u' */);                    \
			in_out_frame_data_for_builder = in_out_frame_data;                \
			obj_value = value;                                                \
			obj_value.set_bit_position_offset_size(bit_position_offset_into_initial_frame, TYPE_BIT_SIZE);  \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			type_bit_size_for_builder = 0;   /* to avoid hexa data highligth (-1 forbidden) */    \
			if (is_enum)                                                      \
				type_bit_size_for_builder = -1;   /* ICIOA trick to know it is an enum !!! */    \
                                                                              \
			obj_value = compute_expression(type_definitions, interpret_data, in_out_frame_data,  \
										   field_type_name.get_var_expression(),                 \
										   data_name, data_simple_name, os_out, os_err);         \
			if (strcmp(TYPE_NAME, "msg") == 0)                                \
			{                                                                 \
				if (obj_value.get_type() != C_value::E_type_msg)              \
				{                                                             \
					M_FATAL_COMMENT("Expecting msg and expression gives " << obj_value.get_type());    \
				}                                                             \
			}                                                                 \
			else                                                              \
			if (strncmp(TYPE_NAME, "float", 5) != 0)                          \
			{                                                                 \
				TYPE_IMPL  value = static_cast<TYPE_IMPL>(obj_value.get_int());          \
				C_value  value_to_compare(value);                             \
				if (value_to_compare != obj_value)                            \
				{                                                             \
					M_FATAL_COMMENT("Overflow : " << obj_value.get_int() << " gives " << value_to_compare.get_int());         \
				}                                                             \
			}                                                                 \
			const int   bit_position_offset_into_initial_frame = obj_value.get_bit_position_offset();        \
			const int   bit_position_size = obj_value.get_bit_position_size();            \
			if ((bit_position_offset_into_initial_frame >= 0) && (bit_position_size > 0))        \
			{                                                                 \
				type_bit_size_for_builder = bit_position_size;                \
				in_out_frame_data_for_builder.set_bit_offset_into_initial_frame(bit_position_offset_into_initial_frame + type_bit_size_for_builder);    \
				if (is_enum)                                                  \
					type_bit_size_for_builder = -type_bit_size_for_builder;   /* ICIOA trick to know it is an enum !!! */    \
			}                                                                 \
		}                                                                     \
                                                                              \
        if (strncmp (final_type.c_str (), "spare", 5) == 0)                   \
            return  true;                                                     \
                                                                              \
		bool    no_error = true;                                              \
		T_attribute_value    attribute_value;                                 \
		string  str_value;                                                    \
        if (is_enum)                                                          \
            str_value = enum_value_to_attribute_value (obj_value,             \
                                    P_enum_def->definition, attribute_value, no_error);     \
        else                                                                                \
            str_value = simple_value_to_attribute_value_main (                \
								type_definitions, interpret_data,             \
								obj_value, final_type,                        \
                                field_type_name, attribute_value, no_error);  \
                                                                                            \
        interpret_data.add_read_variable (data_name, data_simple_name, attribute_value);    \
                                                                              \
        if (interpret_data.must_NOT_output ())                                \
            return  true;                                                     \
                                                                              \
        os_out << data_name << " ";                                           \
        os_out << "= ";                                                       \
        os_out << str_value << endl;                                          \
        interpret_builder_value(type_definitions, in_out_frame_data_for_builder,          \
                                field_type_name, data_name, data_simple_name, \
								attribute_value, str_value,                   \
						        final_type, type_bit_size_for_builder,        \
						        interpret_data.is_little_endian(), ! no_error);           \
                                                                              \
        return  true;                                                         \
    }

}
#endif

//*****************************************************************************
// frame_to_string ************************************************************
//*****************************************************************************

bool    frame_to_string (const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data_param,
							T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string   & final_type = field_type_name.type;

	M_STATE_ENTER ("frame_to_string", final_type);

	M_FATAL_IF_FALSE ((final_type == "string") ||
					  (final_type == "string_nl"));

	//***************************************************************
	// Compute and verify size
	//***************************************************************
	const string  & str_string_size = field_type_name.str_size_or_parameter;
	long                string_size = -1;
	
	if (str_string_size != "")
	{
		string_size = size_expression_to_int(type_definitions, interpret_data, str_string_size);

		if (string_size < 0)
		{
			os_err << "Error string size= " << str_string_size
				   << " data= " << data_name << endl;
			interpret_builder_error(type_definitions, in_out_frame_data_param,
									field_type_name, data_name, data_simple_name,
									"Not valid size for " + data_simple_name);
			return  false;
		}
	}

	//***************************************************************
	// Decode
	//***************************************************************
	T_frame_data           * P_in_out_frame_data = & in_out_frame_data_param;
	T_decode_stream_frame  & decode_stream_frame = interpret_data.get_decode_stream_frame();

	if ((interpret_data.is_decode_in_progress() == false) &&
		(field_type_name.is_a_variable() == false))
	{
		T_frame_data           & inside_frame = decode_stream_frame.frame_data;

		if (string_size >= 0)
		{
			const long               TYPE_BIT_SIZE = string_size * 8;
			if (inside_frame.get_remaining_bits() >= TYPE_BIT_SIZE)
			{
				// Enough data into inside_frame, so simply use it (decoder is useless for now)
				P_in_out_frame_data = & inside_frame;
			}
			else if (interpret_data.must_decode_now() == false)
			{
				if (inside_frame.get_remaining_bits() > 0)
				{
					// For code simplification, do not manage this case.
					// And seems non-sense : 1st part of data is decoded and 2nd part no.
					M_FATAL_COMMENT("Not enough data into inside_frame (" << inside_frame.get_remaining_bits() << " bits < " <<
									TYPE_BIT_SIZE << ") and no decoder to fill it");
				}
				else
				{
					// No data into inside_frame and no decoder, so simply use normal frame
				}
			}
			else
			{
				// Not enough data into inside_frame, so use decoder to fill it
				M_STATE_ENTER ("read_decode_data must_decode_stream", TYPE_BIT_SIZE);

				decode_data_size (type_definitions, in_out_frame_data_param, interpret_data,
								  field_type_name, data_name, data_simple_name,
								  os_out, os_err,
								  decode_stream_frame,
								  final_type.c_str(), TYPE_BIT_SIZE - inside_frame.get_remaining_bits());

				P_in_out_frame_data = & inside_frame;
			}
		}
		else
		{
			// ICIOA incomplet ?
			if (interpret_data.must_decode_now())
			{
				decode_data_bytes_until (
						  type_definitions, in_out_frame_data_param, interpret_data,
						  field_type_name, data_name, data_simple_name,
						  os_out, os_err,
						  decode_stream_frame,
						  final_type.c_str(),
						  "\0", (final_type == "string_nl") ? "\n" : NULL);
				P_in_out_frame_data = & decode_stream_frame.frame_data;
			}
			else if (inside_frame.get_remaining_bits() > 0)
			{
				P_in_out_frame_data = & inside_frame;
			}
		}
	}
	else
	{
		// inside_frame and decoder not used during decoding
	}


	T_frame_data          & in_out_frame_data = * P_in_out_frame_data;

	// NB: following code NOT optimized after decode implementation

    {
        if ((string_size >= 0) &&
			(in_out_frame_data.get_remaining_bits() < string_size*8) &&
			(field_type_name.is_a_variable() == false))
        {
            os_err << "Error not enough bytes (" << in_out_frame_data.get_remaining_entire_bytes()
                   << " instead of " << string_size
                   << ") for data= " << data_name << endl;
            interpret_builder_missing_data(type_definitions, in_out_frame_data, interpret_data,
				                    field_type_name, data_name, data_simple_name,
									"End of packet : not enough data to read " + data_simple_name);
			M_FATAL_MISSING_DATA();
        }

	    int             type_bit_size_for_builder = 0;    /* to avoid hexa data highligth (-1 forbidden) */
		int             bit_position_offset_into_initial_frame = -1;
		int             bit_position_size = -1;
		T_frame_data    in_out_frame_data_for_builder = in_out_frame_data;

		string        value;

		if (field_type_name.is_a_variable() == false)
		{
			// This is a field (not a variable).
			bit_position_offset_into_initial_frame = in_out_frame_data.get_bit_offset_into_initial_frame();

			const bool    manage_nl = (final_type == "string_nl");
			if (string_size > 0)
			{
				if (in_out_frame_data.is_physically_at_beginning_of_byte() != true)
				{
					value.reserve(string_size);
					for (int   idx_str = 0; idx_str < string_size; ++idx_str)
					{
						const char    current_char = in_out_frame_data.read_1_byte();
						if (current_char == '\0')
						{
							in_out_frame_data.move_forward(string_size - (value.length() + 1), 0);
							break;
						}
						value += current_char;
					}
				}
				else
				{
					value = string ((const char *)in_out_frame_data.get_P_bytes(), 0, string_size);
					in_out_frame_data.move_forward(string_size, 0);
				}

				if (manage_nl)
				{
					string::size_type  idx_sep = value.find ('\n');
					if (idx_sep != string::npos)
					{
						if ((idx_sep > 0) && (value[idx_sep-1] == '\r'))
							--idx_sep;

						value.resize(idx_sep-1);
					}
				}
			}
			else if (string_size == 0)
			{
				value = "";
			}
			else
			{
				string_size = 0;
				while (in_out_frame_data.can_move_1_byte_forward())
				{
					const char    current_char = in_out_frame_data.read_1_byte();
					++string_size;

					if (current_char == '\0')
					{
						break;
					}

					if (manage_nl)
					{
						if (current_char == '\n')
						{
							if ((string_size > 1) && (value[string_size-2] == '\r'))
							{
								value.resize(string_size-2);
							}

							break;
						}
					}

					value += current_char;
				}

				// It is NOT an error to NOT find the zero end of string
			}

			bit_position_size = string_size * 8;
			type_bit_size_for_builder = bit_position_size;
			in_out_frame_data_for_builder = in_out_frame_data;

			// In case of decoding,
			//  the current position could be not good into the orginal frame 
#if 0
			// ICIOA decoder fail
			// il faudrait aussi modifier type_bit_size_for_builder ?
			if (&in_out_frame_data == &decode_frame_data)
			{
				in_out_frame_data_for_builder.set_bit_offset_into_initial_frame(
					in_out_frame_data_param.get_bit_offset_into_initial_frame());
			}
#endif
		}
		else
		{
			// This is a variable (not a field)
			C_value  obj_value = field_type_name.get_var_expression().
				compute_expression(type_definitions, interpret_data, in_out_frame_data,
												    data_name, data_simple_name, os_out, os_err);
			M_FATAL_IF_NE(obj_value.get_type(), C_value::E_type_string);

			value = obj_value.get_str();

			if (string_size >= 0)
			{
				M_FATAL_IF_GT(value.size(), string_size);
			}

			bit_position_offset_into_initial_frame = obj_value.get_bit_position_offset();
			bit_position_size = obj_value.get_bit_position_size();
			if ((bit_position_offset_into_initial_frame >= 0) && (bit_position_size > 0))
			{
				type_bit_size_for_builder = bit_position_size;
				if (in_out_frame_data_for_builder.get_initial_sizeof_bits() != 0) /* 2010/09/13 */
					in_out_frame_data_for_builder.set_bit_offset_into_initial_frame(bit_position_offset_into_initial_frame + type_bit_size_for_builder);
			}
		}

		T_attribute_value    attribute_value(value);

        attribute_value.transformed.set_bit_position_offset_size(bit_position_offset_into_initial_frame, bit_position_size);

		string    error_on_value;
		post_treatment_value(type_definitions, interpret_data, field_type_name,
							 attribute_value.transformed, error_on_value);
		value = attribute_value_to_string(attribute_value);

		interpret_data.add_read_variable (data_name, data_simple_name, attribute_value);

        if (interpret_data.must_output ())
        {
            os_out << data_name << " ";
            os_out << "= ";
            os_out << value;
            os_out << endl;

			interpret_builder_value(type_definitions, in_out_frame_data_for_builder,
									field_type_name, data_name, data_simple_name,
									attribute_value, value,
					                final_type, type_bit_size_for_builder,
									interpret_data.is_little_endian(), false);  // no error
        }

		decode_stream_frame.synchronize();  // mandatory each time something has been read into its frame_data
        return  true;
    }
}

//*****************************************************************************
// frame_to_raw ***************************************************************
//*****************************************************************************

bool    frame_to_raw (const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data_param,
							T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string   & final_type = field_type_name.type;

	M_STATE_ENTER ("frame_to_raw", final_type);

	M_FATAL_IF_FALSE ((final_type == "raw") ||
					  (final_type == "subproto") ||
					  (final_type == "insproto"));

	//***************************************************************
	// Compute and verify size
	//***************************************************************
	const string  & str_string_size = field_type_name.str_size_or_parameter;
	long                string_size = -1;
	
	if (str_string_size != "*")
	{
		string_size = size_expression_to_int(type_definitions, interpret_data, str_string_size);

		if (string_size < 0)
		{
			os_err << "Error raw size= " << str_string_size
				   << " data= " << data_name << endl;
			interpret_builder_error(type_definitions, in_out_frame_data_param,
									field_type_name, data_name, data_simple_name,
									"Not valid size for " + data_simple_name);
			return  false;
		}
	}

	if (in_out_frame_data_param.is_physically_at_beginning_of_byte() != true)
	{
		M_FATAL_COMMENT(final_type << " managed only a entire byte position");  // bsew
	}

	//***************************************************************
	// Decode
	//***************************************************************
	T_frame_data           * P_in_out_frame_data = & in_out_frame_data_param;
	T_decode_stream_frame  & decode_stream_frame = interpret_data.get_decode_stream_frame();

	if ((interpret_data.is_decode_in_progress() == false) &&
		(final_type == "raw"))
	{
		T_frame_data           & inside_frame = decode_stream_frame.frame_data;

		if (string_size >= 0)
		{
			const long               TYPE_BIT_SIZE = string_size * 8;
			if (inside_frame.get_remaining_bits() >= TYPE_BIT_SIZE)
			{
				// Enough data into inside_frame, so simply use it (decoder is useless for now)
				P_in_out_frame_data = & inside_frame;
			}
			else if (interpret_data.must_decode_now() == false)
			{
				if (inside_frame.get_remaining_bits() > 0)
				{
					// For code simplification, do not manage this case.
					// And seems non-sense : 1st part of data is decoded and 2nd part no.
					M_FATAL_COMMENT("Not enough data into inside_frame (" << inside_frame.get_remaining_bits() << " bits < " <<
									TYPE_BIT_SIZE << ") and no decoder to fill it");
				}
				else
				{
					// No data into inside_frame and no decoder, so simply use normal frame
				}
			}
			else
			{
				// Not enough data into inside_frame, so use decoder to fill it
				M_STATE_ENTER ("read_decode_data must_decode_stream", TYPE_BIT_SIZE);

				decode_data_size (type_definitions, in_out_frame_data_param, interpret_data,
								  field_type_name, data_name, data_simple_name,
								  os_out, os_err,
								  decode_stream_frame,
								  final_type.c_str(), TYPE_BIT_SIZE - inside_frame.get_remaining_bits());

				P_in_out_frame_data = & inside_frame;
			}
		}
		else
		{
			// ICIOA complet, non ?
			if (interpret_data.must_decode_now())
			{
				decode_data_bytes_until (
						  type_definitions, in_out_frame_data_param, interpret_data,
						  field_type_name, data_name, data_simple_name,
						  os_out, os_err,
						  decode_stream_frame,
						  final_type.c_str(),
						  NULL, NULL);
				P_in_out_frame_data = & decode_stream_frame.frame_data;
			}
			else if (inside_frame.get_remaining_bits() > 0)
			{
				if (in_out_frame_data_param.get_remaining_bits() > 0)
				{
					// For code simplification, do not manage this case.
					// And seems non-sense : 1st part of data is decoded and 2nd part no.
					M_FATAL_COMMENT("Still data into inside_frame (" << inside_frame.get_remaining_bits() <<
									" bits) and data into packet (" << in_out_frame_data_param.get_remaining_bits() << ")");
				}
				P_in_out_frame_data = & inside_frame;
			}
		}
	}
	else
	{
		// inside_frame and decoder not used during decoding
	}


	T_frame_data          & in_out_frame_data = * P_in_out_frame_data;

	// NB: following code NOT optimized after decode implementation

    {
		const long  frame_data_byte_size = in_out_frame_data.get_remaining_bits() / 8;

		if (str_string_size == "*")
		{
			string_size = frame_data_byte_size;
		}

        if (frame_data_byte_size < string_size)
        {
            os_err << "Error not enough bytes (" << frame_data_byte_size
                   << " instead of " << string_size
                   << ") for data= " << data_name << endl;
            interpret_builder_missing_data(type_definitions, in_out_frame_data, interpret_data,
				                    field_type_name, data_name, data_simple_name,
									"End of packet : not enough data to read " + data_simple_name);
			M_FATAL_MISSING_DATA();
        }

		// with    gives "Malformed packet: <proto>"
		// without gives "Malformed packet: <proto>"
		if (string_size == 0)
        {
            return  true;
		}

        if (interpret_data.must_output ())
        {
            os_out << data_name << " ";
            os_out << "= ";
            os_out << endl;
            dump_buffer (os_out, in_out_frame_data.get_P_bytes(), string_size);
        }

        in_out_frame_data.move_forward(string_size, 0);

        if (interpret_data.must_output ())
        {
			E_raw_data_type    raw_data_type = E_raw_data_any;
			if (final_type == "subproto")
				raw_data_type = E_raw_data_sub_proto;
			else if (final_type == "insproto")
				raw_data_type = E_raw_data_ins_proto;

			interpret_builder_raw_data(type_definitions, in_out_frame_data, interpret_data,
									field_type_name, data_name, data_simple_name,
						            string_size * 8,
									raw_data_type);
        }

		decode_stream_frame.synchronize();  // mandatory each time something has been read into its frame_data
        return  true;
    }
}

//*****************************************************************************
// frame_to_any ***************************************************************
//*****************************************************************************

bool    G_is_a_variable_ICIOA = false;

//*****************************************************************************
// frame_to_any simple type ***************************************************
//*****************************************************************************

    //-------------------------------------------------------------------------
    // Macro for read a simple type ...
	// NB: will NOT work if, for example, sizeof(unsigned int) != 32
    //-------------------------------------------------------------------------

#define M_READ_SIMPLE_TYPE_BASE_BEGIN(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL,TYPE_IMPL_BIT_SIZE,TYPE_IMPL_STR)    \
    {

#define M_READ_SIMPLE_TYPE_BASE_COMPUTE(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL,TYPE_IMPL_BIT_SIZE,TYPE_IMPL_STR)    \
        M_FATAL_IF_GT (TYPE_BIT_SIZE, TYPE_IMPL_BIT_SIZE);                    \
                                                                              \
	    int   type_bit_size_for_builder = TYPE_BIT_SIZE;                      \
		T_frame_data    in_out_frame_data_for_builder = in_out_frame_data;    \
                                                                              \
		C_value      obj_value;                                               \
		obj_value.set_external_type(final_type);                              \
                                                                              \
	    if (field_type_name.is_a_variable() == true)                          \
		{                                                                     \
			type_bit_size_for_builder = 0;   /* to avoid hexa data highligth (-1 forbidden) */    \
			if (is_enum)                                                      \
				type_bit_size_for_builder = -1;   /* ICIOA trick to know it is an enum !!! */    \
                                                                              \
			obj_value = field_type_name.get_var_expression().								\
				compute_expression(type_definitions, interpret_data, in_out_frame_data,     \
										   data_name, data_simple_name, os_out, os_err);    \
			if (strcmp(TYPE_NAME, "msg") == 0)                                \
			{                                                                 \
				if (obj_value.get_type() != C_value::E_type_msg)              \
				{                                                             \
					M_FATAL_COMMENT("Expecting msg and expression gives " << obj_value.get_type());    \
				}                                                             \
			}                                                                 \
			else                                                              \
			if (strncmp(TYPE_NAME, "float", 5) != 0)                          \
			{                                                                 \
				TYPE_IMPL  value = static_cast<TYPE_IMPL>(obj_value.get_int());          \
				C_value  value_to_compare(value);                             \
				if (value_to_compare != obj_value)                            \
				{                                                             \
					M_FATAL_COMMENT("Overflow : " << obj_value.get_int() << " gives " << value_to_compare.get_int());         \
				}                                                             \
			}                                                                 \
			const int   bit_position_offset_into_initial_frame = obj_value.get_bit_position_offset();        \
			const int   bit_position_size = obj_value.get_bit_position_size();            \
			if ((bit_position_offset_into_initial_frame >= 0) && (bit_position_size > 0))        \
			{                                                                 \
				type_bit_size_for_builder = bit_position_size;                \
				if (in_out_frame_data_for_builder.get_initial_sizeof_bits() != 0) /* 2010/09/13 */ \
					in_out_frame_data_for_builder.set_bit_offset_into_initial_frame(bit_position_offset_into_initial_frame + type_bit_size_for_builder);    \
				if (is_enum)                                                  \
					type_bit_size_for_builder = -type_bit_size_for_builder;   /* ICIOA trick to know it is an enum !!! */    \
			}                                                                 \
		}                                                                     \
		else if (G_is_a_variable_ICIOA == true)                               \
		{                                                                     \
			type_bit_size_for_builder = 0;   /* to avoid hexa data highligth (-1 forbidden) */    \
			if (is_enum)                                                      \
				type_bit_size_for_builder = -1;   /* ICIOA trick to know it is an enum !!! */    \
                                                                              \
			obj_value = C_value(0);                                           \
			if (strcmp(TYPE_NAME, "msg") == 0)                                \
			{                                                                 \
				obj_value = C_value(C_value::E_type_msg, NULL);               \
			}                                                                 \
			else                                                              \
			if (strncmp(TYPE_NAME, "float", 5) != 0)                          \
			{                                                                 \
				obj_value = C_value(0.0);                                     \
			}                                                                 \
			const int   bit_position_offset_into_initial_frame = obj_value.get_bit_position_offset();        \
			const int   bit_position_size = obj_value.get_bit_position_size();            \
			if ((bit_position_offset_into_initial_frame >= 0) && (bit_position_size > 0))        \
			{                                                                 \
				type_bit_size_for_builder = bit_position_size;                \
				if (in_out_frame_data_for_builder.get_initial_sizeof_bits() != 0) /* 2010/09/13 */ \
					in_out_frame_data_for_builder.set_bit_offset_into_initial_frame(bit_position_offset_into_initial_frame + type_bit_size_for_builder);    \
				if (is_enum)                                                  \
					type_bit_size_for_builder = -type_bit_size_for_builder;   /* ICIOA trick to know it is an enum !!! */    \
			}                                                                 \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			const int   bit_position_offset_into_initial_frame = in_out_frame_data.get_bit_offset_into_initial_frame();   \
		    TYPE_IMPL    value = 0;                                           \
			read_decode_data (type_definitions, in_out_frame_data,            \
					   interpret_data,                                        \
					   field_type_name, data_name, data_simple_name,          \
					   os_out, os_err,                                        \
					   &value,                                                \
					   TYPE_NAME, TYPE_BIT_SIZE,                              \
					   TYPE_IMPL_STR, TYPE_IMPL_BIT_SIZE,                     \
					   interpret_data.must_invert_bytes(),                    \
					   final_type[0] == 'i' /* != 'u' */);                    \
			obj_value = value;                                                \
			/* encoded size could differ from TYPE_BIT_SIZE */                \
			obj_value.set_bit_position_offset_size(                           \
				bit_position_offset_into_initial_frame,                       \
				in_out_frame_data.get_bit_offset() - in_out_frame_data_for_builder.get_bit_offset());  \
			in_out_frame_data_for_builder = in_out_frame_data;                \
		}

#define M_READ_SIMPLE_TYPE_BASE_ADD_OUTPUT(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL,TYPE_IMPL_BIT_SIZE,TYPE_IMPL_STR)    \
		bool    no_error = true;                                              \
		T_attribute_value    attribute_value;                                 \
		string  str_value;                                                    \
        if (is_enum)                                                          \
            str_value = enum_value_to_attribute_value (                       \
								type_definitions, interpret_data,             \
                                obj_value,                                    \
                                P_enum_def->definition,                       \
                                field_type_name, attribute_value, no_error);  \
        else                                                                  \
            str_value = simple_value_to_attribute_value_main (                \
								type_definitions, interpret_data,             \
								obj_value, final_type,                        \
                                field_type_name, attribute_value, no_error);  \
                                                                              \
        interpret_data.add_read_variable (data_name, data_simple_name, attribute_value);    \
                                                                              \
        if (interpret_data.must_NOT_output ())                                \
            return  true;                                                     \
                                                                              \
        os_out << data_name << " ";                                           \
        os_out << "= ";                                                       \
        os_out << str_value << endl;                                          \
        interpret_builder_value(type_definitions, in_out_frame_data_for_builder,          \
                                field_type_name, data_name, data_simple_name, \
								attribute_value, str_value,                   \
						        final_type, type_bit_size_for_builder,        \
						        interpret_data.is_little_endian(), ! no_error);

#define M_READ_SIMPLE_TYPE_BASE_END(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL,TYPE_IMPL_BIT_SIZE,TYPE_IMPL_STR)    \
        return  true;                                                         \
    }



#define M_FCT_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL)             \
bool    frame_to_any_ ## TYPE_NAME                                            \
                     (const T_type_definitions    & type_definitions,         \
					        T_frame_data          & in_out_frame_data,        \
					        T_interpret_data      & interpret_data,           \
                      const T_field_type_name     & field_type_name,          \
                      const string                & data_name,                \
                      const string                & data_simple_name,         \
                            ostream               & os_out,                   \
                            ostream               & os_err)                   \
{                                                                             \
	const string                            & final_type = field_type_name.type;        \
    bool                                      is_enum = false;        \
	const T_enum_definition_representation  * P_enum_def = NULL;        \
	M_READ_SIMPLE_TYPE_BASE_BEGIN(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)       \
	M_READ_SIMPLE_TYPE_BASE_COMPUTE(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)     \
	M_READ_SIMPLE_TYPE_BASE_ADD_OUTPUT(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)  \
	M_READ_SIMPLE_TYPE_BASE_END(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)         \
}                                                                             \
bool    frame_to_any_ ## TYPE_NAME ## _enum                                           \
                     (const T_type_definitions    & type_definitions,         \
					        T_frame_data          & in_out_frame_data,        \
					        T_interpret_data      & interpret_data,           \
                      const T_field_type_name     & field_type_name,          \
                      const string                & data_name,                \
                      const string                & data_simple_name,         \
                            ostream               & os_out,                   \
                            ostream               & os_err)                   \
{                                                                             \
    bool                                      is_enum = true;        \
	const T_enum_definition_representation  * P_enum_def = field_type_name.P_type_enum_def;        \
	const string                            & final_type = P_enum_def->representation_type;        \
	M_READ_SIMPLE_TYPE_BASE_BEGIN(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)       \
	M_READ_SIMPLE_TYPE_BASE_COMPUTE(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)     \
	M_READ_SIMPLE_TYPE_BASE_ADD_OUTPUT(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)  \
	M_READ_SIMPLE_TYPE_BASE_END(#TYPE_NAME, TYPE_BIT_SIZE, TYPE_IMPL, sizeof(TYPE_IMPL)*8, #TYPE_IMPL)         \
}



M_FCT_READ_SIMPLE_TYPE (uint1,    1,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int2,    2,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint2,    2,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int3,    3,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint3,    3,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int4,    4,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint4,    4,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int5,    5,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint5,    5,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int6,    6,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint6,    6,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int7,    7,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint7,    7,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int8,    8,    signed char)
M_FCT_READ_SIMPLE_TYPE (uint8,    8,  unsigned char)
M_FCT_READ_SIMPLE_TYPE ( int9,    9,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint9,    9,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int10,  10,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint10,  10,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int11,  11,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint11,  11,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int12,  12,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint12,  12,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int13,  13,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint13,  13,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int14,  14,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint14,  14,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int15,  15,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint15,  15,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int16,  16,    signed short)
M_FCT_READ_SIMPLE_TYPE (uint16,  16,  unsigned short)
M_FCT_READ_SIMPLE_TYPE ( int17,  17,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint17,  17,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int18,  18,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint18,  18,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int19,  19,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint19,  19,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int20,  20,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint20,  20,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int21,  21,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint21,  21,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int22,  22,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint22,  22,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int23,  23,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint23,  23,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int24,  24,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint24,  24,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int25,  25,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint25,  25,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int26,  26,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint26,  26,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int27,  27,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint27,  27,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int28,  28,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint28,  28,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int29,  29,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint29,  29,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int30,  30,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint30,  30,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int31,  31,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint31,  31,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int32,  32,    signed int)
M_FCT_READ_SIMPLE_TYPE (uint32,  32,  unsigned int)
M_FCT_READ_SIMPLE_TYPE ( int40,  40,    signed long long)
M_FCT_READ_SIMPLE_TYPE (uint40,  40,  unsigned long long)
M_FCT_READ_SIMPLE_TYPE ( int48,  48,    signed long long)
M_FCT_READ_SIMPLE_TYPE (uint48,  48,  unsigned long long)
M_FCT_READ_SIMPLE_TYPE ( int64,  64,    signed long long)
M_FCT_READ_SIMPLE_TYPE (uint64,  64,  unsigned long long)
M_FCT_READ_SIMPLE_TYPE (float32, 32, float)
M_FCT_READ_SIMPLE_TYPE (float64, 64, double)
// alias M_FCT_READ_SIMPLE_TYPE (bool,      sizeof(bool)*8,  bool)
// enum  M_FCT_READ_SIMPLE_TYPE (bool8,    8, unsigned char)
// enum  M_FCT_READ_SIMPLE_TYPE (bool16,  16, unsigned short)
// enum  M_FCT_READ_SIMPLE_TYPE (bool32,  32, unsigned int)
M_FCT_READ_SIMPLE_TYPE ( char,    8,          char)
M_FCT_READ_SIMPLE_TYPE (schar,    8,   signed char)
M_FCT_READ_SIMPLE_TYPE (uchar,    8, unsigned char)
M_FCT_READ_SIMPLE_TYPE (msg,      1, unsigned char)

#define M_READ_SIMPLE_TYPE_BASE_ADD_OUTPUT(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL,TYPE_IMPL_BIT_SIZE,TYPE_IMPL_STR)
M_FCT_READ_SIMPLE_TYPE (spare,    8, unsigned char)

//*****************************************************************************
// frame_to_any_set ***********************************************************
//*****************************************************************************

bool    frame_to_any_set (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	// ICIOA must verify resulting value is compatible with already recorded value ?
	const C_value    value = field_type_name.get_set_expression().
		compute_expression(type_definitions, interpret_data, in_out_frame_data,
							    data_name, data_simple_name, os_out, os_err);
	// Manage expressions inside arrays
	if (data_simple_name.rfind(']') != string::npos)
	{
		const string  new_name = compute_expressions_in_array(type_definitions, interpret_data, in_out_frame_data, data_simple_name, data_name, data_simple_name, os_out, os_err);
		interpret_data.set_read_variable (new_name, value);
	}
	else
	{
		interpret_data.set_read_variable (data_simple_name, value);
	}

	return  true;
}

//*****************************************************************************
// frame_to_any_call **********************************************************
//*****************************************************************************

bool    frame_to_any_call (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const T_function_definition  & fct_def = type_definitions.get_function(field_type_name.name);

	if (frame_to_function (type_definitions,
		                 interpret_data,
		                 in_out_frame_data,
                         fct_def,
						 field_type_name,
                         data_name,
                         data_simple_name,
                         os_out, os_err) != true)
    {
        os_err << "Error function call "
               << " data= " << data_name << endl;
        return  false;
    }

	return  true;
}

//*****************************************************************************
// frame_to_any_struct ********************************************************
//*****************************************************************************

bool    frame_to_any_struct (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const T_struct_definition  * P_struct_def = field_type_name.P_type_struct_def;

	if ((field_type_name.is_a_variable() == true) &&
		(field_type_name.get_var_expression().get_original_string_expression() != "zero"))
	{
		// Compute expression -> name of the original struct
		C_value  obj_value = field_type_name.get_var_expression().
			compute_expression(
								type_definitions, interpret_data, in_out_frame_data,
								data_name, data_simple_name, os_out, os_err);

		// Duplicate with new name (which already specified into name group)
		interpret_data.duplicate_multiple_values(obj_value.get_str(), "");

		// ICIOA affichage !

		return  true;
	}

	bool    save_G_is_a_variable_ICIOA = G_is_a_variable_ICIOA;
	if (field_type_name.is_a_variable() == true)
	{
		// Behavior with commands, loops, conditional ...
		//  is absolutely not guaranteed.
		G_is_a_variable_ICIOA = true;
	}

    if (frame_to_struct (type_definitions,
		                 in_out_frame_data,
						 interpret_data,
                        *P_struct_def,
                         data_name,
                         data_simple_name,
                         os_out, os_err) != true)
    {
		G_is_a_variable_ICIOA = save_G_is_a_variable_ICIOA;
        os_err << "Error struct "
               << " data= " << data_name << endl;
        return  false;
    }
	G_is_a_variable_ICIOA = save_G_is_a_variable_ICIOA;

	return  true;
}

//*****************************************************************************
// frame_to_any_switch ********************************************************
//*****************************************************************************

bool    frame_to_any_switch (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const T_switch_definition  * P_switch = field_type_name.P_type_switch_def;

	if (frame_to_switch (type_definitions,
					 in_out_frame_data,
					 interpret_data,
					*P_switch,
					 field_type_name.str_size_or_parameter,
					 field_type_name,
					 data_name,
					 data_simple_name,
					 os_out, os_err) != true)
	{
		// Error notification done into the function.
		return  false;
	}

	return  true;
}

//*****************************************************************************
// frame_to_any_bitfield ******************************************************
//*****************************************************************************

bool    frame_to_any_bitfield (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const T_bitfield_definition  * P_bitfield_def = field_type_name.P_type_bitfield_def;

	if (frame_to_bitfield (type_definitions,
		                 in_out_frame_data,
						 interpret_data,
						*P_bitfield_def,
                         data_name,
                         data_simple_name,
                         os_out, os_err) != true)
    {
        os_err << "Error bitfield fields_definition"
               << " data= " << data_name << endl;
        return  false;
    }

	return  true;
}

//*****************************************************************************
// frame_to_any_padding_bits **************************************************
//*****************************************************************************

bool    frame_to_any_padding_bits (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const short   remaining_bits_in_byte = in_out_frame_data.get_remaining_bits() % 8;
	in_out_frame_data.move_bit_forward(remaining_bits_in_byte);
    return  true;
}

//*****************************************************************************
// frame_to_any_print *********************************************************
//*****************************************************************************

bool    frame_to_any_print (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string  & final_type = field_type_name.type;

	return  frame_to_print (type_definitions,
							in_out_frame_data,
							interpret_data,
							field_type_name,
							final_type, data_name, data_simple_name,
							os_out, os_err);
}

//*****************************************************************************
// frame_to_any_error *********************************************************
//*****************************************************************************

bool    frame_to_any_error (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string  & final_type = field_type_name.type;

	return  frame_to_print_any(type_definitions, in_out_frame_data,
					   interpret_data,
					   field_type_name, final_type, data_name, data_simple_name,
					   os_out, os_err,
					   interpret_builder_cmd_error);
}

//*****************************************************************************
// frame_to_any_fatal *********************************************************
//*****************************************************************************

bool    frame_to_any_fatal (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string  & final_type = field_type_name.type;

	frame_to_print_any(type_definitions, in_out_frame_data,
					   interpret_data,
					   field_type_name, final_type, data_name, data_simple_name,
					   os_out, os_err,
					   interpret_builder_cmd_fatal);
    return  false;
}

//*****************************************************************************
// frame_to_any_output ********************************************************
//*****************************************************************************

bool    frame_to_any_output (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
    //-------------------------------------------------------------------------
    // Output (of data read) control.
    //-------------------------------------------------------------------------

    if (data_simple_name == "--")
        interpret_data.decr_output_level ();
    else if (data_simple_name == "++")
        interpret_data.incr_output_level ();
    else
    {
        os_err << "Error output  " << data_name << endl;
        interpret_builder_error(type_definitions, in_out_frame_data,
			                    field_type_name, data_name, data_simple_name,
								"Error unknow output value " + data_simple_name);
        return  false;
    }

    return  true;
}

//*****************************************************************************
// frame_to_any_byte_order ****************************************************
//*****************************************************************************

bool    frame_to_any_byte_order (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string  & final_type = field_type_name.type;

    //-------------------------------------------------------------------------
    // Byte order (of data read) control.
    //-------------------------------------------------------------------------
    if (data_simple_name == "big_endian")
    {
        interpret_data.set_big_endian();
    }
    else if (data_simple_name == "little_endian")
    {
        interpret_data.set_little_endian();
    }
    else if (data_simple_name == "as_host")
    {
        interpret_data.set_as_host();
    }
    else
    {
        os_err << "Error " <<  final_type << " " << data_name << endl;
        interpret_builder_error(type_definitions, in_out_frame_data,
			                    field_type_name, data_name, data_simple_name,
								"Error unknow byte_order " + data_simple_name);
        return  false;
    }

    return  true;
}

//*****************************************************************************
// frame_to_any_decoder *******************************************************
//*****************************************************************************

bool    frame_to_any_decoder (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	interpret_data.set_decode_function(data_simple_name);
    return  true;
}

//*****************************************************************************
// frame_to_any_save_position *************************************************
//*****************************************************************************

bool    frame_to_any_save_position (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
    //-------------------------------------------------------------------------
    // Position into data to read.
    //-------------------------------------------------------------------------
    const string  & pos_name = data_simple_name;
    interpret_data.add_read_variable (data_name, pos_name,
									  in_out_frame_data.get_bit_offset_into_initial_frame());
    return  true;
}

//*****************************************************************************
// frame_to_any_position ******************************************************
//*****************************************************************************

bool    frame_to_any_position (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string  & final_type = field_type_name.type;

    //-------------------------------------------------------------------------
    // Position into data to read.
    //-------------------------------------------------------------------------
    long                  bit_offset_pos = 0;

	if (final_type == "goto_position")
	{
		const string  & pos_name = data_simple_name;
		long long       value_int_ll;
		if (get_complex_variable_integer_value (interpret_data,
												pos_name,
												value_int_ll) != E_rc_ok)
		{
			os_err << "Error unknow position " << data_name << endl;
			interpret_builder_error(type_definitions, in_out_frame_data,
									field_type_name, data_name, data_simple_name,
									"Error unknow position " + data_simple_name);
			return  false;
		}

		bit_offset_pos = static_cast<long>(value_int_ll) - in_out_frame_data.get_bit_offset_into_initial_frame();
	}
	else
	{
		const string  & offset_pos_name = data_simple_name;
		long long       value_int_ll;
		if (get_complex_variable_integer_value (interpret_data,
												offset_pos_name,
												value_int_ll) != E_rc_ok)
		{
			os_err << "Error unknow position " << data_name << endl;
			interpret_builder_error(type_definitions, in_out_frame_data,
									field_type_name, data_name, data_simple_name,
									"Error unknow position " + data_simple_name);
			return  false;
		}

		bit_offset_pos = static_cast<long>(value_int_ll);
		if (final_type == "move_position_bytes")
		{
			bit_offset_pos *= 8;
		}
	}

    const long    bit_offset_pos_after_end = bit_offset_pos - in_out_frame_data.get_remaining_bits();
    if (bit_offset_pos_after_end > 0)
    {
        os_err << "Error " << final_type << " " << data_name
               << " goes " << bit_offset_pos_after_end
               << " bits after end of data." << endl;
        interpret_builder_error(type_definitions, in_out_frame_data,
			                    field_type_name, data_name, data_simple_name,
								"Error " + final_type + " " + data_simple_name +
								" goes " + get_string(bit_offset_pos_after_end) +
								" bits after end of data.");
        return  false;
    }

	const long    bit_offset_pos_before_begin = -bit_offset_pos - in_out_frame_data.get_bit_offset();
    if (bit_offset_pos_before_begin > 0)
    {
        os_err << "Error " << final_type << " " << data_name
               << " goes " << bit_offset_pos_before_begin
               << " bits before beginning of data." << endl;
        interpret_builder_error(type_definitions, in_out_frame_data,
			                    field_type_name, data_name, data_simple_name,
								"Error " + final_type + " " + data_simple_name +
								" goes " + get_string(bit_offset_pos_before_begin) +
								" bits before beginning of data.");
        return  false;
    }

	in_out_frame_data.move_bit(bit_offset_pos);

    return  true;
}

//*****************************************************************************
// frame_to_any_check_eof_distance ********************************************
//*****************************************************************************

bool    frame_to_any_check_eof_distance (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	const string  & final_type = field_type_name.type;

	//-------------------------------------------------------------------------
	// Check End Of Frame distance.
	// I.e. check the number of bytes not already read.
	//-------------------------------------------------------------------------

	long    distance = -1;

    if (get_number (data_simple_name.c_str (), &distance) == false)
    {
        os_err << "Error " << final_type << " " << data_name
               << " is NOT a number." << endl;
        interpret_builder_error(type_definitions, in_out_frame_data,
			                    field_type_name, data_name, data_simple_name,
								"Error " + final_type + " " + data_simple_name + " is not a number");
        return  false;
    }
	if (final_type == "check_eof_distance_bytes")
	{
		distance *= 8;
	}

    if (distance != in_out_frame_data.get_remaining_bits())
    {
        os_err << "Error " << final_type << " " << distance
               << " bits != " << in_out_frame_data.get_remaining_bits()
			   << " bits"
               << endl;
        interpret_builder_error(type_definitions, in_out_frame_data,
			                    field_type_name, data_name, data_simple_name,
								"Error " + final_type + " " + get_string(distance) + " bits != " +
								 get_string(in_out_frame_data.get_remaining_bits()) + " bits");
        return  false;
    }

    return  true;
}

//*****************************************************************************
// frame_to_any_chrono ********************************************************
//*****************************************************************************

bool    frame_to_any_chrono (
				      const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
    //-------------------------------------------------------------------------
    // Chrono.
	// Display some performance measure.
    // For integration only.
    //-------------------------------------------------------------------------
	const string  & chrono_cmd = field_type_name.name;
	const string    chrono_val = interpret_data.compute_chrono_value_from_command(chrono_cmd);
	if (chrono_val != "")
	{
		return  frame_to_print (type_definitions,
								in_out_frame_data,
								interpret_data,
								field_type_name,
								"print", chrono_val, chrono_val,
								os_out, os_err);
	}

    return  true;
}

//*****************************************************************************
// build_types_finalize field *************************************************
//*****************************************************************************

void    build_types_finalize_itself(T_type_definitions  & type_definitions,
							        T_field_type_name   & field_type_name)
{
	string    final_type = field_type_name.type;

	// frame_to_field 1st

	if (final_type == "deep_break")
    {
		field_type_name.pf_frame_to_field = frame_to_field_deep_break;
		return;
	}

    if (final_type == "deep_continue")
    {
		field_type_name.pf_frame_to_field = frame_to_field_deep_continue;
		return;
	}

    if (final_type == "break")
    {
		field_type_name.pf_frame_to_field = frame_to_field_break;
		return;
	}

    if (final_type == "continue")
    {
		field_type_name.pf_frame_to_field = frame_to_field_continue;
		return;
	}

    if (final_type == "return")
    {
		field_type_name.pf_frame_to_field = frame_to_field_return;
		return;
	}

    if ((final_type == "while") ||
		(final_type == "do_while"))
    {
		field_type_name.pf_frame_to_field = frame_to_field_while;
		return;
	}

    if ((final_type == "loop_size_bytes") ||
        (final_type == "loop_size_bits"))
    {
		field_type_name.pf_frame_to_field = frame_to_field_loop_size;
		return;
	}

    if (final_type == "loop_nb_times")
    {
		field_type_name.pf_frame_to_field = frame_to_field_loop_nb_times;
		return;
	}

    if (final_type == "if")
    {
		field_type_name.pf_frame_to_field = frame_to_field_if;
		return;
	}

	// frame_to_field other

    {
		field_type_name.pf_frame_to_field = frame_to_field_other;
		// no return;
	}

	// frame_to_any then

    if (final_type == "set")
    {
		field_type_name.pf_frame_to_any = frame_to_any_set;
		return;
	}

    if (final_type == "call")
    {
		field_type_name.pf_frame_to_any = frame_to_any_call;
		return;
	}

    {
		const T_struct_definition  * P_struct_def = type_definitions.get_P_struct(final_type);
		if (P_struct_def != NULL)
        {
			field_type_name.pf_frame_to_any = frame_to_any_struct;
			field_type_name.P_type_struct_def = P_struct_def;
			return;
        }
    }

    //-------------------------------------------------------------------------
    // Enum ?
    //-------------------------------------------------------------------------
    bool    is_enum = false;
	{
		const T_enum_definition_representation  * P_enum_def = type_definitions.get_P_enum(final_type);
		if (P_enum_def != NULL)
		{
			field_type_name.P_type_enum_def = P_enum_def;
			is_enum = true;

			final_type = P_enum_def->representation_type;
		}
	}

    //-------------------------------------------------------------------------
    // Macro for read a simple type ...
    //-------------------------------------------------------------------------
#undef  M_READ_SIMPLE_TYPE
#define M_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_BIT_SIZE,TYPE_IMPL)                 \
	if (final_type == #TYPE_NAME)                                             \
	{                                                                         \
		field_type_name.pf_frame_to_any = frame_to_any_ ##  TYPE_NAME;        \
		if (is_enum == true)                                                  \
		{                                                                     \
			field_type_name.pf_frame_to_any = frame_to_any_ ##  TYPE_NAME ## _enum;  \
		}                                                                     \
		return;                                                               \
	}


    M_READ_SIMPLE_TYPE (uint1,    1,  unsigned int)
    M_READ_SIMPLE_TYPE ( int2,    2,    signed int)
    M_READ_SIMPLE_TYPE (uint2,    2,  unsigned int)
    M_READ_SIMPLE_TYPE ( int3,    3,    signed int)
    M_READ_SIMPLE_TYPE (uint3,    3,  unsigned int)
    M_READ_SIMPLE_TYPE ( int4,    4,    signed int)
    M_READ_SIMPLE_TYPE (uint4,    4,  unsigned int)
    M_READ_SIMPLE_TYPE ( int5,    5,    signed int)
    M_READ_SIMPLE_TYPE (uint5,    5,  unsigned int)
    M_READ_SIMPLE_TYPE ( int6,    6,    signed int)
    M_READ_SIMPLE_TYPE (uint6,    6,  unsigned int)
    M_READ_SIMPLE_TYPE ( int7,    7,    signed int)
    M_READ_SIMPLE_TYPE (uint7,    7,  unsigned int)
    M_READ_SIMPLE_TYPE ( int8,    8,    signed char)
    M_READ_SIMPLE_TYPE (uint8,    8,  unsigned char)

    M_READ_SIMPLE_TYPE ( int9,    9,    signed int)
    M_READ_SIMPLE_TYPE (uint9,    9,  unsigned int)
    M_READ_SIMPLE_TYPE ( int10,  10,    signed int)
    M_READ_SIMPLE_TYPE (uint10,  10,  unsigned int)
    M_READ_SIMPLE_TYPE ( int11,  11,    signed int)
    M_READ_SIMPLE_TYPE (uint11,  11,  unsigned int)
    M_READ_SIMPLE_TYPE ( int12,  12,    signed int)
    M_READ_SIMPLE_TYPE (uint12,  12,  unsigned int)
    M_READ_SIMPLE_TYPE ( int13,  13,    signed int)
    M_READ_SIMPLE_TYPE (uint13,  13,  unsigned int)
    M_READ_SIMPLE_TYPE ( int14,  14,    signed int)
    M_READ_SIMPLE_TYPE (uint14,  14,  unsigned int)
    M_READ_SIMPLE_TYPE ( int15,  15,    signed int)
    M_READ_SIMPLE_TYPE (uint15,  15,  unsigned int)
    M_READ_SIMPLE_TYPE ( int16,  16,    signed short)
    M_READ_SIMPLE_TYPE (uint16,  16,  unsigned short)

    M_READ_SIMPLE_TYPE ( int17,  17,    signed int)
    M_READ_SIMPLE_TYPE (uint17,  17,  unsigned int)
    M_READ_SIMPLE_TYPE ( int18,  18,    signed int)
    M_READ_SIMPLE_TYPE (uint18,  18,  unsigned int)
    M_READ_SIMPLE_TYPE ( int19,  19,    signed int)
    M_READ_SIMPLE_TYPE (uint19,  19,  unsigned int)
    M_READ_SIMPLE_TYPE ( int20,  20,    signed int)
    M_READ_SIMPLE_TYPE (uint20,  20,  unsigned int)
    M_READ_SIMPLE_TYPE ( int21,  21,    signed int)
    M_READ_SIMPLE_TYPE (uint21,  21,  unsigned int)
    M_READ_SIMPLE_TYPE ( int22,  22,    signed int)
    M_READ_SIMPLE_TYPE (uint22,  22,  unsigned int)
    M_READ_SIMPLE_TYPE ( int23,  23,    signed int)
    M_READ_SIMPLE_TYPE (uint23,  23,  unsigned int)
    M_READ_SIMPLE_TYPE ( int24,  24,    signed int)
    M_READ_SIMPLE_TYPE (uint24,  24,  unsigned int)

    M_READ_SIMPLE_TYPE ( int25,  25,    signed int)
    M_READ_SIMPLE_TYPE (uint25,  25,  unsigned int)
    M_READ_SIMPLE_TYPE ( int26,  26,    signed int)
    M_READ_SIMPLE_TYPE (uint26,  26,  unsigned int)
    M_READ_SIMPLE_TYPE ( int27,  27,    signed int)
    M_READ_SIMPLE_TYPE (uint27,  27,  unsigned int)
    M_READ_SIMPLE_TYPE ( int28,  28,    signed int)
    M_READ_SIMPLE_TYPE (uint28,  28,  unsigned int)
    M_READ_SIMPLE_TYPE ( int29,  29,    signed int)
    M_READ_SIMPLE_TYPE (uint29,  29,  unsigned int)
    M_READ_SIMPLE_TYPE ( int30,  30,    signed int)
    M_READ_SIMPLE_TYPE (uint30,  30,  unsigned int)
    M_READ_SIMPLE_TYPE ( int31,  31,    signed int)
    M_READ_SIMPLE_TYPE (uint31,  31,  unsigned int)
    M_READ_SIMPLE_TYPE ( int32,  32,    signed int)
    M_READ_SIMPLE_TYPE (uint32,  32,  unsigned int)

    M_READ_SIMPLE_TYPE ( int40,  40,    signed long long)
    M_READ_SIMPLE_TYPE (uint40,  40,  unsigned long long)
    M_READ_SIMPLE_TYPE ( int48,  48,    signed long long)
    M_READ_SIMPLE_TYPE (uint48,  48,  unsigned long long)
    M_READ_SIMPLE_TYPE ( int64,  64,    signed long long)
    M_READ_SIMPLE_TYPE (uint64,  64,  unsigned long long)
    M_READ_SIMPLE_TYPE (float32, 32, float)
    M_READ_SIMPLE_TYPE (float64, 64, double)
// alias    M_READ_SIMPLE_TYPE ("bool",      sizeof(bool)*8,  bool)
// enum     M_READ_SIMPLE_TYPE ("bool8",    8, unsigned char)
// enum     M_READ_SIMPLE_TYPE ("bool16",  16, unsigned short)
// enum     M_READ_SIMPLE_TYPE ("bool32",  32, unsigned int)
    M_READ_SIMPLE_TYPE (spare,    8, unsigned char)
    M_READ_SIMPLE_TYPE ( char,    8,          char)
    M_READ_SIMPLE_TYPE (schar,    8,   signed char)
    M_READ_SIMPLE_TYPE (uchar,    8, unsigned char)
    M_READ_SIMPLE_TYPE (msg,      1, unsigned char)


	if ((final_type == "string") ||
		(final_type == "string_nl"))
    {
		field_type_name.pf_frame_to_any = frame_to_string;
		return;
    }

	if ((final_type == "raw") ||
		(final_type == "subproto") ||
		(final_type == "insproto"))
    {
		field_type_name.pf_frame_to_any = frame_to_raw;
		return;
    }

    {
		const T_switch_definition  * P_switch = type_definitions.get_P_switch(final_type);
		if (P_switch != NULL_PTR)
		{
			field_type_name.pf_frame_to_any = frame_to_any_switch;
			field_type_name.P_type_switch_def = P_switch;
			return;
        }
    }

	{
		const T_bitfield_definition  * P_bitfield_def = type_definitions.get_P_bitfield(final_type);
		if (P_bitfield_def != NULL)
		{
			field_type_name.pf_frame_to_any = frame_to_any_bitfield;
			field_type_name.P_type_bitfield_def = P_bitfield_def;
			return;
		}
	}

    if (final_type == "padding_bits")
    {
		field_type_name.pf_frame_to_any = frame_to_any_padding_bits;
		return;
    }

    if ((final_type == "debug_print") || (final_type == "print"))
    {
		field_type_name.pf_frame_to_any = frame_to_any_print;
		return;
    }

    if ((final_type == "chat") ||
		(final_type == "note") ||
		(final_type == "warning") ||
		(final_type == "error"))
    {
		field_type_name.pf_frame_to_any = frame_to_any_error;
		return;
    }

    if (final_type == "fatal")
    {
		field_type_name.pf_frame_to_any = frame_to_any_fatal;
		return;
    }

    if (final_type == "output")
    {
		field_type_name.pf_frame_to_any = frame_to_any_output;
		return;
    }

    if (final_type == "byte_order")
    {
		field_type_name.pf_frame_to_any = frame_to_any_byte_order;
		return;
    }

    if (final_type == "decoder")
    {
		field_type_name.pf_frame_to_any = frame_to_any_decoder;
		return;
    }

    if (final_type == "save_position")
    {
		field_type_name.pf_frame_to_any = frame_to_any_save_position;
		return;
    }

    if ((final_type == "goto_position") ||
		(final_type == "move_position_bytes") ||
		(final_type == "move_position_bits"))
	{
		field_type_name.pf_frame_to_any = frame_to_any_position;
		return;
    }

    if ((final_type == "check_eof_distance_bytes") ||
		(final_type == "check_eof_distance_bits"))
    {
		field_type_name.pf_frame_to_any = frame_to_any_check_eof_distance;
		return;
    }

    if (final_type == "chrono")
    {
		field_type_name.pf_frame_to_any = frame_to_any_chrono;
		return;
    }
}

void    build_types_finalize_itself(const T_type_definitions  & type_definitions,
							        const T_field_type_name   & field_type_name)
{
	build_types_finalize_itself(const_cast<T_type_definitions &>(type_definitions),
		                        const_cast<T_field_type_name &>(field_type_name));
}

//*****************************************************************************
// C_interpret_forget_data
// Permits recursive call.
//*****************************************************************************

class C_interpret_forget_data
{
public:
	C_interpret_forget_data(T_interpret_read_values  & interpret_read_values,
							bool                       must_forget)
		:A_interpret_read_values(interpret_read_values),
		 A_must_forget(must_forget),
		 A_read_variable_id(A_interpret_read_values.get_id_of_last_read_variable())
	{
	}
	~C_interpret_forget_data()
	{
		if (A_must_forget == true)
		{
			A_interpret_read_values.sup_all_read_variables_after(A_read_variable_id);
		}
	}

private:
	// Copy and assignment are forbidden
	C_interpret_forget_data(const C_interpret_forget_data  &);
	C_interpret_forget_data & operator=(const C_interpret_forget_data  &);

	T_interpret_read_values        & A_interpret_read_values;
	bool                             A_must_forget;
	T_interpret_read_values::T_id    A_read_variable_id;
};


//*****************************************************************************
// frame_to_any ***************************************************************
//*****************************************************************************

bool    frame_to_any (const T_type_definitions    & type_definitions,
					        T_frame_data          & in_out_frame_data,
					        T_interpret_data      & interpret_data,
                      const T_field_type_name     & field_type_name,
                      const string                & data_name,
                      const string                & data_simple_name,
                            ostream               & os_out,
                            ostream               & os_err)
{
	M_STATE_ENTER ("frame_to_any",
                   "data_type= " << field_type_name.type << " data_name= " << data_name);

	// Will forget data if asked.
	C_interpret_forget_data    ifd(interpret_data, field_type_name.must_forget);

	// Hide the field if asked.
	const int   output_level_offset = field_type_name.get_output_level_offset();
	C_interpret_output_level_move_temporary    iolmt(interpret_data, output_level_offset);

	// Set temporary the decoder function
	C_interpret_decode_set_temporary    idst(interpret_data, field_type_name.str_decoder_function);

	// Set temporary the byte order
	C_interpret_byte_order_set_temporary  ibost(interpret_data, field_type_name.str_byte_order);


    //-------------------------------------------------------------------------
    // Array ?   managed inside frame_to_field
    //-------------------------------------------------------------------------


	if (field_type_name.pf_frame_to_any == NULL)
	{
		// Normally set at initialisation time for all types into type_definitions.
		// But some field_type_name could be created on the fly.
		build_types_finalize_itself(type_definitions, field_type_name);

		if (field_type_name.pf_frame_to_any == NULL)
		{
			//-------------------------------------------------------------------------
			// Error
			//-------------------------------------------------------------------------
			interpret_builder_error(type_definitions, in_out_frame_data,
									field_type_name, data_name, data_simple_name,
									"Not valid type " + field_type_name.type);
			return  false;
		}
	}

	return  field_type_name.pf_frame_to_any(type_definitions,
										   in_out_frame_data,
										   interpret_data,
										   field_type_name,
										   data_name,
										   data_simple_name,
										   os_out, os_err);
}

//*****************************************************************************
// frame_to_main **************************************************************
//*****************************************************************************

bool    frame_to_main (const T_type_definitions  & type_definitions,
					         T_frame_data        & in_out_frame_data,
							 T_interpret_data    & interpret_data,
                             string              & last_extracted_word,       // in/out
                             istream             & is,
                             ostream             & os_out,
                             ostream             & os_err)
{
	M_STATE_ENTER ("frame_to_main",
                   "sizeof_bits=" << in_out_frame_data.get_remaining_bits() <<
				   " last_extracted_word=" << last_extracted_word);

	T_field_type_name    field_type_name;
	last_extracted_word = build_field (is, type_definitions, last_extracted_word, field_type_name);

    if (frame_to_field (type_definitions,
                      in_out_frame_data,
					  interpret_data,
                      field_type_name,
					  "",                 // 20090430 instead of field_type_name.name : seems ok
					  field_type_name.name,
                      os_out, os_err) != true)
    {
        os_err << " type= " << field_type_name.type
               << " variable= " << field_type_name.name << endl;
        return  false;
    }

    return  true;
}

//*****************************************************************************
// interpret_bytes ************************************************************
//*****************************************************************************

bool    interpret_bytes (const T_type_definitions  & type_definitions,
                         const T_byte             *& in_out_P_bytes,
                               size_t              & in_out_sizeof_bytes,
                               istream             & is,
                               ostream             & os_out,
                               ostream             & os_err,
							   T_interpret_data    & interpret_data)
{
	M_STATE_ENTER ("interpret_bytes",
                   "sizeof_bytes=" << in_out_sizeof_bytes);

  // save the current locale for LC_NUMERIC (used for numeric input/output, e.g. strtoll)
  // change the locale for LC_NUMERIC (so 0.236 is a valid number)
  const char  * locale_save = setlocale(LC_NUMERIC, "C");


//	C_interpret_data_set_temporary  interpret_data_set_temporary(interpret_data);  // ICIOA

	T_frame_data           frame_data(in_out_P_bytes, 0, in_out_sizeof_bytes * 8);
	T_decode_stream_frame  decode_stream_frame;
	interpret_data.set_decode_stream_frame(&decode_stream_frame);
	interpret_data.add_read_variable("internal_frame", "internal_frame", (long long)&decode_stream_frame);

	bool    result = false;

    try
    {
		skip_blanks_and_comments(is);

		string    last_extracted_word;
		read_token_left_any (is, last_extracted_word);

		while (true)
		{
			if (frame_to_main (type_definitions,
							   frame_data,
							   interpret_data,
							   last_extracted_word,       // in/out
							   is,
							   os_out,
							   os_err) != true)
			{
				break;
			}

			if (last_extracted_word.empty())
			{
				result = true;
				break;
			}
		}
	}
    catch (C_byte_interpret_exception  & val)
    {
		const string  & str_interpret_fatal_text = val.get_explanation();

		if (val.get_cause() != E_byte_interpret_exception_missing_data)    // already displayed exception
		{
			const string         str_interpret = "fatal  \"" + str_interpret_fatal_text + "\" ;";

			interpret_bytes ( 				   type_definitions,
											   in_out_P_bytes,
											   in_out_sizeof_bytes,
											   str_interpret,
											   os_out,
											   os_err,
											   interpret_data);
		}
	}

	in_out_sizeof_bytes = frame_data.get_remaining_entire_bytes();

	if (result && (frame_data.is_physically_at_beginning_of_byte() != true))
	{
		result = false;  // bsew ICIOA must do a fatal
	}

  // restore the saved locale
  setlocale(LC_NUMERIC, locale_save);

    return  result;
}

//*****************************************************************************
// interpret_bytes ************************************************************
//*****************************************************************************

bool    interpret_bytes (const T_type_definitions  & type_definitions,
                         const T_byte             *& in_out_P_bytes,
                               size_t              & in_out_sizeof_bytes,
                         const string              & in_input_string,
                               ostream             & os_out,
                               ostream             & os_err,
							   T_interpret_data    & interpret_data)
{
	istrstream     is(in_input_string.c_str());

	return  interpret_bytes (type_definitions,
                             in_out_P_bytes,
                             in_out_sizeof_bytes,
                             is,
							 os_out,
							 os_err,
							 interpret_data);
}

//*****************************************************************************
// interpret_bytes ************************************************************
//*****************************************************************************

bool    interpret_bytes (const T_type_definitions  & type_definitions,
                         const T_byte             *& in_out_P_bytes,
                               size_t              & in_out_sizeof_bytes,
                               istream             & is,
                               ostream             & os_out,
                               ostream             & os_err)
{
	T_interpret_data    interpret_data;

	return  interpret_bytes (type_definitions,
                             in_out_P_bytes,
                             in_out_sizeof_bytes,
                             is,
							 os_out,
							 os_err,
							 interpret_data);
}

//*****************************************************************************
// interpret_bytes ************************************************************
//*****************************************************************************

bool    interpret_bytes (const T_type_definitions  & type_definitions,
                         const T_byte             *& in_out_P_bytes,
                               size_t              & in_out_sizeof_bytes,
                         const string              & in_input_string,
                               ostream             & os_out,
                               ostream             & os_err)
{
	istrstream     is(in_input_string.c_str());

	return  interpret_bytes (type_definitions,
                             in_out_P_bytes,
                             in_out_sizeof_bytes,
                             is,
							 os_out,
							 os_err);
}

//*****************************************************************************
// build_types_and_interpret_bytes ********************************************
//*****************************************************************************

bool    build_types_and_interpret_bytes (
                        T_type_definitions  & type_definitions,
                        T_frame_data        & in_out_frame_data,
                        istream             & is,
                        ostream             & os_out,
                        ostream             & os_err)
{
	M_STATE_ENTER ("build_types_and_interpret_bytes",
                   "sizeof_bits=" << in_out_frame_data.get_remaining_bits());

  // save the current locale for LC_NUMERIC (used for numeric input/output, e.g. strtoll)
  // change the locale for LC_NUMERIC (so 0.236 is a valid number)
  const char  * locale_save = setlocale(LC_NUMERIC, "C");

	// Set the interpret_data.
	T_interpret_data    interpret_data;
//	C_interpret_data_set_temporary    idst(interpret_data);
	T_decode_stream_frame  decode_stream_frame;
	interpret_data.set_decode_stream_frame(&decode_stream_frame);
	interpret_data.add_read_variable("internal_frame", "internal_frame", get_string((long long)&decode_stream_frame));

	// Reads type definitions.
	// Returns the 1st not understood word (i.e. a word which is NOT a type definition).
    string    last_extracted_word = build_types (is, type_definitions);
	bool    result = true;

	while (last_extracted_word != "")
    {
        if (frame_to_main (type_definitions,
                           in_out_frame_data,
						   interpret_data,
                           last_extracted_word,       // in/out
                           is,
                           os_out,
                           os_err) != true)
        {
          result = false;
          break;
        }
    }

  // restore the saved locale
  setlocale(LC_NUMERIC, locale_save);

  if (result == true)
  {
	// Verify that this is the end of stream.
    M_FATAL_IF_NE (last_extracted_word, "");
    M_ASSERT (is.eof ());
  }
  
	return  result;
}

//*****************************************************************************
// build_types_and_interpret_bytes ********************************************
//*****************************************************************************

bool    build_types_and_interpret_bytes (
                  const T_byte             *& in_out_P_bytes,
                        size_t              & in_out_sizeof_bytes,
                        istream             & is,
                        ostream             & os_out,
                        ostream             & os_err)
{
    T_frame_data    frame_data(in_out_P_bytes, 0, in_out_sizeof_bytes * 8);

    try
    {
        T_type_definitions                      type_definitions;

        bool  result = build_types_and_interpret_bytes (type_definitions,
                                                        frame_data,
                                                        is,
                                                        os_out,
                                                        os_err);
        in_out_P_bytes      = frame_data.get_P_bytes();
        in_out_sizeof_bytes = frame_data.get_remaining_entire_bytes();

		if (result && (frame_data.is_physically_at_beginning_of_byte() != true))
		{
			result = false;  // bsew ICIOA must do a fatal
		}

        return  result;
	}
    catch (...)
    {
	}

    return  false;
}

/*
 * Copyright 2005-2010 Olivier Aveline <wsgd@free.fr>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

//*****************************************************************************
// Includes.
//*****************************************************************************

#include "precomp.h"
#include "T_expression.h"
#include "byte_interpret_parse.h"
#include "T_type_definitions.h"

// FRHED needs this after previous include to find "std::find" !
#include <algorithm>
using namespace std;



string    words_to_string(const vector<string>::const_iterator      & iter_begin,
						  const vector<string>::const_iterator      & iter_end);
string    words_to_string(const vector<string>      & words);

E_return_code    get_complex_value (const T_type_definitions  & type_definitions,
									const T_interpret_data    & interpret_data,
									const string              & value_str,
                                          C_value             & value);

//*****************************************************************************
// Constructor
//*****************************************************************************

T_expression::T_expression()
	:A_value_alreay_computed(false),
	 A_type(E_type_none),
	 A_operation(E_operation_none)
{
}

//*****************************************************************************
// get_variable_name
//*****************************************************************************
const std::string  &
T_expression::get_variable_name() const
{
	M_FATAL_IF_FALSE(is_a_variable());

	return  A_variable_or_function_name;    // ICIOA array -> reduced !!!
}

//*****************************************************************************
// build_expression_words
//*****************************************************************************
void
T_expression::build_expression_words(
						 const T_type_definitions        & type_definitions,
						 const std::vector<std::string>  & words)
{
	if (A_original_string_expression.empty())
	{
		for (string::size_type  idx = 0; idx < words.size (); ++idx)
		{
			A_original_string_expression += words[idx];
			A_original_string_expression += " ";
		}
	}

    vector<string>::const_iterator  iter;
    vector<string>::const_iterator  iter_begin = words.begin ();
    vector<string>::const_iterator  iter_end   = words.end ();
	if (((iter =  find (iter_begin, iter_end,  "?")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "&&")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "||")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  "&")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  "|")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  "^")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "<<")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, ">>")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "==")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "!=")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "<=")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, ">=")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  "<")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  ">")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  "+")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "-")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "**")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end,  "*")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "/")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "%")) != iter_end))
    {
		A_type = E_type_operation;


        const string  & operation = *iter;

        // faire un vector avec le debut
        vector<string>    words_left (iter_begin, iter);
        if ((operation == "-") && (iter == iter_begin))
        {
            // Operator - unary.

			// faire un vector avec la fin
			++iter;
			if (iter == iter_end)
			{
				M_FATAL_COMMENT ("Found operator " << operation << " at the end of expression (" << words_to_string(words) << ")");
			}
			vector<string>    words_right (iter, iter_end);
            // Add a zero before to make it binary.
			words_right[0] = "(0-" + words_right[0] + ")";
			build_expression_words(type_definitions, words_right);
			return;
        }
#if 1
		else if (operation == "-")
        {
			// Operator - not at the beginning, but could be unary
			vector<string>::const_iterator  iter_prev = iter;
			--iter_prev;
			const string  & str_prev = *iter_prev;
			if ((str_prev ==  "?") ||
				(str_prev == "&&") ||
				(str_prev == "||") ||
				(str_prev ==  "&") ||
				(str_prev ==  "|") ||
				(str_prev ==  "^") ||
				(str_prev == "<<") ||
				(str_prev == ">>") ||
				(str_prev == "==") ||
				(str_prev == "!=") ||
				(str_prev == "<=") ||
				(str_prev == ">=") ||
				(str_prev ==  "<") ||
				(str_prev ==  ">") ||
				(str_prev ==  "+") ||
				(str_prev ==  "-") ||
				(str_prev == "**") ||
				(str_prev ==  "*") ||
				(str_prev ==  "/") ||
				(str_prev ==  "%"))
			{
				++iter;
				if (iter == iter_end)
				{
					M_FATAL_COMMENT ("Found operator " << operation << " at the end of expression (" << words_to_string(iter_begin, iter_end) << ")");
				}

				vector<string>    new_words (iter_begin, iter_end);
				// Add a zero before to make it binary.
				new_words[iter-iter_begin] = "(0-" + (*iter) + ")";
				--iter;
				new_words.erase(new_words.begin()+(iter-iter_begin));

				build_expression_words(type_definitions, new_words);
				return;
			}
		}
#endif
        else if (iter == iter_begin)
        {
            M_FATAL_COMMENT ("Found operator " << operation << " at the beginning of expression (" << words_to_string(words) << ")");
        }

        // faire un vector avec la fin
        ++iter;
		if (iter == iter_end)
		{
			M_FATAL_COMMENT ("Found operator " << operation << " at the end of expression (" << words_to_string(words) << ")");
		}
        vector<string>    words_right (iter, iter_end);

        --iter;

		A_expressions.push_back(T_expression());
		A_expressions.back().build_expression_words(type_definitions, words_left);


        // condition operator.
        if (operation == "?")
        {
			A_operation = E_operation_condition;

            vector<string>::iterator  iter_sep =
                                      find (words_right.begin (),
                                            words_right.end (),
                                            ":");
			if (iter_sep == words_right.begin ())
			{
				M_FATAL_COMMENT ("Found separator : directly after ? expression (" << words_to_string(words_right) << ")");
			}
			if (iter_sep == words_right.end ())
			{
				M_FATAL_COMMENT ("Found separator : at the end of ? expression (" << words_to_string(words_right) << ")");
			}
            
	        vector<string>    condition_words_left (words_right.begin(), iter_sep);
			A_expressions.push_back(T_expression());
			A_expressions.back().build_expression_words(type_definitions, condition_words_left);

			vector<string>    condition_words_right(++iter_sep, words_right.end());
			A_expressions.push_back(T_expression());
			A_expressions.back().build_expression_words(type_definitions, condition_words_right);

            return;
        }

		// Other operators.
		A_expressions.push_back(T_expression());
		A_expressions.back().build_expression_words(type_definitions, words_right);

        if (operation == "&&")
			A_operation = E_operation_logical_and;
        else if (operation == "||")
			A_operation = E_operation_logical_or;
        else if (operation == "&")
			A_operation = E_operation_bit_and;
        else if (operation == "|")
			A_operation = E_operation_bit_or;
        else if (operation == "^")
			A_operation = E_operation_bit_xor;
        else if (operation == "<<")
			A_operation = E_operation_bit_shift_left;
        else if (operation == ">>")
			A_operation = E_operation_bit_shift_right;
        else if (operation == "==")
			A_operation = E_operation_equal;
        else if (operation == "!=")
			A_operation = E_operation_not_equal;
        else if (operation == "<=")
			A_operation = E_operation_less_or_equal;
        else if (operation == ">=")
			A_operation = E_operation_greater_or_equal;
        else if (operation == "<")
			A_operation = E_operation_less;
        else if (operation == ">")
			A_operation = E_operation_greater;
        else if (operation == "*")
			A_operation = E_operation_multiply;
        else if (operation == "/")
			A_operation = E_operation_divide;
        else if (operation == "%")
			A_operation = E_operation_modulo;
        else if (operation == "+")
			A_operation = E_operation_addition;
        else if (operation == "-")
			A_operation = E_operation_subtraction;
        else if (operation == "**")
			A_operation = E_operation_pow;
        else
        {
			M_FATAL_COMMENT ("Bad operator " << operation << " : bug in software. expression (" << words_to_string(words) << ")");
        }
        return;
    }

    if (words.size () == 1)
    {
        build_expression_str (type_definitions, words[0]);
        return;
    }

    if (words.size () == 2)
    {
		A_type = E_type_function_call;
		A_variable_or_function_name = words[0];

		const string  & function_name = words[0];
		const string  & function_parameters = words[1];

		if (function_name == "string.length")
		{
			M_FATAL_IF_FALSE(true);
		}

#if 0
		{
			// Built-in functions with 1 parameter.
			A_expressions.push_back(T_expression());
			A_expressions.back().build_expression_str(type_definitions, function_parameters);
			return;
		}
		else
#endif
		const T_function_definition  * P_fct = NULL_PTR;
		if ((function_name != "print"))
		{
			P_fct = type_definitions.get_P_function(function_name);
			if (P_fct == NULL_PTR)
			{
				M_FATAL_COMMENT("Function " << function_name << " does not exist.");
			}
		}

		vector<string>    fct_parameters;
		{
			if ((function_parameters == "") ||
				(function_parameters[0] != '(') ||
				(function_parameters[function_parameters.size()-1] != ')'))
			{
				M_FATAL_COMMENT("Bad function parameters (" << function_parameters << ")");
			}

			// Remove ( and ).
			string            function_parameters_copy = function_parameters;
			function_parameters_copy.erase(0, 1);
			function_parameters_copy.erase(function_parameters_copy.size()-1);

			// Split on ,.
			string_to_words(function_parameters_copy, fct_parameters, K_parser_cfg_parameters);
		}

		for (unsigned int idx=0; idx<fct_parameters.size(); ++idx)
		{
			A_expressions.push_back(T_expression());
			A_expressions.back().build_expression_str(type_definitions, fct_parameters[idx]);
		}

		if (function_name == "print")
		{
			// verifier qu'il y a au moins 1 parametre
			// voir si param 0 calculable
			//   + promote
		}
		else if (P_fct != NULL_PTR)
		{
			// Check the number of parameters
			if (fct_parameters.size() < P_fct->get_nb_of_mandatory_parameters())
			{
				M_FATAL_COMMENT("Too few parameters for function " << function_name);
			}
			if (fct_parameters.size() > P_fct->get_function_parameters().size())
			{
				M_FATAL_COMMENT("Too many parameters for function " << function_name);
			}
		}

		return;
    }

	for (string::size_type  idx = 0; idx < words.size (); ++idx)
    {
        M_STATE_ERROR ("words[" << idx << "] = " << words[idx]);
    }
    M_FATAL_COMMENT ("Bad expression (" << words_to_string(words) << ")");
}

//*****************************************************************************
// build_expression_str
//*****************************************************************************
void
T_expression::build_expression_str(
						 const T_type_definitions  & type_definitions,
						 const std::string         & in_str)
{
	M_STATE_ENTER("build_expression_str", in_str);

	if (A_original_string_expression.empty())
	{
		A_original_string_expression = in_str;
	}

    vector<string>    words;
    string_to_words (in_str, words, K_parser_cfg_C);

	if (words.empty ())
	{
		M_FATAL_COMMENT("Empty expression.");
	}

    if ((words.size () == 1) && (words[0] == in_str))
    {
		string  & new_str = words[0];
        remove_word_limits (new_str, '(', ')');
        if (new_str != in_str)
        {
            build_expression_str (type_definitions, new_str);
			return;
        }

		// Manage expressions inside arrays
		if (new_str.find(']') != string::npos)
		{
			build_expression_array(type_definitions,new_str);
			return;
		}

		T_interpret_data  interpret_data;
        E_return_code     rc = get_complex_value (type_definitions, interpret_data, new_str, A_value);
		if (rc == E_rc_ok)
        {
			A_type = E_type_value;
        }
        else if (is_a_valid_long_variable_name(new_str))
        {
			A_type = E_type_variable;
			A_variable_or_function_name = new_str;
        }
		else
		{
			M_FATAL_COMMENT("Does not understand expression '" << new_str << "'");
		}
        return;
    }

    build_expression_words (type_definitions, words);
}

//*****************************************************************************
// build_expression
//*****************************************************************************
void
T_expression::build_expression(
						 const T_type_definitions  & type_definitions,
						 const std::string         & str)
{
	build_expression_str(type_definitions,str);
	pre_compute_expression(type_definitions);
}

//*****************************************************************************
// build_expression_array
//*****************************************************************************
void
T_expression::build_expression_array(
						 const T_type_definitions  & type_definitions,
						 const std::string         & in_str)
{
	M_STATE_ENTER("build_expression_array", in_str);

	// Compute the value of each array index.
	string    left_part;
	string    str_array_index;
	string    right_part = in_str;
	string    reduced_variable_name;
	while (decompose_left_sep_middle_sep_right (right_part,
													  '[',
													  ']',
													  left_part,
													  str_array_index,
													  right_part) == E_rc_ok)
	{
		reduced_variable_name += left_part;
		reduced_variable_name += "[]";

		M_FATAL_IF_EQ(str_array_index, "");

		A_expressions.push_back(T_expression());
		A_expressions.back().build_expression_str(type_definitions, str_array_index);

		if (right_part == "")
		{
			break;
		}
	}

	A_type = E_type_variable;
	A_variable_or_function_name = reduced_variable_name;

	M_STATE_DEBUG("A_variable_or_function_name=" << A_variable_or_function_name);
}

//*****************************************************************************
// pre_compute_expression
//*****************************************************************************
void
T_expression::pre_compute_expression(
						 const T_type_definitions      & type_definitions)
{
	if (A_value_alreay_computed == true)
	{
		return;
	}

	T_interpret_data    interpret_data;
	T_frame_data        in_out_frame_data;
	const string        data_name;
	const string      & data_simple_name = data_name;
	ostrstream          os_out;
	ostrstream        & os_err = os_out;
	bool                pre_compute_result_do_not_care = true;

	compute_expression(
				type_definitions,
				interpret_data,
				in_out_frame_data,
				data_name,
				data_simple_name,
				os_out,
				os_err,
				true /* pre_compute */,
				pre_compute_result_do_not_care);
}

//*****************************************************************************
// compute_expression
//*****************************************************************************
const C_value &
T_expression::compute_expression(
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err) const
{
	bool    pre_compute_result_do_not_care = true;

	return  compute_expression(
			type_definitions,
			interpret_data,
			in_out_frame_data,
			data_name,
			data_simple_name,
			os_out,
			os_err,
			false /* pre_compute */,
			pre_compute_result_do_not_care);
}

//*****************************************************************************
// compute_expression_no_io
//*****************************************************************************
const C_value &
T_expression::compute_expression_no_io(
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data) const
{
	T_frame_data        in_out_frame_data;
	const string        data_name;
	const string      & data_simple_name = data_name;
	ostrstream          os_out;
	ostrstream        & os_err = os_out;

	return  compute_expression(
				type_definitions,
				interpret_data,
				in_out_frame_data,
				data_name,
				data_simple_name,
				os_out,
				os_err);
}

//*****************************************************************************
// compute_expression_static
//*****************************************************************************
const C_value &
T_expression::compute_expression_static(
						 const T_type_definitions      & type_definitions) const
{
	T_interpret_data    interpret_data;
	T_frame_data        in_out_frame_data;
	const string        data_name;
	const string      & data_simple_name = data_name;
	ostrstream          os_out;
	ostrstream        & os_err = os_out;

	return  compute_expression(
				type_definitions,
				interpret_data,
				in_out_frame_data,
				data_name,
				data_simple_name,
				os_out,
				os_err);
}

//*****************************************************************************
// compute_expression
//-----------------------------------------------------------------------------
// For performance optimisation,
//  we try to compute (partially or completely) expression during initialisation
// This is possible if the expression does not require existing variable. 
// - pre_compute = true means trying to compute during initialisation
//   pre_compute = false :
//   - if error -> exception
//   - do not care about pre_compute_result
//   pre_compute = true :
//   - if non existent needed variable -> no exception, pre_compute_result set to false
//   - if other error -> exception
//   - must care about pre_compute_result
// - pre_compute_result = result of pre computation
//*****************************************************************************
const C_value &
T_expression::compute_expression(
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err,
							   bool                      pre_compute,
							   bool                    & pre_compute_result) const
{
	if (A_value_alreay_computed == false)
	{
		if (A_type == E_type_value)
		{
			// nothing to do
			pre_compute_result = true;
		}
		else if ((A_type == E_type_variable) && (A_expressions.empty()))
		{
			pre_compute_result = interpret_data.get_value_of_read_variable (
									A_variable_or_function_name,
									A_value);
			
			if ((pre_compute_result != true) && (pre_compute == false))
			{
				M_FATAL_COMMENT("unknow variable/field = " << A_variable_or_function_name);
			}
		}
		else if (A_type == E_type_variable)
		{
			compute_expression_variable_array(
				type_definitions,
				interpret_data,
				in_out_frame_data,
				data_name,
				data_simple_name,
				os_out,
				os_err,
				pre_compute,
				pre_compute_result);
		}
		else if (A_type == E_type_operation)
		{
			compute_expression_operation(
				type_definitions,
				interpret_data,
				in_out_frame_data,
				data_name,
				data_simple_name,
				os_out,
				os_err,
				pre_compute,
				pre_compute_result);
		}
		else if (A_type == E_type_function_call)
		{
			compute_expression_function(
				type_definitions,
				interpret_data,
				in_out_frame_data,
				data_name,
				data_simple_name,
				os_out,
				os_err,
				pre_compute,
				pre_compute_result);
		}
		else
		{
			M_FATAL_COMMENT("expression.type unknow = " << A_type);
		}

		if (pre_compute && pre_compute_result)
		{
			A_value_alreay_computed = true;
		}
	}

	return  A_value;
}

//*****************************************************************************
// compute_expression_variable_array
//*****************************************************************************
void
T_expression::compute_expression_variable_array(
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err,
							   bool                      pre_compute,
							   bool                    & pre_compute_result) const
{
	if (pre_compute) { pre_compute_result = false; return; }   /* ICIOA temporaire */

	string  variable_name = A_variable_or_function_name;

	for (vector<T_expression>::reverse_iterator
			iter  = A_expressions.rbegin();
			iter != A_expressions.rend();
			++iter)
	{
		T_expression  expression = *iter;

		// Compute index expression
		C_value  idx_value = expression.compute_expression(
								type_definitions,
								interpret_data,
								in_out_frame_data,
								data_name,
								data_simple_name,
								os_out,
								os_err);
		M_FATAL_IF_NE(idx_value.get_type(), C_value::E_type_integer);

		// Compute where to insert
		string::size_type  insert_idx = variable_name.rfind("[]");
		M_FATAL_IF_EQ(insert_idx, string::npos);

		++insert_idx;

		// Insert index string
		variable_name.insert(insert_idx, idx_value.as_string());
	}


	if (interpret_data.get_value_of_read_variable (
			variable_name,
			A_value) != true)
	{
		M_FATAL_COMMENT("unknow variable/field = " << variable_name);
	}
}

//*****************************************************************************
// compute_expression_function
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
							   bool                    & pre_compute_result);

void
T_expression::compute_expression_function(
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err,
							   bool                      pre_compute,
							   bool                    & pre_compute_result) const
{
	pre_compute_result = true;

	// static for performance (no measurement) to avoid ctor/dtor each time
	// *      for performance (no measurement) to avoid copy each time
	static const C_value  * P_parameter_values[50];    // ICIOA magic number

	for (unsigned int   idx = 0; idx < A_expressions.size(); ++idx)
	{
		bool    pre_compute_result_local = true;

		P_parameter_values[idx] = & A_expressions[idx].compute_expression(
					type_definitions, interpret_data, in_out_frame_data,
					data_name, data_simple_name, os_out, os_err,
					pre_compute, pre_compute_result_local);
		pre_compute_result &= pre_compute_result_local;
	}

	if ((pre_compute == true) && (pre_compute_result == false))
	{
		return;    // parameters pre computation failed (should be normal)
	}


	if (A_variable_or_function_name == "print")
	{
		string    printf_format = (* P_parameter_values[0]).get_str();;
		promote_printf_string_to_64bits(printf_format);

		// Compute values to print.
		vector<C_value>    values_to_print;
		for (unsigned int   idx = 1; idx < A_expressions.size(); ++idx)
		{
			values_to_print.push_back(* P_parameter_values[idx]);
		}

		// printf.
		A_value = C_value::sprintf_values(printf_format, values_to_print);
	}
	else if (A_variable_or_function_name == "to_string")
	{
		A_value = * P_parameter_values[0];
		A_value.to_string();
	}
	else if (A_variable_or_function_name == "to_numeric")
	{
		A_value = * P_parameter_values[0];
		A_value.to_numeric();
	}
	else if (A_variable_or_function_name == "to_float")
	{
		A_value = * P_parameter_values[0];
		A_value.to_float();
	}
	else if (A_variable_or_function_name == "to_integer")
	{
		A_value = * P_parameter_values[0];

		C_value  value_base = 0;
		if (A_expressions.size() > 1)
		{
			value_base = * P_parameter_values[1];
		}

		A_value.to_int(value_base.get_int());
	}
	else if (A_variable_or_function_name == "getenv")
	{
		const char  * env_value = getenv(P_parameter_values[0]->as_string().c_str());
		A_value = (env_value == NULL_PTR) ? "" : env_value;
	}
	else if (A_variable_or_function_name == "string.length")
	{
		A_value = * P_parameter_values[0];
		A_value = A_value.get_str().length();
	}
	else if (A_variable_or_function_name == "string.substr")
	{
		A_value = * P_parameter_values[0];
		C_value  value_idx = * P_parameter_values[1];

		C_value  value_count = string::npos;
		if (A_expressions.size() > 2)
		{
			value_count = * P_parameter_values[2];
		}

		A_value = A_value.get_str().substr(value_idx.get_int(),value_count.get_int());
	}
	else if (A_variable_or_function_name == "string.erase")
	{
		A_value = * P_parameter_values[0];

		C_value  value_idx = * P_parameter_values[1];

		C_value  value_count = string::npos;
		if (A_expressions.size() > 2)
		{
			value_count = * P_parameter_values[2];
		}

		string  str_copy = A_value.get_str();
		str_copy.erase(value_idx.get_int(),value_count.get_int());
		A_value = str_copy;
	}
	else if (A_variable_or_function_name == "string.insert")
	{
		A_value = * P_parameter_values[0];

		C_value  value_idx = * P_parameter_values[1];
		C_value  value_str = * P_parameter_values[2];

		string  str_copy = A_value.get_str();
		str_copy.insert(value_idx.get_int(),value_str.get_str());
		A_value = str_copy;
	}
	else if (A_variable_or_function_name == "string.replace")
	{
		A_value = * P_parameter_values[0];

		C_value  value_idx = * P_parameter_values[1];
		C_value  value_count = * P_parameter_values[2];
		C_value  value_str = * P_parameter_values[3];

		string  str_copy = A_value.get_str();
		str_copy.replace(value_idx.get_int(),value_count.get_int(),value_str.get_str());
		A_value = str_copy;
	}
	else if (A_variable_or_function_name == "string.replace_all")
	{
		A_value = * P_parameter_values[0];

		C_value  value_str_old = * P_parameter_values[1];
		C_value  value_str_new = * P_parameter_values[2];

		A_value = get_replace_all(A_value.get_str(), value_str_old.get_str(), value_str_new.get_str());
	}
	else if (A_variable_or_function_name == "string.find")
	{
		A_value = * P_parameter_values[0];

		C_value  value_str = * P_parameter_values[1];

		C_value  value_idx = 0;
		if (A_expressions.size() > 2)
		{
			value_idx = * P_parameter_values[2];
		}

		A_value = A_value.get_str().find(value_str.get_str(), value_idx.get_int());
	}
#if 0
	else if (A_variable_or_function_name == "string.")
	{
		A_value = * P_parameter_values[0];

//			A_value = A_value.get_str().find(str, idx);
//			A_value = A_value.get_str().find_first_not_of(str,idx);
//			A_value = A_value.get_str().find_first_of(str,idx);
//			A_value = A_value.get_str().find_last_not_of(str,idx);
//			A_value = A_value.get_str().find_last_of(str,idx);
//			A_value = A_value.get_str().rfind(str,idx);
	}
#endif
	else
	{
		const T_function_definition  * P_fct = type_definitions.get_P_function(A_variable_or_function_name);
		if (P_fct == NULL_PTR)
		{
			M_FATAL_COMMENT("Function " << A_variable_or_function_name << " does not exist.");
		}

		A_value = T_expression_compute_function (type_definitions,
								  interpret_data,
								  in_out_frame_data,
								 *P_fct,
								  A_expressions,  // function_parameters
								  data_name,
								  data_simple_name,
								  os_out,
								  os_err,
								  pre_compute, pre_compute_result);
	}
}

//*****************************************************************************
// compute_expression_operation
//*****************************************************************************
void
T_expression::compute_expression_operation(
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err,
							   bool                      pre_compute,
							   bool                    & pre_compute_result) const
{
	pre_compute_result = true;

#if 0
    // faire un vector avec le debut
	if ((operation == "-") && (iter == iter_begin))
    {
        // Operator - unary.
		vector<string>    words;
		words.push_back("0");
		words.insert(words.end(), iter_begin, iter_end);

		return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words.begin(), words.end(), data_name, data_simple_name, os_out, os_err);
	}
#endif

	bool             pre_compute_result_left = true;
	const C_value  & value_left =
		A_expressions[0].compute_expression(
			type_definitions, interpret_data, in_out_frame_data,
			data_name, data_simple_name, os_out, os_err,
			pre_compute, pre_compute_result_left);

	pre_compute_result = pre_compute_result && pre_compute_result_left;


	// pre_compute : try to compute all expressions
	// compute     : minimal computation
	//               for condition/logical operators

    if (A_operation == E_operation_condition)	// operator ? :
    {
		bool  condition = true;
		if ((pre_compute == false) || (pre_compute_result_left == true))
		{
			condition = value_left.get_bool ();
		}

		if ((condition == true) || pre_compute)
		{
			bool             pre_compute_result_local;
			A_value = A_expressions[1].compute_expression(
				type_definitions, interpret_data, in_out_frame_data,
				data_name, data_simple_name, os_out, os_err,
				pre_compute, pre_compute_result_local);

			pre_compute_result = pre_compute_result && pre_compute_result_local;
		}
		if ((condition == false) || pre_compute)
		{
			bool             pre_compute_result_local;
			const C_value  & value_ref = A_expressions[2].compute_expression(
				type_definitions, interpret_data, in_out_frame_data,
				data_name, data_simple_name, os_out, os_err,
				pre_compute, pre_compute_result_local);

			if (condition == false)
			{
				A_value = value_ref;
			}

			pre_compute_result = pre_compute_result && pre_compute_result_local;
		}
    }
	else if (A_operation == E_operation_logical_and)
	{
		bool  condition = true;
		if ((pre_compute == false) || (pre_compute_result_left == true))
		{
			condition = value_left.get_bool ();
			A_value = value_left;
		}

		if ((condition == true) || pre_compute)
		{
			bool             pre_compute_result_local;
			const C_value  & value_right =
					  A_expressions[1].compute_expression(
						type_definitions, interpret_data, in_out_frame_data,
						data_name, data_simple_name, os_out, os_err,
						pre_compute, pre_compute_result_local);

			pre_compute_result = pre_compute_result && pre_compute_result_local;

			if ((pre_compute == false) || (pre_compute_result == true))
			{
				A_value = value_left && value_right;
			}
		}
	}
    else if (A_operation == E_operation_logical_or)
	{
		bool  condition = false;
		if ((pre_compute == false) || (pre_compute_result_left == true))
		{
			condition = value_left.get_bool ();
			A_value = value_left;
		}

		if ((condition == false) || pre_compute)
		{
			bool             pre_compute_result_local;
			const C_value  & value_right =
					  A_expressions[1].compute_expression(
						type_definitions, interpret_data, in_out_frame_data,
						data_name, data_simple_name, os_out, os_err,
						pre_compute, pre_compute_result_local);

			pre_compute_result = pre_compute_result && pre_compute_result_local;

			if ((pre_compute == false) || (pre_compute_result == true))
			{
				A_value = value_left || value_right;
			}
		}
	}
	else
	{
		bool             pre_compute_result_local;
		const C_value  & value_right =
			A_expressions[1].compute_expression(
				type_definitions, interpret_data, in_out_frame_data,
				data_name, data_simple_name, os_out, os_err,
				pre_compute, pre_compute_result_local);

		pre_compute_result = pre_compute_result && pre_compute_result_local;

		if ((pre_compute == false) || (pre_compute_result == true))
		{
			// Others operators.
			if (A_operation == E_operation_bit_and)
				A_value = value_left & value_right;
			else if (A_operation == E_operation_bit_or)
				A_value = value_left | value_right;
			else if (A_operation == E_operation_bit_xor)
				A_value = value_left ^ value_right;
			else if (A_operation == E_operation_bit_shift_left)
				A_value = value_left << value_right;
			else if (A_operation == E_operation_bit_shift_right)
				A_value = value_left >> value_right;
			else if (A_operation == E_operation_equal)
				A_value = value_left == value_right;
			else if (A_operation == E_operation_not_equal)
				A_value = value_left != value_right;
			else if (A_operation == E_operation_less_or_equal)
				A_value = value_left <= value_right;
			else if (A_operation == E_operation_greater_or_equal)
				A_value = value_left >= value_right;
			else if (A_operation == E_operation_less)
				A_value = value_left <  value_right;
			else if (A_operation == E_operation_greater)
				A_value = value_left >  value_right;
			else if (A_operation == E_operation_addition)
				A_value = value_left +  value_right;
			else if (A_operation == E_operation_subtraction)
				A_value = value_left -  value_right;
			else if (A_operation == E_operation_pow)
				A_value = C_value::pow(value_left, value_right);
			else if (A_operation == E_operation_multiply)
				A_value = value_left *  value_right;
			else if (A_operation == E_operation_divide)
				A_value = value_left /  value_right;
			else if (A_operation == E_operation_modulo)
				A_value = value_left %  value_right;
			else
			{
				M_FATAL_COMMENT ("Bad operator " << A_operation << " : bug in software.");
			}
		}
	}

//	if (pre_compute && pre_compute_result)
//	{
//		A_value_alreay_computed = true;
//	}
}
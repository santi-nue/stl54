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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

//*****************************************************************************
// Includes.
//*****************************************************************************

#include <vector>
#include <algorithm>

using namespace std;

#include "precomp.h"
#include "C_value.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret_parse.h"


//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <value>
// <value> could be :
//   <int_value>
//   <enum_value>                           NOT IMPLEMENTED
//   <variable_value>
//*****************************************************************************

E_return_code    get_complex_value (const T_type_definitions  & type_definitions,
									const T_interpret_data    & interpret_data,
									const string              & value_str,
                                          C_value             & value)
{
    long long    value_int = 0;
    double       value_flt = 0.0;

    if (get_number (value_str.c_str (), value_int))
    {
        value = value_int;
    }
    else if (get_number (value_str.c_str (), value_flt))
    {
        value = value_flt;
    }
    else if (type_definitions.get_type_value(value_str, value))
    {
    }
	// very bad perfomance
    else if (interpret_data.get_value_of_read_variable (value_str, value))
    {
    }
    else
    {
        string  real_value_str = value_str;
        remove_string_limits (real_value_str);

        if (real_value_str == value_str)
        {
			if (strchr(value_str.c_str(), '*') != NULL_PTR)
			{
				return  E_rc_multiple_value;
			}

            return  E_rc_not_ok;
        }

        value = real_value_str;
    }

    return  E_rc_ok;
}

//*****************************************************************************
// words_to_string
//*****************************************************************************

string    words_to_string(const vector<string>::const_iterator      & iter_begin,
						  const vector<string>::const_iterator      & iter_end)
{
	string  str;
	for (vector<string>::const_iterator  iter  = iter_begin;
										 iter != iter_end;
									   ++iter)
	{
		str += *iter;
		str += " ";
	}

	return  str;
}

string    words_to_string(const vector<string>      & words)
{
	string  str;
	for (vector<string>::const_iterator  iter  = words.begin();
										 iter != words.end();
									   ++iter)
	{
		str += *iter;
		str += " ";
	}

	return  str;
}
#if 0
//*****************************************************************************
// compute_expression
// ----------------------------------------------------------------------------
// Format :
// 
//*****************************************************************************

C_value    printf_args_to_string (const T_type_definitions  & type_definitions,
										T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
								  const string              & print_args,
						 const std::string             & data_name,
                         const std::string             & data_simple_name,
                               std::ostream            & os_out,
                               std::ostream            & os_err);

C_value    compute_function (const T_type_definitions    & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
                         const T_function_definition   & fct_def,
						       string                    function_parameters,
                         const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err);  // ICIOA

C_value    compute_expression (
						 const T_type_definitions  & type_definitions,
							   T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const vector<string>::const_iterator      & iter_begin,
						 const vector<string>::const_iterator      & iter_end,
						 const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
    vector<string>::const_iterator  iter;
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
        ((iter =  find (iter_begin, iter_end,  "*")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "/")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "%")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "**")) != iter_end))
    {
        const string  & operation = *iter;

        // faire un vector avec le debut
		if ((operation == "-") && (iter == iter_begin))
        {
			// Operator - unary.
#if 0
			vector<string>    words_right;
			words_right.push_back("0");
			words_right.insert(words_right.end(), iter_begin, iter_end);
#else
			// faire un vector avec la fin
			++iter;
			if (iter == iter_end)
			{
				M_FATAL_COMMENT ("Found operator " << operation << " at the end of expression (" << words_to_string(iter_begin, iter_end) << ")");
			}
			vector<string>    words_right (iter, iter_end);
            // Add a zero before to make it binary.
			words_right[0] = "(0-" + words_right[0] + ")";
#endif

			return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right.begin(), words_right.end(), data_name, data_simple_name, os_out, os_err);
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

				vector<string>    words (iter_begin, iter_end);
				// Add a zero before to make it binary.
				words[iter-iter_begin] = "(0-" + (*iter) + ")";
				--iter;
				words.erase(words.begin()+(iter-iter_begin));

				return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words.begin(), words.end(), data_name, data_simple_name, os_out, os_err);
			}
		}
#endif
        else if (iter == iter_begin)
        {
            M_FATAL_COMMENT ("Found operator " << operation << " at the beginning of expression (" << words_to_string(iter_begin, iter_end) << ")");
        }
		vector<string>::const_iterator    words_left_iter_begin = iter_begin;
		vector<string>::const_iterator    words_left_iter_end   = iter;


        // faire un vector avec la fin
        ++iter;
		if (iter == iter_end)
		{
			M_FATAL_COMMENT ("Found operator " << operation << " at the end of expression (" << words_to_string(iter_begin, iter_end) << ")");
		}
		vector<string>::const_iterator    words_right_iter_begin = iter;
		vector<string>::const_iterator    words_right_iter_end   = iter_end;

        --iter;

        C_value    value_left = compute_expression (type_definitions, interpret_data, in_out_frame_data, words_left_iter_begin, words_left_iter_end, data_name, data_simple_name, os_out, os_err);

        // Choose which part of words_right to compute.
        if (operation == "?")
        {
            vector<string>::const_iterator  iter_sep =
                                      find (words_right_iter_begin,
                                            words_right_iter_end,
                                            ":");
			if (iter_sep == words_right_iter_begin)
			{
				M_FATAL_COMMENT ("Found separator : directly after ? expression (" << words_to_string(words_right_iter_begin, words_right_iter_end) << ")");
			}
			if (iter_sep == words_right_iter_end)
			{
				M_FATAL_COMMENT ("Found separator : at the end of ? expression (" << words_to_string(words_right_iter_begin, words_right_iter_end) << ")");
			}
            
            if (value_left.get_bool ())
            {
				// Keep only before separator
				words_right_iter_end = iter_sep;
            }
            else
            {
				// Keep only after separator
				words_right_iter_begin = ++iter_sep;
            }

            return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right_iter_begin, words_right_iter_end, data_name, data_simple_name, os_out, os_err);
        }

        // Avoid computation of right part if NOT necessary.
        if (operation == "&&")
            return  value_left && compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right_iter_begin, words_right_iter_end, data_name, data_simple_name, os_out, os_err);
        if (operation == "||")
            return  value_left || compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right_iter_begin, words_right_iter_end, data_name, data_simple_name, os_out, os_err);

        C_value    value_right = compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right_iter_begin, words_right_iter_end, data_name, data_simple_name, os_out, os_err);
        C_value    ret_value;
        if (operation == "&")
            ret_value = value_left & value_right;
        else if (operation == "|")
            ret_value = value_left | value_right;
        else if (operation == "^")
            ret_value = value_left ^ value_right;
        else if (operation == "<<")
            ret_value = value_left << value_right;
        else if (operation == ">>")
            ret_value = value_left >> value_right;
        else if (operation == "==")
            ret_value = value_left == value_right;
        else if (operation == "!=")
            ret_value = value_left != value_right;
        else if (operation == "<=")
            ret_value = value_left <= value_right;
        else if (operation == ">=")
            ret_value = value_left >= value_right;
        else if (operation == "<")
            ret_value = value_left <  value_right;
        else if (operation == ">")
            ret_value = value_left >  value_right;
        else if (operation == "*")
            ret_value = value_left *  value_right;
        else if (operation == "/")
            ret_value = value_left /  value_right;
        else if (operation == "%")
            ret_value = value_left %  value_right;
        else if (operation == "+")
            ret_value = value_left +  value_right;
        else if (operation == "-")
            ret_value = value_left -  value_right;
        else if (operation == "**")
			ret_value = C_value::pow(value_left, value_right);
        else
        {
			M_FATAL_COMMENT ("Bad operator " << operation << " : bug in software. expression (" << words_to_string(iter_begin, iter_end) << ")");
        }
        return  ret_value;
    }

	if ((iter_end - iter_begin) == 1)
    {
        return  compute_expression (type_definitions, interpret_data, in_out_frame_data, *iter_begin, data_name, data_simple_name, os_out, os_err);
    }

	if ((iter_end - iter_begin) == 2)
    {
		const string  & word_0 = *iter_begin;
		const string  & word_1 = *(iter_begin+1);

		if (word_0 == "print")
		{
			return  printf_args_to_string(type_definitions, interpret_data, in_out_frame_data,
										  word_1,
										  data_name, data_simple_name, os_out, os_err);
		}
		else if (word_0 == "to_string")
		{
			C_value  val = compute_expression(type_definitions, interpret_data, in_out_frame_data, word_1, data_name, data_simple_name, os_out, os_err);
			val.to_string();
			return  val;
		}
		else if (word_0 == "to_numeric")
		{
			C_value  val = compute_expression(type_definitions, interpret_data, in_out_frame_data, word_1, data_name, data_simple_name, os_out, os_err);
			val.to_numeric();
			return  val;
		}
		else
		{
			const T_function_definition  * P_fct = type_definitions.get_P_function(word_0);
			if (P_fct == NULL_PTR)
			{
				M_FATAL_COMMENT("Function " << word_0 << " does not exist.");
			}

			return  compute_function (type_definitions,
									  interpret_data,
									  in_out_frame_data,
									 *P_fct,
									  word_1,  // function_parameters
									  data_name,
									  data_simple_name,
									  os_out,
									  os_err);
		}
    }

	int  idx = 0;
	for (vector<string>::const_iterator  iter = iter_begin; iter < iter_end; ++iter)
    {
        M_STATE_ERROR ("words[" << idx++ << "] = " << *iter);
    }
    M_FATAL_COMMENT ("Bad expression (" << words_to_string(iter_begin, iter_end) << ")");
	return  0;                               // avoid compiler warning
}

#if 1
inline
C_value    compute_expression (
						 const T_type_definitions  & type_definitions,
							   T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const vector<string>      & words,
						 const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	return  compute_expression(type_definitions, interpret_data, in_out_frame_data,
		words.begin(), words.end(),
		data_name, data_simple_name, os_out, os_err);

}
#else
C_value    compute_expression (
						 const T_type_definitions  & type_definitions,
							   T_interpret_data    & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const vector<string>      & words,
						 const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
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
        ((iter =  find (iter_begin, iter_end,  "*")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "/")) != iter_end) ||
        ((iter = rfind (iter_begin, iter_end,  "%")) != iter_end) ||
        ((iter =  find (iter_begin, iter_end, "**")) != iter_end))
    {
        const string  & operation = *iter;

        // faire un vector avec le debut
        vector<string>    words_left (iter_begin, iter);
        if ((operation == "-") && (iter == iter_begin))
        {
            // Operator - unary.
            // Add a zero before to make it binary.
            words_left.push_back ("0");
        }
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

        C_value    value_left = compute_expression (type_definitions, interpret_data, in_out_frame_data, words_left, data_name, data_simple_name, os_out, os_err);

        // Choose which part of words_right to compute.
        if (operation == "?")
        {
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
            
            if (value_left.get_bool ())
            {
                words_right.erase (iter_sep, words_right.end ());
            }
            else
            {
                words_right.erase (words_right.begin (), ++iter_sep);
            }

            return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right, data_name, data_simple_name, os_out, os_err);
        }

        // Avoid computation of right part if NOT necessary.
        if (operation == "&&")
            return  value_left && compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right, data_name, data_simple_name, os_out, os_err);
        if (operation == "||")
            return  value_left || compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right, data_name, data_simple_name, os_out, os_err);

        C_value    value_right = compute_expression (type_definitions, interpret_data, in_out_frame_data, words_right, data_name, data_simple_name, os_out, os_err);
        C_value    ret_value;
        if (operation == "&")
            ret_value = value_left & value_right;
        else if (operation == "|")
            ret_value = value_left | value_right;
        else if (operation == "^")
            ret_value = value_left ^ value_right;
        else if (operation == "<<")
            ret_value = value_left << value_right;
        else if (operation == ">>")
            ret_value = value_left >> value_right;
        else if (operation == "==")
            ret_value = value_left == value_right;
        else if (operation == "!=")
            ret_value = value_left != value_right;
        else if (operation == "<=")
            ret_value = value_left <= value_right;
        else if (operation == ">=")
            ret_value = value_left >= value_right;
        else if (operation == "<")
            ret_value = value_left <  value_right;
        else if (operation == ">")
            ret_value = value_left >  value_right;
        else if (operation == "*")
            ret_value = value_left *  value_right;
        else if (operation == "/")
            ret_value = value_left /  value_right;
        else if (operation == "%")
            ret_value = value_left %  value_right;
        else if (operation == "+")
            ret_value = value_left +  value_right;
        else if (operation == "-")
            ret_value = value_left -  value_right;
        else if (operation == "**")
			ret_value = C_value::pow(value_left, value_right);
        else
        {
			M_FATAL_COMMENT ("Bad operator " << operation << " : bug in software. expression (" << words_to_string(words) << ")");
        }
        return  ret_value;
    }

    if (words.size () == 1)
    {
        return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words[0], data_name, data_simple_name, os_out, os_err);
    }

    if (words.size () == 2)
    {
		if (words[0] == "print")
		{
			return  printf_args_to_string(type_definitions, interpret_data, in_out_frame_data,
										  words[1],
										  data_name, data_simple_name, os_out, os_err);
		}
		else if (words[0] == "to_string")
		{
			C_value  val = compute_expression(type_definitions, interpret_data, in_out_frame_data, words[1], data_name, data_simple_name, os_out, os_err);
			val.to_string();
			return  val;
		}
		else if (words[0] == "to_numeric")
		{
			C_value  val = compute_expression(type_definitions, interpret_data, in_out_frame_data, words[1], data_name, data_simple_name, os_out, os_err);
			val.to_numeric();
			return  val;
		}
		else
		{
			const T_function_definition  * P_fct = type_definitions.get_P_function(words[0]);
			if (P_fct == NULL_PTR)
			{
				M_FATAL_COMMENT("Function " << words[0] << " does not exist.");
			}

			return  compute_function (type_definitions,
									  in_out_frame_data,
									 *P_fct,
									  words[1],  // function_parameters
									  data_name,
									  data_simple_name,
									  os_out,
									  os_err);
		}
    }

	for (string::size_type  idx = 0; idx < words.size (); ++idx)
    {
        M_STATE_ERROR ("words[" << idx << "] = " << words[idx]);
    }
    M_FATAL_COMMENT ("Bad expression (" << words_to_string(words) << ")");
	return  0;                               // avoid compiler warning
}
#endif

//*****************************************************************************
// compute_expression
// ----------------------------------------------------------------------------
// Format :
// 
//*****************************************************************************

C_value    compute_expression (
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const string                  & in_str,
						 const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER("compute_expression", in_str);

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
            return  compute_expression (type_definitions, interpret_data, in_out_frame_data, new_str, data_name, data_simple_name, os_out, os_err);
        }

		// Manage expressions inside arrays
		if (new_str.rfind(']') != string::npos)
		{
			new_str = compute_expressions_in_array(type_definitions, interpret_data, in_out_frame_data, new_str, data_name, data_simple_name, os_out, os_err);
		}

        C_value          value;
        E_return_code    rc = get_complex_value (type_definitions, interpret_data, new_str, value);
        if (rc != E_rc_ok)
        {
            M_FATAL_COMMENT ("pb get_complex_value : " << new_str);
        }
        return  value;
    }

    return  compute_expression (type_definitions, interpret_data, in_out_frame_data, words, data_name, data_simple_name, os_out, os_err);
}
#else
//*****************************************************************************
// compute_expression
// ----------------------------------------------------------------------------
// Format :
// 
//*****************************************************************************
#include "T_expression.h"
C_value    compute_expression (
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const string                  & in_str,
						 const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER("compute_expression", in_str);

	T_expression    expression;
	expression.build_expression(type_definitions, in_str);

	return  expression.compute_expression(
								type_definitions,
								interpret_data,
								in_out_frame_data,
								data_name,
								data_simple_name,
								os_out,
								os_err);
}
#endif
//*****************************************************************************
// compute_expression_no_io
// ----------------------------------------------------------------------------
// 
//*****************************************************************************

C_value    compute_expression_no_io (
						 const T_type_definitions  & type_definitions,
							   T_interpret_data    & interpret_data,
						 const std::string         & str)
{
	T_frame_data        in_out_frame_data;
	const string        data_name;
	const string      & data_simple_name = data_name;
	ostrstream          os_out;
	ostrstream        & os_err = os_out;

	return  compute_expression(type_definitions, interpret_data, in_out_frame_data,
							   str,
							   data_name, data_simple_name, os_out, os_err);
}

//*****************************************************************************
// compute_expression_static
// ----------------------------------------------------------------------------
// 
//*****************************************************************************

C_value    compute_expression_static (const T_type_definitions  & type_definitions,
									  const std::string         & str)
{
	T_interpret_data    interpret_data;

	return  compute_expression_no_io(type_definitions, interpret_data, str);
}

//*****************************************************************************
// compute_expression_array
// ----------------------------------------------------------------------------
// Format :
// 
//*****************************************************************************

string    compute_expressions_in_array (
						 const T_type_definitions      & type_definitions,
							   T_interpret_data        & interpret_data,
					           T_frame_data            & in_out_frame_data,
						 const string                  & in_str,
						 const string                  & data_name,
                         const string                  & data_simple_name,
                               ostream                 & os_out,
                               ostream                 & os_err)
{
	M_STATE_ENTER("compute_expressions_in_array", in_str);

	// Compute the value of each array index.
	string    simple_type = in_str;
	string    str_array_index;
	string    str_arrays_result;
	while (decompose_type_sep_value_sep (simple_type,
													  '[',
													  ']',
													  simple_type,
													  str_array_index) == E_rc_ok)
	{
		// Compute the value of the extracted index.
		C_value  value_index = compute_expression (type_definitions, interpret_data, in_out_frame_data, str_array_index, data_name, data_simple_name, os_out, os_err);
		M_FATAL_IF_NE(value_index.get_type(), C_value::E_type_integer);

		// Add the computed index to the result array.
		const string  str_array_index_result = "[" + value_index.as_string() + "]";
		str_arrays_result.insert(0, str_array_index_result);
	}

	const string  result = simple_type + str_arrays_result;

	M_STATE_DEBUG("result=" << result);
	return  result;
}


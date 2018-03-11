/*
 * Copyright 2005-2018 Olivier Aveline <wsgd@free.fr>
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
#include "T_interpret_read_values.h"
#include "byte_interpret_common.h"


//*****************************************************************************
// T_interpret_read_values
//*****************************************************************************

T_interpret_read_values::T_interpret_read_values()
	: A_msg_global_idx_begin(0)
	, A_msg_global_idx_end(-1)
	, A_msg_pinfo_idx_begin(0)
	, A_msg_pinfo_idx_end(-1)
	, A_msg_other_idx_begin(0)
	, A_this_msg_attribute_value(C_value(C_value::E_type_msg, NULL))
	, A_this_msg_attribute_value_used(false)
{
}

bool
T_interpret_read_values::is_read_variable (const string  & var_name) const
{
	const T_attribute_value  * P_attr = get_P_attribute_value_of_read_variable(var_name);
    if (P_attr != NULL)
	{
		return  true;
	}

	return  false;
}

const C_value *
T_interpret_read_values::get_P_value_of_read_variable (const string   & var_name) const
{
	const T_attribute_value  * P_attr = get_P_attribute_value_of_read_variable(var_name);
    if (P_attr != NULL)
	{
		return  & P_attr->transformed;
    }

	return  NULL;
}

bool
T_interpret_read_values::get_value_of_read_variable (
                                            const string   & var_name,
                                                  C_value  & out_value) const
{
	const C_value  * P_value = get_P_value_of_read_variable (var_name);
    if (P_value == NULL)
    {
        out_value = C_value ();
        return  false;
    }
    out_value = *P_value;
    return  true;
}

const string &
T_interpret_read_values::get_str_value_of_read_variable (
                                               const string  & var_name) const
{
	const C_value  * P_value = get_P_value_of_read_variable (var_name);
    if (P_value == NULL)
    {
        static const string  default_str;
        return  default_str;
    }
    return  P_value->as_string ();
}

bool
T_interpret_read_values::get_int_value_of_read_variable (
                                         const string     & var_name,
                                               long long  & out_value) const
{
	const C_value  * P_value = get_P_value_of_read_variable (var_name);
    if (P_value == NULL)
    {
        return  false;
    }

    if (P_value->get_type () != C_value::E_type_integer)
    {
        return  false;
    }

    out_value = P_value->get_int ();
    return  true;
}

T_interpret_read_values::T_id
T_interpret_read_values::add_read_variable (const string   & var_name_long,
                                            const string   & var_name_short,
                                            const C_value  & in_value)
{
	T_attribute_value    attribute_value(in_value);

	return  add_read_variable(var_name_long, var_name_short, attribute_value);
}

void
T_interpret_read_values::set_read_variable (const string   & var_name,
                                            const C_value  & in_value)
{
	set_read_variable(var_name, T_attribute_value(in_value));
}

void
T_interpret_read_values::set_read_variable (const string             & var_name,
                                            const T_attribute_value  & in_value)
{
	M_STATE_ENTER("set_read_variable", A_current_path << " " << var_name << " = " << in_value.transformed.as_string());

	T_attribute_value  * P_attr = const_cast<T_attribute_value*>(get_P_attribute_value_of_read_variable(var_name));
    if (P_attr == NULL)
	{
	    M_FATAL_COMMENT("set of an unknow variable/field (" << var_name << ")");
    }

	*P_attr = in_value;
	// reference !!!
}

void    
T_interpret_read_values::sup_read_variable (T_id    var_id)
{
	M_STATE_ENTER("sup_read_variable", A_current_path << " " << var_id);

	if ((var_id < 0) ||
		(var_id >= A_msg.size()))
	{
	    M_FATAL_COMMENT("suppress of an unknow variable/field id (" << var_id << ")");
	}

	T_interpret_value  & interpret_value = A_msg[var_id];

	if (interpret_value.get_reference_counter() != 0)
	{
		// 2009/11/18 impossible since only function parameters could be reference
		//  and they are supressed at the end of the function
	    M_FATAL_COMMENT("suppress of a still referenced variable/field id (" <<
						var_id << ", " <<
						A_msg[var_id].get_name() << ")");
	}

	if (interpret_value.get_type() == T_interpret_value::E_type_reference)
	{
		A_msg[interpret_value.get_attribute_value().transformed.get_int()].decr_reference_counter();
	}

	if (var_id == (A_msg.size() - 1))
	{
		A_msg.pop_back();
	}
	else
	{
	  // To not erase the variable will lead to an accumulative effect.
	  // Exemple: decode function called very often, parameters never erased
	  // But the real problem must be the variable inside the function ?
		A_msg.erase(A_msg.begin()+var_id);
	}
}

void
T_interpret_read_values::sup_read_variables (int     nb_of_var, T_id    var_id[])
{
	for (int   idx = nb_of_var - 1; idx >= 0; --idx)
	{
		sup_read_variable(var_id[idx]);
	}
}

void
T_interpret_read_values::sup_all_read_variables_after (T_id    var_id)
{
	M_STATE_ENTER("sup_all_read_variables_after", var_id);

	if (var_id >= A_msg.size())
	{
		return;
	}

	A_msg.erase(A_msg.begin()+var_id+1, A_msg.end());
}

T_interpret_read_values::T_id
T_interpret_read_values::add_read_variable (const string             & /* var_name_long */,
                                            const string             & var_name_short,
                                            const T_attribute_value  & in_value)
{
	M_STATE_ENTER("add_read_variable", A_current_path << " " << var_name_short);

	if (A_current_path == "")
	{
		A_msg.push_back(T_interpret_value(var_name_short, in_value));
	}
	else
	{
		string    full_name = A_current_path;
		full_name += ".";
		full_name += var_name_short;
		A_msg.push_back(T_interpret_value(full_name, in_value));
	}

	return  A_msg.size() - 1;
}

T_interpret_read_values::T_id
T_interpret_read_values::get_id_of_last_read_variable () const
{
	return  A_msg.size() - 1;
}

T_interpret_read_values::T_id
T_interpret_read_values::add_ref_variable(const string  & ref_name,
										  const string  & var_name)
{
	M_STATE_ENTER("add_ref_variable", A_current_path << " " << ref_name << " -> " << var_name);

	T_id                       var_id;
	const T_attribute_value  * P_attribute_value = get_P_attribute_value_of_read_variable(var_name, var_id);

	if (P_attribute_value == NULL)
	{
	    M_FATAL_COMMENT("in_out parameter on an unknow variable/field (" << var_name << ")");
	}

	if (var_id < 0)
	{
	    M_FATAL_COMMENT("in_out parameter on variable/field of another msg (" << var_name << ")");
	}

	T_interpret_read_values::T_id  ref_id =
		add_read_variable (ref_name, ref_name, C_value(var_id));

	A_msg[ref_id].set_type(T_interpret_value::E_type_reference);
	A_msg[var_id].incr_reference_counter();

	return  ref_id;
}

//const T_interpret_read_values  * get_P_interpret_read_values_of_msg(int   msg_id);

const T_attribute_value *
T_interpret_read_values::get_P_attribute_value_of_read_variable (
                                               const string  & var_name,
													 T_id    & var_id) const
{
	const int  A_msg_size = A_msg.size();

	if ((A_msg_global_idx_end > A_msg_global_idx_begin) &&
		(var_name.compare(0, 7, "global.") == 0))
	{
		return  get_P_attribute_value_of_read_variable(var_name, var_id,
													   A_msg.rbegin() + (A_msg_size - A_msg_global_idx_end),
													   A_msg.rend()                 - A_msg_global_idx_begin,
													   true /*full_name_only*/);
	}

	if ((A_msg_pinfo_idx_end > A_msg_pinfo_idx_begin) &&
		(var_name.compare(0, 6, "pinfo.") == 0))
	{
		return  get_P_attribute_value_of_read_variable(var_name, var_id,
													   A_msg.rbegin() + (A_msg_size - A_msg_pinfo_idx_end),
													   A_msg.rend()                 - A_msg_pinfo_idx_begin,
													   true /*full_name_only*/);
	}

	return  get_P_attribute_value_of_read_variable(var_name, var_id,
												   A_msg.rbegin(),
												   A_msg.rend() - A_msg_other_idx_begin,
												   false /*full_name_only*/);
}

const T_attribute_value *
T_interpret_read_values::get_P_attribute_value_of_read_variable (
                                               const string                                      & var_name,
													 T_id                                        & var_id,
													 T_interpret_values::const_reverse_iterator    A_msg_rstart,
													 T_interpret_values::const_reverse_iterator    A_msg_rstop,
													 bool                                          full_name_only) const
{
	if ((A_current_path != "") && (full_name_only == false))
	{
		string    full_name = A_current_path;
		full_name += ".";
		full_name += var_name;

		for (T_interpret_values::const_reverse_iterator
												iter  = A_msg_rstart;
												iter != A_msg_rstop;
											  ++iter)
		{
			const T_interpret_value  & interpret_value = *iter;
			const string             & interpret_value_name = interpret_value.get_name();

			if ((full_name == interpret_value_name) ||
				(var_name == interpret_value_name))
			{
				if (interpret_value.get_type() == T_interpret_value::E_type_reference)
				{
					var_id = interpret_value.get_attribute_value().transformed.get_int();
					return  & A_msg[var_id].get_attribute_value();
				}
				else
				{
					var_id = A_msg.size() - (iter - A_msg.rbegin()) - 1;
					return  & interpret_value.get_attribute_value();
				}
			}
			else if (interpret_value.get_type() == T_interpret_value::E_type_msg)
			{
				const char  * following_name = NULL;
				if (full_name.find(interpret_value_name) == 0)
				{
					following_name = full_name.c_str() + interpret_value_name.length() + 1;
				}
				else if (var_name.find(interpret_value_name) == 0)
				{
					following_name = var_name.c_str() + interpret_value_name.length() + 1;
				}
				if (following_name != NULL)
				{
					C_value::T_msg    msg = interpret_value.get_attribute_value().transformed.get_msg();
					if (msg == NULL)
					{
						M_FATAL_COMMENT("Message reference " << var_name << " not initialized");
					}

					const T_interpret_read_values  * P_interpret_read_values_msg = 
												 static_cast<const T_interpret_read_values  *>(msg);
//					if (P_interpret_read_values_msg == NULL_PTR)
//					{
//						M_FATAL_COMMENT("Message id=" << msg_id << " (" <<
//										var_name << ") not found. Bug");
//						return  NULL_PTR;
//					}
					const T_attribute_value  * P_attribute_value =
						P_interpret_read_values_msg->get_P_attribute_value_of_read_variable(following_name);

					// could not reference a value of another msg.
					// At this time reference is only for in_out parameter, and
					//  since I do not want to modify data of another msg,
					//  it is normal.
//					var_id = A_msg.size() - (iter - A_msg.rbegin()) - 1;
					var_id = -1;

					return  P_attribute_value;
				}
			}
			else
			{
				// Search value from end of name.
				int  idx_dot = interpret_value_name.size() - var_name.size() -1;
				if ((idx_dot >= 0) &&
					(interpret_value_name[idx_dot] == '.') &&
					(strcmp(var_name.c_str(), interpret_value_name.c_str()+(idx_dot+1)) == 0))
				{
					// Forbidden access.
					if ((strncmp(interpret_value.get_name().c_str(), "global.", 7) == 0) ||
						(strncmp(interpret_value.get_name().c_str(), "pinfo.", 6) == 0))
						continue;

					return  & interpret_value.get_attribute_value();
				}
			}
		}
	}
	else
	{
		for (T_interpret_values::const_reverse_iterator
												iter  = A_msg_rstart;
												iter != A_msg_rstop;
											  ++iter)
		{
			const T_interpret_value  & interpret_value = *iter;
			const string             & interpret_value_name = interpret_value.get_name();

			if (var_name == interpret_value_name)
			{
				if (interpret_value.get_type() == T_interpret_value::E_type_reference)
				{
					var_id = interpret_value.get_attribute_value().transformed.get_int();
					return  & A_msg[var_id].get_attribute_value();
				}
				else
				{
					var_id = A_msg.size() - (iter - A_msg.rbegin()) - 1;
					return  & interpret_value.get_attribute_value();
				}
			}
			else if (interpret_value.get_type() == T_interpret_value::E_type_msg)
			{
				const char  * following_name = NULL;
				if (var_name.find(interpret_value_name) == 0)
				{
					following_name = var_name.c_str() + interpret_value_name.length() + 1;
				}
				if (following_name != NULL)
				{
					C_value::T_msg    msg = interpret_value.get_attribute_value().transformed.get_msg();
					if (msg == NULL)
					{
						M_FATAL_COMMENT("Message reference " << var_name << " not initialized");
					}

					const T_interpret_read_values  * P_interpret_read_values_msg =
												 static_cast<const T_interpret_read_values  *>(msg);
//					if (P_interpret_read_values_msg == NULL_PTR)
//					{
//						M_FATAL_COMMENT("Message id=" << msg_id << " (" <<
//										var_name << ") not found. Bug");
//						return  NULL_PTR;
//					}
					const T_attribute_value  * P_attribute_value =
						P_interpret_read_values_msg->get_P_attribute_value_of_read_variable(following_name);

					// could not reference a value of another msg.
					// At this time reference is only for in_out parameter, and
					//  since I do not want to modify data of another msg,
					//  it is normal.
//					var_id = A_msg.size() - (iter - A_msg.rbegin()) - 1;
					var_id = -1;

					return  P_attribute_value;
				}
			}
			else if (full_name_only == false)
			{
				// Search value from end of name.
				int  idx_dot = interpret_value_name.size() - var_name.size() -1;
				if ((idx_dot >= 0) &&
					(interpret_value_name[idx_dot] == '.') &&
					(strcmp(var_name.c_str(), interpret_value_name.c_str()+(idx_dot+1)) == 0))
				{
					// Forbidden access.
					if ((strncmp(interpret_value_name.c_str(), "global.", 7) == 0) ||
						(strncmp(interpret_value_name.c_str(), "pinfo.", 6) == 0))
						continue;

					return  & interpret_value.get_attribute_value();
				}
			}
		}
	}

	if (var_name == "this_msg")
	{
		// Check that this_msg has been set.
		// If not, we are into pre-computation, so this_msg is NOT defined.
		if (A_this_msg_attribute_value.transformed.get_msg() != NULL)
		{
			T_interpret_read_values  * this_not_const = const_cast<T_interpret_read_values*>(this);
			this_not_const->A_this_msg_attribute_value_used = true;
			var_id = -1;
			M_STATE_DEBUG("A_this_msg_attribute_value_used=" << A_this_msg_attribute_value_used);
			return  & A_this_msg_attribute_value;
		}
	}

	return  NULL;
}

const T_interpret_read_values *
T_interpret_read_values::get_P_interpret_read_values_min_max (
                                              const string  & var_name,
													int     & var_idx_min,
													int     & var_idx_max,
													int     & idx_end_var_name) const
{
	const T_interpret_read_values  * P_result = NULL;
	var_idx_min = -1;
	var_idx_max = -1;
	idx_end_var_name = -1;

	string    full_name = A_current_path;
	full_name += ".";
	full_name += var_name;

	string    end_name = ".";
	end_name += var_name;

	for (T_interpret_values::const_reverse_iterator
											iter  = A_msg.rbegin();
											iter != A_msg.rend();
										  ++iter)
	{
		const T_interpret_value  & interpret_value = *iter;

		if ((full_name == interpret_value.get_name()) ||
			(var_name == interpret_value.get_name()))
		{
			if (interpret_value.get_type() == T_interpret_value::E_type_reference)
			{
				continue;
			}
			else
			{
				var_idx_min = A_msg.size() - (iter - A_msg.rbegin()) - 1;
				if (var_idx_max < 0)
					var_idx_max = var_idx_min;
				idx_end_var_name = interpret_value.get_name().size();
				P_result = this;
			}
		}
		else if ((strncmp(var_name.c_str(), interpret_value.get_name().c_str(), var_name.size()) == 0) &&
				 ((interpret_value.get_name()[var_name.size()] == '.') ||
				  (interpret_value.get_name()[var_name.size()] == '[')))
		{
			if (interpret_value.get_type() == T_interpret_value::E_type_reference)
			{
				continue;
			}
			else
			{
				var_idx_min = A_msg.size() - (iter - A_msg.rbegin()) - 1;
				if (var_idx_max < 0)
					var_idx_max = var_idx_min;
				idx_end_var_name = var_name.size();
				P_result = this;
			}
		}
		else if ((strncmp(full_name.c_str(), interpret_value.get_name().c_str(), full_name.size()) == 0) &&
				 ((interpret_value.get_name()[full_name.size()] == '.') ||
				  (interpret_value.get_name()[full_name.size()] == '[')))
		{
			if (interpret_value.get_type() == T_interpret_value::E_type_reference)
			{
				continue;
			}
			else
			{
				var_idx_min = A_msg.size() - (iter - A_msg.rbegin()) - 1;
				if (var_idx_max < 0)
					var_idx_max = var_idx_min;
				idx_end_var_name = full_name.size();
				P_result = this;
			}
		}
		else if (interpret_value.get_type() == T_interpret_value::E_type_msg)
		{
			const char  * following_name = NULL;
			if (full_name.find(interpret_value.get_name()) == 0)
			{
				following_name = full_name.c_str() + interpret_value.get_name().length() + 1;
			}
			else if (var_name.find(interpret_value.get_name()) == 0)
			{
				following_name = var_name.c_str() + interpret_value.get_name().length() + 1;
			}
			if (following_name != NULL)
			{
				C_value::T_msg    msg = interpret_value.get_attribute_value().transformed.get_msg();
				if (msg == NULL)
				{
					M_FATAL_COMMENT("Message reference " << var_name << " not initialized");
				}

				const T_interpret_read_values  * P_interpret_read_values_msg = 
											 static_cast<const T_interpret_read_values  *>(msg);
//					if (P_interpret_read_values_msg == NULL_PTR)
//					{
//						M_FATAL_COMMENT("Message id=" << msg_id << " (" <<
//										var_name << ") not found. Bug");
//						return  NULL_PTR;
//					}
				P_result =
					P_interpret_read_values_msg->
					get_P_interpret_read_values_min_max(following_name, var_idx_min, var_idx_max, idx_end_var_name);

				return  P_result;
			}
		}
		else
		{
			// ICIOA

			// Search value from end of name.
			const string::size_type  idx_rfind = interpret_value.get_name().rfind(end_name);
			if ((idx_rfind != string::npos) &&
				((idx_rfind + end_name.size()) == interpret_value.get_name().length()))
			{
				// Forbidden access for global et pinfo.
				if ((strncmp(interpret_value.get_name().c_str(), "global.", 7) != 0) &&
					(strncmp(interpret_value.get_name().c_str(), "pinfo.", 6) != 0))
				{
					var_idx_min = A_msg.size() - (iter - A_msg.rbegin()) - 1;
					if (var_idx_max < 0)
						var_idx_max = var_idx_min;
					idx_end_var_name = interpret_value.get_name().size();
					P_result = this;
				}
			}
		}

		if ((var_idx_max > 0) &&
			(var_idx_min != (A_msg.size() - (iter - A_msg.rbegin()) - 1)))
		{
			// Fin de sequence
			break;
		}
	}

	return  P_result;
}

void
T_interpret_read_values::duplicate_multiple_values(
					  const std::string                var_name_src,
					  const std::string                var_name_dst)
{
	M_STATE_ENTER("duplicate_multiple_values", "src=" << var_name_src << "  dst=" << var_name_dst);

	int   var_idx_min = -1;
	int   var_idx_max = -1;
	int   idx_end_var_name = -1;
	const T_interpret_read_values  * P_result = 
		get_P_interpret_read_values_min_max (var_name_src, var_idx_min, var_idx_max, idx_end_var_name);

	if (P_result == NULL)
	{
		M_FATAL_COMMENT("Field/variable " << var_name_src << " not found.");
	}

	string  full_var_name_dst = A_current_path;
	if ((full_var_name_dst != "") && (var_name_dst != ""))
	{
		full_var_name_dst += '.';
	}
	full_var_name_dst += var_name_dst;

	for (int   idx = var_idx_min; idx <= var_idx_max; ++idx)
	{
		const T_interpret_value  & interpret_value_src = P_result->A_msg[idx];

		// Copy
		T_interpret_value    interpret_value = interpret_value_src;

		// Name replacement
		string  new_name = interpret_value.get_name();
		new_name.replace(0, idx_end_var_name, full_var_name_dst);
		interpret_value.set_name(new_name);

		A_msg.push_back(interpret_value);
	}
}

void
T_interpret_read_values::copy_global_values(
					  const T_interpret_read_values  & interpret_read_values_src)
{
	if (A_msg_global_idx_end >= A_msg_global_idx_begin)
	{
		// already exist
		copy_multiple_values(interpret_read_values_src, "global.*", A_msg_global_idx_begin, A_msg_global_idx_end);
	}
	else
	{
		// append
		A_msg_global_idx_begin = A_msg.size();
		copy_multiple_values(interpret_read_values_src, "global.*", -1, -1);
		A_msg_global_idx_end = A_msg.size();
		A_msg_other_idx_begin = A_msg_global_idx_end;
	}
}

void
T_interpret_read_values::copy_multiple_values(
					  const T_interpret_read_values  & interpret_read_values_src,
					  const std::string                var_name_with_star,
					        int                        dest_idx_begin,
					  const int                        dest_idx_end)
{
	M_STATE_ENTER("copy_multiple_values", var_name_with_star);

	const string::size_type  pos_star = var_name_with_star.find('*');

	if (pos_star == string::npos)
	{
		M_FATAL_COMMENT("* not found at the end of the field/variable name.");
	}

	if (pos_star != var_name_with_star.size()-1)
	{
		M_FATAL_COMMENT("* must be only at the end of the field/variable name.");
	}

	if (&interpret_read_values_src == (const T_interpret_read_values *)this)
	{
		M_FATAL_COMMENT("Impossible to copy on itself.");
	}

	int    nb_of_values_copied = 0;
	for (T_interpret_values::const_iterator    iter  = interpret_read_values_src.A_msg.begin();
											   iter != interpret_read_values_src.A_msg.end();
											 ++iter)
	{
		const string             & long_name = iter->get_name();
		const string::size_type    long_name_size = long_name.size();

		if (long_name_size < pos_star)
		{
			if (nb_of_values_copied > 0)
			{
				// End of copy sequence
				break;
			}
			continue;
		}
		if (strncmp(long_name.c_str(), var_name_with_star.c_str(), pos_star) == 0)
		{
			if (dest_idx_end >= 0)
			{
				// already exist
				if (dest_idx_begin >= dest_idx_end)
				{
					M_FATAL_COMMENT("More global data than before !");
				}

				A_msg[dest_idx_begin] = *iter;
				++dest_idx_begin;
			}
			else
			{
				// append
				A_msg.push_back(*iter);
			}
			++nb_of_values_copied;
		}
		else if (nb_of_values_copied > 0)
		{
			// End of copy sequence
			break;
		}
	}

	M_STATE_LEAVE("from nb_values_src=" << interpret_read_values_src.A_msg.size() <<
				  " nb_of_values_copied=" << nb_of_values_copied <<
				  " gives nb_values_dst=" << A_msg.size());
}

void    
T_interpret_read_values::get_multiple_P_attribute_value_of_read_variable (
							const string               & var_name_with_star,
								  T_var_name_P_values  & name_values) const
{
	M_STATE_ENTER("get_multiple_P_attribute_value_of_read_variable", var_name_with_star);

	const string::size_type  pos_star = var_name_with_star.find('*');

	if (pos_star == string::npos)
	{
		M_FATAL_COMMENT("* not found at the end of the field/variable name.");
	}

	if (pos_star != var_name_with_star.size()-1)
	{
		M_FATAL_COMMENT("* must be only at the end of the field/variable name.");
	}

	for (T_interpret_values::const_iterator    iter  = A_msg.begin();
											   iter != A_msg.end();
											 ++iter)
	{
		const string             & long_name = iter->get_name();
		const string::size_type    long_name_size = long_name.size();

		if (long_name_size < pos_star)
		{
			continue;
		}
		if (strncmp(long_name.c_str(), var_name_with_star.c_str(), pos_star) == 0)
		{
			T_var_name_P_value    var_name_P_value;
			var_name_P_value.var_name = long_name;
			var_name_P_value.P_value  = &iter->get_attribute_value();

			name_values.push_back(var_name_P_value);
		}
	}
}

const T_attribute_value *
T_interpret_read_values::get_P_attribute_value_of_read_variable (
                                               const string  & var_name) const
{
	T_id    id_do_not_care;

	return  get_P_attribute_value_of_read_variable(var_name, id_do_not_care);
}

const T_attribute_value &
T_interpret_read_values::get_attribute_value_of_read_variable (
                                               const string  & var_name) const
{
	const T_attribute_value  * P_attr = get_P_attribute_value_of_read_variable(var_name);
    if (P_attr == NULL)
	{
        static const T_attribute_value  default_attr;
        return  default_attr;
    }

    return  * P_attr;
}

//*****************************************************************************
// get_full_str_value_of_read_variable
//*****************************************************************************
string
T_interpret_read_values::get_full_str_value_of_read_variable (const string  & var_name) const
{
	const T_attribute_value &  attribute_value =
	      get_attribute_value_of_read_variable(var_name);

	return  attribute_value_to_string(attribute_value);
}

//*****************************************************************************
// reset
//*****************************************************************************
void    
T_interpret_read_values::reset()
{
	M_STATE_ENTER("reset", "");

	A_msg.clear();
	A_current_path = "";
	A_msg_global_idx_begin = 0;
	A_msg_global_idx_end = -1;
	A_msg_pinfo_idx_begin = 0;
	A_msg_pinfo_idx_end = -1;
	A_msg_other_idx_begin = 0;
	A_this_msg_attribute_value = T_attribute_value();
}

//*****************************************************************************
// reset_short_names
//*****************************************************************************
void    
T_interpret_read_values::reset_short_names()
{
	M_STATE_ENTER("reset_short_names", "");
// fonction a supprimer
}

//*****************************************************************************
// reset_position_offset_sizes
//*****************************************************************************
void
T_interpret_read_values::reset_position_offset_sizes()
{
	M_STATE_ENTER("reset_position_offset_sizes", "");

	// Reset position size.
	// Not used once the msg is finished.
	// No sense to use it in another msg and crash possible.
	for (T_interpret_values::iterator    iter  = A_msg.begin();
									     iter != A_msg.end();
									   ++iter)
	{
		T_interpret_value &  interpret_value = *iter;
		T_attribute_value &  attribute_value = const_cast<T_attribute_value&>(interpret_value.get_attribute_value());
		attribute_value.transformed.set_bit_position_offset_size(-1, -1);
	}
}

//*****************************************************************************
// msg_is_ended
//*****************************************************************************
// Interpretation of the msg is terminated.
// So content is useless, except for :
// - global : all global must be kept
// - global + this_msg : all data must be kept
//*****************************************************************************
void
T_interpret_read_values::msg_is_ended()
{
	M_STATE_ENTER("msg_is_ended", "");

	// Check there is some global data.
	if (A_msg_global_idx_end <= A_msg_global_idx_begin)
	{
		// No global data, so nothing to do, this object will be deleted (or reseted)
		M_STATE_LEAVE("No global data  A_msg_global_idx_begin=" << A_msg_global_idx_begin << "  A_msg_global_idx_end=" << A_msg_global_idx_end);
		return;
	}

	// Check this_msg has been used
	if (A_this_msg_attribute_value_used == false)
	{
		M_STATE_DEBUG("Remove all data (except global) : from idx " << A_msg_global_idx_end << " to " << A_msg.size()-1);

		// We can remove every data (except global)
		//  because they will NOT be accessed (through this_msg in global)
		A_msg.erase(A_msg.begin() + A_msg_global_idx_end, A_msg.end());

		A_current_path = "";
		A_msg_pinfo_idx_begin = 0;
		A_msg_pinfo_idx_end = -1;
		A_msg_other_idx_begin = A_msg_global_idx_end;
		A_this_msg_attribute_value = T_attribute_value();
		A_this_msg_attribute_value_used = false;
	}
	else
	{
		// this_msg has been used.
		// It could have been save inside global data.
		// So can not remove any data.
		M_STATE_DEBUG("this_msg has been used");
	}

	// position and size are no more necessary
	reset_position_offset_sizes();
}

//*****************************************************************************
// add_this_msg
//*****************************************************************************
void
T_interpret_read_values::add_this_msg()
{
	A_this_msg_attribute_value = T_attribute_value(C_value(C_value::E_type_msg, this));
	A_this_msg_attribute_value_used = false;
}

//*****************************************************************************
// swap
//*****************************************************************************
void    swap(T_interpret_read_values  & lhs,
			 T_interpret_read_values  & rhs)
{
	swap(lhs.A_msg,                           rhs.A_msg);
	swap(lhs.A_msg_global_idx_begin,          rhs.A_msg_global_idx_begin);
	swap(lhs.A_msg_global_idx_end,            rhs.A_msg_global_idx_end);
	swap(lhs.A_msg_pinfo_idx_begin,           rhs.A_msg_pinfo_idx_begin);
	swap(lhs.A_msg_pinfo_idx_end,             rhs.A_msg_pinfo_idx_end);
	swap(lhs.A_msg_other_idx_begin,           rhs.A_msg_other_idx_begin);
	swap(lhs.A_current_path,                  rhs.A_current_path);
	swap(lhs.A_this_msg_attribute_value,      rhs.A_this_msg_attribute_value);
	swap(lhs.A_this_msg_attribute_value_used, rhs.A_this_msg_attribute_value_used);
}










void
T_interpret_read_values::read_variable_group_begin(const std::string        & name)
{
	M_STATE_ENTER("read_variable_group_begin", A_current_path << " " << name);

	if (A_current_path != "")
	{
		A_current_path += ".";
	}

	A_current_path += name;
}

void
T_interpret_read_values::read_variable_group_end()
{
	M_STATE_ENTER("read_variable_group_end", A_current_path);

	M_FATAL_IF_EQ(A_current_path, "");

	string::size_type  idx_separator = A_current_path.find_last_of('.');
	if (idx_separator == string::npos)
	{
		A_current_path = "";
	}
	else
	{
		A_current_path.erase(idx_separator);
	}
}

void
T_interpret_read_values::global_variable_group_begin()
{
	M_STATE_ENTER("global_variable_group_begin", A_current_path);

	// global must be at root level
	M_FATAL_IF_NE(A_current_path, "");

	read_variable_group_begin("global");
	A_msg_global_idx_begin = A_msg.size();
}

void
T_interpret_read_values::global_variable_group_end()
{
	M_STATE_ENTER("global_variable_group_end", A_current_path);
	A_msg_global_idx_end = A_msg.size();
	A_msg_other_idx_begin = A_msg_global_idx_end;
	read_variable_group_end();
}

void
T_interpret_read_values::pinfo_variable_group_begin()
{
	M_STATE_ENTER("pinfo_variable_group_begin", A_current_path);

	// pinfo must be at root level
	M_FATAL_IF_NE(A_current_path, "");

	read_variable_group_begin("pinfo");
	A_msg_pinfo_idx_begin = A_msg.size();
}

void
T_interpret_read_values::pinfo_variable_group_end()
{
	M_STATE_ENTER("pinfo_variable_group_end", A_current_path);
	A_msg_pinfo_idx_end = A_msg.size();
	A_msg_other_idx_begin = A_msg_pinfo_idx_end;
	read_variable_group_end();
}

/*
 * Copyright 2005-2012 Olivier Aveline <wsgd@free.fr>
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

#ifndef T_interpret_read_values_h
#define T_interpret_read_values_h

//*****************************************************************************
// Includes.
//*****************************************************************************

#include <string>
#include <map>
using namespace std;

#include "C_reference_counter.h"
#include "C_reference_counter_ptr.h"
#include "T_attribute_value.h"

// ICIOA new
#include "T_interpret_value.h"
//#define USE_IVTREE
#define USE_IVVECTOR

//*****************************************************************************
// T_interpret_read_values
//*****************************************************************************

struct T_interpret_read_values : public C_reference_counter,
                                 public CT_debug_object_counter<T_interpret_read_values>
{
	typedef int       T_id;

    bool              is_read_variable (const string   & var_name) const;
    bool    get_value_of_read_variable (const string   & var_name,
                                              C_value  & out_value) const;
    const C_value  * get_P_value_of_read_variable (const string   & var_name) const;


	const string &  get_str_value_of_read_variable (
                                              const string  & var_name) const;
    bool    get_int_value_of_read_variable (
                                      const string     & var_name,
                                            long long  & out_value) const;

    T_id    add_read_variable (const string   & var_name_long,
                               const string   & var_name_short,
                               const C_value  & in_value);

	void    set_read_variable (const string   & var_name,
                               const C_value  & in_value);
    void    set_read_variable (const string             & var_name,
                               const T_attribute_value  & in_value);

	// 
    void    sup_read_variable (T_id    var_id);
    void    sup_read_variables (int     nb_of_var, T_id    var_id[]);
    void    sup_all_read_variables_after (T_id    var_id);

	// 2008/11/12 new T_attribute_value interface
    T_id    add_read_variable (const string             & var_name_long,  // not used anymore
							   const string             & var_name_short,
                               const T_attribute_value  & in_value);

//    T_id    get_id_of_read_variable (const string   & var_name);
    T_id    get_id_of_last_read_variable () const;

	const T_attribute_value &  get_attribute_value_of_read_variable (
                                              const string  & var_name) const;
    const T_attribute_value *  get_P_attribute_value_of_read_variable (
                                              const string  & var_name) const;

	string    get_full_str_value_of_read_variable (
                                              const string  & var_name) const;

	// ref_name is a reference to (the value of) var_name.
	T_id	add_ref_variable(const string  & ref_name,
							 const string  & var_name);

	// msg_name is a reference to msg data.
//	T_id	add_msg_variable(const string  & msg_name,
//							 const string  & var_name);

	void    reset();
	void    reset_short_names();

	struct T_var_name_P_value : public CT_debug_object_counter<T_var_name_P_value>
	{
		string               var_name;
		const T_attribute_value  * P_value;
	};
	typedef vector<T_var_name_P_value>    T_var_name_P_values;

	// NB: interpret_read_values_src could not be this
	void	copy_multiple_values(
					  const T_interpret_read_values  & interpret_read_values_src,
					  const std::string                var_name_with_star);

    void    get_multiple_P_attribute_value_of_read_variable (
							const string               & var_name_with_star,
								  T_var_name_P_values  & name_values) const;

	void	duplicate_multiple_values(
					  const std::string                var_name_src,
					  const std::string                var_name_dst);


// ICIOA ivvector begin 
	void    read_variable_group_begin(const std::string  & name);
	void    read_variable_group_end();

	vector<T_interpret_value>  DEBUG_get_msg() const { return  A_msg;}
	std::string                DEBUG_get_current_path() const { return  A_current_path;}

private:
	typedef vector<T_interpret_value>      T_interpret_values;
	vector<T_interpret_value>    A_msg;
	std::string                  A_current_path;
// ICIOA ivvector end 

	// ICIOA msg
	T_attribute_value            A_this_msg_attribute_value;

    const T_attribute_value *  get_P_attribute_value_of_read_variable (
                                              const string  & var_name,
													T_id    & var_id) const;

    const T_interpret_read_values *  get_P_interpret_read_values_min_max (
                                              const string  & var_name,
													int     & var_idx_min,
													int     & var_idx_max,
													int     & idx_end_var_name) const;

	friend void    swap(T_interpret_read_values  & lhs,
						T_interpret_read_values  & rhs);
};

typedef C_reference_counter_ptr<T_interpret_read_values>        T_RCP_interpret_read_values;
typedef C_reference_counter_ptr<const T_interpret_read_values>  T_RCP_const_interpret_read_values;

void    swap(T_interpret_read_values  & lhs,
			 T_interpret_read_values  & rhs);

#endif /* T_interpret_read_values_h */

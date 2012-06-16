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

// ****************************************************************************
// Includes.
// ****************************************************************************

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "precomp.h"
#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "T_type_definitions.h"
#include "F_common_print.h"

// FRHED needs this after previous include to find "std::find" !
#include <algorithm>
using namespace std;


//*****************************************************************************
// Identifier syntax.
//*****************************************************************************

bool    is_a_valid_identifier_name (const string   & var_name,
									const char       also_authorized)
{
	// Must not be empty
	if (var_name.empty())
		return  false;

	// Must start with a letter or an _
	if ((isalpha(var_name[0]) == 0) && (var_name[0] != '_'))
		return  false;

	// Must only contain letter, number or _
	// NB: ":" is reserved to enums (<Enum_type_name>::<enum_symbolic_value>)
	// NB: "." is reserved to <name1>.<name2>
	for (int  idx = 0; idx < var_name.size(); ++idx)
	{
		if ((isalnum(var_name[idx]) == 0) &&
			(var_name[idx] != '_') &&
			(var_name[idx] != also_authorized))
		{
			return  false;
		}
	}

	return  true;
}
#if 0
bool    is_a_valid_short_identifier_name (const string   & var_name)
{
	// Must not be empty
	if (var_name.empty())
		return  false;

	// Must start with a letter or an _
	if ((isalpha(var_name[0]) == 0) && (var_name[0] != '_'))
		return  false;

	// Must only contain letter, number or _
	// NB: ":" is reserved to enums (<Enum_type_name>::<enum_symbolic_value>)
	// NB: "." is reserved to <name1>.<name2>
	for (int  idx = 0; idx < var_name.size(); ++idx)
	{
		if ((isalnum(var_name[0]) == 0) && (var_name[0] != '_'))
			return  false;
	}

	return  true;
}
#endif

//*****************************************************************************
// Aliases.
//*****************************************************************************

void
T_type_definitions::add_alias( const string      & alias_name,
                               const string      & already_existent_type,
                               const E_override    must_override)
{
	// Check it is possible to define a new enum type with this name.
	// No returned value, FATAL is called if it is not possible.
	could_define_new_type(alias_name, T_type_definitions::E_type_alias, must_override);

    map_alias_type[alias_name] = already_existent_type;    // possible override ok
}

// Search recursively into the map of aliases to find the final type to be
//  used.
const string &
T_type_definitions::get_final_type (const string  & orig_type) const
{
    T_map_alias_type::const_iterator  iter = map_alias_type.find (orig_type);
    if (iter != map_alias_type.end ())
    {
        return  get_final_type (iter->second);
    }
    else
    {
        return  orig_type;
    }
}

//*****************************************************************************
// Field base
//*****************************************************************************
bool
T_field_type_name_base::has_post_treatment() const
{
	bool  has_post_treatment = false;

	if ((no_statement.as_string() != "") ||
		(transform_expression.is_defined()) ||
		(transform_quantum.as_string() != "") ||
		(transform_offset.as_string() != "") ||
		(str_display != "") ||
		(str_display_expression != "") ||
		(constraints.empty() == false) ||
		(str_size_or_parameter != ""))
	{
		has_post_treatment = true;
	}

	return  has_post_treatment;
}

//*****************************************************************************
// Field
//*****************************************************************************

T_field_type_name::T_field_type_name()
	:output_directive(E_output_directive_none),
	 A_is_a_variable(false),
	 wsgd_field_idx (-1)
{
}

// Because of transform (or ...), the resulting value could change.
// NB: the value is no more read from the packet by wireshark.
bool
T_field_type_name::must_force_manage_as_biggest_float() const
{
	if (transform_expression.is_defined())
	{
		if (transform_expression_type == C_value::E_type_float)
		{
			return  true;
		}
		else if ((transform_expression_type == C_value::E_type_msg) &&
				 (strncmp(type.c_str(), "float", 5) == 0))
		{
			return  true;
		}
	}

	if ((transform_quantum.as_string() != "") ||
		(transform_offset.as_string() != ""))
	{
		if ((transform_quantum.get_type() == C_value::E_type_float) ||
			(transform_offset.get_type() == C_value::E_type_float))
		{
			return  true;
		}
		else if (strncmp(type.c_str(), "float", 5) == 0)
		{
			return  true;
		}
	}

	// ICIOA 2010/12/30 temporary since when ?
	// currently necessary because it seems that var expression
	//  are automatically managed as float64
	//  independenty of the type specified.
	// See coment into C_byte_interpret_wsgd_builder::value
	// So "var float64  toto = 12.3" is identic to :
	//    "var float32  toto = 12.3"
#if 1
	if ((new_expression.is_defined() == true) &&
		(strncmp(type.c_str(), "float", 5) == 0))
	{
		return  true;
	}
#endif
	return  false;
}

bool
T_field_type_name::must_force_manage_as_biggest_int() const
{
	if (must_force_manage_as_biggest_float() == false)
	{
		if (transform_expression.is_defined())
		{
			return  true;
		}
		if ((transform_quantum.as_string() != "") ||
			(transform_offset.as_string() != ""))
		{
			return  true;
		}

		// ICIOA 2010/12/30 see comment into must_force_manage_as_biggest_float
#if 1
		if ((new_expression.is_defined() == true) &&
			(strncmp(type.c_str(), "float", 5) != 0) &&
			(strncmp(type.c_str(), "string", 6) != 0))
		{
			return  true;
		}
#endif
	}

	return  false;
}

ostream &  operator<< (ostream & os, const T_field_type_name  & rhs)
{
	if (rhs.is_a_variable())
		os << "var ";

    os << rhs.type;

	if (rhs.transform_quantum.as_string() != "")
	{
		os << "{q=" << rhs.transform_quantum.as_string();
		if (rhs.transform_offset.as_string() != "")
		{
			os << ":o=" << rhs.transform_offset.as_string();
		}
		os << "}";
	}
	else if (rhs.transform_offset.as_string() != "")
	{
		os << "{o=" << rhs.transform_offset.as_string() << "}";
	}

	for (vector<T_field_constraint>::const_iterator  iter  = rhs.constraints.begin();
													 iter != rhs.constraints.end();
												   ++iter)
	{
		const T_field_constraint  & constraint = *iter;

		if (constraint.equal.as_string() != "")
		{
			os << "{val=" << constraint.equal.as_string() << "}";
		}
		else if (constraint.min.as_string() != "")
		{
			os << "{min=" << constraint.min.as_string();
			if (constraint.max.as_string() != "")
			{
				os << ":max=" << constraint.max.as_string();
			}
			os << "}";
		}
		else if (constraint.max.as_string() != "")
		{
			os << "{max=" << constraint.max.as_string() << "}";
		}
	}

	if (rhs.str_display != "")
	{
		os << "{d=" << rhs.str_display << "}";
	}
	else if (rhs.str_display_expression != "")
	{
		os << "{de=" << rhs.str_display_expression << "}";
	}

	if (rhs.str_size_or_parameter != "")
	{
		os << "(" << rhs.str_size_or_parameter << ")";
	}

	os << "    ";
	os << rhs.name;

	if (rhs.new_expression.is_defined())
	{
		os << " = " << rhs.new_expression.get_original_string_expression();
	}

	if (rhs.P_bitfield)
    {
        os << "bitfield";
	}
	else if (rhs.P_switch)
    {
        os << "switch";
	}
	else if (rhs.P_sub_struct)
    {
        os << endl;
        os << "{" << endl;
        print (os, *rhs.P_sub_struct, "");
        os << "}";

		if (rhs.sub_struct_2.empty () == false)
		{
			os << endl;
			os << "else" << endl;
			os << "{" << endl;
			print (os, rhs.sub_struct_2, "");
			os << "}";
		}
    }
	else
	{
		os << " ;";
	}

    return  os;
}

//*****************************************************************************
// Struct 
//*****************************************************************************

ostream &  operator<< (ostream & os, const T_struct_fields  & rhs)
{
  for (T_struct_fields::const_iterator  iter  = rhs.begin ();
                                        iter != rhs.end ();
                                      ++iter)
  {
    os << *iter << endl;
  }
  return  os;
}

ostream &  operator<< (ostream & os, const T_struct_definition  & rhs)
{
  os << "field_struct_idx " << rhs.field_struct_idx << endl;
  os << "printf_args " << rhs.printf_args << endl;
  os << rhs.fields;
  return  os;
}

//*****************************************************************************
// Enums
//*****************************************************************************

ostream &  operator<< (ostream & os, const T_enum_name_val  & rhs)
{
    return  os << rhs.name << "    " << rhs.value;
}

ostream &  operator<< (ostream & os,
                 const T_enum_definition_representation  & rhs)
{
    print (os, rhs.definition, "");
    os << rhs.bit_size << " ";
    os << rhs.is_signed << " ";
    os << rhs.representation_type;
    return  os;
}

bool
T_enum_definition_representation::get_integer_value(const string     & symbolic_name,
														  long long  & integer_value) const
{
    const T_enum_definition  & enum_def = definition;

	for (T_enum_definition::const_iterator  iter  = enum_def.begin ();
											iter != enum_def.end ();
										  ++iter)
	{
		if (symbolic_name == iter->name)
		{
			integer_value = iter->value;
			return  true;
		}
	}

	return  false;
}

const string &
T_enum_definition_representation::get_symbolic_name(long long    integer_value) const
{
    const T_enum_definition  & enum_def = definition;

	for (T_enum_definition::const_iterator  iter  = enum_def.begin ();
											iter != enum_def.end ();
										  ++iter)
	{
		if (integer_value == iter->value)
		{
			return  iter->name;
		}
	}

	static const string    not_found;
	return  not_found;
}

#if 0
// returns false if value does NOT match
bool    print_enum_value (ostream            & os,
                          long long            value,
                    const T_enum_definition  & enum_definition)
{
    for (int  idx = 0; idx < enum_definition.size (); ++idx)
    {
        if (enum_definition[idx].value == value)
        {
            os << enum_definition[idx].name << " ("
               << value << ")";
            return  true;
        }
    }

    os << "Enum_unknown_value" << " ("
       << value << ")";
    return  false;
}
#endif

#if 0
// returns 
string    enum_value_to_string (long long            value,
                          const T_enum_definition  & enum_definition,
						        bool               & no_error)
{
    for (uint  idx = 0; idx < enum_definition.size (); ++idx)
    {
        if (enum_definition[idx].value == value)
        {
			no_error = true;
            return  enum_definition[idx].name + " (" + get_string(value) + ")";
        }
    }

	no_error = false;
    return  "Enum_unknown_value (" + get_string(value) + ")";
}
#endif

//*****************************************************************************
// Switchs
//*****************************************************************************

ostream &  operator<< (ostream              & os,
                 const T_switch_case_value  & rhs)
{
  if (rhs.is_default_case)
    os << "default : ";
  else
	  os << "case " << rhs.case_value.as_string() << " : ";

  if (rhs.fields.size () > 1)
    os << endl;
    
  os << rhs.fields;
  
  return  os;
}

ostream &  operator<< (ostream              & os,
                 const T_switch_definition  & rhs)
{
    for (T_switch_case::const_iterator  iter  = rhs.switch_case.begin ();
                                        iter != rhs.switch_case.end ();
                                      ++iter)
    {
      os << *iter << endl;
    }
  return  os;
}

//*****************************************************************************
// check_function_parameter_value *********************************************
//*****************************************************************************

void    check_function_parameter_value(
				const T_type_definitions    & type_definitions,
				const T_function_parameter  & function_parameter,
				const C_value               & obj_value)
{
	// Check value type.
	if (obj_value.get_type() == C_value::E_type_float)
	{
		if ((function_parameter.type != "any") &&
			(function_parameter.type != "float32") &&
			(function_parameter.type != "float64"))
		{
			M_FATAL_COMMENT("Float value for parameter " << function_parameter.name << ". Expecting " << function_parameter.type);
		}
	}
	else if (obj_value.get_type() == C_value::E_type_string)
	{
		if ((function_parameter.type != "any") &&
			(function_parameter.type != "string"))
		{
			M_FATAL_COMMENT("String value for parameter " << function_parameter.name << ". Expecting " << function_parameter.type);
		}
	}
    else if (obj_value.get_type() == C_value::E_type_msg)
    {
        if ((function_parameter.type != "any") &&
			(function_parameter.type != "msg"))
        {
            M_FATAL_COMMENT("msg value for parameter " << function_parameter.name << ". Expecting " << function_parameter.type);
        }
    }
	else // integer
	{
#define M_READ_SIMPLE_TYPE_BASE(TYPE_NAME,TYPE_SIZE,TYPE_IMPL,TYPE_IMPL_SIZE) \
	else if (function_parameter.type == TYPE_NAME)                            \
	{                                                                         \
		M_FATAL_IF_GT (TYPE_SIZE, TYPE_IMPL_SIZE);                            \
																			  \
		TYPE_IMPL    value = static_cast<TYPE_IMPL>(obj_value.get_int());     \
		C_value  value_to_compare(value);                                     \
		if (value_to_compare != obj_value)                                    \
		{                                                                     \
			M_FATAL_COMMENT("Overflow value " << obj_value.get_int() << " on parameter " << function_parameter.type << " " << function_parameter.name);         \
		}                                                                     \
	}

#define M_READ_SIMPLE_TYPE(TYPE_NAME,TYPE_SIZE,TYPE_IMPL)                     \
	M_READ_SIMPLE_TYPE_BASE(TYPE_NAME, (TYPE_SIZE / 8), TYPE_IMPL, sizeof (TYPE_IMPL))

		const T_enum_definition_representation  * P_enum = NULL;

		if (function_parameter.type == "any") ;
		M_READ_SIMPLE_TYPE ( "int8",    8,    signed char)
		M_READ_SIMPLE_TYPE ("uint8",    8,  unsigned char)
		M_READ_SIMPLE_TYPE ( "int16",  16,    signed short)
		M_READ_SIMPLE_TYPE ("uint16",  16,  unsigned short)
		M_READ_SIMPLE_TYPE ( "int24",  24,    signed int)
		M_READ_SIMPLE_TYPE ("uint24",  24,  unsigned int)
		M_READ_SIMPLE_TYPE ( "int32",  32,    signed int)
		M_READ_SIMPLE_TYPE ("uint32",  32,  unsigned int)
		M_READ_SIMPLE_TYPE ( "int64",  64,    signed long long)
		M_READ_SIMPLE_TYPE ("uint64",  64,  unsigned long long)
		else if ((P_enum = type_definitions.get_P_enum(function_parameter.type)) != NULL)
		{
			if (P_enum->get_symbolic_name(obj_value.get_int()) == "")
			{
				M_FATAL_COMMENT("Bad integer value " << obj_value.get_int() << " for parameter " << function_parameter.type << " " << function_parameter.name);
			}
		}
		else
		{
			M_FATAL_COMMENT("Integer value for parameter " << function_parameter.name << ". Expecting " << function_parameter.type);
		}

#undef  M_READ_SIMPLE_TYPE_BASE
#undef  M_READ_SIMPLE_TYPE
	}
}

//****************************************************************************
// Functions
//****************************************************************************
void
T_function_parameter::set_default_value(const C_value  & rhs)
{
	A_default_value = rhs;
	A_has_default_value = true;
}

ostream &  operator<< (ostream               & os,
                 const T_function_parameter  & rhs)
{
  os << "  " << rhs.direction << rhs.type << "  " << rhs.name;
  return  os;
}

T_function_definition::T_function_definition()
	:A_nb_of_mandatory_parameters(0)
{
}

void
T_function_definition::add_function_parameter(const T_function_parameter  & function_parameter)
{
	function_parameters.push_back(function_parameter);

	if (function_parameter.has_default_value() == false)
    {
      ++A_nb_of_mandatory_parameters;
    }
}

ostream &  operator<< (ostream                & os,
                 const T_function_definition  & rhs)
{
	os << rhs.return_type << "  (";

	for (T_function_parameters::const_iterator  iter  = rhs.function_parameters.begin ();
										        iter != rhs.function_parameters.end ();
                                              ++iter)
    {
      os << *iter << endl;
    }

	os << ")" << endl;
	return  os;
}

//*****************************************************************************
// T_type_definitions << ******************************************************
//*****************************************************************************
ostream &  operator<< (ostream & os,
                 const T_type_definitions  & rhs)
{
  for (T_map_alias_type::const_iterator  iter  = rhs.map_alias_type.begin ();
                                         iter != rhs.map_alias_type.end ();
                                       ++iter)
  {
    os << "alias " << iter->first << " " << iter->second << endl;
  }
  os << endl;
  
  for (T_map_struct_definition::const_iterator  iter  = rhs.map_struct_definition.begin ();
                                                iter != rhs.map_struct_definition.end ();
                                              ++iter)
  {
    os << "struct " << iter->first << endl;
	os << "{" << endl;

	for (T_struct_fields::const_iterator  iter_s  = iter->second.fields.begin ();
										  iter_s != iter->second.fields.end ();
                                        ++iter_s)
    {
      os << "  " << *iter_s << endl;
    }
	os << "}" << endl;
  }
  os << endl;

  for (T_map_enum_definition_representation::const_iterator
                  iter  = rhs.map_enum_definition_representation.begin ();
                  iter != rhs.map_enum_definition_representation.end ();
                ++iter)
  {
    os << "enum" << iter->second.bit_size << "  " << iter->first << endl;
	os << "{" << endl;
	os << "  " << iter->second << endl;
	os << "}" << endl;
  }
  os << endl;

  for (T_map_switch_definition::const_iterator  iter  = rhs.map_switch_definition.begin ();
                                                iter != rhs.map_switch_definition.end ();
                                              ++iter)
  {
    os << "switch " << iter->first << endl;
	os << "{" << endl;
	os << "  " << iter->second << endl;
	os << "}" << endl;
  }
  os << endl;



  os << "Pas fini" << endl;
  return  os;
}

//*****************************************************************************
// string_to_numeric **********************************************************
//*****************************************************************************

#include "byte_interpret_compute_expression.h"

C_value    string_to_numeric(const T_type_definitions  & type_definitions,
							 const string              & str_to_convert_to_numeric,
							 const string              & field_name,
							 const char                * attr)
{
	C_value    val = str_to_convert_to_numeric;

	val.promote();

	if (val.is_numeric() == false)
	{
		// Could be a static expression (without any attribute/variable).
		val = compute_expression_static (type_definitions, val.as_string());
//		M_FATAL_COMMENT(attr << " (" << str << ") is not a numeric value");
	}

	return  val;
}

//*****************************************************************************
// T_type_definitions Field
//*****************************************************************************
void 
T_type_definitions::set_field_type(T_field_type_name_base  & field,
					 const string             & type_param) const
{
//	M_FATAL_IF_NE(field.type, "");
	field.type = this->get_final_type (type_param);
}

void 
T_type_definitions::set_field_name(T_field_type_name_base  & field,
					 const string             & name_param) const
{
//	M_FATAL_IF_NE(field.name, "");
	field.name = name_param;
}

void 
T_type_definitions::set_field_condition_expression(T_field_type_name_base  & field,
					 const string             & name_param) const
{
//	M_FATAL_IF_NE(field.name, "");
	field.name = name_param;
}

void 
T_type_definitions::set_field_type_size_or_parameter(T_field_type_name_base  & field,
						  const string             & size_param) const
{
	M_FATAL_IF_NE(field.str_size_or_parameter, "");

	field.str_size_or_parameter = size_param;

	if ((field.str_size_or_parameter == "") &&
		(field.type != "string") &&
		(field.type != "string_nl"))
	{
		M_FATAL_COMMENT("Empty () for " << field.type);
	}
}

void 
T_type_definitions::set_field_no_statement(T_field_type_name_base  & field,
								  const string             & val_param) const
{
	M_FATAL_IF_NE(field.no_statement.as_string(), "");

	field.no_statement = string_to_numeric(*this, val_param, field.name, "no_statement");
}

void 
T_type_definitions::set_field_transform_quantum(T_field_type_name_base  & field,
								  const string             & val_param) const
{
	M_FATAL_IF_NE(field.transform_quantum.as_string(), "");
	M_FATAL_IF_NE(field.transform_offset.as_string(), "");

	field.transform_quantum = string_to_numeric(*this, val_param, field.name, "quantum");
}

void 
T_type_definitions::set_field_transform_offset (T_field_type_name_base  & field,
								  const string             & val_param) const
{
	M_FATAL_IF_NE(field.transform_offset.as_string(), "");

	field.transform_offset = string_to_numeric(*this, val_param, field.name, "offset");
}

void 
T_type_definitions::set_field_transform_expression (
										T_field_type_name_base  & field,
								  const string                  & expr_param) const
{
	M_FATAL_IF_NE(field.transform_quantum.as_string(), "");
	M_FATAL_IF_NE(field.transform_offset.as_string(), "");
	M_FATAL_IF_NE(field.transform_expression.is_defined(), false);

	field.transform_expression.build_expression(*this, expr_param);
	field.transform_expression_type = C_value::E_type_msg;  /* means unspecified */
}

void 
T_type_definitions::set_field_transform_expression_integer (
										T_field_type_name_base  & field,
								  const string                  & expr_param) const
{
	M_FATAL_IF_NE(field.transform_quantum.as_string(), "");
	M_FATAL_IF_NE(field.transform_offset.as_string(), "");
	M_FATAL_IF_NE(field.transform_expression.is_defined(), false);

	field.transform_expression.build_expression(*this, expr_param);
	field.transform_expression_type = C_value::E_type_integer;
}

void 
T_type_definitions::set_field_transform_expression_float (
										T_field_type_name_base  & field,
								  const string                  & expr_param) const
{
	M_FATAL_IF_NE(field.transform_quantum.as_string(), "");
	M_FATAL_IF_NE(field.transform_offset.as_string(), "");
	M_FATAL_IF_NE(field.transform_expression.is_defined(), false);

	field.transform_expression.build_expression(*this, expr_param);
	field.transform_expression_type = C_value::E_type_float;
}

void 
T_type_definitions::set_field_transform_expression_string (
										T_field_type_name_base  & field,
								  const string                  & expr_param) const
{
	M_FATAL_IF_NE(field.transform_quantum.as_string(), "");
	M_FATAL_IF_NE(field.transform_offset.as_string(), "");
	M_FATAL_IF_NE(field.transform_expression.is_defined(), false);

	field.transform_expression.build_expression(*this, expr_param);
	field.transform_expression_type = C_value::E_type_string;
}

void 
T_type_definitions::add_field_constraint_min_max(T_field_type_name_base  & field,
										   const string                  & min_param,
										   const string                  & max_param) const
{
	T_field_constraint    constraint;
	constraint.min = string_to_numeric(*this, min_param, field.name, "min");
	constraint.max = string_to_numeric(*this, max_param, field.name, "max");

	field.constraints.push_back(constraint);
}

void 
T_type_definitions::add_field_constraint_min  (T_field_type_name_base  & field,
								 const string             & val_param) const
{
	T_field_constraint    constraint;
	constraint.min = string_to_numeric(*this, val_param, field.name, "min");

	field.constraints.push_back(constraint);
}

void 
T_type_definitions::add_field_constraint_max  (T_field_type_name_base  & field,
								 const string             & val_param) const
{
	T_field_constraint    constraint;
	constraint.max = string_to_numeric(*this, val_param, field.name, "max");

	field.constraints.push_back(constraint);
}

void 
T_type_definitions::add_field_constraint_equal(T_field_type_name_base  & field,
								 const string             & val_param) const
{
	T_field_constraint    constraint;
	constraint.equal = string_to_numeric(*this, val_param, field.name, "equal");

	field.constraints.push_back(constraint);
}

void 
T_type_definitions::prepend_field_constraint_min_max(T_field_type_name_base  & field,
										   const string                  & min_param,
										   const string                  & max_param) const
{
	T_field_constraint    constraint;
	constraint.min = string_to_numeric(*this, min_param, field.name, "min");
	constraint.max = string_to_numeric(*this, max_param, field.name, "max");

	field.constraints.insert(field.constraints.begin(), constraint);
}

void 
T_type_definitions::prepend_field_constraint_min  (T_field_type_name_base  & field,
								 const string             & val_param) const
{
	T_field_constraint    constraint;
	constraint.min = string_to_numeric(*this, val_param, field.name, "min");

	field.constraints.insert(field.constraints.begin(), constraint);
}

void 
T_type_definitions::prepend_field_constraint_max  (T_field_type_name_base  & field,
								 const string             & val_param) const
{
	T_field_constraint    constraint;
	constraint.max = string_to_numeric(*this, val_param, field.name, "max");

	field.constraints.insert(field.constraints.begin(), constraint);
}

void 
T_type_definitions::prepend_field_constraint_equal(T_field_type_name_base  & field,
								 const string             & val_param) const
{
	T_field_constraint    constraint;
	constraint.equal = string_to_numeric(*this, val_param, field.name, "equal");

	field.constraints.insert(field.constraints.begin(), constraint);
}

void 
T_type_definitions::set_field_display           (T_field_type_name_base  & field,
								   const string             & val_param) const
{
	M_FATAL_IF_NE(field.str_display, "");
	M_FATAL_IF_NE(field.str_display_expression, "");

	field.str_display = val_param;

	promote_printf_string_to_64bits(field.str_display);
}

void 
T_type_definitions::set_field_display_expression(T_field_type_name_base  & field,
								   const string             & val_param) const
{
	M_FATAL_IF_NE(field.str_display, "");
	M_FATAL_IF_NE(field.str_display_expression, "");

    field.str_display_expression = val_param;
}

void 
T_type_definitions::set_field_subdissector(T_field_type_name_base  & field,
								   const string             & val_param) const
{
	M_FATAL_IF_NE(field.str_dissector, "");

    field.str_dissector = val_param;

	const_cast<T_type_definitions*>(this)->add_subdissector(val_param);
}

void
T_type_definitions::add_subdissector(const string             & val_param)
{
	if (find(vector_subdissector_name.begin(),
			 vector_subdissector_name.end(),
			 val_param) == vector_subdissector_name.end())
	{
		vector_subdissector_name.push_back(val_param);
	}
}

void 
T_type_definitions::set_field_decoder(T_field_type_name_base  & field,
							    const string                  & val_param) const
{
	M_FATAL_IF_NE(field.str_decoder_function, "");

    field.str_decoder_function = val_param;
}

void 
T_type_definitions::set_field_byte_order(T_field_type_name_base  & field,
								   const string                  & val_param) const
{
	M_FATAL_IF_NE(field.str_byte_order, "");

    field.str_byte_order = val_param;
}

//*****************************************************************************
// Global
//*****************************************************************************

const T_enum_definition_representation  *
T_type_definitions::get_P_enum(const string      & type_name) const
{
	T_map_enum_definition_representation::const_iterator  iter =
		map_enum_definition_representation.find(type_name);

	if (iter != map_enum_definition_representation.end ())
	{
		return  &iter->second;
	}

	return  NULL;
}

bool
T_type_definitions::is_an_enum   (const string      & type_name) const
{
	return  get_P_enum(type_name) != NULL_PTR;
}

const T_struct_definition  *
T_type_definitions::get_P_struct(const string      & type_name) const
{
	T_map_struct_definition::const_iterator  iter =
		map_struct_definition.find(type_name);

	if (iter != map_struct_definition.end ())
	{
		return  &iter->second;
	}

	return  NULL;
}

T_struct_definition  *
T_type_definitions::get_P_struct(const string      & type_name)
{
	T_map_struct_definition::iterator  iter =
		map_struct_definition.find(type_name);

	if (iter != map_struct_definition.end ())
	{
		return  &iter->second;
	}

	return  NULL;
}

bool
T_type_definitions::is_a_struct   (const string      & type_name) const
{
	return  get_P_struct(type_name) != NULL_PTR;
}

const T_bitfield_definition *
T_type_definitions::get_P_bitfield(const string      & type_name) const
{
	T_map_bitfield_definition::const_iterator  iter =
		map_bitfield_definition.find(type_name);

	if (iter != map_bitfield_definition.end ())
	{
		return  &iter->second;
	}

	return  NULL;
}

bool
T_type_definitions::is_a_bitfield (const string      & type_name) const
{
	return  get_P_bitfield(type_name) != NULL_PTR;
}

const T_switch_definition &
T_type_definitions::get_switch  (const string      & type_name) const
{
	const T_switch_definition  * P_switch_def = get_P_switch(type_name);

	if (P_switch_def == NULL)
	{
		M_FATAL_COMMENT("Switch " << type_name << " unknow.");
	}

	return  *P_switch_def;
}

const T_switch_definition *
T_type_definitions::get_P_switch  (const string      & type_name) const
{
	T_map_switch_definition::const_iterator  iter =
		map_switch_definition.find(type_name);

	if (iter != map_switch_definition.end ())
	{
		return  &iter->second;
	}

	return  NULL;
}

bool
T_type_definitions::is_a_switch   (const string      & type_name) const
{
	return  get_P_switch(type_name) != NULL_PTR;
}

const T_function_definition &
T_type_definitions::get_function  (const string      & type_name) const
{
	const T_function_definition  * P_fct_def = get_P_function(type_name);

	if (P_fct_def == NULL)
	{
		M_FATAL_COMMENT("Function " << type_name << " unknow.");
	}

	return  *P_fct_def;
}

const T_function_definition *
T_type_definitions::get_P_function(const string      & type_name) const
{
	T_map_function_definition::const_iterator  iter =
		map_function_definition.find(type_name);

	if (iter != map_function_definition.end ())
	{
		return  &iter->second;
	}

	return  NULL;
}

bool
T_type_definitions::is_a_function (const string      & type_name) const
{
	return  get_P_function(type_name) != NULL_PTR;
}

// Enum  : get integer value from symbolic value.
// Const : get value 
// Returns false if nothing found.
bool
T_type_definitions::get_type_value (const string   & in_symbolic_name,
										  C_value  & out_value) const
{
	string    str_left;
	string    str_right;
	if (get_before_separator_after (in_symbolic_name,
									"::",
									str_left,
									str_right) == E_rc_ok)
	{
		const T_enum_definition_representation  * P_enum = get_P_enum(str_left);
		if (P_enum != NULL)
		{
			long long    integer_value = -1;
			if (P_enum->get_integer_value(str_right, integer_value))
			{
				out_value = C_value(integer_value);
				out_value.set_str(str_right);
				return  true;
			}
			else
			{
				// M_FATAL_COMMENT ?
			}
		}
		else
		{
			T_map_const_value::const_iterator  iter = map_const_value.find(in_symbolic_name);
			if (iter != map_const_value.end())
			{
				out_value = iter->second;
				return  true;
			}
		}
	}
	else if (in_symbolic_name == "nil")
	{
		out_value = C_value(C_value::E_type_msg);
		return  true;
	}

	return  false;
}

T_type_definitions::E_type_kind
T_type_definitions::get_defined_type_kind(const string   & type_name) const
{
   if (map_alias_type.find (type_name) != map_alias_type.end ())
		return  E_type_alias;
   if (map_const_value.find (type_name) != map_const_value.end ())
		return  E_type_const;
    if (map_struct_definition.find (type_name) != map_struct_definition.end ())
		return  E_type_struct;
    if (map_bitfield_definition.find (type_name) != map_bitfield_definition.end ())
		return  E_type_bitfield;
    if (map_enum_definition_representation.find (type_name) != map_enum_definition_representation.end ())
		return  E_type_enum;
    if (map_switch_definition.find (type_name) != map_switch_definition.end ())
		return  E_type_switch;
    if (map_function_definition.find (type_name) != map_function_definition.end ())
		return  E_type_function;

	return  E_type_none;
}

bool
T_type_definitions::is_a_defined_type_name(const string   & type_name) const
{
	return  get_defined_type_kind(type_name) != E_type_none;
}

T_type_definitions::E_type_kind
T_type_definitions::get_forward_type_kind(const string   & type_name) const
{
	T_map_forward_type::const_iterator  iter = map_forward_type.find (type_name);
	if (iter != map_forward_type.end ())
		return  iter->second;

	return  E_type_none;
}

bool
T_type_definitions::is_a_forward_type_name(const string   & type_name) const
{
	return  get_forward_type_kind(type_name) != E_type_none;
}

bool
T_type_definitions::is_a_basic_type_name(const string   & type_name) const
{
	if ((type_name ==  "int8")  ||
		(type_name == "uint8")  ||
		(type_name ==  "int16") ||
		(type_name == "uint16") ||
		(type_name ==  "int24") ||
		(type_name == "uint24") ||
		(type_name ==  "int32") ||
		(type_name == "uint32") ||
		(type_name ==  "int40") ||
		(type_name == "uint40") ||
		(type_name ==  "int48") ||
		(type_name == "uint48") ||
		(type_name ==  "int64") ||
//		(type_name == "uint64") ||
		(type_name == "float32") ||
		(type_name == "float64") ||
//		(type_name == "bool8") ||
//		(type_name == "bool16") ||
//		(type_name == "bool32") ||
		(type_name == "spare") ||
		(type_name ==  "char") ||
		(type_name == "schar") ||
		(type_name == "uchar") ||
		(type_name == "string") ||
		(type_name == "string_nl") ||
		(type_name == "raw") ||
		(type_name == "subproto") ||
		(type_name == "insproto"))
	{
		return  true;
	}

	long    bit_size = 0;
	if ((strncmp(type_name.c_str(), "uint", 4) == 0) &&
	    (get_number(type_name.c_str()+4, &bit_size) == true) &&
		(bit_size >   0) &&
		(bit_size <  32))
	{
		/* bsew uintXX */
		return  true;
	}
	if ((strncmp(type_name.c_str(), "int", 3) == 0) &&
	    (get_number(type_name.c_str()+3, &bit_size) == true) &&
		(bit_size >   1) &&
		(bit_size <  32))        // bsew limitation (byte_order)
	{
		/* bsew intXX */
		return  true;
	}

	return  false;
}

bool
T_type_definitions::is_a_type_name(const string   & type_name) const
{
	return  is_a_defined_type_name(type_name) ||
			is_a_basic_type_name(type_name) ||
			is_a_forward_type_name(type_name);
}

// 
void             
T_type_definitions::add_forward_declaration(const E_type_kind   type_kind,
											const string      & type_name)
{
	M_FATAL_IF_EQ(type_kind, E_type_none);
	M_FATAL_IF_EQ(type_kind, E_type_alias);
	M_FATAL_IF_EQ(type_name, "");

	const E_type_kind    existing_type_kind = get_defined_type_kind(type_name);
	const E_type_kind    forward_type_kind  = get_forward_type_kind(type_name);

	// Verify it already exist.
	if ((existing_type_kind == type_kind) ||
		(forward_type_kind == type_kind))
		return;

	// Verify it already exist with a different kind.
	if (existing_type_kind != E_type_none)
	{
        M_FATAL_COMMENT ("A " << existing_type_kind << " type named " << type_name << " already exist (override will not work)");
	}

	if (forward_type_kind  != E_type_none)
	{
        M_FATAL_COMMENT ("A " << forward_type_kind << " type named " << type_name << " already declared (override will not work)");
	}

	// Add it.
	map_forward_type[type_name] = type_kind;
}
								  
void
T_type_definitions::could_define_new_type(const string      & type_name,
										  const E_type_kind   type_kind,
										  const E_override    must_override) const
{
	M_FATAL_IF_EQ(type_kind, E_type_none);
	M_FATAL_IF_EQ(type_name, "");

	{
		const E_type_kind    forward_type_kind  = get_forward_type_kind(type_name);
		if (forward_type_kind != E_type_none)
		{
			if (type_kind != forward_type_kind)
			{
		        M_FATAL_COMMENT ("A " << forward_type_kind << " type named " << type_name << " already declared (override will not work)");
			}

			// ICIOA remove  (do not care)
		}
	}

	const E_type_kind    existing_type_kind = get_defined_type_kind(type_name);

	if (existing_type_kind == E_type_none)
		return;

	// A type with the given name already exist.

	// Check that the already existent type has the same kind.
	if (existing_type_kind != type_kind)
	{
        M_FATAL_COMMENT ("A " << existing_type_kind << " type named " << type_name << " already exist (override will not work)");
	}

	// Could NOT override alias since 
	//  since alias could already have been be used (by build_field).
	if (existing_type_kind == E_type_alias)
    {
        M_FATAL_COMMENT (type_kind << " " << type_name << " already exist (and override is forbidden for " << type_kind << ")");
    }

	// Could NOT override const since 
	//  since const could already have been be used (by other const).
	if (existing_type_kind == E_type_const)
    {
        M_FATAL_COMMENT (type_kind << " " << type_name << " already exist (and override is forbidden for " << type_kind << ")");
    }

	// Is override specified ?
	if (must_override == E_override_no)
    {
        M_FATAL_COMMENT (type_kind << " " << type_name << " already exist (try override ?)");
    }

	// Could we have other problems on override ???
	// Normally no.
}

ostream &  operator<< (ostream                          & os,
				 const T_type_definitions::E_type_kind  & rhs)
{
	if (rhs == T_type_definitions::E_type_none)
		os << "none";
	else if (rhs == T_type_definitions::E_type_alias)
		os << "alias";
	else if (rhs == T_type_definitions::E_type_const)
		os << "const";
	else if (rhs == T_type_definitions::E_type_struct)
		os << "struct";
	else if (rhs == T_type_definitions::E_type_bitfield)
		os << "bitfield";
	else if (rhs == T_type_definitions::E_type_enum)
		os << "enum";
	else if (rhs == T_type_definitions::E_type_switch)
		os << "switch";
	else if (rhs == T_type_definitions::E_type_function)
		os << "function";
	else
	{
		M_FATAL_COMMENT("Unexpected value (" << (int)rhs << ") for T_type_definitions::E_type_kind");
	}

	return  os;
}

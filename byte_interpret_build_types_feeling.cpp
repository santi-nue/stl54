/*
 * Copyright 2005-2009 Olivier Aveline <wsgd@free.fr>
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

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "precomp.h"
#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_build_types_feeling.h"

#include "tinyxml.h"

#if 1

//*****************************************************************************
// byte_interpret_get_include_file_name
//*****************************************************************************
#if 0
static string    S_basic_directory;

void      byte_interpret_set_include_directory(const string  & dir_name)
{
	S_basic_directory = dir_name;
}

string    byte_interpret_get_include_file_name(const string  & file_name)
{
	if (S_basic_directory == "")
		return  file_name;

	return  S_basic_directory + "\\" + file_name;
}
#endif

string    byte_interpret_get_include_directory();

//*****************************************************************************
// normalize_type_name
//*****************************************************************************

string    normalize_type_name(string    str)
{
	for (string::size_type    idx = 0; idx < str.size(); ++idx)
	{
		if (str[idx] == ' ')
			str[idx] = '_';
		else if (str[idx] == '-')
			str[idx] = '_';
		else if (str[idx] == '/')
			str[idx] = '_';
		else if (str[idx] == '(')
			str[idx] = '_';
		else if (str[idx] == ')')
			str[idx] = '_';
	}

	return  str;
}

//*****************************************************************************
// normalize_field_name
//*****************************************************************************

#define normalize_field_name  normalize_type_name

//*****************************************************************************
// 
//*****************************************************************************

void  check_TiXmlElement(TiXmlElement  * root)
{
	const char  * name = root->Attribute("name");
	const char  * id   = root->Attribute("id");

	if ((name != NULL) &&
		(id   != NULL) &&
		(strcmp(name, id) != 0))
	{
//		cout << "TiXmlElement " << name << " (name) != " << id << " (id)" << endl; 
	}
}

int   compute_size_TiXmlElement(TiXmlElement  * root)
{
	int           begin_int;
	const char  * begin = root->Attribute("begin", &begin_int);
	int           end_int;
	const char  * end   = root->Attribute("end"  , &end_int);

	if ((begin == NULL) || (end == NULL))
	{
		return  -1; 
	}

	return  end_int - begin_int + 1;
}

int   compute_int_TiXmlElement(TiXmlElement  * root, const char  * name)
{
	int           begin_int;
	const char  * begin = root->Attribute("begin", &begin_int);
	int           end_int;
	const char  * end   = root->Attribute("end"  , &end_int);

	if ((begin == NULL) || (end == NULL))
	{
		return  -1; 
	}

	return  end_int - begin_int + 1;
}

//*****************************************************************************
// generate_type_name
//*****************************************************************************
static
string  generate_type_name(string    base_type_name)
{
	static int     S_automatically_generated_type_name_counter = 0;

	base_type_name += "_generated_feeling_";
	base_type_name += get_string(++S_automatically_generated_type_name_counter);

	return  normalize_type_name(base_type_name);
}







//*****************************************************************************
// feeling_manage_field_base
//*****************************************************************************

void      feeling_manage_field_base(T_type_definitions  & type_definitions,
									T_field_type_name   & field_type_name,
							        TiXmlElement        * root,
							  const char                * base_type_name)
{
	M_STATE_ENTER("feeling_manage_field_base", root << "  " << base_type_name);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
	const char  * isBigendian = root->Attribute("isBigendian");
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
	const char  * quantum = root->Attribute("quantum");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

	type_definitions.set_field_type(field_type_name, base_type_name + get_string(compute_size_TiXmlElement(root)));
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));
#if 0
	if (isBigendian != NULL)                  // ICIOA isBigendian non traite !!!
	{
		cout << "isBigendian=" << isBigendian << "  ";
	}
#endif
	if (quantum != NULL)
	{
		type_definitions.set_field_transform_quantum(field_type_name, quantum);
	}
}

//*****************************************************************************
// feeling_manage_field_integerField
//*****************************************************************************

void      feeling_manage_field_integerField(T_type_definitions  & type_definitions,
											T_field_type_name   & field_type_name,
							                TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_integerField", root);

	feeling_manage_field_base(type_definitions, field_type_name, root, "uint");
}

//*****************************************************************************
// feeling_manage_field_floatField
//*****************************************************************************

void      feeling_manage_field_floatField(T_type_definitions  & type_definitions,
										  T_field_type_name   & field_type_name,
							              TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_floatField", root);

	feeling_manage_field_base(type_definitions, field_type_name, root, "float");
}

//*****************************************************************************
// feeling_manage_type_base
//*****************************************************************************

string    feeling_manage_type_base(T_type_definitions  & type_definitions,
							       TiXmlElement        * root,
						     const string              & alias_name,
							 const char                * base_type_name)
{
	M_STATE_ENTER("feeling_manage_type_base", root << "  " << base_type_name);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
	const char  * isBigendian = root->Attribute("isBigendian");
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
	const char  * quantum = root->Attribute("quantum");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

//	string    alias_name = normalize_type_name(name);

	string    type_name = base_type_name;
	type_name += get_string(compute_size_TiXmlElement(root));
#if 0
	if (isBigendian != NULL)                  // ICIOA isBigendian non traite !!!
	{
		cout << "isBigendian=" << isBigendian << "  ";
	}
#endif
	if (quantum != NULL)
	{
		type_name += "{q=";
		type_name += quantum;
		type_name += "}";
	}

	type_definitions.add_alias(alias_name, type_name);

	return  alias_name;
}

//*****************************************************************************
// feeling_manage_type_integerField
//*****************************************************************************

string    feeling_manage_type_integerField(T_type_definitions  & type_definitions,
							               TiXmlElement        * root,
						             const string              & type_name)
{
	M_STATE_ENTER("feeling_manage_type_integerField", root);

	return  feeling_manage_type_base(type_definitions, root, type_name, "uint");
}

//*****************************************************************************
// feeling_manage_type_floatField
//*****************************************************************************

string    feeling_manage_type_floatField(T_type_definitions  & type_definitions,
							             TiXmlElement        * root,
						           const string              & type_name)
{
	M_STATE_ENTER("feeling_manage_type_floatField", root);

	return  feeling_manage_type_base(type_definitions, root, type_name, "float");
}

//*****************************************************************************
// feeling_manage_type_enumValue
//*****************************************************************************

string    feeling_manage_type_enumValue(T_type_definitions  & type_definitions,
							            TiXmlElement        * root,
						          const string              & enum_name)
{
	M_STATE_ENTER("feeling_manage_type_enumValue", root);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
//	const char  * isBigendian = root->Attribute("isBigendian");
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

	type_definitions.could_define_new_type(enum_name, type_definitions.E_type_enum);

	T_map_enum_definition_representation  & map_enum_def_rep = type_definitions.map_enum_definition_representation;

//	const string    enum_name = force_enum_name != NULL ? force_enum_name : normalize_type_name(name);

    T_enum_definition_representation  & def_rep = map_enum_def_rep[enum_name];


    long long    min_value = LONGLONG_MAX;
    long long    max_value = LONGLONG_MIN;

	for (TiXmlElement  * element  = root->FirstChildElement("enum");
		                 element != NULL;
						 element  = element->NextSiblingElement())
	{
		T_enum_name_val    enum_name_val;
//		enum_name_val.name  = "\"";
		enum_name_val.name += element->Attribute("name");
//		enum_name_val.name += "\"";

		M_FATAL_IF_FALSE(get_number (element->Attribute("value"), enum_name_val.value));

        min_value = min (min_value, enum_name_val.value);
        max_value = max (max_value, enum_name_val.value);

        def_rep.definition.push_back (enum_name_val);
	}

    def_rep.is_signed = (min_value < 0);

    def_rep.bit_size = compute_size_TiXmlElement(root);
    def_rep.representation_type =
                string (def_rep.is_signed ? "" : "u") +
                "int" +
                get_string (def_rep.bit_size);

	return  enum_name;
}

//*****************************************************************************
// feeling_manage_field_enumValue
//*****************************************************************************

void      feeling_manage_field_enumValue(T_type_definitions  & type_definitions,
										 T_field_type_name   & field_type_name,
							             TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_enumValue", root);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
//	const char  * isBigendian = root->Attribute("isBigendian");
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

	const string    enum_type_name = generate_type_name(name);

	feeling_manage_type_enumValue(type_definitions, root, enum_type_name);

	type_definitions.set_field_type(field_type_name, enum_type_name);
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));
}

//*****************************************************************************
// feeling_manage_field_variableIntegerField
//*****************************************************************************

void      feeling_manage_field_variableIntegerField(T_type_definitions  & type_definitions,
													T_field_type_name   & field_type_name,
													TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_variableIntegerField", root);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
	M_FATAL_IF_FALSE(name != NULL);

	// ICIOA
	type_definitions.set_field_type(field_type_name, "raw");
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));

	TiXmlElement  * element  = root->FirstChildElement("size");
	M_FATAL_IF_FALSE(element != NULL);

	const char  * id = element->Attribute("id");
	M_FATAL_IF_FALSE(id != NULL);

	type_definitions.set_field_type_size_or_parameter(field_type_name, normalize_field_name(id));
}

//*****************************************************************************
// feeling_manage_type_concatField
//*****************************************************************************

string    feeling_manage_type_concatField(T_type_definitions  & type_definitions,
							              TiXmlElement        * root,
						            const string              & concat_type_name)
{
	M_STATE_ENTER("feeling_manage_type_concatField", root << "  " << concat_type_name);

	check_TiXmlElement(root);

	string    type_name = "raw(";
	type_name += get_string(compute_size_TiXmlElement(root) / 8);
	type_name += ")";

	type_definitions.add_alias(concat_type_name, type_name);    // ICIOA concat a revoir

	return  "ICIOA pas fini";
}

//*****************************************************************************
// feeling_manage_field_typedField
//*****************************************************************************

void   feeling_manage_field_typedField(T_type_definitions  & type_definitions,
									   T_field_type_name   & field_type_name,
								       TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_typedField", root);

	// <typedField name="Originating Entity ID" id="Originating Entity ID" abstractName="ORG ENT ID" begin="0" end="47" type="ID"/>
	const char  * name = root->Attribute("name");
	const char  * id   = root->Attribute("id");
//	const char  * abstractName = root->Attribute("abstractName");
	const char  * type = root->Attribute("type");
//	compute_size_TiXmlElement(root);

	// Take id instead of name because id is referenced by arrays.
	const char  * field_name = id ? id : name;

	type_definitions.set_field_type(field_type_name, normalize_type_name(type));
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));

	// Conditions
	TiXmlElement  * activity_element = root->FirstChildElement("activityCondition");
	if (activity_element != NULL)
	{
		T_field_type_name     sub_field_type_name;
		type_definitions.set_field_type(sub_field_type_name, normalize_type_name(type));
		type_definitions.set_field_name(sub_field_type_name, normalize_field_name(field_name));
		if (field_type_name.P_sub_struct.get() == NULL)
			field_type_name.P_sub_struct.reset(new T_struct_definition);
		field_type_name.P_sub_struct->fields.push_back(sub_field_type_name);

		type_definitions.set_field_type(field_type_name, "if");
		type_definitions.set_field_name(field_type_name, "");

		TiXmlElement  * condition_element  = activity_element->FirstChildElement();
		const char    * condition_value = condition_element->Value();

		if ((strcmp (condition_value, "equalityCondition") == 0) ||
			(strcmp (condition_value, "transEqualityCondition") == 0))       // ICIOA ?
		{
			const char    * condition_id    = condition_element->Attribute("id");
			const char    * condition_value = condition_element->Attribute("value");

			// condition_id is supposed to be a field.
			// condition_value is supposed to be a value
			//  and since it could be negative, I could not normalize it.
			field_type_name.name += "(";
			field_type_name.name += normalize_field_name(condition_id);
			field_type_name.name += " == ";
			field_type_name.name += condition_value;
			field_type_name.name += ")";
		}
		else
		{
			M_FATAL_COMMENT("Unexpected activityCondition child = " << condition_value);
		}
	}
}

//*****************************************************************************
// 
//*****************************************************************************

void    feeling_manage_field_structuredField(T_type_definitions  & type_definitions,
											 T_field_type_name   & field_type_name,
								             TiXmlElement        * root);

//*****************************************************************************
// feeling_manage_field_arrayField
//*****************************************************************************

void    feeling_manage_field_arrayField(T_type_definitions  & type_definitions,
										T_field_type_name   & field_type_name,
								        TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_arrayField", root);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
	const char  * isBigendian = root->Attribute("isBigendian");              // ICIOA non traite
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

	type_definitions.set_field_type(field_type_name, "struct");
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));

	if (field_type_name.P_sub_struct.get() == NULL)
		field_type_name.P_sub_struct.reset(new T_struct_definition);
	T_struct_fields  & struct_definition = field_type_name.P_sub_struct->fields;


	for (TiXmlElement  * element  = root->FirstChildElement();
		                 element != NULL;
						 element  = element->NextSiblingElement())
	{
		T_field_type_name    sub_field_type_name;

		const char  * value = element->Value();

		if (strcmp (value, "structuredField") == 0)
		{
			feeling_manage_field_structuredField(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "relativeField") == 0)
		{
			// idem as structuredField
			feeling_manage_field_structuredField(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "typedField") == 0)
		{
			feeling_manage_field_typedField(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "arrayField") == 0)
		{
			feeling_manage_field_arrayField(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "enumValue") == 0)
		{
			feeling_manage_field_enumValue(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "integerField") == 0)
		{
			feeling_manage_field_integerField(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "floatField") == 0)
		{
			feeling_manage_field_floatField(type_definitions, sub_field_type_name, element);
		}
		else if (strcmp (value, "size") == 0)
		{
			field_type_name.str_arrays.push_back(normalize_field_name(element->Attribute("id")));
			continue;
		}
		else if (strcmp (value, "modSize") == 0)   // ICIOA padding ? uniquement dans DIS.xml
		{
			const char    * mod = element->Attribute("div");

			TiXmlElement  * size_element = element->FirstChildElement("size");
			M_FATAL_IF_FALSE(size_element != NULL);

			field_type_name.str_arrays.push_back(normalize_field_name(size_element->Attribute("id")));
			continue;
		}
		else if (strcmp (value, "divSize") == 0)
		{
			const char    * div = element->Attribute("div");

			TiXmlElement  * size_element = element->FirstChildElement("size");
			M_FATAL_IF_FALSE(size_element != NULL);

			field_type_name.str_arrays.push_back(
							normalize_field_name(size_element->Attribute("id")) + "/" + div);     // ICIOA a-priori c'est bon ?
			continue;
		}
		else 
		{
			M_FATAL_COMMENT("value " << value << " inconnu");
		}

        struct_definition.push_back (sub_field_type_name);
	}
}

//*****************************************************************************
// feeling_manage_field_structuredField
//*****************************************************************************

static string  S_msg_type_name;                         // ICIOA static
static string  S_msg_header_type_name = "PDU_Header";   // ICIOA static + hard coded
static string  S_msg_id_field_name    = "PDU_Type";     // ICIOA static + hard coded
static T_switch_definition  S_main_switch_definition;   // ICIOA static

void    feeling_manage_field_structuredField(T_type_definitions  & type_definitions,
											 T_field_type_name   & field_type_name,
								             TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_field_structuredField", root);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
	const char  * isBigendian = root->Attribute("isBigendian");              // ICIOA non traite
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

	type_definitions.set_field_type(field_type_name, "struct");
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));

	if (normalize_type_name(name) == S_msg_header_type_name)
	{
		M_STATE_DEBUG("Msg header found");
		// Special trick, we are into the 1st field of the message = the header.
		// Do not care of the other fields except the id.
		type_definitions.set_field_type(field_type_name, S_msg_header_type_name);
	}

	if (field_type_name.P_sub_struct.get() == NULL)
		field_type_name.P_sub_struct.reset(new T_struct_definition);
	T_struct_fields  & struct_definition = field_type_name.P_sub_struct->fields;


	for (TiXmlElement  * element  = root->FirstChildElement();
		                 element != NULL;
						 element  = element->NextSiblingElement())
	{
		T_field_type_name    field_type_name;

		const char  * value = element->Value();

		if (strcmp (value, "structuredField") == 0)
		{
			feeling_manage_field_structuredField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "typedField") == 0)
		{
			feeling_manage_field_typedField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "arrayField") == 0)
		{
			feeling_manage_field_arrayField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "enumValue") == 0)
		{
			feeling_manage_field_enumValue(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "integerField") == 0)
		{
			feeling_manage_field_integerField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "floatField") == 0)
		{
			feeling_manage_field_floatField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "constValue") == 0)
		{
			const char    * const_name = element->Attribute("name");
			const string    const_field_name = normalize_field_name(const_name);

			M_STATE_DEBUG("constValue " << const_field_name << " found");

			if (const_field_name == S_msg_id_field_name)
			{
				M_STATE_DEBUG("Msg id found");

				S_main_switch_definition.case_type = "int" + get_string(compute_size_TiXmlElement(element));
//				S_main_switch_definition.case_type = "0000003";  // ICIOA !!!

				T_field_type_name    field_type_name;
				type_definitions.set_field_type(field_type_name, S_msg_type_name);
				type_definitions.set_field_name(field_type_name, "");       // tip to not see a useless supplementary tree

				T_switch_case_value    switch_case_value;
				switch_case_value.case_value = element->Attribute("value");
				switch_case_value.case_value.promote();
				switch_case_value.fields.push_back(field_type_name);

				S_main_switch_definition.switch_case.push_back(switch_case_value);
			}
		}
		else 
		{
			M_FATAL_COMMENT("value " << value << " inconnu");
		}

        struct_definition.push_back (field_type_name);
	}

	if (normalize_type_name(name) == S_msg_header_type_name)
	{
		// Dans ce cas, on utilise le type declare auparavant.
		// Donc pas de sub_struct
		struct_definition.clear();
	}
}

//*****************************************************************************
// feeling_manage_type_structuredField
//*****************************************************************************

string    feeling_manage_type_structuredField(T_type_definitions  & type_definitions,
								              TiXmlElement        * root,
						                const string              & struct_name)
{
	M_STATE_ENTER("feeling_manage_type_structuredField", root);

	check_TiXmlElement(root);

	const char  * name = root->Attribute("name");
//	const char  * id = root->Attribute("id");
//	const char  * begin = root->Attribute("begin");
//	const char  * end = root->Attribute("end");
	const char  * isBigendian = root->Attribute("isBigendian");              // ICIOA non traite
//	const char  * visibility = root->Attribute("visibility");
//	const char  * abstractVisibility = root->Attribute("abstractVisibility");
//	const char  * comment = root->Attribute("comment");

	M_FATAL_IF_FALSE(name != NULL);

//	const string    struct_name = normalize_type_name(name);

	if (type_definitions.map_struct_definition.find (struct_name) !=
        type_definitions.map_struct_definition.end ())
	{
        M_FATAL_COMMENT ("struct " << struct_name << " override.");
    }

    T_struct_definition  & struct_definition = type_definitions.map_struct_definition[struct_name];

	if (struct_name == S_msg_header_type_name)
	{
		T_field_type_name    field_type_name;
		type_definitions.set_field_type(field_type_name, "byte_order");
		type_definitions.set_field_name(field_type_name, "big_endian");     // ICIOA en dur

		struct_definition.fields.push_back (field_type_name);
	}


	for (TiXmlElement  * element  = root->FirstChildElement();
		                 element != NULL;
						 element  = element->NextSiblingElement())
	{
		T_field_type_name    field_type_name;

		const char  * value = element->Value();

		if (strcmp (value, "structuredField") == 0)
		{
			feeling_manage_field_structuredField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "relativeField") == 0)
		{
			// idem as structuredField
			feeling_manage_field_structuredField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "typedField") == 0)
		{
			feeling_manage_field_typedField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "arrayField") == 0)
		{
			feeling_manage_field_arrayField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "enumValue") == 0)
		{
			feeling_manage_field_enumValue(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "integerField") == 0)
		{
			feeling_manage_field_integerField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "floatField") == 0)
		{
			feeling_manage_field_floatField(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "variableIntegerField") == 0)
		{
			feeling_manage_field_variableIntegerField(type_definitions, field_type_name, element);
		}
		else 
		{
			M_FATAL_COMMENT("value " << value << " inconnu");
		}

		struct_definition.fields.push_back (field_type_name);
	}

	return  struct_name;
}

//*****************************************************************************
// feeling_manage_type
//*****************************************************************************

void  feeling_manage_type(T_type_definitions  & type_definitions,
						  TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_type", root);

	const string    type_name = normalize_type_name(root->Attribute("name"));
//	cout << "type = " << type_name << endl;

	TiXmlElement  * element = NULL;

	if ((element = root->FirstChildElement("enumValue")) != NULL)
	{
		feeling_manage_type_enumValue(type_definitions, element, type_name);
	}
	else if ((element = root->FirstChildElement("structuredField")) != NULL)
	{
		feeling_manage_type_structuredField(type_definitions, element, type_name);
	}
	else if ((element = root->FirstChildElement("relativeField")) != NULL)
	{
		// idem as structuredField
		feeling_manage_type_structuredField(type_definitions, element, type_name);
	}
	else if ((element = root->FirstChildElement("concatField")) != NULL)
	{
		feeling_manage_type_concatField(type_definitions, element, type_name);
	}
	else if ((element = root->FirstChildElement("integerField")) != NULL)
	{
		feeling_manage_type_integerField(type_definitions, element, type_name);
	}
	else if ((element = root->FirstChildElement("floatField")) != NULL)
	{
		feeling_manage_type_floatField(type_definitions, element, type_name);
	}
	else
	{
		M_FATAL_COMMENT("type inconnu pour type nomme " << type_name);
	}
}

//*****************************************************************************
// feeling_manage_typeList
//*****************************************************************************

void  feeling_manage_typeList(T_type_definitions  & type_definitions,
						      TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_typeList", root);

	for (TiXmlElement  * element  = root->FirstChildElement("type");
		                 element != NULL;
						 element  = element->NextSiblingElement())
	{
		feeling_manage_type(type_definitions, element);
	}
}

//*****************************************************************************
// feeling_manage_msg_file
//*****************************************************************************

void    feeling_manage_msg_file(T_type_definitions  & type_definitions,
						  const string              & full_file_name)
{
	M_STATE_ENTER("feeling_manage_msg_file", full_file_name);

	TiXmlDocument   doc(full_file_name.c_str());
	bool loadOkay = doc.LoadFile();
	if (loadOkay != true)
	{
		M_FATAL_COMMENT("Could not load " << full_file_name);
	}

	TiXmlElement  * root = doc.FirstChildElement("message");
	if (root == NULL)
	{
		M_FATAL_COMMENT("Could not find message into " << full_file_name);
		return;
	}

	root = root->FirstChildElement("messageFieldList");
	M_FATAL_IF_FALSE(root != NULL);

	root = root->FirstChildElement("relativeField");
	M_FATAL_IF_FALSE(root != NULL);

	const char  * name = root->Attribute("name");
	S_msg_type_name = normalize_type_name(name);
	// idem as structuredField
	feeling_manage_type_structuredField(type_definitions, root, S_msg_type_name);
}

//*****************************************************************************
// feeling_manage_messageList
//*****************************************************************************

void  feeling_manage_messageList(T_type_definitions  & type_definitions,
						   const string              & directory,
						         TiXmlElement        * root)
{
	M_STATE_ENTER("feeling_manage_messageList", directory << "  " << root);

	for (TiXmlElement  * element  = root->FirstChildElement("message");
		                 element != NULL;
						 element  = element->NextSiblingElement())
	{
//		cout << element->Attribute("name") << "  ";
//		cout << element->Attribute("file") << endl;

		feeling_manage_msg_file(type_definitions,
								directory + "\\" + element->Attribute("file"));
	}

	// ICIOA hard coded 
	type_definitions.map_switch_definition["T_msg_switch"] = S_main_switch_definition;
}

//*****************************************************************************
// feeling_manage_main_file
//*****************************************************************************

void    feeling_manage_main_file(T_type_definitions  & type_definitions,
						   const string              & directory,
						   const string              & file_name)
{
	M_STATE_ENTER("feeling_manage_main_file", directory << "  " << file_name);

	const string    full_file_name = directory + "\\" + file_name;
	TiXmlDocument   doc(full_file_name.c_str());
	bool loadOkay = doc.LoadFile();
	if (loadOkay != true)
	{
		M_FATAL_COMMENT("Could not load " << full_file_name);
	}

	TiXmlElement  * root = doc.FirstChildElement("messageFormat");
	if (root == NULL)
	{
		M_FATAL_COMMENT("Could not find messageFormat into " << full_file_name);
		return;
	}

	M_FATAL_IF_FALSE(root != NULL);

	{
		TiXmlElement  * element = root->FirstChildElement("typeList");
		M_FATAL_IF_FALSE(element != NULL);
		feeling_manage_typeList(type_definitions, element);
	}

	{
		TiXmlElement  * element = root->FirstChildElement("messageList");
		M_FATAL_IF_FALSE(element != NULL);
		feeling_manage_messageList(type_definitions, directory, element);
	}

	// Do not manage filterList
}

//*****************************************************************************
// build_feeling_types ********************************************************
//*****************************************************************************
// 
//*****************************************************************************

bool      build_feeling_types (const string              & sub_directory,
							   const string              & main_file_name,
                                     T_type_definitions  & type_definitions)
{
	M_STATE_ENTER ("build_feeling_types", sub_directory << "  " << main_file_name);

	feeling_manage_main_file(type_definitions,
                             byte_interpret_get_include_directory() + "\\" + sub_directory,
							 main_file_name);

	return  false;
}
#else
bool      build_feeling_types (const string              & sub_directory,
							   const string              & main_file_name,
                                     T_type_definitions  & type_definitions)
{
	return  false;
}
#endif

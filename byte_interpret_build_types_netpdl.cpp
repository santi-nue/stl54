/*
 * Copyright 2008-2013 Olivier Aveline <wsgd@free.fr>
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
// I do NOT like netpdl
// NB: it is my think based on what I have understood
//*****************************************************************************
//
// manage very low level type
// e.g.: type="fixed" (means fixed size) size="4" (means 4 bytes)
//       this field could be an int, a float, an enum, ... anything on 4 bytes
// You know (perhaps) what is the type in looking at showtemplate.
// e.g.: showtemplate="FieldDec" so it is an integer.
//
// manage very low level type
// so must specify little_endian on each field where it is necessary !
//
// manage very low level type
// When you want to use a value read from the packet,
//  you must use the following syntax (e.g. for an integer) :
//  buf2int(hlen)
// Why not something like :
//  $hlen
// because hlen is not an integer,
//  it is just a "memory address inside the packet with an associated size/mask)
// 
// manage very low level type
// e.g. from tcp.xml :
// <field type="bit" name="hlen" longname="Header length" mask="0xF000" size="2"
//  comment="This field is in multiple of 4 bytes"
//  showtemplate="FieldMul4Dec"/>
// In this case, I imagine that the value displayed is the value read * 4.
// -> ok
// But if you use hlen inside your format later, do not forget, anywhere,
//  to multiply it per 4 :
//  buf2int(hlen) * 4
//
// bit type
// You cannot manage a bit type field alone !
// You must take care of the following fields
//  to verify if they are part of the same group !
// Look at the deductions (inside netpdl_manage_format_fields_field_bit)
//  which are mandatory to know if 2 consecutive bit field are part of the same group.
// And they are only deductions, you are still not sure !
//
// padding
// The scope of the padding is the entire PDU it seems.
// And other cases ?
//*****************************************************************************

//*****************************************************************************
// Includes.
//*****************************************************************************

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "precomp.h"
#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_build_types_netpdl.h"

#include "tinyxml.h"

#if 1

//*****************************************************************************
// byte_interpret_get_include_file_name
//*****************************************************************************

string    byte_interpret_get_include_directory();


//*****************************************************************************
// normalize_type_name
// To avoid wireshark crash on proto_register_field_array
//*****************************************************************************
static
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
// generate_type_name
//*****************************************************************************
static
string  generate_type_name(string    base_type_name)
{
	static int     S_automatically_generated_type_name_counter = 0;

	base_type_name += "_generated_netpdl_";
	base_type_name += get_string(++S_automatically_generated_type_name_counter);

	return  normalize_type_name(base_type_name);
}

//*****************************************************************************
// normalize_field_name
//*****************************************************************************

#define normalize_field_name  normalize_type_name

//*****************************************************************************
// normalize_expression
//*****************************************************************************
static
string    normalize_expression(string    str)
{
	// ICIOA will NOT work if there is no blanks or ...

	mod_replace_all(str, "buf2int(", "(");
	mod_replace_all(str, " div ", " / ");
	mod_replace_all(str, " mod ", " % ");
	mod_replace_all(str, " bitwand ", " & ");
	mod_replace_all(str, " bitwor ", " | ");
	mod_replace_all(str, " bitwnot ", " ! ");  // ICIOA NOT implemented
	mod_replace_all(str, " eq ", " == ");
	mod_replace_all(str, " ne ", " != ");
	mod_replace_all(str, " lt ", " < ");
	mod_replace_all(str, " le ", " <= ");
	mod_replace_all(str, " gt ", " > ");
	mod_replace_all(str, " ge ", " >= ");
	mod_replace_all(str, " and ", " && ");
	mod_replace_all(str, " or ", " || ");
	mod_replace_all(str, " not ", " ! ");  // ICIOA NOT implemented

	str += ")";
	str = "(" + str;

	return  str;
}

//*****************************************************************************
// get_byte_size_TiXmlElement
//*****************************************************************************

long   get_byte_size_TiXmlElement(TiXmlElement  * element)
{
	const char  * size         = element->Attribute("size");
	M_FATAL_IF_EQ(size, NULL_PTR);

	long    size_long;
	if (get_number(size, &size_long) != true)
	{
		M_FATAL_COMMENT("size " << size << " is not an integer");
	}

	M_FATAL_IF_LE(size_long, 0);

	return  size_long;
}

//*****************************************************************************
// get_bit_size_TiXmlElement
//*****************************************************************************

long   get_bit_size_TiXmlElement(TiXmlElement  * element)
{
	return  get_byte_size_TiXmlElement(element) * 8;
}

//*****************************************************************************
// update_field_type_name_with_showtemplate
//*****************************************************************************

void    update_field_type_name_with_showtemplate(
									const T_type_definitions  & type_definitions,
										  T_field_type_name   & field_type_name,
									const char                * showtemplate,
									const long                  bit_size = -1)
{
	if (showtemplate == NULL_PTR)
		return;

	if (strcmp(showtemplate, "FieldMul4Dec") == 0)
	{
		// Never transform a value in netpdl
//		field_type_name.transform_quantum = 4;

		type_definitions.set_field_display_expression(field_type_name, "print(\"%d\", this*4)");
	}
	else if (strcmp(showtemplate, "FieldDec") == 0)
	{
	}
	else if (strcmp(showtemplate, "FieldHex") == 0)
	{
		type_definitions.set_field_display(field_type_name, "hex");
	}
	else if (strcmp(showtemplate, "Field4BytesHex") == 0)
	{
		type_definitions.set_field_display(field_type_name, "%04x");
	}
	else if (strcmp(showtemplate, "FieldBin") == 0)
	{
		type_definitions.set_field_display(field_type_name, "bin");
	}
	else
	{
		M_FATAL_COMMENT("Unknow showtemplate " << showtemplate);
	}
}

//*****************************************************************************
// update_field_type_name_with_showtemplate
//*****************************************************************************

void    update_field_type_name_with_showtemplate(
									const T_type_definitions  & type_definitions,
										  T_field_type_name   & field_type_name,
									const TiXmlElement        * element,
									const long                  bit_size = -1)
{
	const char  * showtemplate = element->Attribute("showtemplate");

	update_field_type_name_with_showtemplate(type_definitions, field_type_name, showtemplate, bit_size);
}






//*****************************************************************************
// netpdl_manage_format_fields_field_fixed
//*****************************************************************************

void  netpdl_manage_format_fields_field_fixed(T_type_definitions  & type_definitions,
											  T_field_type_name   & field_type_name,
											  TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_field_fixed", element);

	const char  * name         = element->Attribute("name");
//	const char  * longname     = element->Attribute("longname");
//	const char  * size         = element->Attribute("size");
//	const char  * showtemplate = element->Attribute("showtemplate");
//	const char  * comment      = element->Attribute("comment");

	M_FATAL_IF_EQ(name, NULL_PTR);
//	M_FATAL_IF_EQ(size, NULL_PTR);

	const long  byte_size = get_byte_size_TiXmlElement(element);
	const long  bit_size = byte_size * 8;

	// NOT good, netpdl does not specify the type !
	// ICIOA
	if (byte_size > 4)    // > 8 possible (tcp.xml md5digest)
	{
		type_definitions.set_field_type(field_type_name, "raw");
		type_definitions.set_field_type_size_or_parameter(field_type_name, get_string(byte_size));
	}
	else
	{
		type_definitions.set_field_type(field_type_name, "uint" + get_string(bit_size));

		update_field_type_name_with_showtemplate(type_definitions, field_type_name, element);
	}

	type_definitions.set_field_name(field_type_name, normalize_field_name(name));
}

//*****************************************************************************
// netpdl_manage_format_fields_field_variable
// expr="$packetlength - $currentoffset"         example from telnet.xml ICIOA
//*****************************************************************************

void  netpdl_manage_format_fields_field_variable(T_type_definitions  & type_definitions,
												 T_field_type_name   & field_type_name,
												 TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_field_variable", element);

	const char  * name = element->Attribute("name");
	const char  * expr = element->Attribute("expr");
	M_FATAL_IF_EQ(name, NULL_PTR);
	M_FATAL_IF_EQ(expr, NULL_PTR);

	// ICIOA
	type_definitions.set_field_type(field_type_name, "raw");
	type_definitions.set_field_type_size_or_parameter(field_type_name, normalize_expression(expr));

	type_definitions.set_field_name(field_type_name, normalize_field_name(name));
}

//*****************************************************************************
// netpdl_manage_format_fields_field_bit
// ICIOA bitfield inside bitfield not managed (example tcp.xml)
//*****************************************************************************

void  netpdl_manage_format_fields_field_bit(T_type_definitions  & type_definitions,
											T_field_type_name   & field_type_name,
											TiXmlElement       *& element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_field_bit", element);

	const long  bit_size_1st = get_bit_size_TiXmlElement(element);
	long long   bit_mask_previous = 0;

	type_definitions.set_field_type(field_type_name, "bitfield");    // bitstream also ok
	type_definitions.set_field_name(field_type_name, "");

	field_type_name.P_bitfield_inline.reset(new T_bitfield_definition);

	T_bitfield_definition  & bitfield_definition = *field_type_name.P_bitfield_inline;
	bitfield_definition.is_a_bitstream = true;
	type_definitions.set_field_type(bitfield_definition.master_field, "uint" + get_string(bit_size_1st));
	type_definitions.set_field_name(bitfield_definition.master_field, "last_bitfield_value");
	type_definitions.set_field_display(bitfield_definition.master_field, "bin");


	TiXmlElement       * element_1st  = element;
	TiXmlElement       * element_last = element;
	for (                ;
						 element != NULL_PTR;
						 element  = element->NextSiblingElement())
	{
		if (element != element_1st)
		{
			// Check it is a field.
			{
				const char  * value = element->Value();
				if (strcmp(value, "field") != 0)
				{
					M_STATE_DEBUG("Next element is not a field");
					break;
				}
			}

			// Check it is a field bit.
			{
				const char  * type = element->Attribute("type");
				M_FATAL_IF_EQ(type, NULL_PTR);
				if (strcmp(type, "bit") != 0)
				{
					M_STATE_DEBUG("Next element is not a field bit");
					break;
				}
			}

			// Check the size is identic to 1st size.
			{
				const long  bit_size = get_bit_size_TiXmlElement(element);
				if (bit_size != bit_size_1st)
				{
					M_STATE_DEBUG("Next element has not the same size");
					break;
				}
			}
		}

		// Get the mask value.
		const char  * mask         = element->Attribute("mask"); 
		M_FATAL_IF_EQ(mask, NULL_PTR);
		long long        bit_mask = 0;
		if (get_number(mask, bit_mask) != true)
		{
			M_FATAL_COMMENT("Unable to interpret bit mask " << mask);
		}
		M_FATAL_IF_LE(bit_mask, 0);

		if (element != element_1st)
		{
			// Check the current mask has no common part with previous mask.
			if (bit_mask & bit_mask_previous)
			{
				M_STATE_DEBUG("Next element have a common part of the bit mask");
				break;
			}

			// Check the current mask is < to the previous mask.
			if (bit_mask >= bit_mask_previous)
			{
				M_STATE_DEBUG("Next element has a bigger bit mask");
				break;
			}

			// The current bit field element belongs to the same bit field group.
		}

		// Save bit mask.
		// Save element_last.
		bit_mask_previous = bit_mask;
		element_last = element;

		const char  * name         = element->Attribute("name");
		M_FATAL_IF_EQ(name, NULL_PTR);

		T_field_type_name     bit_field_type_name;
		type_definitions.set_field_type(bit_field_type_name, bitfield_definition.master_field.type);
		type_definitions.set_field_name(bit_field_type_name, normalize_field_name(name));
		string  new_expression = "(last_bitfield_value & " + string(mask) + ")";
		bit_field_type_name.A_is_a_variable = true;

		{
			// Must exist better, no ?
			long long    bit_mask_to_compare = 1;
			int          necessary_shift = 0;
			for (; ; ++necessary_shift)
			{
				if (bit_mask_to_compare & bit_mask)
					break;

				bit_mask_to_compare <<= 1;
			}

			new_expression += " >> " + get_string(necessary_shift);
		}
		bit_field_type_name.new_expression.build_expression(type_definitions, new_expression);

		update_field_type_name_with_showtemplate(type_definitions, bit_field_type_name, element);

		M_STATE_DEBUG("Add field bit element " << bit_field_type_name.name << " = " << bit_field_type_name.new_expression.get_original_string_expression());

		bitfield_definition.fields_definition.push_back(bit_field_type_name);
	}

	element = element_last;
}

//*****************************************************************************
// netpdl_manage_format_fields_field_line
//*****************************************************************************

void  netpdl_manage_format_fields_field_line(T_type_definitions  & type_definitions,
												 T_field_type_name   & field_type_name,
												 TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_field_line", element);

	const char  * name = element->Attribute("name");
	M_FATAL_IF_EQ(name, NULL_PTR);

	type_definitions.set_field_type(field_type_name, "string_nl");
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));
}

//*****************************************************************************
// netpdl_manage_format_fields_field_padding
// ICIOA  (example tcp.xml)
//*****************************************************************************

void  netpdl_manage_format_fields_field_padding(T_type_definitions  & type_definitions,
												T_field_type_name   & field_type_name,
												TiXmlElement       *& element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_field_padding", element);
	M_STATE_ERROR("do not how to manage padding");  // ICIOA

	const char  * align = element->Attribute("align");
	M_FATAL_IF_EQ(align, NULL_PTR);

	// ICIOA a quoi s'applique le padding ?
}

//*****************************************************************************
// forward
//*****************************************************************************

void  netpdl_manage_format_fields(T_type_definitions   & type_definitions,
								  T_struct_fields      & struct_fields,
								  TiXmlElement         * root);

//*****************************************************************************
// netpdl_manage_format_fields_field
//*****************************************************************************

void  netpdl_manage_format_fields_field(T_type_definitions  & type_definitions,
										T_field_type_name   & field_type_name,
										TiXmlElement       *& element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_field", element);

	const char  * type = element->Attribute("type");
	M_FATAL_IF_EQ(type, NULL_PTR);

	if (strcmp(type, "fixed") == 0)
	{
		netpdl_manage_format_fields_field_fixed(type_definitions, field_type_name, element);
	}
	else if (strcmp(type, "bit") == 0)
	{
		netpdl_manage_format_fields_field_bit(type_definitions, field_type_name, element);
	}
	else if (strcmp(type, "variable") == 0)
	{
		netpdl_manage_format_fields_field_variable(type_definitions, field_type_name, element);
	}
	/* ICIOA tokenended */
	/* ICIOA tokenwrapped */
	else if (strcmp(type, "line") == 0)
	{
		netpdl_manage_format_fields_field_line(type_definitions, field_type_name, element);
	}
	else if (strcmp(type, "padding") == 0)
	{
		netpdl_manage_format_fields_field_padding(type_definitions, field_type_name, element);
	}
	/* ICIOA plugin */
	else
	{
		M_FATAL_COMMENT("Unknow type " << type << " inside <field>");
	}
}

//*****************************************************************************
// netpdl_manage_format_fields_block
//*****************************************************************************

void  netpdl_manage_format_fields_block(T_type_definitions  & type_definitions,
										T_field_type_name   & field_type_name,
										TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_block", element);

	const char  * name = element->Attribute("name");
	M_FATAL_IF_EQ(name, NULL_PTR);

	type_definitions.set_field_type(field_type_name, "struct");
	type_definitions.set_field_name(field_type_name, normalize_field_name(name));
	field_type_name.P_sub_struct.reset(new T_struct_definition);

	netpdl_manage_format_fields(type_definitions, field_type_name.P_sub_struct->fields, element);
}

//*****************************************************************************
// netpdl_manage_format_fields_includeblk
// inline struct
//*****************************************************************************

void  netpdl_manage_format_fields_includeblk(T_type_definitions  & type_definitions,
											 T_field_type_name   & field_type_name,
											 TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_includeblk", element);

	const char  * name = element->Attribute("name");
	M_FATAL_IF_EQ(name, NULL_PTR);

	// Not really inline.
	// Could be a problem if the block contains break ... ?
	type_definitions.set_field_type(field_type_name, normalize_type_name(name));
	type_definitions.set_field_name(field_type_name, "");
}

//*****************************************************************************
// netpdl_manage_format_fields_loop
//*****************************************************************************

void  netpdl_manage_format_fields_loop(T_type_definitions  & type_definitions,
									   T_field_type_name   & field_type_name,
									   TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_loop", element);

	const char  * type = element->Attribute("type");
	const char  * expr = element->Attribute("expr");

	M_FATAL_IF_EQ(type, NULL_PTR);
	M_FATAL_IF_EQ(expr, NULL_PTR);

	if (strcmp(type, "size") == 0)
	{
		type_definitions.set_field_type(field_type_name, "loop_size_bytes");
	}
	else if (strcmp(type, "times2repeat") == 0)
	{
		type_definitions.set_field_type(field_type_name, "loop_nb_times");
	}
	else if (strcmp(type, "while") == 0)
	{
		type_definitions.set_field_type(field_type_name, "while");
	}
	else if (strcmp(type, "do-while") == 0)
	{
		type_definitions.set_field_type(field_type_name, "do_while");
	}
	else
	{
		M_FATAL_COMMENT("Unknow loop type " << type);
	}

	type_definitions.set_field_condition_expression(field_type_name, normalize_expression(expr));
	field_type_name.P_sub_struct.reset(new T_struct_definition);

	netpdl_manage_format_fields(type_definitions, field_type_name.P_sub_struct->fields, element);
}

//*****************************************************************************
// netpdl_manage_format_fields_loopctrl
//*****************************************************************************

void  netpdl_manage_format_fields_loopctrl(T_type_definitions  & type_definitions,
										   T_field_type_name   & field_type_name,
										   TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_loopctrl", element);

	const char  * type = element->Attribute("type");
	M_FATAL_IF_EQ(type, NULL_PTR);

	type_definitions.set_field_type(field_type_name, type);    // break or continue
}

//*****************************************************************************
// netpdl_manage_format_fields_if
//*****************************************************************************

void  netpdl_manage_format_fields_if(T_type_definitions  & type_definitions,
									 T_field_type_name   & field_type_name,
									 TiXmlElement        * element)
{
	M_STATE_ENTER("netpdl_manage_format_fields_if", element);

	const char  * expr = element->Attribute("expr");
	M_FATAL_IF_EQ(expr, NULL_PTR);

	type_definitions.set_field_type(field_type_name, "if");
	type_definitions.set_field_name(field_type_name, normalize_expression(expr));

	{
		TiXmlElement  * element_if_true  = element->FirstChildElement("if-true");
		M_FATAL_IF_EQ(element_if_true, NULL_PTR);

		field_type_name.P_sub_struct.reset(new T_struct_definition);
		netpdl_manage_format_fields(type_definitions, field_type_name.P_sub_struct->fields, element);
	}

	{
		TiXmlElement  * element_if_false = element->FirstChildElement("if-false");
		if (element_if_false != NULL_PTR)
		{
			netpdl_manage_format_fields(type_definitions, field_type_name.sub_struct_2, element);
		}
	}

	// ICIOA missing-packetdata not managed
}

//*****************************************************************************
// netpdl_manage_format_fields_switch
//*****************************************************************************

void  netpdl_manage_format_fields_switch(T_type_definitions  & type_definitions,
										 T_field_type_name   & field_type_name,
										 TiXmlElement        * root)
{
	M_STATE_ENTER("netpdl_manage_format_fields_switch", root);

	const char  * expr = root->Attribute("expr");
	M_FATAL_IF_EQ(expr, NULL_PTR);


	const string    switch_type_name = generate_type_name("netpdl_switch");

	T_switch_definition  & switch_definition = type_definitions.map_switch_definition[switch_type_name];

	type_definitions.set_field_type(field_type_name, switch_type_name);
	type_definitions.set_field_type_size_or_parameter(field_type_name, normalize_expression(expr));
	type_definitions.set_field_name(field_type_name, "");

	for (TiXmlElement  * element  = root->FirstChildElement();
		                 element != NULL_PTR;
						 element  = element->NextSiblingElement())
	{
		T_switch_case_value    switch_case_value;

		const char  * value = element->Value();
		M_FATAL_IF_EQ(value, NULL_PTR);

		if (strcmp (value, "case") == 0)
		{
			const char  * case_value = element->Attribute("value");
			M_FATAL_IF_EQ(case_value, NULL_PTR);
			switch_case_value.case_value = case_value;
			switch_case_value.case_value.promote();
			/* ICIOA const char  * case_maxvalue = element->Attribute("maxvalue"); */

			if (switch_case_value.case_value.get_type() == C_value::E_type_integer)
			{
				switch_definition.case_type = "int64";
			}
			else
			{
				switch_definition.case_type = "string";
			}
		}
		else if (strcmp (value, "default") == 0)
		{
			switch_case_value.is_default_case = true;
		}
		else
		{ 
			M_FATAL_COMMENT("Unknow value " << value << " inside <switch>");
		}

		netpdl_manage_format_fields(type_definitions, switch_case_value.fields, element);

		switch_definition.switch_case.push_back(switch_case_value);
	}
}

//*****************************************************************************
// netpdl_manage_format_fields
//*****************************************************************************

void  netpdl_manage_format_fields(T_type_definitions   & type_definitions,
								  T_struct_fields      & struct_fields,
								  TiXmlElement         * root)
{
	M_STATE_ENTER("netpdl_manage_format_fields", root);

	for (TiXmlElement  * element  = root->FirstChildElement();
		                 element != NULL_PTR;
						 element  = element->NextSiblingElement())
	{
		T_field_type_name     field_type_name;

		const char  * value = element->Value();

		if (strcmp (value, "field") == 0)
		{
			netpdl_manage_format_fields_field(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "block") == 0)
		{
			netpdl_manage_format_fields_block(type_definitions, field_type_name, element);
		}
		else if (strcmp (value, "includeblk") == 0)
		{
			netpdl_manage_format_fields_includeblk(type_definitions, field_type_name, element);
		}
		else if (strcmp(value, "loop") == 0)
		{
			netpdl_manage_format_fields_loop(type_definitions, field_type_name, element);
		}
		else if (strcmp(value, "loopctrl") == 0)
		{
			netpdl_manage_format_fields_loopctrl(type_definitions, field_type_name, element);
		}
		else if (strcmp(value, "if") == 0)
		{
			netpdl_manage_format_fields_if(type_definitions, field_type_name, element);
		}
		else if (strcmp(value, "switch") == 0)
		{
			netpdl_manage_format_fields_switch(type_definitions, field_type_name, element);
		}
		else
		{ 
			M_FATAL_COMMENT("Unknow value " << value << " inside <fields>");
		}

		if (field_type_name.type.empty())    // ICIOA
		{
			M_STATE_ERROR("field_type_name.type is empty");
			continue;
		}

		struct_fields.push_back (field_type_name);
	}
}

//*****************************************************************************
// netpdl_manage_format_block
//*****************************************************************************

void  netpdl_manage_format_block(T_type_definitions  & type_definitions,
								 TiXmlElement        * root)
{
	M_STATE_ENTER("netpdl_manage_format_block", root);

	const char  * name = root->Attribute("name");
	const string  struct_name = normalize_type_name(name);

	type_definitions.could_define_new_type(struct_name, T_type_definitions::E_type_struct);

	T_struct_definition    struct_definition;

	netpdl_manage_format_fields(type_definitions, struct_definition.fields, root);

	type_definitions.map_struct_definition[struct_name] = struct_definition;
}

//*****************************************************************************
// netpdl_manage_format
//*****************************************************************************

void  netpdl_manage_format(T_type_definitions  & type_definitions,
						   TiXmlElement        * root)
{
	M_STATE_ENTER("netpdl_manage_format", root);

	// Some struct ...
	for (TiXmlElement  * element_block  = root->FirstChildElement("block");
		                 element_block != NULL_PTR;
						 element_block  = element_block->NextSiblingElement("block"))
	{
		netpdl_manage_format_block(type_definitions, element_block);
	}

	// The fields of the protocol ...
	TiXmlElement  * element  = root->FirstChildElement("fields");
	M_FATAL_IF_EQ(element, NULL_PTR);

	{
		T_struct_definition    struct_definition;

		T_field_type_name     field_type_name;
		type_definitions.set_field_type(field_type_name, "byte_order");
		type_definitions.set_field_name(field_type_name, "big_endian");

		struct_definition.fields.push_back(field_type_name);

		netpdl_manage_format_fields(type_definitions, struct_definition.fields, element);

		// ICIOA hard coded name
		type_definitions.map_struct_definition["T_netpdl_header"] = struct_definition;
	}

	// 
	{
		T_struct_definition    struct_definition;

		{
			T_field_type_name     field_type_name;
			type_definitions.set_field_type(field_type_name, "T_netpdl_header");
			type_definitions.set_field_name(field_type_name, "");

			struct_definition.fields.push_back(field_type_name);
		}
		{
			T_field_type_name     field_type_name;
			type_definitions.set_field_type(field_type_name, "subproto");
			type_definitions.set_field_type_size_or_parameter(field_type_name, "*");
			type_definitions.set_field_name(field_type_name, "data");

			struct_definition.fields.push_back(field_type_name);
		}

		// ICIOA hard coded name
		type_definitions.map_struct_definition["T_netpdl_msg"] = struct_definition;
	}
}

//*****************************************************************************
// netpdl_manage_main_file
//*****************************************************************************

void    netpdl_manage_main_file(T_type_definitions  & type_definitions,
						  const string              & full_file_name)
{
	M_STATE_ENTER("netpdl_manage_main_file", full_file_name);

	TiXmlDocument   doc(full_file_name.c_str());
	bool loadOkay = doc.LoadFile();
	if (loadOkay != true)
	{
		M_FATAL_COMMENT("Could not load " << full_file_name);
	}

	TiXmlElement  * root = doc.FirstChildElement("protocol");
	if (root == NULL)
	{
		M_FATAL_COMMENT("Could not find protocol into " << full_file_name);
		return;
	}

	M_FATAL_IF_FALSE(root != NULL);

	{
		TiXmlElement  * element = root->FirstChildElement("execute-code");
		// Do not manage  at this time
	}

	{
		TiXmlElement  * element = root->FirstChildElement("format");
		M_FATAL_IF_EQ(element, NULL_PTR);
		netpdl_manage_format(type_definitions, element);
	}

	// Do not manage  encapsulation

	// Do not manage  visualization for now
}

//*****************************************************************************
// build_netpdl_types *********************************************************
//*****************************************************************************

bool      build_netpdl_types (const string              & main_file_name,
                                    T_type_definitions  & type_definitions)
{
	M_STATE_ENTER ("build_netpdl_types", main_file_name);

	netpdl_manage_main_file(type_definitions,
                            byte_interpret_get_include_directory() + "/" + main_file_name);

	return  false;
}
#else
bool      build_netpdl_types (const string              & main_file_name,
                                    T_type_definitions  & type_definitions)
{
	return  false;
}
#endif

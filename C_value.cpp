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

#include "precomp.h"
#include "C_value.h"
#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"

#include <cstdio>
#include <cmath>
using namespace std;


//*****************************************************************************
// 
//*****************************************************************************

ostream &  operator<< (ostream  & os, const C_value::E_type  type)
{
	if (type == C_value::E_type_integer)
	{
		os << "integer";
	}
	else if (type == C_value::E_type_float)
	{
		os << "float";
	}
	else if (type == C_value::E_type_string)
	{
		os << "string";
	}
	else if (type == C_value::E_type_msg)
	{
		os << "msg";
	}
	else if (type == C_value::E_type_struct)
	{
		os << "struct";
	}
	else
	{
		os << "unknow_type=" << static_cast<int>(type);
	}

	return  os;
}

//*****************************************************************************
// 
//*****************************************************************************

ostream &  operator<< (ostream  & os, const C_value        & rhs)
{
	os << "type=" << rhs.A_type << "  ";
    os << "int=" << rhs.A_integer << "  ";
    os << "flt=" << rhs.A_flt << "  ";
    os << "str=" << rhs.A_str << "  ";
	os << "bit_position_offset=" << rhs.A_bit_position_offset << "  ";
	os << "bit_position_size=" << rhs.A_bit_position_size << "  ";
//	os << "external_type__cvtbd=" << rhs.A_external_type__cvtbd << "  ";
	os << "external_type_bit_size__cvtbd=" << rhs.A_external_type_bit_size__cvtbd << "  ";

	return  os;
}

//*****************************************************************************
// 
//*****************************************************************************

C_value::C_value ()
    :A_type (E_type_string),
	 A_external_type_bit_size__cvtbd(0),
     A_integer (0),
     A_flt (0.0),
     A_str (""),                 // means NOT intialized for quantum, offset, constraints
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
}

C_value::C_value (const E_type               type)
    :A_type (type),
	 A_external_type_bit_size__cvtbd(0),
     A_integer (0),
     A_flt (0.0),
     A_str (""),                 // means NOT intialized for quantum, offset, constraints
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
	if (A_type == E_type_msg)
	{
		A_str = "nil";
	}
}

C_value::C_value (const E_type               type,
						T_msg                msg)
    :A_type (type),
	 A_external_type_bit_size__cvtbd(0),
     A_integer ((long long)msg),
     A_flt ((double)(long long)msg),
     A_str (get_string(msg)),
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
	if (A_type != E_type_msg)
	{
		M_FATAL_COMMENT("Value type must be msg and not " << type);
	}
	if (A_integer == 0)
	{
		A_str = "nil";
	}
}


#define CTOR_INT_LL(TYPE)                                                     \
C_value::C_value (TYPE    integer)                                            \
    :A_type (E_type_integer),                                                 \
	 A_external_type_bit_size__cvtbd(0),                                      \
     A_integer (integer),                                                     \
     A_flt ((double)integer),                                                 \
     A_str (get_string ((long long)integer)),                                 \
	 A_bit_position_offset(-1),                                               \
	 A_bit_position_size(-1)                                                  \
{                                                                             \
}

// long long cast is mandatory for char ctor
#define CTOR_INT_CHAR(TYPE)    CTOR_INT_LL(TYPE)

// long long cast gives better performance (see get_string implementation)
#define CTOR_INT(TYPE)         CTOR_INT_LL(TYPE)

CTOR_INT(bool)
CTOR_INT_CHAR(         char)
CTOR_INT_CHAR(  signed char)
CTOR_INT_CHAR(unsigned char)
CTOR_INT(  signed short)
CTOR_INT(unsigned short)
CTOR_INT(  signed int)
CTOR_INT(unsigned int)
CTOR_INT(  signed long)
CTOR_INT_LL(unsigned long)
CTOR_INT_LL(  signed long long)
CTOR_INT_LL(unsigned long long)


C_value::C_value (float        flt)
    :A_type (E_type_float),
	 A_external_type_bit_size__cvtbd(0),
     A_integer ((long long)flt),
     A_flt (flt),
     A_str (get_string (flt)),        // pb precision
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
}

C_value::C_value (double       flt)
    :A_type (E_type_float),
	 A_external_type_bit_size__cvtbd(0),
     A_integer ((long long)flt),
     A_flt (flt),
     A_str (get_string (flt)),        // pb precision
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
}

C_value::C_value (const string    & str)
    :A_type (E_type_string),
	 A_external_type_bit_size__cvtbd(0),
     A_integer (0),
     A_flt (0.0),
     A_str (str),
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
	// No automatic transformation to integer or float.
	// This must be done explicitely by user.
}

// NB: this constructor is necessary,
//  because the string version is NOT called with :
// C_value("anything")
// It seems that an integer constructor is called instead !!!
C_value::C_value (const char      * str)
    :A_type (E_type_string),
	 A_external_type_bit_size__cvtbd(0),
     A_integer (0),
     A_flt (0.0),
     A_str (str),
	 A_bit_position_offset(-1),
	 A_bit_position_size(-1)
{
}

//*****************************************************************************
// Assignment operator
//*****************************************************************************
C_value &
C_value::operator=(const C_value  & rhs)
{
	A_type                 = rhs.A_type;
	A_integer              = rhs.A_integer;
	A_flt                  = rhs.A_flt;
	A_str                  = rhs.A_str;

	A_bit_position_offset  = rhs.A_bit_position_offset;
	A_bit_position_size    = rhs.A_bit_position_size;

	// Special behavior
	// ICIOA a revoir
	// si la valeur de this est definie :
	// - ne pas la modifier
	// - verifier que la valeur assignee est compatible
	// si la valeur de this n'est pas definie :
	// - ne pas la modifier ? 
	// voir M_COMPUTE_NEW_EXTERNAL_TYPE_RESULT_RHS
	if (A_external_type_bit_size__cvtbd == 0)
	{
		A_external_type_bit_size__cvtbd = rhs.A_external_type_bit_size__cvtbd;
	}

	return  *this;
}

//*****************************************************************************
// external_type
//*****************************************************************************

void
C_value::set_external_type(const std::string  & final_type)
{
//	A_external_type__cvtbd = final_type;
	A_external_type_bit_size__cvtbd = 0;

	long    bit_size = 0;
	if ((strncmp(final_type.c_str(), "uint", 4) == 0) &&
		(get_number(final_type.c_str()+4, &bit_size) == true))
	{
		A_external_type_bit_size__cvtbd = bit_size;
	}
	else if ((strncmp(final_type.c_str(), "int", 3) == 0) &&
			 (get_number(final_type.c_str()+3, &bit_size) == true))
	{
		A_external_type_bit_size__cvtbd = - bit_size;
	}
	else if ((strncmp(final_type.c_str(), "float", 3) == 0) &&
			 (get_number(final_type.c_str()+5, &bit_size) == true))
	{
		A_external_type_bit_size__cvtbd = - bit_size;
	}
	// else string ???
}
	
bool
C_value::get_external_type_signed ()  const
{
	return  A_external_type_bit_size__cvtbd < 0;
}

int
C_value::get_external_type_bit_size ()  const
{
	if (A_external_type_bit_size__cvtbd < 0)
		return  - A_external_type_bit_size__cvtbd;
	else
		return  A_external_type_bit_size__cvtbd;
}

int
C_value::get_external_type_byte_size ()  const
{
	if (A_external_type_bit_size__cvtbd < 0)
		return  - A_external_type_bit_size__cvtbd / 8;
	else
		return  A_external_type_bit_size__cvtbd / 8;
}

void
C_value::set_external_type_bit_size(int    external_type_bit_size)
{
	A_external_type_bit_size__cvtbd = external_type_bit_size;
}

//*****************************************************************************
// set_bit_position_offset_size
//*****************************************************************************
void
C_value::set_bit_position_offset_size(int  offset, int  size)
{
	A_bit_position_offset = offset;
	A_bit_position_size = size;
}

//*****************************************************************************
// Get methods : Fatal if internal type does not match.
//*****************************************************************************

long long
C_value::get_int ()  const
{
    M_FATAL_IF_NE (A_type, E_type_integer);
    return  A_integer;
}

double
C_value::get_flt ()  const
{
    M_FATAL_IF_NE (A_type, E_type_float);
    return  A_flt;
}

const string  &
C_value::get_str ()  const
{
    M_FATAL_IF_NE (A_type, E_type_string);
    return  A_str;
}

C_value::T_msg
C_value::get_msg ()  const
{
    M_FATAL_IF_NE (A_type, E_type_msg);
    return  reinterpret_cast<T_msg>(A_integer);
}

bool
C_value::get_bool ()  const
{
    if (A_type == E_type_integer)
        return  A_integer != 0;
//    else if (A_type == E_type_float)    // ICIOA 2009/03/08 if necessary -> as_bool
//        return  A_flt != 0;

    M_FATAL_COMMENT ("get_bool bad type " << A_type);
    return  false;
}

//*****************************************************************************
// No fatal.
//*****************************************************************************

const string  &
C_value::as_string () const
{
    return  A_str;
}

//*****************************************************************************
// Fatal if fail to promote value to a numeric value.
//*****************************************************************************
void
C_value::to_numeric()
{
	M_STATE_ENTER("to_numeric", "");

	promote();

	M_FATAL_IF_NE (is_numeric (), true);
}

//*****************************************************************************
// Fatal if fail to promote value to a int value.
//*****************************************************************************
void
C_value::to_int(int  base)
{
	M_STATE_ENTER("to_int", "");

	if (A_type == E_type_float)
	{
		A_type = E_type_integer;
	}
	else if (A_type == E_type_string)
	{
		// Try to promote to integer
		long long    value;
		if (get_number(A_str.c_str(), base, value))
		{
			A_type = E_type_integer;
			A_integer = value;
			A_flt = (double)value;
		}
	}

	M_FATAL_IF_NE (get_type (), E_type_integer);
}

//*****************************************************************************
// Fatal if fail to promote value to a float value.
//*****************************************************************************
void
C_value::to_float()
{
	M_STATE_ENTER("to_float", "");

	if (A_type == E_type_integer)
	{
		A_type = E_type_float;
	}
	else if (A_type == E_type_string)
	{
		// Try to promote to float
		double    value;
		if (get_number(A_str.c_str(), value))
		{
			A_type = E_type_float;
			A_integer = (long long)value;
			A_flt = value;
		}
	}

	M_FATAL_IF_NE (get_type (), E_type_float);
}

//*****************************************************************************
// get_format_percent_letter
//*****************************************************************************

char    get_format_percent_letter(const char  * str)
{
	char    format_percent_letter = '\0';    // nothing found

	while ((str = strchr(str, '%')) != NULL)
	{
		++str;

		if (*str == '%')
		{
			++str;
			continue;
		}

		while ((*str != '\0') &&
			   ((isalpha(*str) == 0) || (*str == 'h') || (*str == 'l') || (*str == 'L')))
		{
			++str;
		}

		format_percent_letter = tolower(*str);
		break;
	}

	return  format_percent_letter;
}

//*****************************************************************************
// Try to apply format to the value.
// Only str modified.
//*****************************************************************************
void
C_value::format(const string  & display)
{
	M_STATE_ENTER("format", "display=" << display << "  type=" << A_type << "  str=" << A_str);

	char    tmp_str[999+1];

	if (display == "hex")
	{
	    if (A_type == E_type_integer)
		{
			sprintf(tmp_str, "0x%llx", A_integer);
			A_str = tmp_str;
		}
	}
	else if (display == "oct")
	{
	    if (A_type == E_type_integer)
		{
			sprintf(tmp_str, "0%llo", A_integer);
			A_str = tmp_str;
		}
	}
	else if (display == "bin")
	{
	    if (A_type == E_type_integer)
		{
			strcpy(tmp_str, "");
			bool    output_zero = false;
			for (int  idx = 63; idx >= 0; --idx)
			{
				if (A_integer & (1LL << idx))
				{
					if (output_zero == false)
					{
						strcat(tmp_str, "b");
						output_zero = true;
					}
					strcat(tmp_str, "1");
				}
				else if (output_zero)
				{
					strcat(tmp_str, "0");
				}
			}
			if (output_zero == false)
			{
				strcpy(tmp_str, "b0");
			}
			A_str = tmp_str;
		}
	}
	else
	{
		char   format_percent_letter = get_format_percent_letter(display.c_str());

		if (format_percent_letter == 's')
		{
			sprintf(tmp_str, display.c_str(), A_str.c_str());
			A_str = tmp_str;
		}
		else if ((format_percent_letter != '\0') && (is_numeric()))
		{
			if ((format_percent_letter == 'e') ||
				(format_percent_letter == 'f') ||
				(format_percent_letter == 'g'))
			{
				sprintf(tmp_str, display.c_str(), A_flt);
				A_str = tmp_str;
			}
			else
			{
				sprintf(tmp_str, display.c_str(), A_integer);
				A_str = tmp_str;
			}
		}
		else
		{
			// Error, unexpected display string
			M_STATE_ERROR("format bad display " << display);
		}
	}
}

//*****************************************************************************
// Re-initialise string without format.
// Nothing done for a string value.
//*****************************************************************************
void
C_value::format_reset()
{
	M_STATE_ENTER("format_reset", "");

	if (A_type == E_type_float)
	{
		A_str = get_string(A_flt);
	}
	else if (A_type == E_type_integer)
	{
		A_str = get_string(A_integer);
	}
	else if (A_type == E_type_msg)
	{
		if (A_integer != 0)
		{
			A_str = get_string(A_integer);
		}
	}
}

//*****************************************************************************
// Only str modified.
// Type and other values NOT modified.
//*****************************************************************************
void
C_value::set_str(const string  & str)
{
	A_str = str;
}

//*****************************************************************************
// Try to promote to numeric type.
// Returns the new type.
//*****************************************************************************

C_value::E_type
C_value::promote()
{
	if (A_type == E_type_string)
	{
		// Try to promote to integer
		{
			long long    value;
			if (get_number(A_str.c_str(), value))
			{
				A_type = E_type_integer;
				A_integer = value;
				A_flt = (double)value;
				return  A_type;
			}
		}

		// Try to promote to float
		{
			double    value;
			if (get_number(A_str.c_str(), value))
			{
				A_type = E_type_float;
				A_integer = (long long)value;
				A_flt = value;
				return  A_type;
			}
		}
	}

	return  A_type;
}

//*****************************************************************************
// ICIOA codage global a revoir
// 
// ICIOA determination de A_external_type__cvtbd a revoir (a verifier au minimum)
//*****************************************************************************

class C_value_set_position_offset
{
	C_value  & A_value_to_set;
	int        A_pos;
	int        A_offset;
//	string     A_external_type__cvtbd;
	int        A_external_type_bit_size__cvtbd;

public:
	C_value_set_position_offset(C_value  & val, int pos, int offset,
//						  const string   & external_type__cvtbd)//,
								int        external_type_bit_size__cvtbd)
		:A_value_to_set(val),
		 A_pos(pos),
		 A_offset(offset),
//		 A_external_type__cvtbd(external_type__cvtbd)//,
		 A_external_type_bit_size__cvtbd(external_type_bit_size__cvtbd)
	{
	}

	~C_value_set_position_offset()
	{
		A_value_to_set.set_bit_position_offset_size(A_pos, A_offset);
//		A_value_to_set.set_external_type(A_external_type__cvtbd);
		A_value_to_set.set_external_type_bit_size(A_external_type_bit_size__cvtbd);
	}
};


#define M_COMPUTE_NEW_POSITION_OFFSET_RESULT_RHS(RESULT,RHS)                \
	if (RHS A_bit_position_size > 0)                                        \
	{                                                                       \
		if (RESULT A_bit_position_size <= 0)                                \
		{                                                                   \
			RESULT A_bit_position_offset = RHS A_bit_position_offset;       \
			RESULT A_bit_position_size   = RHS A_bit_position_size;         \
		}                                                                   \
		else                                                                \
		{                                                                   \
			int  bit_position_offset_end = max((RESULT A_bit_position_offset + RESULT A_bit_position_size), (RHS A_bit_position_offset + RHS A_bit_position_size));  \
			RESULT A_bit_position_offset = min(RESULT A_bit_position_offset, RHS A_bit_position_offset);   \
			RESULT A_bit_position_size   = bit_position_offset_end - RESULT A_bit_position_offset;         \
		}                                                                   \
	}

#define M_COMPUTE_NEW_EXTERNAL_TYPE_RESULT_RHS(RESULT,RHS)                  \
    if (RESULT A_external_type_bit_size__cvtbd == 0)                        \
	{                                                                       \
		RESULT A_external_type_bit_size__cvtbd = RHS A_external_type_bit_size__cvtbd;     \
    }

#define M_COMPUTE_NEW_POSITION_OFFSET()                                     \
	M_COMPUTE_NEW_POSITION_OFFSET_RESULT_RHS(this-> ,rhs.);                 \
	M_COMPUTE_NEW_EXTERNAL_TYPE_RESULT_RHS(this-> ,rhs.);                   \
	C_value_set_position_offset  vspo(*this, this->A_bit_position_offset, this->A_bit_position_size, this->A_external_type_bit_size__cvtbd)


//*****************************************************************************
// printf function for C_value
//*****************************************************************************
C_value
C_value::sprintf_values(const std::string      & printf_format,
	                    const vector<C_value>  & values_to_print)
{
	M_STATE_ENTER("sprintf_values", "printf_format=" << printf_format);

	char    tmp_str[999+1];

	// No parameters.
	if (values_to_print.empty())
	{
		sprintf(tmp_str, printf_format.c_str());
		return  tmp_str;
	}

	// 1 parameter.
	// This will NOT work if I want to do a %s on a numeric value.
	// Must I change format to manage %s as I do here ?
#if 0
	if (values_to_print.size() == 1)
	{
		C_value  val = values_to_print[0];
		val.format(printf_format);
		return  val.as_string();
	}
#endif

	// Highlight data.
	int   A_bit_position_offset = -1;
	int   A_bit_position_size = -1;

	// Many parameters.
	strcpy(tmp_str, "");
	unsigned int       value_idx = 0;
	string::size_type  idx_begin = 0;
	string::size_type  idx_format = 0;
	while (idx_begin < printf_format.size())
	{
		idx_format = printf_format.find ('%', idx_begin);

		if (idx_format == string::npos)
		{
			strcat(tmp_str, &printf_format[idx_begin]);
			break;
		}

		strncat(tmp_str, &printf_format[idx_begin], idx_format-idx_begin);
		++idx_format;

		// if %%
		if (printf_format[idx_format] == '%')
		{
			// This is a simple % character
			strcat(tmp_str, "%");

			idx_begin = idx_format + 1;
			continue;
		}

		idx_begin = idx_format - 1;

		M_FATAL_IF_GE (value_idx, values_to_print.size());

		// 
		idx_format = printf_format.find ('%', idx_begin+1);

		if (idx_format == string::npos)
		{
			idx_format = printf_format.size();
		}

		char   tmp_str_format[999+1];
		strncpy(tmp_str_format, &printf_format[idx_begin], idx_format-idx_begin);
		tmp_str_format[idx_format-idx_begin] = '\0';

		M_STATE_DEBUG("str=" << tmp_str << "  " <<
					  "str_format=" << tmp_str_format << "  " <<
					  "idx_begin=" << idx_begin << "  " <<
					  "idx_format=" << idx_format << "  " <<
					  "value_idx=" << value_idx);

		// 
		const C_value  & value = values_to_print[value_idx];

		M_COMPUTE_NEW_POSITION_OFFSET_RESULT_RHS(,value.);

		if (value.get_type() == C_value::E_type_string)
		{
			sprintf(tmp_str+strlen(tmp_str), tmp_str_format, value.as_string().c_str());
		}
		else
		{
			// Search if %...s is specified.
			const char  * p_format = tmp_str_format;
			while ((*p_format != '\0') &&
				   ((isalpha(*p_format) == 0) || (*p_format == 'l') || (*p_format == 'L')))
			{
				++p_format;
			}

			if (*p_format == 's')
			{
				sprintf(tmp_str+strlen(tmp_str), tmp_str_format, value.as_string().c_str());
			}
			else if (value.get_type() == C_value::E_type_float)
			{
				sprintf(tmp_str+strlen(tmp_str), tmp_str_format, value.get_flt());
			}
			else if (value.get_type() == C_value::E_type_integer)
			{
				sprintf(tmp_str+strlen(tmp_str), tmp_str_format, value.get_int());
			}
			else if (value.get_type() == C_value::E_type_msg)
			{
				sprintf(tmp_str+strlen(tmp_str), tmp_str_format, value.get_msg());
			}
		}

		// Next.
		idx_begin = idx_format;
		++value_idx;
	}

	C_value  return_value(tmp_str);
	return_value.set_bit_position_offset_size(A_bit_position_offset,
                                              A_bit_position_size);

	return  return_value;
}

//*****************************************************************************
// Logical operators.
// Allowed : NOT string and NOT string
//*****************************************************************************
bool
C_value::operator&& (const C_value  & rhs) const
{
    M_FATAL_IF_EQ (    A_type, E_type_string);
    M_FATAL_IF_EQ (rhs.A_type, E_type_string);

    return  A_flt && rhs.A_flt;
}

bool
C_value::operator|| (const C_value  & rhs) const
{
    M_FATAL_IF_EQ (    A_type, E_type_string);
    M_FATAL_IF_EQ (rhs.A_type, E_type_string);

    return  A_flt || rhs.A_flt;
}

//*****************************************************************************
// Comparison operators.
// Allowed :     string and     string
// Allowed :     msg    and     msg                only for== and !=
// Allowed : NOT string and NOT string
//*****************************************************************************
bool
C_value::operator== (const C_value  & rhs) const
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_str == rhs.A_str;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_integer == rhs.A_integer;
    }

    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator == could NOT be used for struct");
    }

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            return  A_integer == rhs.A_integer;
        else
            return  A_integer == rhs.A_flt;
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            return  A_flt == rhs.A_integer;
        else
            return  A_flt == rhs.A_flt;
    }
}

bool
C_value::operator!= (const C_value  & rhs) const
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_str != rhs.A_str;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_integer != rhs.A_integer;
    }

    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator != could NOT be used for struct");
    }

	if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            return  A_integer != rhs.A_integer;
        else
            return  A_integer != rhs.A_flt;
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            return  A_flt != rhs.A_integer;
        else
            return  A_flt != rhs.A_flt;
    }
}

bool
C_value::operator<= (const C_value  & rhs) const
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_str <= rhs.A_str;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator <= could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator <= could NOT be used for struct");
    }

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            return  A_integer <= rhs.A_integer;
        else
            return  A_integer <= rhs.A_flt;
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            return  A_flt <= rhs.A_integer;
        else
            return  A_flt <= rhs.A_flt;
    }
}

bool
C_value::operator>= (const C_value  & rhs) const
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_str >= rhs.A_str;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator >= could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator >= could NOT be used for struct");
    }

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            return  A_integer >= rhs.A_integer;
        else
            return  A_integer >= rhs.A_flt;
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            return  A_flt >= rhs.A_integer;
        else
            return  A_flt >= rhs.A_flt;
    }
}

bool
C_value::operator< (const C_value  & rhs) const
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_str < rhs.A_str;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator < could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator < could NOT be used for struct");
    }

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            return  A_integer < rhs.A_integer;
        else
            return  A_integer < rhs.A_flt;
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            return  A_flt < rhs.A_integer;
        else
            return  A_flt < rhs.A_flt;
    }
}

bool
C_value::operator> (const C_value  & rhs) const
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        return  A_str > rhs.A_str;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator > could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator > could NOT be used for struct");
    }

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            return  A_integer > rhs.A_integer;
        else
            return  A_integer > rhs.A_flt;
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            return  A_flt > rhs.A_integer;
        else
            return  A_flt > rhs.A_flt;
    }
}


//*****************************************************************************
// Provides OPERATOR from OPERATOR=
//*****************************************************************************

#define M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(OPERATOR)         \
C_value                                                        \
C_value::operator OPERATOR (const C_value  & rhs) const        \
{                                                              \
	C_value    left(*this);                                \
	left OPERATOR ## = rhs;                                \
	return  left;                                          \
}

//*****************************************************************************
// Bit operators.
// Allowed : integers
//*****************************************************************************
C_value &
C_value::operator&= (const C_value  & rhs)
{
    M_FATAL_IF_NE (    A_type, E_type_integer);
    M_FATAL_IF_NE (rhs.A_type, E_type_integer);

	M_COMPUTE_NEW_POSITION_OFFSET();

    *this = C_value (A_integer & rhs.A_integer);
	return  *this;
}

C_value &
C_value::operator|= (const C_value  & rhs)
{
    M_FATAL_IF_NE (    A_type, E_type_integer);
    M_FATAL_IF_NE (rhs.A_type, E_type_integer);

	M_COMPUTE_NEW_POSITION_OFFSET();

    *this = C_value (A_integer | rhs.A_integer);
	return  *this;
}

C_value &
C_value::operator^= (const C_value  & rhs)
{
    M_FATAL_IF_NE (    A_type, E_type_integer);
    M_FATAL_IF_NE (rhs.A_type, E_type_integer);

	M_COMPUTE_NEW_POSITION_OFFSET();

    *this = C_value (A_integer ^ rhs.A_integer);
	return  *this;
}

C_value &
C_value::operator<<= (const C_value  & rhs)
{
    M_FATAL_IF_NE (    A_type, E_type_integer);
    M_FATAL_IF_NE (rhs.A_type, E_type_integer);

	M_COMPUTE_NEW_POSITION_OFFSET();

    *this = C_value (A_integer << rhs.A_integer);
	return  *this;
}

C_value &
C_value::operator>>= (const C_value  & rhs)
{
    M_FATAL_IF_NE (    A_type, E_type_integer);
    M_FATAL_IF_NE (rhs.A_type, E_type_integer);

	M_COMPUTE_NEW_POSITION_OFFSET();

    *this = C_value (A_integer >> rhs.A_integer);
	return  *this;
}

M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(&)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(|)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(^)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(<<)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(>>)

//*****************************************************************************
// Arithmetic operators.
// Allowed : NOT string, NOT msg and NOT string, NOT msg
// + : string  and string  ok
// % : integer and integer only
//*****************************************************************************
C_value &
C_value::operator+= (const C_value  & rhs)
{
	M_COMPUTE_NEW_POSITION_OFFSET();

    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_IF_NE (A_type, rhs.A_type);
        *this = C_value (A_str + rhs.A_str);
		return  *this;
    }

    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator += could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator += could NOT be used for struct");
    }

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_integer + rhs.A_integer);
        else
            *this = C_value (A_integer + rhs.A_flt);
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_flt + rhs.A_integer);
        else
            *this = C_value (A_flt + rhs.A_flt);
    }

	return  *this;
}

C_value &
C_value::operator-= (const C_value  & rhs)
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_COMMENT("operator -= could NOT be used for string");
    }
    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator -= could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator -= could NOT be used for struct");
    }

	M_COMPUTE_NEW_POSITION_OFFSET();

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_integer - rhs.A_integer);
        else
            *this = C_value (A_integer - rhs.A_flt);
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_flt - rhs.A_integer);
        else
            *this = C_value (A_flt - rhs.A_flt);
    }

	return  *this;
}

C_value &
C_value::operator*= (const C_value  & rhs)
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_COMMENT("operator *= could NOT be used for string");
    }
    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator *= could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator *= could NOT be used for struct");
    }

	M_COMPUTE_NEW_POSITION_OFFSET();

    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_integer * rhs.A_integer);
        else
            *this = C_value (A_integer * rhs.A_flt);
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_flt * rhs.A_integer);
        else
            *this = C_value (A_flt * rhs.A_flt);
    }

	return  *this;
}

C_value &
C_value::operator/= (const C_value  & rhs)
{
    if ((    A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_COMMENT("operator /= could NOT be used for string");
    }
    if ((    A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
		M_FATAL_COMMENT("operator /= could NOT be used for msg");
    }
    if ((    A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator /= could NOT be used for struct");
    }

	M_COMPUTE_NEW_POSITION_OFFSET();
#if 0
    // All divide must give float ?
    *this = C_value (A_flt / rhs.A_flt);
#else
    if (A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_integer / rhs.A_integer);
        else
            *this = C_value (A_integer / rhs.A_flt);
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            *this = C_value (A_flt / rhs.A_integer);
        else
            *this = C_value (A_flt / rhs.A_flt);
    }
#endif
	return  *this;
}

C_value &
C_value::operator%= (const C_value  & rhs)
{
    M_FATAL_IF_NE (    A_type, E_type_integer);
    M_FATAL_IF_NE (rhs.A_type, E_type_integer);

	M_COMPUTE_NEW_POSITION_OFFSET();

    *this = C_value (A_integer % rhs.A_integer);

	return  *this;
}

M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(+)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(-)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(*)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(/)
M_OPERATOR_CONST_FROM_OPERATOR_EQUAL(%)

//*****************************************************************************
// Pow.
// Allowed : NOT string and NOT string
//*****************************************************************************
C_value
C_value::pow_internal(const C_value  & lhs, const C_value  & rhs)
{
    if ((lhs.A_type == E_type_string) ||
        (rhs.A_type == E_type_string))
    {
        M_FATAL_COMMENT("operator ** could NOT be used for string");
    }
    if ((lhs.A_type == E_type_msg) ||
        (rhs.A_type == E_type_msg))
    {
        M_FATAL_COMMENT("operator ** could NOT be used for msg");
    }
    if ((lhs.A_type == E_type_struct) ||
        (rhs.A_type == E_type_struct))
    {
        M_FATAL_COMMENT("operator ** could NOT be used for struct");
    }

	double  pow_val = 0.0;

    if (lhs.A_type == E_type_integer)
    {
        if (rhs.A_type == E_type_integer)
		{
			pow_val = ::pow (static_cast<double>(lhs.A_integer), static_cast<int>(rhs.A_integer));
			return  static_cast<long long>(pow_val);
		}
        else
            pow_val = ::pow (static_cast<double>(lhs.A_integer), rhs.A_flt);
    }
    else
    {
        if (rhs.A_type == E_type_integer)
            pow_val = ::pow (lhs.A_flt, static_cast<int>(rhs.A_integer));
        else
            pow_val = ::pow (lhs.A_flt, rhs.A_flt);
    }

	return  pow_val;
}
C_value
C_value::pow(const C_value  & lhs, const C_value  & rhs)
{
	C_value  val = pow_internal(lhs, rhs);

	M_COMPUTE_NEW_POSITION_OFFSET_RESULT_RHS(val., rhs.);

	return  val;
}

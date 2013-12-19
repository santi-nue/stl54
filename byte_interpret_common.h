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

#ifndef BYTE_INTERPRET_COMMON_H
#define BYTE_INTERPRET_COMMON_H

//*****************************************************************************
// Includes.
//*****************************************************************************

#include "precomp.h"

#include <cstring>
#include <climits>
#include <string>
#include <vector>
#include <strstream>
#ifndef non_existent_stringstream__
#include <sstream>
#endif
using namespace std;

#ifdef WIN32
#else
#include <sys/times.h>
#endif

#include "CT_debug_object_counter.h"


//****************************************************************************
//
//****************************************************************************

#ifndef LONGLONG_MAX
#define LONGLONG_MAX        ((long long)(~0ULL>>1))
#define LONGLONG_MIN        (-LONGLONG_MAX - 1)
#endif

#ifdef __lynx__
typedef unsigned int      uint;
#endif
#ifdef WIN32
typedef unsigned int      uint;
typedef unsigned int      ulong;
#else
#include <sys/types.h>
#endif

typedef          long long     longlong;
typedef unsigned long long    ulonglong;

typedef unsigned char   T_byte;

#define NULL_PTR  0

#ifdef FRHED
#define endl "\r\n"
#endif

#define M_WHERE  __FILE__, __LINE__

//****************************************************************************
// Exceptions
//****************************************************************************

enum E_byte_interpret_exception
{
	E_byte_interpret_exception_missing_data,
	E_byte_interpret_exception_loop_deep_break,
	E_byte_interpret_exception_loop_deep_continue,
	E_byte_interpret_exception_loop_break,
	E_byte_interpret_exception_loop_continue,
	E_byte_interpret_exception_return,
	E_byte_interpret_exception_fatal
};

class C_byte_interpret_exception
{
public:
	C_byte_interpret_exception(
						 const char                        * file_name,
							   int                           file_line,
							   E_byte_interpret_exception    bie,
                         const std::string                 & str)
		:A_file_name(file_name),
		 A_file_line(file_line),
		 A_bie(bie),
		 A_str(str)
	{
	}

	E_byte_interpret_exception    get_cause() const          { return  A_bie; }
	const std::string             get_explanation() const;

private:
	std::string                   A_file_name;
	int                           A_file_line;
	E_byte_interpret_exception    A_bie;
	std::string                   A_str;
};

//****************************************************************************
// M_FATAL_IF_ ...
//****************************************************************************

void    fatal_pb (const string  & lhs,
				  const string  & comp,
				  const string  & rhs,
				  const char    * file_name,
				  const size_t    file_line);

#define M_FATAL_IF_FALSE(assertion)                                           \
    if ((assertion) == false)                                                 \
    {                                                                         \
        fatal_pb (#assertion, "is", "false",                                  \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_IF_NE(lhs,rhs)                                                \
    if ((lhs) != rhs)                                                         \
    {                                                                         \
        fatal_pb (#lhs " (" + get_string (lhs) + ") ",                        \
                  "!=",                                                       \
                  #rhs " (" + get_string (rhs) + ") ",                        \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_IF_EQ(lhs,rhs)                                                \
    if ((lhs) == rhs)                                                         \
    {                                                                         \
        fatal_pb (#lhs " (" + get_string (lhs) + ") ",                        \
                  "==",                                                       \
                  #rhs " (" + get_string (rhs) + ") ",                        \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_IF_LT(lhs,rhs)                                                \
    if ((lhs) <  rhs)                                                         \
    {                                                                         \
        fatal_pb (#lhs " (" + get_string (lhs) + ") ",                        \
                  "<",                                                        \
                  #rhs " (" + get_string (rhs) + ") ",                        \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_IF_LE(lhs,rhs)                                                \
    if ((lhs) <= rhs)                                                         \
    {                                                                         \
        fatal_pb (#lhs " (" + get_string (lhs) + ") ",                        \
                  "<=",                                                       \
                  #rhs " (" + get_string (rhs) + ") ",                        \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_IF_GT(lhs,rhs)                                                \
    if ((lhs) >  rhs)                                                         \
    {                                                                         \
        fatal_pb (#lhs " (" + get_string (lhs) + ") ",                        \
                  ">",                                                        \
                  #rhs " (" + get_string (rhs) + ") ",                        \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_IF_GE(lhs,rhs)                                                \
    if ((lhs) >= rhs)                                                         \
    {                                                                         \
        fatal_pb (#lhs " (" + get_string (lhs) + ") ",                        \
                  ">=",                                                       \
                  #rhs " (" + get_string (rhs) + ") ",                        \
                  __FILE__, __LINE__);                                        \
    }

#define M_FATAL_COMMENT(comment)                                              \
    {                                                                         \
        std::ostrstream  M_FATAL_COMMENT_oss;                                 \
        M_FATAL_COMMENT_oss << comment << ends;                               \
    	fatal_pb ("FATAL :", "", M_FATAL_COMMENT_oss.str (),                  \
                  __FILE__, __LINE__);                                        \
        M_FATAL_COMMENT_oss.freeze (false);           /* avoid memory leak */ \
    }

#define M_ASSERT(assertion)     M_FATAL_IF_FALSE(assertion)
#define M_ASSERT_EQ(lhs,rhs)    M_FATAL_IF_NE(lhs,rhs)
#define M_ASSERT_NE(lhs,rhs)    M_FATAL_IF_EQ(lhs,rhs)

//****************************************************************************
// Traces
//****************************************************************************

#define M_STATE_ENTER(function_name,OSTREAM_OUTPUT_EXPR)                      \
	C_trace    M_trace(function_name);                                        \
	M_STATE_base (" ", "Enter", function_name << " " << OSTREAM_OUTPUT_EXPR)

#define M_STATE_LEAVE(OSTREAM_OUTPUT_EXPR)                      \
	M_STATE_base (" ", "Leave", M_trace.A_function_name << " " << OSTREAM_OUTPUT_EXPR);  \
	M_trace.leave_trace_done()


#define M_STATE_DEBUG(OSTREAM_OUTPUT_EXPR)                                    \
	M_STATE_base (" ", "     ", OSTREAM_OUTPUT_EXPR)
#define M_STATE_INFO(OSTREAM_OUTPUT_EXPR)                                     \
	M_STATE_base ("i", "     ", OSTREAM_OUTPUT_EXPR)
#define M_STATE_WARNING(OSTREAM_OUTPUT_EXPR)                                  \
	M_STATE_print ("W", "     ", OSTREAM_OUTPUT_EXPR)
#define M_STATE_ERROR(OSTREAM_OUTPUT_EXPR)                                    \
	M_STATE_print ("E", "     ", OSTREAM_OUTPUT_EXPR)
#define M_STATE_FATAL(OSTREAM_OUTPUT_EXPR)                                    \
	M_STATE_print ("F", "     ", OSTREAM_OUTPUT_EXPR)
#define M_STATE_ASSERT(OSTREAM_OUTPUT_EXPR)                                   \
	M_STATE_print ("A", "     ", OSTREAM_OUTPUT_EXPR)

#define M_STATE_base(PREFIX1,PREFIX2,OSTREAM_OUTPUT_EXPR)                     \
	if (G_debug_state_output)                                                 \
	{                                                                         \
		M_STATE_print (PREFIX1, PREFIX2, OSTREAM_OUTPUT_EXPR);                \
	}

#define M_STATE_print(PREFIX1,PREFIX2,OSTREAM_OUTPUT_EXPR)                    \
	C_trace::print_beginning_of_trace(get_state_ostream(), PREFIX1, PREFIX2); \
	get_state_ostream() << OSTREAM_OUTPUT_EXPR << endl << flush


class C_debug_set_temporary
{
public:
	C_debug_set_temporary(bool  debug);
	C_debug_set_temporary();
	~C_debug_set_temporary();

	void    set(bool  debug);
	void    unset();
	void    forget();

private:
	C_debug_set_temporary(const C_debug_set_temporary  &);

	bool    previous_value;
	bool    value_modified;
};

void    set_debug(bool    debug);
bool    get_debug();

// Use previous functions.
extern bool    G_debug_state_output;


ostream  & get_state_ostream();
ostream  & set_state_ostream(ostream  & new_state_ostream);


//#define M_STATE_print(OSTREAM_OUTPUT_EXPR)

struct C_trace
{
	C_trace(const char  * function_name);
	~C_trace();

	static
	void    print_beginning_of_trace(      ostream  & os,
									 const char     * prefix1,
									 const char     * prefix2);

	// Specifiy the leave trace has been done
	void    leave_trace_done();

	const char  * A_function_name;
	bool          A_must_do_leave_trace;
};

//****************************************************************************
// Misc
//****************************************************************************

enum E_return_code
{
    E_rc_ok,                       // everything is ok
    E_rc_not_integer,              // format ok BUT integer value NOK
    E_rc_not_found,                // format expected NOT found
	E_rc_multiple_value,           // is a mulitple value
    E_rc_not_ok                    // NOT ok
};

#define K_COMMENT_START  '#'

// NB: from foundation/csc_common
#define M_SIZE_TAB(array) (sizeof(array)/sizeof(array[0]))


//****************************************************************************
// get_string
//****************************************************************************
template <class TYPE>
//inline
string    get_string (const TYPE&   rhs)
{
  ostrstream    oss;
 
  oss << rhs << ends;

  string  tmp (oss.str ());
 
  oss.freeze (false);

  return  tmp;
}
#if 1
#include <cstdio>
// moins performant sur le tableau ?
template <>
inline
string    get_string (const long long&   rhs)
{
  char    str_tmp[99+1];
#ifdef WIN32
  sprintf(str_tmp, "%I64d", rhs);
#else
  sprintf(str_tmp, "%lld", rhs);
#endif
  return  str_tmp;
}
#endif
//****************************************************************************
// find : Frhed did not succeed to find it !
//****************************************************************************
#if 0
#ifdef FRHED
template <class T_iter,
          class T_value>
inline
T_iter     find (      T_iter     _First,
                       T_iter     _Last,
                 const T_value  & _Val)
{
	for (; _First != _Last; ++_First)
		if (*_First == _Val)
			break;
	return (_First);
}
#endif
#endif
//****************************************************************************
// rfind : reverse find
//****************************************************************************
template <class T_iter,
          class T_value>
inline
T_iter    rfind (const T_iter     begin,
                 const T_iter     end,
                 const T_value  & value)
{
    if (begin == end)
        return  end;

    T_iter  current = end;

    do
    {
        --current;

        if (*current == value)
            return  current;

    } while (current != begin);

    return  end;
}

//*****************************************************************************
// get_files_in_dir
//-----------------------------------------------------------------------------
// search into the directory <dir_name> files named
//  <begin_file_name>*<end_file_name>
//  and put the name of the files found into file_names vector.
// dir_name is prepend to the file name if full_name_required is set.
// returns 0 if no error
//*****************************************************************************

int   get_files_in_dir (const string          & dir_name,
						const string          & begin_file_name,
						const string          & end_file_name,
						      vector<string>  & file_names,
						const bool              full_name_required);


#endif /* BYTE_INTERPRET_COMMON_H */

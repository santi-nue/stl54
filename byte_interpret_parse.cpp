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

#ifndef __lynx__
#include <sys/stat.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <strstream>
using namespace std;

#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"


// must be called for each new line seen
void    build_types_context_line_new (const istream  & is);


//*****************************************************************************
// skip_blanks  including new lines
//*****************************************************************************

void    skip_blanks (istream  & is)
{
    // While istream is ok ...
	while (is.good())
	{
		// NB: do NOT use unget or putback (bug on VC++ 2003/2005/2008)
		const istream::int_type  next_char = is.peek();

		if (isspace(next_char) == 0)
		{
			// This is not a space (blank, tab, \r, \n ...)
			break;
		}

		if (next_char == '\n')
		{
			// new line notification
			build_types_context_line_new(is);
		}

		is.get();
	}
}

//*****************************************************************************
// skip_blanks_and_comments
//*****************************************************************************

void    skip_blanks_and_comments (istream  & is)
{
    // While istream is ok ...
	while (is.good())
	{
		// skip blanks including new lines
		skip_blanks(is);

		// NB: do NOT use unget or putback (bug on VC++ 2003/2005/2008)
		const istream::int_type  next_char = is.peek();

		if (next_char != K_COMMENT_START)
		{
			// This is not a comment (and not a space).
			break;
		}

		// This is a comment.
		// Skip the end of the line.
	    string  tmp;
		getline (is, tmp);

		// new line notification
		build_types_context_line_new(is);
	}
}

//*****************************************************************************
// skip_line
//*****************************************************************************

void    skip_line (istream  & is)
{
    // If istream is ok ...
	if (is.good())
	{
		// Skip the end of the line.
	    string  tmp;
		getline (is, tmp);

		// new line notification
		build_types_context_line_new(is);
	}
}

//*****************************************************************************
// is_istream_empty
//*****************************************************************************
bool    is_istream_empty (istream &         is)
{
    skip_blanks_and_comments (is);
    return  is.eof () || is.fail ();
}

// ****************************************************************************
//    Name      : mod_replace_all
//!@n Role      : modify the string : all occurences of <replace_old>
//!@n              are replaced by <replace_new>
//!@n Important : Avoid possible infinite loop
// ****************************************************************************

void       mod_replace_all (string  & value_mod,
                      const string  & replace_old,
                      const string  & replace_new)
{
    string::size_type  pos_found;
    string::size_type  pos_search = 0;
    while ((pos_found = value_mod.find (replace_old,
                                        pos_search)) != string::npos)
    {
        value_mod.replace (pos_found, replace_old.size (), replace_new);
        pos_search = pos_found + replace_new.size ();
    }
}

string     get_replace_all (      string    str_copy,
                            const string  & replace_old,
                            const string  & replace_new)
{
    mod_replace_all (str_copy, replace_old, replace_new);
    return  str_copy;
}

//*****************************************************************************
// remove_word_limits *********************************************************
//*****************************************************************************

void    remove_word_limits (string  & str,
                      const char      limit_left,
                      const char      limit_right)
{
    if ((str[0] == limit_left) &&
        (str.size () >= 2) &&
        (str[str.size () - 1] == limit_right))
    {
        str.erase (str.size () - 1);
        str.erase (0,1);
    }
}

//*****************************************************************************
// remove_string_limits *******************************************************
//*****************************************************************************

void    remove_string_limits (string  & str)
{
	const string::size_type  original_size = str.size ();

    remove_word_limits (str, '"', '"');
    if (str.size () != original_size)
        return;

    remove_word_limits (str, '\'', '\'');
}

//*****************************************************************************
// remove_spaces **************************************************************
// space : blank, tab, new line ...
//*****************************************************************************
#if 0
void    remove_spaces (string  & str)
{
    istrstream    is (str.c_str ());

    string    result;
    string    tmp;
    while ((is >> ws) && (is >> tmp))
    {
        result += tmp;
    }

    str = result;
}
#endif
//*****************************************************************************
// is_separator
//*****************************************************************************
// A separator is anything which could not be used into an identifier or an
//  integer/float value.
// Anything else : letters numbers _ : .
//*****************************************************************************
bool    is_separator (const char   c)
{
    return  (isalnum (c) == 0) && (c != '_') && (c != ':') && (c != '.');
}
bool    is_open (const char   c)
{
    return  (c == '(') || (c == '[') || (c == '{');
}
bool    is_str_limit (const char   c)
{
    return  (c == '"') || (c == '\'');
}

//*****************************************************************************
// end_of_str :
// - '\0' means : space, tab, new_line, eof         and ;
// - any caracter
// returns true if ok (i.e. end_of_str found)
//
// ATTENTION: do NOT use unget or putback (bug on VC++ 2003/2005/2008)
//*****************************************************************************
bool    append_to_string_until (istream       & is,
                                string        & result,
                          const char            end_of_str,
                          const E_parser_cfg    parser_cfg);

#define M_GET_FROM_ISTREAM(THE_IS)                   \
	if (c_read == '\n')                              \
	{                                                \
		build_types_context_line_new(THE_IS);        \
	}                                                \
	THE_IS.get()

bool    append_to_string_until_base (istream       & is,
                                string        & result,
                          const char            end_of_str,
                          const E_parser_cfg    parser_cfg)
{
	M_STATE_ENTER ("append_to_string_until",
				   "result=" << result <<
				   " end_of_str=" << int(end_of_str));

    while (true)
    {
		const istream::int_type    c_read_int = is.peek();
        if (c_read_int < 0)
        {
            // Fin du istream.
            if (result.empty ())
                return  false;

            return  (end_of_str == '\0');
        }

		char    c_read = c_read_int;

		if (end_of_str == '\0')
		{
			if (isspace (c_read))
			{
				if (result.empty ())
				{
					// Token not already found.
					// Do NOT add the blank.
					M_GET_FROM_ISTREAM(is);
					continue;
				}

				if (parser_cfg & K_parser_cfg_manage_blanks)
				{
					// End of the token.
					// Not appended to the result string.
					M_GET_FROM_ISTREAM(is);
					return  true;
				}
			}

	        if ((parser_cfg & K_parser_cfg_manage_dot_comma) &&
				(c_read == ';'))       // 2009/01/18
		    {
				if (result.empty () == false)
				{
					// End of the token.
					// Not appended to the result string.
					return  true;
				}
				else
				{
					// This is the token.
					result = c_read;
					M_GET_FROM_ISTREAM(is);
					return  true;
				}
			}

	        if (parser_cfg & K_parser_cfg_manage_comma)
	        {
				if (c_read == ',')
				{
					// End of parameter.
					// , is removed.
					M_GET_FROM_ISTREAM(is);
					return  true;
				}
			}

			if ((parser_cfg == K_parser_cfg_C) &&
		        (result.empty () == false))
	        {
				if (c_read != '[')  // 2010/07/15 to avoid split of an_array[idx]
				{
 					if (is_open (c_read))
					{
						// Supposed to be if( or while( ...
						return  true;
					}
					if ((is_separator (c_read) != is_separator (result[0])) ||
						(result[0] == '('))
					{
						return  true;
					}
				}
			}

			if ((parser_cfg & K_parser_cfg_ignore_comments_sharp) &&
				(c_read == K_COMMENT_START) &&
				(result.empty ()))					// comment must have a space before
			{
				// This is a comment.
#if 1
				// Ignore it.
				string  end_of_line;
				getline (is, end_of_line);
				build_types_context_line_new(is);
				continue;
#else
				// Get it.
				getline (is, result);
				build_types_context_line_new(is);
				return  true;
#endif
	        }

#if 1
			if ((parser_cfg != K_parser_cfg_parameters) &&    // 20090521 manage "..."+"..."
				(is_str_limit (c_read)) &&
				(result.empty () == false))
			{
				// A string begins (and so previous token is terminated).
				return  true;
			}
#else
			if ((is_str_limit (c_read)) &&
				(result.empty () == false))
			{
				// A string begins (and so previous token is terminated).
				return  true;
			}
#endif
		}

        // Transform any space into blank.
        if (isspace (c_read))
            c_read = ' ';

        result += c_read;
		M_GET_FROM_ISTREAM(is);

        if (c_read == end_of_str)
        {
            return  true;
        }

#define M_BEGIN_END(open_char,close_char)                                     \
        else if (c_read == open_char)                                         \
        {                                                                     \
            if (append_to_string_until (is, result, close_char,               \
                                        parser_cfg) == false)                 \
                return  false;                                                \
        }

// 2009/02/21 : some time before, I had removed the 2 last lines
// without -> bug on "1"+"2" which gives "1"+ and "2" intead of "1" + and "2"
// with    -> bug on fatal ("Unknow msg identifier (%d)", Type);
// Seems ok with supplementary test on K_parser_cfg_manage_comma
#define M_BEGIN_END_STR(open_close_char)                                      \
        else if (c_read == open_close_char)                                   \
        {                                                                     \
            if (append_to_string_until (is, result, open_close_char,          \
                                        parser_cfg) == false)                 \
                return  false;                                                \
            if ((end_of_str == '\0') && ((parser_cfg & K_parser_cfg_manage_comma) == 0))      \
                return  true;                                                 \
        }
//        }

        if (is_str_limit (end_of_str)) { }
        M_BEGIN_END ('(', ')')
        M_BEGIN_END ('[', ']')
        M_BEGIN_END_STR ('"')
        M_BEGIN_END_STR ('\'')
//         M_BEGIN_END ('{', '}')
        else if ((c_read == '{') &&
                 ((end_of_str != '\0') || (result.size () > 1)))
        {
            append_to_string_until (is, result, '}', parser_cfg);
        }
    }
}

//*****************************************************************************
// end_of_str :
// - '\0' means : space, tab, new_line, eof         and ;
// - any caracter
// returns true if ok (i.e. end_of_str found)
//*****************************************************************************
bool    append_to_string_until (istream       & is,
                                string        & result,
                          const char            end_of_str,
                          const E_parser_cfg    parser_cfg)
{
	const bool  ok = append_to_string_until_base(is, result, end_of_str, parser_cfg);

	if (ok && (result != ""))
	{
		// Remove all blanks at the end.
		string::size_type  idx = result.size() - 1;
		while (idx >=0)
		{
			if (isspace(result[idx]) == 0)
			{
				++idx;
				if (idx < result.size())
				{
					result.erase(idx);
				}
				break;
			}

			--idx;
		}
	}

	return  ok;
}

//*****************************************************************************
//*****************************************************************************
bool       read_token_word_cplx (istream       & is,
                                 string        & str_result,
                           const E_parser_cfg    parser_cfg)
{
    str_result = "";
	skip_blanks_and_comments(is);
    if (is.good () == false)
        return  false;

    return  append_to_string_until (is, str_result, '\0', parser_cfg);
}

//*****************************************************************************
//*****************************************************************************
void    istream_to_words (istream         & is,
                          vector<string>  & words,
                    const E_parser_cfg      parser_cfg)
{
    string    word;
    while (read_token_word_cplx (is, word, parser_cfg))
    {
        words.push_back (word);
    }
    if (is.eof () == false)
    {
        M_FATAL_COMMENT ("NOT eof !");
    }
}

//*****************************************************************************
//*****************************************************************************
void    string_to_words (const string          & str,
                               vector<string>  & words,
                         const E_parser_cfg      parser_cfg)
{
    istrstream    is (str.c_str ());
    istream_to_words (is, words, parser_cfg);
}

//*****************************************************************************
// istream_hexa_space_to_frame ************************************************
// ----------------------------------------------------------------------------
// Format :
// <byte in hexa>  <byte in hexa>  <byte in hexa>  ...
//*****************************************************************************
void    istream_hexa_space_to_frame (istream        & is,
                                     T_byte_vector  & frame)
{
    while (is_istream_empty (is) == false)
    {
        // Get the hexa word.
        string    str_value_hexa;
        is >> str_value_hexa;

        // Convert to int value.
        int    value_hexa;
        sscanf (str_value_hexa.c_str (), "%x", &value_hexa);

        frame.push_back (value_hexa);
    }
}

//*****************************************************************************
// string_hexa_to_frame *******************************************************
// ----------------------------------------------------------------------------
// Format :
// <byte in hexa><byte in hexa><byte in hexa><byte in hexa>...
//*****************************************************************************
void    string_hexa_to_frame (const string         & str_value_hexa,
                                    T_byte_vector  & frame)
{
    M_ASSERT_EQ (str_value_hexa.size () % 2, 0);

    const char  * p_str_value_hexa_end = str_value_hexa.c_str () +
                                         str_value_hexa.size ();

    for (const char  * p_str_value_hexa  = str_value_hexa.c_str ();
                       p_str_value_hexa != p_str_value_hexa_end;
                       p_str_value_hexa += 2)
    {
        // Convert to int value.
        int    value_hexa;
        sscanf (p_str_value_hexa, "%02x", &value_hexa);

        frame.push_back (value_hexa);
    }
}

//*****************************************************************************
// istream_hexa_to_frame ******************************************************
// ----------------------------------------------------------------------------
// Format :
// <byte in hexa><byte in hexa>  <byte in hexa>  <byte in hexa>  ...
//*****************************************************************************
void    istream_hexa_to_frame (istream        & is,
                               T_byte_vector  & frame)
{
    while (is_istream_empty (is) == false)
    {
        // Get the hexa word.
        string    str_value_hexa;
        is >> str_value_hexa;

        // Parse the hexa word.
        string_hexa_to_frame (str_value_hexa, frame);
    }
}

//*****************************************************************************
// istream_hexa_dump_to_frame *************************************************
// ----------------------------------------------------------------------------
// Format :
// [<??> <??>] <byte in hexa><byte in hexa>  <byte in hexa>  <end_of_hexa>  ...
//*****************************************************************************
void    istream_hexa_dump_to_frame (
                                 istream        & is,
                           const int              nb_of_first_words_to_ignore,
                           const string         & end_of_hexa,
                                 T_byte_vector  & frame)
{
    while (is_istream_empty (is) == false)
    {
        // Get the line.
        string    str_line;
        getline (is, str_line);

        istrstream    is_line (str_line.c_str ());

        // Ignore the first words.
        for (int   idx = 0; idx < nb_of_first_words_to_ignore; ++idx)
        {
            string    to_ignore;
            is_line >> to_ignore;
        }

        while (true)
        {
            // Get the hexa word.
            string    str_value_hexa;
            is_line >> str_value_hexa;

            if (str_value_hexa == end_of_hexa)
                break;

            // Parse the hexa word.
            string_hexa_to_frame (str_value_hexa, frame);
        }
    }
}

//*****************************************************************************
// istream_to_frame ***********************************************************
//*****************************************************************************
// void    istream_to_frame (istream        & is,
//                           T_byte_vector  & frame)
// {
//     istream_hexa_space_to_frame (is, frame);
// }

//*****************************************************************************
// bin_file_to_frame **********************************************************
//*****************************************************************************
void    bin_file_to_frame (const string         & file_name,
                                 T_byte_vector  & frame)
{
	M_STATE_ENTER ("bin_file_to_frame", file_name);

    // Compute size of the file.
    size_t       size_file = 0;
    {
        struct stat  stat_data;
        int    return_code_stat = stat (file_name.c_str(),
                                        &stat_data);
        M_ASSERT_EQ (return_code_stat, 0);
        size_file = stat_data.st_size;
    }

    //
    frame.resize (size_file);

    // 
    FILE  * file = fopen (file_name.c_str(), "rb");
    M_ASSERT_NE (file, NULL_PTR);

    const size_t  nb_read = fread (&frame[0], size_file, 1, file);
    M_ASSERT_EQ (nb_read, 1);

    fclose (file);
}

//*****************************************************************************
// get_number *****************************************************************
//*****************************************************************************
// Currently accept leading spaces BUT there is no warranty. 
// string to integer accepts decimal, octal and hexa.
//*****************************************************************************

bool    get_number (const char*   word,
                          long*   P_number)
{
  if (P_number != NULL_PTR)
    *P_number = 0;

  if (strcmp (word,"") == 0)
    return  false;

  long    number = 0;
  char*   endptr = NULL_PTR;

  errno = 0;
  number = strtol (word,&endptr,0);

  if ((errno != 0) ||
      (endptr == NULL_PTR) ||
      (*endptr != '\0'))
    return  false;

  if (P_number != NULL_PTR)
    *P_number = number;

  return  true;
}

bool    get_number (const char*        word,
                          long long  & number)
{
	return  get_number(word, 0, number);
}

bool    get_number (const char*        word,
						  int          base,
                          long long  & number)
{
  number = 0;

  if (strcmp (word,"") == 0)
    return  false;

  char*   endptr = NULL_PTR;

  errno = 0;
#if defined WIN32
  number = _strtoi64 (word,&endptr,base);
#else
  number = strtoll (word,&endptr,base);
#endif

  if ((errno != 0) ||
      (endptr == NULL_PTR) ||
      (*endptr != '\0'))
    return  false;

  return  true;
}

bool    get_number (const char*        word,
                          double     & out_number)
{
    out_number = 0.0;

    if (strcmp (word,"") == 0)
        return  false;

    char*   endptr = NULL_PTR;

    errno = 0;
    out_number = strtod (word, &endptr);

    if ((errno != 0) ||
        (endptr == NULL_PTR) ||
        (*endptr != '\0'))
        return  false;

	// Zero leading is simply ignored by strtod -> wrong result value.
	{
		const char  * first_digit = word;
		while (isdigit(*first_digit) == 0)
		{
			++first_digit;
			if (*first_digit == '\0')
				break;
		}
		if (*first_digit == '0')
		{
			++first_digit;
			if (isdigit(*first_digit))
			{
				// Zero followed by a digit -> octal number, not a float number.
				return  false;
			}
		}
	}

	return  true;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <str_left without separator><separator><str_right which could be anything>]
//*****************************************************************************
E_return_code  get_before_separator_after (const string  & str,
                                           const char      separator,
                                                 string  & str_left,
                                                 string  & str_right)
{
    // Searching for separator.
    string::size_type  idx = str.find (separator);

    if (idx == string::npos)
        return  E_rc_not_found;

    str_left  = str.substr (0, idx - 0);
    str_right = str.substr (idx + 1);

    return  E_rc_ok;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <str_left without separator><separator><str_right which could be anything>]
//*****************************************************************************
E_return_code  get_before_separator_after (const string  & str,
                                           const string  & separator,
                                                 string  & str_left,
                                                 string  & str_right)
{
    // Searching for separator.
    string::size_type  idx = str.find (separator);

    if (idx == string::npos)
        return  E_rc_not_found;

    str_left  = str.substr (0, idx - 0);
	str_right = str.substr (idx + separator.size());

    return  E_rc_ok;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <left_part><separator_left><value_str><separator_right>
// e.g with ( ) :
// really_anything_including(...)(...(...)...(...)...)
// <--- left_part --------------> <--- value_str --->
//*****************************************************************************
E_return_code    decompose_type_sep_value_sep (
                        const string     orig_type,
                        const char       separator_left,
                        const char       separator_right,
                              string   & left_part,
                              string   & value_str)
{
    // Searching for separator_right which must be at the end.
    string::size_type  idx_right = orig_type.rfind (separator_right);

    if (idx_right == string::npos)
        return  E_rc_not_found;

    if (idx_right != orig_type.size () - 1)
        return  E_rc_not_found;

	// Searching for separator_left.
	int                number_of_separator_left_to_find = 1;
    string::size_type  idx_left = idx_right-1;

	while (idx_left > 0)                         // do NOT accept empty left_part
	{
		if (orig_type[idx_left] == separator_left)
		{
			--number_of_separator_left_to_find;
			if (number_of_separator_left_to_find <= 0)
				break;
		}
		else if ((separator_left != separator_right) &&
				 (orig_type[idx_left] == separator_right))
		{
			++number_of_separator_left_to_find;
		}

		--idx_left;
	}

	if ((idx_left <= 0) || (number_of_separator_left_to_find != 0))
        return  E_rc_not_found;

	// Extract data part.
    // NB: orig_type is a copy and not a reference
	//      to avoid pb if left_part or value_str are the same reference
    left_part = orig_type.substr (0, idx_left - 0);
    value_str = orig_type.substr (idx_left+1, idx_right - (idx_left+1));

    return  E_rc_ok;
}

//*****************************************************************************
// ----------------------------------------------------------------------------
// Format :
// <left_part><separator_left><middle_part><separator_right><right_part>
// e.g with ( ) :
// really_anything_but_not(...)(...(...)...(...)...)really_anything_including(...)
// <--- left_part ------------> <--- middle_part -> <--- right_part ------------->
//*****************************************************************************
E_return_code    decompose_left_sep_middle_sep_right (
                        const string     orig_type,
                        const char       separator_left,
                        const char       separator_right,
                              string   & left_part,
                              string   & middle_part,
                              string   & right_part)
{
    // Searching for separator_left.
    string::size_type  idx_left = orig_type.find (separator_left);

    if (idx_left == string::npos)
        return  E_rc_not_found;

	// Searching for separator_right.
	int                number_of_separator_right_to_find = 1;
    string::size_type  idx_right = idx_left+1;

	while (idx_right < orig_type.size())
	{
		if (orig_type[idx_right] == separator_right)
		{
			--number_of_separator_right_to_find;
			if (number_of_separator_right_to_find <= 0)
				break;
		}
		else if ((separator_left != separator_right) &&
				 (orig_type[idx_right] == separator_left))
		{
			++number_of_separator_right_to_find;
		}

		++idx_right;
	}

	if ((idx_right >= orig_type.size()) || (number_of_separator_right_to_find != 0))
        return  E_rc_not_found;

	// Extract data part.
    // NB: orig_type is a copy and not a reference
	//      to avoid pb if left_part or middle_part or right_part are the same reference
    left_part   = orig_type.substr (0, idx_left - 0);
    middle_part = orig_type.substr (idx_left+1, idx_right - (idx_left+1));
    right_part  = orig_type.substr (idx_right+1);

    return  E_rc_ok;
}

//*****************************************************************************
// I definitively do not try :
// - to solve all the cases
// - to detect all the errors
//*****************************************************************************

void    promote_printf_string_to_64bits(string   & printf_string)
{
	const string::size_type  size_before = printf_string.size();

	string::size_type  idx_any = 0;
	while ((idx_any	= printf_string.find ('%', idx_any)) != string::npos)
	{
		++idx_any;

		if (printf_string[idx_any] == '%')
		{
			// This is not a printf format directive.
			++idx_any;
			continue;
		}

		while (isalpha(printf_string[idx_any]) == 0)
		{
			if (idx_any == printf_string.size())
			{
				M_STATE_ERROR("bad printf format string gives >" << printf_string << "< ");
				return;
			}

			++idx_any;
		}

		if (printf_string[idx_any] != 's')
		{
			if ((printf_string[idx_any] != 'f') &&
				(printf_string[idx_any] != 'F') &&
				(printf_string[idx_any] != 'e') &&
				(printf_string[idx_any] != 'E') &&
				(printf_string[idx_any] != 'g') &&
				(printf_string[idx_any] != 'G'))
			{
#ifdef WIN32
				printf_string.insert(idx_any, "I64");
				idx_any += 4;
#else
				printf_string.insert(idx_any, "ll");
				idx_any += 3;
#endif
			}
		}
	}

	if (printf_string.size() != size_before)
	{
		M_STATE_DEBUG("promote_printf_string_to_64bits gives >" << printf_string << "<");
	}
}

//*****************************************************************************
// Dump hexa.
//*****************************************************************************
static const long  K_NB_BYTES_PER_LINE = 16;
 
void    dump_buffer (      ostream &  os,
                     const void *     A_buffer,
                     const long       P_user_length)
{
  const unsigned char *   PA_buffer = static_cast<const unsigned char *>(A_buffer);
  long     P_length = P_user_length;
  long     I_count;
  long     I_limite_boucle;
  long     I_offset;
  char     line[256];
  char     line_part[256];

  I_offset = 0;

  while (P_length > 0)
  {
    if (P_length >= K_NB_BYTES_PER_LINE)
    {
         I_limite_boucle = K_NB_BYTES_PER_LINE;
    }
    else
    {
         I_limite_boucle = P_length;
    }

    // affichage offset
    sprintf (line, "%08lx : ", I_offset);

    // dump hexa
    for (I_count = 0 ; I_count < I_limite_boucle ; I_count++)
    {
      sprintf (line_part, "%02x ", PA_buffer[I_count]);
      strcat  (line, line_part);
    }
  
    // ecriture fin ligne dump hexa en blanc
    // si nb < K_NB_BYTES_PER_LINE
    while (I_count < K_NB_BYTES_PER_LINE)
    {
      sprintf (line_part, "   ");
      strcat  (line, line_part);
      I_count++;
    }
  
  
    // separation entre hexa et ascii
    sprintf (line_part, " - ");
    strcat  (line, line_part);
  
  
    // dump ascii
    for (I_count = 0 ; I_count < I_limite_boucle ; I_count++)
    {
      if (isprint(PA_buffer[I_count]))
      {
        sprintf (line_part, "%c", PA_buffer[I_count]);
      }
      else {
        sprintf (line_part, ".");
      }
      strcat (line, line_part);
    }
 
    assert(strlen(line) < 256);
 
    // ecriture fin ligne dump ascii en blanc
    // si nb < K_NB_BYTES_PER_LINE
    while (I_count < K_NB_BYTES_PER_LINE)
    {
      sprintf (line_part, " ");
      strcat (line, line_part);
      I_count++;
    }


    // print line in output file      
    assert(strlen(line) < 256);
    os << line << endl;

    PA_buffer += K_NB_BYTES_PER_LINE;
    P_length -= K_NB_BYTES_PER_LINE;
    I_offset += K_NB_BYTES_PER_LINE;
  }
}


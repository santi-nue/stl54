/*
 * Copyright 2005-2019 Olivier Aveline <wsgd@free.fr>
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

// Necessary for windows pre-compiled headers.
// At the end, does NOT work : missing functions at link time
// After other includes, does NOT work : does NOT compile (on "ifstream   ifs") 
#include "precomp.h"

#include <iostream>
#include <string>
using namespace std;

#include "byte_interpret_parse.h"
#include "byte_interpret_build_types_read_token.h"


//*****************************************************************************
// read_token_simple_word
// - ignore comments
// - ignore leading spaces
// - manage ;
//*****************************************************************************

bool       read_token_simple_word (
                            istream       & is,
                            string        & str_result)
{
    str_result = "";

    skip_blanks_and_comments(is);
    if (is.good () == false)
        return  false;

    // ifstream unget/putback bug with VC++ 2003/2005/2008 :
    //  the char is NOT unget (i.e. putback into the stream) and the badbit is set 
    // The problem is NOT triggered by unget/putback
    //  but by the previous read operation on 
    //  a input string "<field_name>;"
    //  which ends at position 4096 (8192, ...) of the input file.
    // Microsoft says : conform to C++ standard
    // NB: this code is better since it is able to manage "<field_name>;<another word>"
    while (true)
    {
        const istream::int_type  peek_char = is.peek();
        if (peek_char < 0)
        {
            break;
        }
//		if (isspace(peek_char) || iscntrl(peek_char))
        if (isgraph(peek_char) == 0)
        {
            break;
        }
        if (peek_char == ';')
        {
            if (str_result.empty())
            {
                str_result.push_back(is.get());
            }
            break;
        }

        str_result.push_back(is.get());
    }

    return  true;
}

//*****************************************************************************
// read_token_key_word_specified
// - read a key word
// - fatal if != key_word_expected
//*****************************************************************************

void       read_token_key_word_specified (
                            istream       & is,
                      const string        & key_word_expected)
{
    string    str_result;

    if (read_token_simple_word(is, str_result) == false)
    {
        M_FATAL_COMMENT("Expecting " << key_word_expected << " and found nothing (end of file)");
    }

    if (str_result != key_word_expected)
    {
        M_FATAL_COMMENT("Expecting " << key_word_expected << " and found " << str_result);
    }
}

//*****************************************************************************
// read_token_key_word_specified
// - read a key word
// - fatal if != one of the key_word_expected
//*****************************************************************************

void       read_token_key_word_specified (
                            istream       & is,
                            string        & str_result,
                      const string        & key_word_expected_1,
                      const string        & key_word_expected_2)
{
    if (read_token_simple_word(is, str_result) == false)
    {
        M_FATAL_COMMENT("Expecting " << key_word_expected_1 <<
            " or " << key_word_expected_2 << " and found nothing (end of file)");
    }

    if ((str_result != key_word_expected_1) &&
        (str_result != key_word_expected_2))
    {
        M_FATAL_COMMENT("Expecting " << key_word_expected_1 <<
            " or " << key_word_expected_2 << " and found " << str_result);
    }
}

//*****************************************************************************
// read_token_key_word_specified
// - read a key word
// - fatal if != one of the key_word_expected
//*****************************************************************************

void       read_token_key_word_specified (
                            istream       & is,
                            string        & str_result,
                      const string        & key_word_expected_1,
                      const string        & key_word_expected_2,
                      const string        & key_word_expected_3)
{
    if (read_token_simple_word(is, str_result) == false)
    {
        M_FATAL_COMMENT("Expecting " << key_word_expected_1 <<
            " or " << key_word_expected_2 <<
            " or " << key_word_expected_3 << " and found nothing (end of file)");
    }

    if ((str_result != key_word_expected_1) &&
        (str_result != key_word_expected_2) &&
        (str_result != key_word_expected_3))
    {
        M_FATAL_COMMENT("Expecting " << key_word_expected_1 <<
            " or " << key_word_expected_2 <<
            " or " << key_word_expected_3 << " and found " << str_result);
    }
}

//*****************************************************************************
// read_token_end_of_statement
//*****************************************************************************

void       read_token_end_of_statement (
                            istream       & is)
{
    read_token_key_word_specified(is, ";");
}

//*****************************************************************************
// read_token_simple_word_check_unexpected
//*****************************************************************************

bool       read_token_simple_word_check_unexpected (
                            istream       & is,
                            string        & str_result,
                      const char          * P_unexpected_char)
{
    if (read_token_simple_word(is, str_result) == false)
        return  false;

    if (P_unexpected_char != NULL)
    {
        if (strpbrk(str_result.c_str(), P_unexpected_char) != NULL)
        {
            M_TRACE_ERROR("read_token_simple_word unexpected character into " << str_result);
            return  false;
        }
    }

    return  true;
}

//*****************************************************************************
// read_token_type_simple
//*****************************************************************************

bool       read_token_type_simple (
                            istream       & is,
                            string        & str_result)
{
    return  read_token_simple_word_check_unexpected(is, str_result, "{}()[]<>\"';");
}

//*****************************************************************************
// read_token_key_word
//*****************************************************************************

bool       read_token_key_word (
                            istream       & is,
                            string        & str_result)
{
    if (read_token_simple_word_check_unexpected(is, str_result, "}()[]<>\"';") == false)
        return  false;

    if (str_result == "{")
        return  true;

    if (strchr(str_result.c_str(), '{') != NULL)
    {
        M_TRACE_ERROR("read_token_simple_word unexpected character into " << str_result);
        return  false;
    }

    return  true;
}

//*****************************************************************************
// read_token_simple_word_or_string
//*****************************************************************************
// a simple word or "..." or '...'
extern
bool    is_str_limit (const char   c);

bool       read_token_simple_word_or_string (
                            istream       & is,
                            string        & str_result)
{
    str_result = "";

    skip_blanks_and_comments(is);
    if (is.good () == false)
        return  false;

    if (is_str_limit(is.peek ()))
    {
        return  read_token_word_cplx(is, str_result);
    }

    return  read_token_simple_word(is, str_result);
}

//*****************************************************************************
// read_token_function_name
// Accept "" and nil.
//*****************************************************************************

bool       read_token_function_name (
                            istream       & is,
                            string        & str_result)
{
    bool    result = read_token_simple_word(is, str_result);
    if (result == true)
    {
        if (str_result == "\"\"")
        {
            str_result = "";
        }
        else if (str_result == "nil")
        {
            str_result = "";
        }
    }

    return  result;
}

//*****************************************************************************
// read_token_field_name
// Accept "".
//*****************************************************************************

bool       read_token_field_name (
                            istream       & is,
                            string        & str_result)
{
//	bool    result = read_token_simple_word(is, str_result);   // NOT ok on ID{ext=a c};  or   ID{ext="a c"};
//	bool    result = read_token_type_complex(is, str_result);  // NOT ok on ID{ext=a c};  or   ID{ext="a c"};
    bool    result = read_token_left_any(is, str_result);
    
    if (result == true)
    {
        if (str_result == "\"\"")
        {
            str_result = "";
        }
    }

    return  result;
}

//*****************************************************************************
// read_token_parameters_vector
//*****************************************************************************

bool   read_token_parameters_vector(istream				  & is,
                                    vector<string>		  & fct_parameters_vector,
                                    const E_parser_cfg		parser_cfg)
{
    string    fct_parameters;
    bool      ret = read_token_parameters(is, fct_parameters, parser_cfg);

    if (ret == true)
    {
        if ((fct_parameters[0] != '(') ||
            (fct_parameters[fct_parameters.size() - 1] != ')'))
        {
            M_FATAL_COMMENT("Expecting ( and ) for function parameters " << fct_parameters);
        }

        // Remove ( and ).
        fct_parameters.erase(0, 1);
        fct_parameters.erase(fct_parameters.size() - 1);

        // Split on ,.
        string_to_words(fct_parameters, fct_parameters_vector, K_parser_cfg_parameters);
    }

    return ret;
}
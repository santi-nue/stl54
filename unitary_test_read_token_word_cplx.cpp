/*
 * Copyright 2005-2020 Olivier Aveline <wsgd@free.fr>
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

using namespace std;

#include "unitary_tests_tools.h"
#include "byte_interpret_parse.h"


//*****************************************************************************
// test_read_token_word_cplx
//*****************************************************************************

M_TEST_FCT(test_read_token_word_cplx)
{
    string    str_result;

    // Words and strings
    {
        istringstream  iss("word + string - 1234.45 * 0xdf / -067 % \"a string\" ");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "word");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "+");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "string");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "-");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "1234.45");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "*");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "0xdf");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "/");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "-067");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "%");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "\"a string\"");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), false);
        M_TEST_EQ(str_result, "");

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // Comments and words with blocks
    {
        istringstream  iss("word + # comment \n type( (al) lo ) type[idx - 3] type{d=bin} ");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "word");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "+");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "type( (al) lo )");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "type[idx - 3]");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "type{d=bin}");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), false);
        M_TEST_EQ(str_result, "");

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // ;
    {
        istringstream  iss("word + type( al;lo ) begin;end type[idx ;- 3] type{d=bi;n} ");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "word");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "+");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "type( al;lo )");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "begin");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, ";");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "end");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "type[idx ;- 3]");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
        M_TEST_EQ(str_result, "type{d=bi;n}");

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // K_parser_cfg_parameters permits to extract function parameters
    // E.g: <param1> , <param2> , ...
    {
        istringstream  iss("wo\"r,d\" + type,( al;lo ) begin,end type[idx ,;- 3] t,ype{d=b,i;n} ");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
        M_TEST_EQ(str_result, "wo\"r,d\" + type");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
        M_TEST_EQ(str_result, "( al;lo ) begin");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
        M_TEST_EQ(str_result, "end type[idx ,;- 3] t");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
        M_TEST_EQ(str_result, "ype{d=b,i;n}");

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }

    // K_parser_cfg_expression : expression until ;
    {
        istringstream  iss("print (...); lhs + rhs; a_value == 0 ? (((a_value * 2) / a_value) + 2) : 5 ; ");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
        M_TEST_EQ(str_result, "print (...)");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
        M_TEST_EQ(str_result, ";");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
        M_TEST_EQ(str_result, "lhs + rhs");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
        M_TEST_EQ(str_result, ";");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
        M_TEST_EQ(str_result, "a_value == 0 ? (((a_value * 2) / a_value) + 2) : 5");

        str_result = "";
        M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
        M_TEST_EQ(str_result, ";");

        str_result = "";
        iss >> str_result;
        M_TEST_EQ(str_result, "");
    }
}

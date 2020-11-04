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

void    string_to_words (const string          & str,
                               vector<string>  & words,
                         const E_parser_cfg      parser_cfg);


//*****************************************************************************
// test_string_to_words
//*****************************************************************************

M_TEST_FCT(test_string_to_words)
{
    {
        vector<string>    words;
        string_to_words("Hello world! \"How are you today ?\" Seems fine.", words);

        M_TEST_EQ(words.size(), 5);
        M_TEST_EQ(words[0], "Hello");
        M_TEST_EQ(words[1], "world!");
        M_TEST_EQ(words[2], "\"How are you today ?\"");
        M_TEST_EQ(words[3], "Seems");
        M_TEST_EQ(words[4], "fine.");
    }

    {
        vector<string>    words;
        string_to_words("  string(12)  str;  uint16{q=34.23}{max=1234.68}[number_of_integer]  int ;", words);

        M_TEST_EQ(words.size(), 6);
        M_TEST_EQ(words[0], "string(12)");
        M_TEST_EQ(words[1], "str");
        M_TEST_EQ(words[2], ";");
        M_TEST_EQ(words[3], "uint16{q=34.23}{max=1234.68}[number_of_integer]");
        M_TEST_EQ(words[4], "int");
        M_TEST_EQ(words[5], ";");
    }

    {
        vector<string>    words;
        string_to_words("  while (field < 12*3) { uint12  int ; }", words);

        M_TEST_EQ(words.size(), 7);
        M_TEST_EQ(words[0], "while");
        M_TEST_EQ(words[1], "(field < 12*3)");
        M_TEST_EQ(words[2], "{");
        M_TEST_EQ(words[3], "uint12");
        M_TEST_EQ(words[4], "int");
        M_TEST_EQ(words[5], ";");
        M_TEST_EQ(words[6], "}");
    }

    {
        vector<string>    words;
        string_to_words("  (field < 12*3)  ", words, K_parser_cfg_C);

        M_TEST_EQ(words.size(), 1);
        M_TEST_EQ(words[0], "(field < 12*3)");
    }

    {
        vector<string>    words;
        string_to_words("field < 12-3*(3.4 * 345)", words, K_parser_cfg_C);

        M_TEST_EQ(words.size(), 7);
        M_TEST_EQ(words[0], "field");
        M_TEST_EQ(words[1], "<");
        M_TEST_EQ(words[2], "12");
        M_TEST_EQ(words[3], "-");
        M_TEST_EQ(words[4], "3");
        M_TEST_EQ(words[5], "*");
        M_TEST_EQ(words[6], "(3.4 * 345)");
    }

    {
        vector<string>    words;
        string_to_words("\"Hello \" + \"world!\" == \"Hello world!\"", words, K_parser_cfg_C);

        M_TEST_EQ(words.size(), 5);
        M_TEST_EQ(words[0], "\"Hello \"");
        M_TEST_EQ(words[1], "+");
        M_TEST_EQ(words[2], "\"world!\"");
        M_TEST_EQ(words[3], "==");
        M_TEST_EQ(words[4], "\"Hello world!\"");
    }

    {
        vector<string>    words;
        string_to_words("\"Hello \"+\"world!\"==\"Hello world!\"", words, K_parser_cfg_C);

        M_TEST_EQ(words.size(), 5);
        M_TEST_EQ(words[0], "\"Hello \"");
        M_TEST_EQ(words[1], "+");
        M_TEST_EQ(words[2], "\"world!\"");
        M_TEST_EQ(words[3], "==");
        M_TEST_EQ(words[4], "\"Hello world!\"");
    }

    {
        vector<string>    words;
        string_to_words("  \"Unknow msg identifier (%d) or %s\",Type1,Type2  ", words, K_parser_cfg_parameters);

        M_TEST_EQ(words.size(), 3);
        M_TEST_EQ(words[0], "\"Unknow msg identifier (%d) or %s\"");
        M_TEST_EQ(words[1], "Type1");
        M_TEST_EQ(words[2], "Type2");
    }

    {
        vector<string>    words;
        string_to_words("  \"Unknow msg identifier (%d) or %s\"  ,  Type1  ,  Type2  ", words, K_parser_cfg_parameters);

        M_TEST_EQ(words.size(), 3);
        M_TEST_EQ(words[0], "\"Unknow msg identifier (%d) or %s\"");
        M_TEST_EQ(words[1], "Type1");
        M_TEST_EQ(words[2], "Type2");
    }

    {
        vector<string>    words;
        string_to_words("  12.3 * 2 + 17.1, \"par2\"  ", words, K_parser_cfg_parameters);

        M_TEST_EQ(words.size(), 2);
        M_TEST_EQ(words[0], "12.3 * 2 + 17.1");
        M_TEST_EQ(words[1], "\"par2\"");
    }

    {
        vector<string>    words;
        string_to_words("  12.3 * 2 + 17.1, \"par2\" + \"par1\"  ", words, K_parser_cfg_parameters);

        M_TEST_EQ(words.size(), 2);
        M_TEST_EQ(words[0], "12.3 * 2 + 17.1");
        M_TEST_EQ(words[1], "\"par2\" + \"par1\"");
    }
}

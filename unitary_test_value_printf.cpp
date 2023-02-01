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

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"

#include "C_value.h"
#include "byte_interpret_parse.h"


//*****************************************************************************
// test_value_printf
//*****************************************************************************

M_TEST_FCT(test_value_printf)
{
    C_value            value;
    vector<C_value>    values;
    string             format;

#define M_TEST_OK(FORMAT,VALUE,RESULT)                                 \
    values.clear();                                                    \
    values.push_back(C_value(VALUE));                                  \
    format = FORMAT;                                                   \
    promote_printf_string_to_64bits(format);                           \
    value = C_value::sprintf_values(format,values);                    \
    M_TEST_EQ(value.as_string(), RESULT)


    M_TEST_OK("%d",   123, "123");
    M_TEST_OK("0x%x", 123, "0x7b");
    M_TEST_OK("0%o",  123, "0173");
    M_TEST_OK("%s",   123, "123");
    M_TEST_OK("%f",   123.0, "123.000000");            // NB: do not care about precise format
    M_TEST_OK("%e",   123.0, "1.230000e+02");          // NB: do not care about precise format
    M_TEST_OK("%g",   123.0, "123");                   // NB: do not care about precise format
    M_TEST_OK("%a",   123.0, "0x1.ec00000000000p+6");  // NB: do not care about precise format

    M_TEST_OK("%d",   -123, "-123");
	//M_TEST_OK("0x%x", -123, "-0x7b");     // donne 0xffffffffffffff85
	//M_TEST_OK("0%o",  -123, "-0173");     // donne 01777777777777777777605

    M_TEST_OK("%s",   -123, "-123");
    M_TEST_OK("%f",   -123.0, "-123.000000");            // NB: do not care about precise format
    M_TEST_OK("%e",   -123.0, "-1.230000e+02");          // NB: do not care about precise format
    M_TEST_OK("%g",   -123.0, "-123");                   // NB: do not care about precise format
    M_TEST_OK("%a",   -123.0, "-0x1.ec00000000000p+6");  // NB: do not care about precise format

       
    M_TEST_OK("%f", -123.0, "-123.000000");              // NB: do not care about precise format

    M_TEST_OK("augmentation = %d%% (non consolide)", 123, "augmentation = 123% (non consolide)");
    M_TEST_OK("augmentation = %s%% (non consolide)", 123, "augmentation = 123% (non consolide)");

    M_TEST_OK("%d",  123456789101112    , "123456789101112");
    M_TEST_OK("%d",  1234567891011121314, "1234567891011121314");
    M_TEST_OK("%d",  -4427611715        , "-4427611715");



    M_TEST_OK("%05d", 123, "00123");
    M_TEST_OK("%05i", 123, "00123");
    M_TEST_OK("%05u", 123, "00123");
    M_TEST_OK("0%05o",  123, "000173");
    M_TEST_OK("0x%05x", 123, "0x0007b");
    M_TEST_OK("0x%05X", 123, "0x0007B");


#undef  M_TEST_OK
}

//*****************************************************************************
// test_same_result_printf
//*****************************************************************************

template<typename TYPE>
std::string to_string(const std::string format,
                      const std::string format_original,
                      const TYPE        value_c,
                      const std::string printed_value)
{
    std::ostringstream oss;
    oss << '"' << format << '"';
    if (format != format_original)
    {
        oss << " (was " << '"' << format_original << '"' << ")";
    }
    oss << ", " << value_c;
    oss << " --> " << printed_value;
    return oss.str();
}

template<typename TYPE>
void test_same_result_printf(const std::string format_param, const TYPE value_c)
{
    std::string format = format_param;
    promote_printf_string_to_64bits(format);

    char printf_value[1000 + 1];
    const std::string format_printf = sizeof(TYPE) >= 8 ? format : format_param;
    sprintf(printf_value, format_printf.c_str(), value_c);

    vector<C_value>    values;
    values.push_back(C_value(value_c));
    const auto printed_value = C_value::sprintf_values(format, values).as_string();

    cout << "  ";
    cout << "print " << to_string(format, format_param, value_c, printed_value);
    if (printed_value != printf_value)
    {
        cout << " != ";
        cout << "printf " << to_string(format_printf, format_param, value_c, printf_value);
    }
    cout << "\n";
    M_TEST_EQ(printed_value, printf_value);
}

M_TEST_FCT(test_value_printf_int)
{
    const char * formats[] = {
        "%d",
        "%05d",
        "%i",
        "%05i",
        "%u",
        "%05u",
        "0x%x",
        "0x%05x",
        "0%o",
        "0%05o",
    };

    for (std::string format : formats)
    {
        test_same_result_printf(format,         123);
        test_same_result_printf(format,         257);
        test_same_result_printf(format,       65539);
        test_same_result_printf(format,  4985123963);
        test_same_result_printf(format,         123LL);
        test_same_result_printf(format,         257LL);
        test_same_result_printf(format,       65539LL);
        test_same_result_printf(format,  4985123963LL);
        //test_same_result_printf(format,        -123);  // u x o are KO, ie output differ from printf
        //test_same_result_printf(format,        -257);  //  because C_value are always 64 bits
        //test_same_result_printf(format,      -65539);  // idem
        test_same_result_printf(format, -4985123963);
        test_same_result_printf(format,        -123LL);
        test_same_result_printf(format,        -257LL);
        test_same_result_printf(format,      -65539LL);
        test_same_result_printf(format, -4985123963LL);
    }

    // floats, invalid types for these formats !
    for (std::string format : formats)
    {
        test_same_result_printf(format, 123.1);
        test_same_result_printf(format, 257.2);
        test_same_result_printf(format, 65539.3);
        test_same_result_printf(format, 4985123963.4);
        test_same_result_printf(format, -123.5);
        test_same_result_printf(format, -257.6);
        test_same_result_printf(format, -65539.7);
        test_same_result_printf(format, -4985123963.8);
    }
}

M_TEST_FCT(test_value_printf_flt)
{
    const char * formats[] = {
        "%f",
        "%.2f",
        "%e",
        "%.3e",
        "%g",
        "%.4g",
        "%a",
    };

    for (std::string format : formats)
    {
        test_same_result_printf(format,         123.1);
        test_same_result_printf(format,         257.2);
        test_same_result_printf(format,       65539.3);
        test_same_result_printf(format,  4985123963.4);
        test_same_result_printf(format,        -123.5);
        test_same_result_printf(format,        -257.6);
        test_same_result_printf(format,      -65539.7);
        test_same_result_printf(format, -4985123963.8);
    }

    // integers, invalid types for these formats !
    for (std::string format : formats)
    {
        test_same_result_printf(format,         123);
        test_same_result_printf(format,         257);
        test_same_result_printf(format,       65539);
        test_same_result_printf(format,  4985123963);
        test_same_result_printf(format,         123LL);
        test_same_result_printf(format,         257LL);
        test_same_result_printf(format,       65539LL);
        test_same_result_printf(format,  4985123963LL);
        //test_same_result_printf(format,        -123);  // KO, ie output -nan differ from printf
        //test_same_result_printf(format,        -257);  // idem
        //test_same_result_printf(format,      -65539);  // idem
        test_same_result_printf(format, -4985123963);
        test_same_result_printf(format,        -123LL);
        test_same_result_printf(format,        -257LL);
        test_same_result_printf(format,      -65539LL);
        test_same_result_printf(format, -4985123963LL);
    }
}

M_TEST_FCT(test_value_printf_str)
{
    const char * formats[] = {
        "%s",
        "%5s",
        "%-5s",
        "%%",
    };

    for (std::string format : formats)
    {
        test_same_result_printf(format, "123");
        test_same_result_printf(format, "4985123963");
        test_same_result_printf(format, "abcdefghijk");
    }
}

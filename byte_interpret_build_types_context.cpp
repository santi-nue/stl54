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

// Necessary for windows pre-compiled headers.
// At the end, does NOT work : missing functions at link time
// After other includes, does NOT work : does NOT compile (on "ifstream   ifs") 
#include "precomp.h"

#include <string>
#include <vector>

using namespace std;

#include "byte_interpret_build_types_context.h"
#include "byte_interpret_common.h"


//*****************************************************************************
// T_build_types_context_file
//*****************************************************************************

struct T_build_types_context_file
{
    T_build_types_context_file(string    file_name_param)
        :file_name(file_name_param),
         P_is(NULL),
         file_line(-1)
    {
    }

    T_build_types_context_file(const string    & file_name_param,
                               const istream   & is_param)
        :file_name(file_name_param),
         P_is(&is_param),
         file_line(1)
    {
    }

          string     file_name;
    const istream  * P_is;

    int       file_line;

    string    previous_type;
    string    type_kind;
    string    type_name;
};

vector<T_build_types_context_file>  A_include_files;

T_build_types_context_file  & get_current_context()
{
    if (A_include_files.empty())
    {
//        M_FATAL_COMMENT("Bug in the software");
        A_include_files.push_back(T_build_types_context_file("Not known file or not a file"));
    }

    return  A_include_files.back();
}

//*****************************************************************************
// Types
//*****************************************************************************

void         build_types_context_type_kind_begin(const std::string  & type_kind)
{
    T_build_types_context_file  & context = get_current_context();

    if (context.type_kind != "")
    {
        M_FATAL_COMMENT("prev kind not ended");
    }
    context.type_kind = type_kind;
}
void         build_types_context_type_begin(const std::string  & type_name)
{
    T_build_types_context_file  & context = get_current_context();

    if (context.type_name != "")
    {
        M_FATAL_COMMENT("prev type_name not ended");
    }
    context.type_name = type_name;
}
void         build_types_context_type_end  (const std::string  & type_name)
{
    T_build_types_context_file  & context = get_current_context();

    if (type_name != context.type_name)
    {
        M_FATAL_COMMENT("Ended type_name does not match last type_name");
    }
    context.previous_type = context.type_kind + " " + context.type_name;
    context.type_name = "";
}
void         build_types_context_type_kind_end(const std::string  & type_kind)
{
    T_build_types_context_file  & context = get_current_context();

    if (type_kind != context.type_kind)
    {
        M_FATAL_COMMENT("Ended type_kind does not match last type_kind");
    }
    context.type_kind = "";
}
std::string  build_types_context_type_where()
{
    T_build_types_context_file  & context = get_current_context();

    string  result;

    if ((context.type_kind == "") || (context.type_name == ""))
    {
        if (context.previous_type == "enum1 bool1")    // automatically created type
        {
            result += "At beginning of type definitions.\n";
        }
        else if (context.previous_type == "")
        {
            result += "At beginning of file.\n";
        }
        else
        {
            result += "After definition of " + context.previous_type + ".\n";
        }
    }
    if (context.type_kind != "")
    {
        if (context.type_name == "")
        {
            result += "Starting definition of " + context.type_kind + ".\n";
        }
        else
        {
            result += "Into definition of " + context.type_kind + " " + context.type_name + ".\n";
        }
    }

    return  result;
}

//*****************************************************************************
// Include files
//*****************************************************************************

void    build_types_context_include_file_open(const std::string  & file_name)
{
    A_include_files.push_back(T_build_types_context_file(file_name));
}

void    build_types_context_include_file_open(const std::string  & file_name,
                                              const istream      & is)
{
    A_include_files.push_back(T_build_types_context_file(file_name, is));
}

void    build_types_context_include_file_close(const std::string  & file_name)
{
    if (A_include_files.empty())
    {
        M_FATAL_COMMENT("No opened include files specified.");
    }
    if (file_name != A_include_files.back().file_name)
    {
        M_FATAL_COMMENT("Closed include file does not match last opened include file");
    }

    A_include_files.pop_back();
}

void         build_types_context_line_new ()
{
    if (A_include_files.empty())
    {
        // It is possible
        return;
    }

    A_include_files.back().file_line++;
}
void         build_types_context_line_new (const istream  & is)
{
    if (A_include_files.empty())
    {
        // It is possible
        return;
    }

    if (A_include_files.back().P_is == &is)
    {
        A_include_files.back().file_line++;
    }
}

string  build_types_context_include_file_where()
{
    string     result;

    if (A_include_files.empty())
    {
        return  result;
    }

    result += "In file ";
    result += A_include_files.back().file_name;

    for (int   idx = static_cast<int>(A_include_files.size())-2; idx >= 0; --idx)
    {
        result += ",\n";
        result += " included in file ";
        result += A_include_files[idx].file_name;
    }
    result += " :\n";

    if (A_include_files.back().file_line > 0)
    {
        result += "\n";
        result += "At line " + get_string(A_include_files.back().file_line) + ".\n";
    }

    return  result;
}

//****************************************************************************
// 
//****************************************************************************

void         build_types_context_reset()
{
    A_include_files.clear();
}

string    build_types_context_where()
{
    string     result;

    if (A_include_files.empty())
    {
        // build_types not already started
        return  result;
    }

    result += "In file ";
    result += A_include_files.back().file_name;

    for (int   idx = static_cast<int>(A_include_files.size())-2; idx >= 0; --idx)
    {
        result += ",\n";
        result += " included in file ";
        result += A_include_files[idx].file_name;
    }
    result += " :\n";
    result += "\n";
    result += build_types_context_type_where();

    if (A_include_files.back().file_line > 0)
    {
        result += "At line " + get_string(A_include_files.back().file_line) + ".\n";
    }

    return  result;
}


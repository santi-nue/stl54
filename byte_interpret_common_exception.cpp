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

// ****************************************************************************
// Includes.
// ****************************************************************************

#include "precomp.h"
#include "byte_interpret_common_base.h"
#include "byte_interpret_common_exception.h"

//****************************************************************************
// C_byte_interpret_exception
//****************************************************************************

C_byte_interpret_exception::C_byte_interpret_exception(
                      const char                        * file_name,
                            int                           file_line,
                            E_byte_interpret_exception    bie,
                      const std::string                 & str)
    :std::exception(),
    A_file_name(file_name),
    A_file_line(file_line),
    A_bie(bie),
    A_str(str)
{
    A_explanation = get_explanation();
}

const char*
C_byte_interpret_exception::what() const throw()
{
    return  A_explanation.c_str();
}

const std::string 
C_byte_interpret_exception::get_explanation() const
{
    string  debug_str;
    if (A_file_name != "")
    {
        debug_str += " at " + A_file_name + " line=" + get_string(A_file_line);
    }
    return  A_str + debug_str;
}


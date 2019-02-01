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

#include "precomp.h"
#include "T_interpret_byte_order.h"
#include "byte_interpret_common.h"
using namespace std;


//*****************************************************************************
// NB: byte order     inverted : intel,    little endian
//     byte order NOT inverted : motorola, big    endian, network
//*****************************************************************************

bool    is_host_byte_order_inverted ()
{
    const short  value = 1;
    return  (*(const char*)&value) != 0;
}

//*****************************************************************************
// T_interpret_byte_order
//*****************************************************************************
bool    T_interpret_byte_order::A_is_host_byte_order_inverted = ::is_host_byte_order_inverted();

T_interpret_byte_order::T_interpret_byte_order ()
    :A_must_invert_bytes (false)
{
}

bool
T_interpret_byte_order::is_little_endian() const
{
    if (A_is_host_byte_order_inverted)
    {
        if (A_must_invert_bytes)
            return  false;
        else
            return  true;
    }
    else
    {
        if (A_must_invert_bytes)
            return  true;
        else
            return  false;
    }
}

void
T_interpret_byte_order::set_big_endian()
{
    A_must_invert_bytes = A_is_host_byte_order_inverted;
}

void
T_interpret_byte_order::set_little_endian()
{
    A_must_invert_bytes = ! A_is_host_byte_order_inverted;
}

void
T_interpret_byte_order::set_as_host()
{
    A_must_invert_bytes = false;
}

void
T_interpret_byte_order::set_data_order(const std::string  & byte_order)
{
    if (byte_order == "big_endian")
    {
        set_big_endian();
    }
    else if (byte_order == "little_endian")
    {
        set_little_endian();
    }
    else if (byte_order == "as_host")
    {
        set_as_host();
    }
    else
    {
        M_FATAL_COMMENT("Bad byte_order " << byte_order);
    }
}

string
T_interpret_byte_order::get_data_order() const
{
    if (A_is_host_byte_order_inverted)
    {
        if (A_must_invert_bytes)
            return  "big_endian";
        else
            return  "little_endian";
    }
    else
    {
        if (A_must_invert_bytes)
            return  "little_endian";
        else
            return  "big_endian";
    }
}

string
T_interpret_byte_order::get_host_order()
{
    if (A_is_host_byte_order_inverted)
        return  "little_endian";
    else
        return  "big_endian";
}

//*****************************************************************************
// C_interpret_byte_order_set_temporary
//*****************************************************************************

C_interpret_byte_order_set_temporary::C_interpret_byte_order_set_temporary(
                                     T_interpret_byte_order  & interpret_byte_order,
                               const std::string             & byte_order)
    :A_interpret_byte_order(interpret_byte_order),
     A_byte_order(byte_order),
     A_previous_byte_order_is_little_endian(interpret_byte_order.is_little_endian())
{
    if (A_byte_order != "")
    {
        A_interpret_byte_order.set_data_order(A_byte_order);
    }
}

C_interpret_byte_order_set_temporary::~C_interpret_byte_order_set_temporary()
{
    if (A_byte_order != "")
    {
        if (A_previous_byte_order_is_little_endian)
        {
            A_interpret_byte_order.set_little_endian();
        }
        else
        {
            A_interpret_byte_order.set_big_endian();
        }
    }
}

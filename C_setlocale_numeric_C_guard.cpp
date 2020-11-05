/*
 * Copyright 2020 Olivier Aveline <wsgd@free.fr>
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

#include <cstdio>
#include <clocale>
#include <string.h>
#include <cstdlib>

using namespace std;

#include "C_setlocale_numeric_C_guard.h"


//*****************************************************************************
// C_setlocale_numeric_C_guard
//*****************************************************************************

C_setlocale_numeric_C_guard::C_setlocale_numeric_C_guard()
    : _locale_saved(setlocale(LC_NUMERIC, "C"))
{
    if (_locale_saved)
    {
        _locale_saved = strdup(_locale_saved);
    }
}

// restore the saved locale
C_setlocale_numeric_C_guard::~C_setlocale_numeric_C_guard()
{
    setlocale(LC_NUMERIC, _locale_saved);
    free(_locale_saved);
}

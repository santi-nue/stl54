/*
 * Copyright 2005-2008 Olivier Aveline <wsgd@free.fr>
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

#ifndef BYTE_INTERPRET_BUILD_TYPES_H
#define BYTE_INTERPRET_BUILD_TYPES_H

//****************************************************************************
// Includes.
//****************************************************************************

#include "T_type_definitions.h"

#include <string>

using namespace std;

//****************************************************************************
// byte_interpret_get_include_file_name
// byte_interpret_set_include_file_name
//****************************************************************************

void      byte_interpret_set_include_directory(const string  & dir_name);
string    byte_interpret_get_include_file_name(const string  & file_name);


//****************************************************************************
// build_types
//****************************************************************************

void      build_types (const string              & file_name,
                             T_type_definitions  & type_definitions);


//****************************************************************************
// build_types
// Returns the last extracted word (which has NOT been treated).
//****************************************************************************

string    build_types (istream             & is,
                       T_type_definitions  & type_definitions);


//****************************************************************************
// build_field
// Returns the last extracted word (which has NOT been treated).
//****************************************************************************

string    build_field (istream                           & is,
				 const T_type_definitions                & type_definitions,
                       string                              first_word,
                       T_field_type_name                 & field_type_name);


#endif /* BYTE_INTERPRET_BUILD_TYPES_H */

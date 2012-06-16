/*
 * Copyright 2005-2012 Olivier Aveline <wsgd@free.fr>
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

#ifndef BYTE_INTERPRET_BUILD_TYPES_FEELING_H
#define BYTE_INTERPRET_BUILD_TYPES_FEELING_H

//****************************************************************************
// Includes.
//****************************************************************************

#include "T_type_definitions.h"

#include <string>


//****************************************************************************
// build_feeling_types
// Build types specified into <sub_directory>/<main_file_name>.
// Returns the last extracted word (which has NOT been treated).
//****************************************************************************

bool      build_feeling_types (const std::string         & sub_directory,
							   const std::string         & main_file_name,
                                     T_type_definitions  & type_definitions);


#endif /* BYTE_INTERPRET_BUILD_TYPES_FEELING_H */

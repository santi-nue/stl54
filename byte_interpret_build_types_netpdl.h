/*
 * Copyright 2008-2009 Olivier Aveline <wsgd@free.fr>
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

#ifndef BYTE_INTERPRET_BUILD_TYPES_NETPDL_H
#define BYTE_INTERPRET_BUILD_TYPES_NETPDL_H

//****************************************************************************
// Includes.
//****************************************************************************

#include "T_type_definitions.h"

#include <string>


//****************************************************************************
// build_types
// Returns the last extracted word (which has NOT been treated).
//****************************************************************************

bool      build_netpdl_types (const std::string         & main_file_name,
                                    T_type_definitions  & type_definitions);


#endif /* BYTE_INTERPRET_BUILD_TYPES_NETPDL_H */

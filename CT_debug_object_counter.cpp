/*
 * Copyright 2012 Olivier Aveline <wsgd@free.fr>
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
#include "CT_debug_object_counter.h"

#include <sstream>
#include <map>

using namespace std;

//****************************************************************************
// C_debug_object_counter
//****************************************************************************

#ifdef USE_C_debug_object_counter
int  C_debug_object_counter::A_debug_counter = 0;

std::map<std::string,int>  S_C_debug_object_counter_map_class_counter;
std::map<std::string,int>  S_C_debug_object_counter_map_class_counter_old;
std::map<std::string,int>  S_C_debug_object_counter_map_class_sizeof;

C_debug_object_counter::C_debug_object_counter()
{
	++A_debug_counter;
}

C_debug_object_counter::C_debug_object_counter(const C_debug_object_counter  & )
{
	++A_debug_counter;
}

C_debug_object_counter::~C_debug_object_counter()
{
	--A_debug_counter;
}

std::string
C_debug_object_counter::get_debug_string()
{
	ostringstream  oss;
	oss << "nb_of_objects=" << A_debug_counter;

	for (map<std::string,int>::const_iterator  
		iter  = S_C_debug_object_counter_map_class_counter.begin();
		iter != S_C_debug_object_counter_map_class_counter.end();
	  ++iter)
	{
		const string  & name = iter->first;
		const int       counter_new = iter->second;
		const int       class_sizeof = S_C_debug_object_counter_map_class_sizeof[name];

		if (S_C_debug_object_counter_map_class_counter_old.empty())
		{
			oss << endl;
			oss << name << "=" << counter_new;
			oss << "    " << class_sizeof << "/" << class_sizeof*counter_new;
			continue;
		}

		int       counter_old = 0;
		{
			map<std::string,int>::const_iterator
				iter_old = S_C_debug_object_counter_map_class_counter_old.find(name);
			if (iter_old != S_C_debug_object_counter_map_class_counter_old.end())
			{
				counter_old = iter_old->second;
			}
		}

		if (counter_new != counter_old)
		{
			oss << endl;
			oss << name << "=" << counter_new;
			oss << "    ";
			if (counter_new > counter_old)
				oss << "+";
			oss << (counter_new - counter_old);
			oss << "    " << class_sizeof << "/" << class_sizeof*counter_new;
		}
	}

    S_C_debug_object_counter_map_class_counter_old = S_C_debug_object_counter_map_class_counter;

	return  oss.str();
}

void
C_debug_object_counter::incr(const char * class_name, const int  sizeof_class)
{
	++A_debug_counter;
	S_C_debug_object_counter_map_class_counter[class_name] += 1;
	S_C_debug_object_counter_map_class_sizeof[class_name] = sizeof_class;
}

void
C_debug_object_counter::decr(const char * class_name)
{
	--A_debug_counter;
	S_C_debug_object_counter_map_class_counter[class_name] -= 1;
}

#endif

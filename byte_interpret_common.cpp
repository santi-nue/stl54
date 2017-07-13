/*
 * Copyright 2005-2017 Olivier Aveline <wsgd@free.fr>
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
#include "byte_interpret_common.h"

#include <iostream>
#include <vector>
#include <cstdio>

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

// ****************************************************************************
// M_state_... trace output stream.
// ****************************************************************************

static ostream  * S_P_state_ostream = &cout;

ostream  & get_state_ostream()
{
	return  * S_P_state_ostream;
}

ostream  & set_state_ostream(ostream  & new_state_ostream)
{
	ostream  & old_state_ostream = * S_P_state_ostream;

	S_P_state_ostream = & new_state_ostream;

	return  old_state_ostream;
}

// ****************************************************************************
// M_state_... trace flag.
// ****************************************************************************
void    set_debug(E_debug_status    debug)
{
	C_trace::A_debug_status = debug;
}

E_debug_status  get_debug()
{
	return  C_trace::A_debug_status;
}


C_debug_set_temporary::C_debug_set_temporary(E_debug_status  debug)
	:previous_value(get_debug()),
	 value_modified(false)
{
	set(debug);
}

C_debug_set_temporary::C_debug_set_temporary()
	:previous_value(get_debug()),
	 value_modified(false)
{
}

C_debug_set_temporary::~C_debug_set_temporary()
{
	unset();
}

void
C_debug_set_temporary::set(E_debug_status  debug)
{
	set_debug(debug);
	value_modified = true;
}
void
C_debug_set_temporary::unset()
{
	if (value_modified)
	{
		set_debug(previous_value);
		value_modified = false;
	}
}
void
C_debug_set_temporary::forget()
{
	value_modified = false;
}

// ****************************************************************************
// M_state_... trace class.
// ****************************************************************************
E_debug_status  C_trace::A_debug_status = E_debug_status_OFF;

C_trace::C_trace(const char  * function_name)
    :A_function_name(function_name),
	 A_must_do_leave_trace(true)
{
}

C_trace::~C_trace()
{
	if (A_must_do_leave_trace)
	{
		M_STATE_base(" ", "Leave", A_function_name);
	}
}

//#ifdef WIN32
#include "T_perf_time.h"

bool         perf_time_val_last_trace_initialized = false;
T_perf_time  perf_time_val_last_trace;
//#endif

void
C_trace::print_beginning_of_trace(      ostream  & os,
								  const char     * prefix1,
								  const char     * prefix2)
{
	os << prefix1 << " ";

    if (A_debug_status == E_debug_status_ON)
    {
	    T_perf_time  timeb_val;

	    os << timeb_val << " ";

	    if ((perf_time_val_last_trace_initialized) &&
		    (timeb_val != perf_time_val_last_trace))
	    {
		    long    diff_time_ms = perf_time_diff_ms(timeb_val, perf_time_val_last_trace);
		    if (diff_time_ms > 1)
		    {
			    char  time_str[99+1];
			    sprintf(time_str, "%4ld ", diff_time_ms);
			    os << time_str;
		    }
		    else
		    {
			    os << "     ";
		    }
	    }
	    else
	    {
		    os << "     ";
	    }
	    perf_time_val_last_trace = timeb_val;
	    perf_time_val_last_trace_initialized = true;
    }

	os << prefix2 << " "; 
}

void
C_trace::leave_trace_done()
{
	A_must_do_leave_trace = false;
}

//*****************************************************************************
// fatal_pb
//*****************************************************************************

void    fatal_pb (const string  & lhs,
				  const string  & comp,
				  const string  & rhs,
				  const char    * file_name,
				  const size_t    file_line)
{
	ostrstream  oss;
	oss << lhs << " "
		<< comp << " "
		<< rhs << " "
        << "at " << file_name << "[" << file_line << "]";
	oss << ends;
	const char  * oss_str = oss.str ();
    oss.freeze (false);           /* avoid memory leak */

    M_STATE_FATAL(oss_str);
    throw  C_byte_interpret_exception(M_WHERE, E_byte_interpret_exception_fatal, oss_str);
}

//*****************************************************************************
// get_files_in_dir ***********************************************************
//*****************************************************************************
#ifdef WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

int   get_files_in_dir (const string          & dir_name,
						const string          & begin_file_name,
						const string          & end_file_name,
						      vector<string>  & file_names,
						const bool              full_name_required)
{
  M_STATE_ENTER ("get_files_in_dir",
                 "dir_name=" << dir_name <<
				 "  begin_file_name=" << begin_file_name <<
				 "  end_file_name=" << end_file_name);

#ifdef WIN32

  const string    filter = dir_name + "\\" + begin_file_name + "*" + end_file_name;

  HANDLE           hFindFile;
  WIN32_FIND_DATA  FindData;

  hFindFile = FindFirstFile (filter.c_str(),&FindData);
  if (hFindFile == INVALID_HANDLE_VALUE)
  {
    M_STATE_ERROR("filter " << filter << " not found");
    return  -1;
  }

  BOOL hOK = (hFindFile != INVALID_HANDLE_VALUE);
  while (hOK)
  {
    if ((full_name_required) && (dir_name != ""))
      file_names.push_back(dir_name + "\\" + FindData.cFileName);
	else
      file_names.push_back(FindData.cFileName);

	//-------------------------------------------------------
	// Recherche du fichier suivant
	//-------------------------------------------------------
	hOK = FindNextFile (hFindFile,&FindData);
  }

  FindClose (hFindFile);

#else

  const size_t  begin_file_name_length = begin_file_name.size();
  const size_t    end_file_name_length = end_file_name.size();

  DIR  * dir = opendir(dir_name.c_str());

  if (dir == NULL)
  {
    M_STATE_ERROR("dir " << dir_name << " not found");
    return  -1;
  }

  for (struct dirent * dp  = readdir(dir);
                       dp != NULL;
                       dp  = readdir(dir))
  {
#ifdef __linux__
    const size_t    d_namlen = strlen (dp->d_name);
#else
    const size_t    d_namlen = dp->d_namlen;
#endif

    if (d_namlen <= (begin_file_name_length + end_file_name_length))
      continue;

    if (strncmp (dp->d_name,
                 begin_file_name.c_str(),
                 begin_file_name_length) != 0)
      continue;

    if (strncmp (dp->d_name + d_namlen - end_file_name_length,
                 end_file_name.c_str(),
                 end_file_name_length) != 0)
      continue;

    const string    file_name (dp->d_name, 0, d_namlen);

    if ((full_name_required) && (dir_name != ""))
      file_names.push_back(dir_name + "/" + file_name);
	else
      file_names.push_back(file_name);
  }

  closedir(dir);

#endif

  return  0;
}

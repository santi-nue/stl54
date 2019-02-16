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
#include "byte_interpret_common.h"

#include <iostream>
#include <vector>
#include <cstdio>


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
    M_TRACE_ENTER("get_files_in_dir",
        "dir_name=" << dir_name <<
        "  begin_file_name=" << begin_file_name <<
        "  end_file_name=" << end_file_name);

#ifdef WIN32

    const string     filter = dir_name + "\\" + begin_file_name + "*" + end_file_name;

    WIN32_FIND_DATA  FindData;
    HANDLE           hFindFile = FindFirstFile(filter.c_str(), &FindData);
    if (hFindFile == INVALID_HANDLE_VALUE)
    {
        M_TRACE_ERROR("filter " << filter << " not found");
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
        hOK = FindNextFile(hFindFile, &FindData);
    }

    FindClose(hFindFile);

#else

    const size_t  begin_file_name_length = begin_file_name.size();
    const size_t    end_file_name_length = end_file_name.size();

    DIR  * dir = opendir(dir_name.c_str());

    if (dir == NULL)
    {
        M_TRACE_ERROR("dir " << dir_name << " not found");
        return  -1;
    }

    for (struct dirent * dp  = readdir(dir);
                         dp != NULL;
                         dp  = readdir(dir))
    {
#ifdef __linux__
        const size_t    d_namlen = strlen(dp->d_name);
#else
        const size_t    d_namlen = dp->d_namlen;
#endif

        if (d_namlen <= (begin_file_name_length + end_file_name_length))
            continue;

        if (strncmp(dp->d_name,
            begin_file_name.c_str(),
            begin_file_name_length) != 0)
            continue;

        if (strncmp(dp->d_name + d_namlen - end_file_name_length,
            end_file_name.c_str(),
            end_file_name_length) != 0)
            continue;

        const string    file_name(dp->d_name, 0, d_namlen);

        if ((full_name_required) && (dir_name != ""))
            file_names.push_back(dir_name + "/" + file_name);
        else
            file_names.push_back(file_name);
    }

    closedir(dir);

#endif

    return  0;
}

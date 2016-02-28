/*
 * Copyright 2005-2014 Olivier Aveline <wsgd@free.fr>
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

#include <cstdio>
#include <cerrno>
#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <strstream>
using namespace std;

#include "byte_interpret_common.h"
#include "byte_interpret_parse.h"
#include "byte_interpret.h"

//*****************************************************************************
// output_time_spent **********************************************************
//*****************************************************************************
bool  S_must_output_time = false;
void  output_time_spent (const char  * comment)
{
#ifndef WIN32
	struct tms       tms_var;
    static time_t    S_begin_time = times (&tms_var);
    static time_t    S_last_time = S_begin_time;

    time_t    new_time = times (&tms_var);

    if (S_must_output_time)
    {
        cout << "time spent (" << comment
             << ") = " << new_time - S_last_time << " cs / "
             << new_time - S_begin_time << " cs."
             << endl;
    }

    S_last_time = new_time;
#endif
}

//*****************************************************************************
// is_string_ended_by *********************************************************
//*****************************************************************************

bool    is_string_ended_by (const char  * file_name,
							const char  * suffix)
{
	const size_t    suffix_length = strlen (suffix);
	const size_t    file_name_length = strlen (file_name);

	if (file_name_length < suffix_length)
		return  false;

	const char  * file_name_suffix = file_name + file_name_length - suffix_length;

	return  strcmp (file_name_suffix, suffix) == 0;
}

//*****************************************************************************
// is_frame_desc_file_name ****************************************************
//*****************************************************************************

bool    is_frame_desc_file_name (const char  * file_name)
{
	return  is_string_ended_by (file_name, ".fdesc");
}

bool    is_frame_desc_file_name (const string  & file_name)
{
	return  is_frame_desc_file_name (file_name.c_str ());
}

//*****************************************************************************
// is_frame_bin_file_name *****************************************************
//*****************************************************************************

bool    is_frame_bin_file_name (const char  * file_name)
{
	return  is_string_ended_by (file_name, ".fbin");
}

bool    is_frame_bin_file_name (const string  & file_name)
{
	return  is_frame_bin_file_name (file_name.c_str ());
}

//*****************************************************************************
// is_frame_hexa_file_name ****************************************************
//*****************************************************************************

bool    is_frame_hexa_file_name (const char  * file_name)
{
	return  is_string_ended_by (file_name, ".fhex") ||
            is_string_ended_by (file_name, ".frame");
}

bool    is_frame_hexa_file_name (const string  & file_name)
{
	return  is_frame_hexa_file_name (file_name.c_str ());
}

//*****************************************************************************
// is_frame_hexa_dump_file_name ***********************************************
//*****************************************************************************

bool    is_frame_hexa_dump_file_name (const char  * file_name)
{
    return  strstr (file_name, ".fhexd") != NULL_PTR;
}

bool    is_frame_hexa_dump_file_name (const string  & file_name)
{
	return  is_frame_hexa_dump_file_name (file_name.c_str ());
}

//*****************************************************************************
// wait_for_any_operator_input ************************************************
//*****************************************************************************

void    wait_for_any_operator_input ()
{
	cout << "Enter to exit" << endl;
	char    do_not_care;
	cin.get (do_not_care);
}

//*****************************************************************************
// help_role ******************************************************************
//*****************************************************************************

void    help_role (const char  * prog_name)
{
    cerr << "Interpret binary data." << endl;
}

//*****************************************************************************
// usage **********************************************************************
//*****************************************************************************

void  usage (const char  * prog_name)
{
    ostream  & os = cerr;

    os << "usage: " << prog_name << 
" [ options ] <type description> <what must be read> <binary data to interpret>\n"
"\n";
    help_role (prog_name);

	os <<
"\n*** "
"Options are : \n"
"-help  : print this.\n"
"-trace : output traces.\n"
"-perf  : display execution times.\n"
"\n";

    os <<
"\n*** "
"<type description> <what must be read> could be specified :\n"
"- directly into the command line\n"
"- from stdin  if you specify -fdesc -\n"
"- from a file if you specify -fdesc <file_name> or <file_name>.fdesc\n"
"\n";
    os <<
"Look at http://wsgd.free.fr/fdesc_format.html for the type description syntax.\n"
"Or " << prog_name << "  -help_syntax\n" 
"\n";
    os <<
"\n*** "
"<binary data to interpret> could be specified :\n"
"- directly into the command line if you specify --\n"
"- from stdin                     if you specify -\n"
"- from a file                    if you specify :\n"
"  -frame_bin     <file_name> or <file_name>.fbin\n"
"  -frame_hex     <file_name> or <file_name>.fhex\n"
"  -frame_hexd... <file_name> or <file_name>.fhexd...\n"
"\n";

    os <<
"Format command line, stdin or file -frame_hex :  02E3 B7 C24F112c b2 7C ...\n"
"\n"
"Format file -frame_hexd1 :\n"
"<word1>  02E3 B7 C2 435D6F112c b2 7C\n"
"<word1>  435D6F112 02E3 B7 C2 412c b2 7C\n"
"\n"
"Format file -frame_hexd2- :\n"
"<word1> <word2>  02E3 B7 C2 435D6F112c b2 7C     - to ignore\n"
"<word1> <word2>  435D6F112 02E3 B7 C2 412c b2 7C - to ignore\n"
"\n"
"Format file -frame_hexd<num><end_of_hexa> :\n"
"<word1> ... <word<num>> 02E3 B7 C2 43512c b2 7C     <end_of_hexa> to ignore\n"
"<word1> ... <word<num>> 435D6F112 02E3 B7 C2 412c   <end_of_hexa> to ignore\n"
"\n";

    os <<
"\n*** "
"Very simple examples :\n"
"\n"
"> byte_interpret.exe  my_file.fdesc  -frame_bin  file_which_contains_binary_data\n"
"...\n"
"\n"
"> byte_interpret.exe  int32  toto ;  -- 00 01 02 03\n"
"toto = 50462976\n"
"\n"
"> byte_interpret.exe byte_order big_endian ; int32  toto ;  -- 00 01 02 03\n"
"toto = 66051\n"
"\n"
"> byte_interpret.exe byte_order big_endian ; float32 toto ;  -- 00 01 02 03\n"
"toto = 9.25572e-041\n"
#ifndef WIN32
"\n"
"NB: for Linux/Unix systems, ; must be changed to \\; or \";\" or \"int32  toto ;\"\n"
#endif
       << endl;

#ifdef WIN32
    // To avoid window closing.
	wait_for_any_operator_input ();
#endif
}

//*****************************************************************************
// main ***********************************************************************
//*****************************************************************************

int   main (int  argc, const char * const  argv[])
{
    output_time_spent ("begin");

    int   arg_idx = 1;

    if (arg_idx >= argc)
    {
        usage (argv[0]);
        return  2;
    }

    if (strcmp (argv[arg_idx], "-help_role") == 0)
    {
        help_role (argv[0]);
        return  0;
    }

    if (strcmp (argv[arg_idx], "-help") == 0)
    {
        usage (argv[0]);
        return  0;
    }

    if (strcmp (argv[arg_idx], "-trace") == 0)
    {
        set_debug (E_debug_status_ON);
        ++arg_idx;
    }

    if (strcmp (argv[arg_idx], "-perf") == 0)
    {
        S_must_output_time = true;
        ++arg_idx;
    }

    output_time_spent ("apres alias");

    if (arg_idx >= argc)
    {
        usage (argv[0]);
        return  2;
    }

    // Lecture des types.
    istream  * P_istream = NULL_PTR;
    string     types_and_commands;

	if (strcmp (argv[arg_idx], "-help_syntax") == 0)
	{
        ++arg_idx;
        P_istream = new istrstream ("print syntax ;");
	}
    else if ((strcmp (argv[arg_idx], "-fdesc") == 0) ||
        (is_frame_desc_file_name (argv[arg_idx])))
    {
        if (strcmp (argv[arg_idx], "-fdesc") == 0)
            ++arg_idx;

        const string    file_name (argv[arg_idx]);
        ++arg_idx;

        if ((file_name == "-") || (file_name == "stdin"))
            P_istream = &cin;
        else
		{
            P_istream = new ifstream (file_name.c_str ());

			const string::size_type  last_separator = file_name.find_last_of("/\\");
			if (last_separator != string::npos)
			{
				const string    directory = file_name.substr(0, last_separator);
				byte_interpret_set_include_directory(directory);
			}
		}
    }
    else
    {
        while (arg_idx < argc)
        {
            if (strcmp (argv[arg_idx], "--") == 0)
            {
                break;
            }
            if (strcmp (argv[arg_idx], "-") == 0)
            {
                break;
            }
            if ((strcmp (argv[arg_idx], "-frame_bin") == 0) ||
                (is_frame_bin_file_name (argv[arg_idx])))
            {
                break;
            }
            if ((strcmp (argv[arg_idx], "-frame") == 0) ||
                (strcmp (argv[arg_idx], "-frame_hex") == 0) ||
                (is_frame_hexa_file_name (argv[arg_idx])))
            {
                break;
            }
            if ((strncmp (argv[arg_idx], "-frame_hexd", 11) == 0) ||
                (is_frame_hexa_dump_file_name (argv[arg_idx])))
            {
                break;
            }

            types_and_commands += argv[arg_idx];
            types_and_commands += " ";

            ++arg_idx;
        }

        P_istream = new istrstream (types_and_commands.c_str ());
    }

    output_time_spent ("avant frame");

    T_byte_vector    byte_vector;

    if (arg_idx == argc)
    {
        cerr << "No data is specified." << endl;
    }
    else if (strcmp (argv[arg_idx], "-") == 0)
    {
        ++arg_idx;
        istream_hexa_to_frame (cin, byte_vector);
    }
    else if ((strcmp (argv[arg_idx], "-frame_bin") == 0) ||
             (is_frame_bin_file_name (argv[arg_idx])))
    {
        if (strcmp (argv[arg_idx], "-frame_bin") == 0)
            ++arg_idx;

        bin_file_to_frame (argv[arg_idx], byte_vector);
        ++arg_idx;
    }
    else if ((strcmp (argv[arg_idx], "-frame") == 0) ||
             (strcmp (argv[arg_idx], "-frame_hex") == 0) ||
             (is_frame_hexa_file_name (argv[arg_idx])))
    {
        if ((strcmp (argv[arg_idx], "-frame") == 0) ||
            (strcmp (argv[arg_idx], "-frame_hex") == 0))
            ++arg_idx;

        ifstream   ifs (argv[arg_idx]);
        istream_hexa_to_frame (ifs, byte_vector);
        ++arg_idx;
    }
    else if ((strncmp (argv[arg_idx], "-frame_hexd", 11) == 0) ||
             (is_frame_hexa_dump_file_name (argv[arg_idx])))
    {
        const char  * dump_arg_nb = argv[arg_idx] + 11;
        if (strncmp (argv[arg_idx], "-frame_hexd", 11) == 0)
            ++arg_idx;
        else
        {
            dump_arg_nb = strstr (argv[arg_idx], ".fhexd") + 6;
        }

        if (*dump_arg_nb == '\0')
        {
            usage (argv[0]);
            return  3;
        }
        if (isdigit (*dump_arg_nb) == 0)
        {
            usage (argv[0]);
            return  3;
        }
        const int  nb_of_first_words_to_ignore = *dump_arg_nb - '0';

        const char  * dump_arg_str = dump_arg_nb + 1;

        ifstream   ifs (argv[arg_idx]);
        istream_hexa_dump_to_frame (ifs,
                                    nb_of_first_words_to_ignore,
                                    dump_arg_str,
                                    byte_vector);
        ++arg_idx;
    }
    else
    {
        if (strcmp (argv[arg_idx], "--") == 0)
        {
            ++arg_idx;
        }

        while (arg_idx < argc)
        {
            // Convert to int value.
            string_hexa_to_frame (argv[arg_idx], byte_vector);
            ++arg_idx;
        }
    }

    output_time_spent ("apres frame");

    if (arg_idx != argc)
    {
        usage (argv[0]);
        return  4;
    }

	// Do not use cerr to avoid order problems between cout and cerr.
	ostream  & os_err = cout;

	const T_byte  * P_bytes = byte_vector.size () ? &byte_vector[0] : NULL;
    size_t          sizeof_bytes = byte_vector.size ();

    bool    result = build_types_and_interpret_bytes (P_bytes, sizeof_bytes,
                                                      *P_istream,
                                                      cout,
                                                      os_err);

    output_time_spent ("end");

    if (sizeof_bytes != 0)
    {
        os_err << sizeof_bytes << " bytes have NOT been read." << endl;
        return  6;
    }

    return  0;
}


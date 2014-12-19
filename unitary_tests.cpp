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

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"

#include "C_value.h"
#include "T_type_definitions.h"
#include "T_frame_data.h"
#include "T_frame_data_write.h"
#include "T_attribute_value.h"
#include "T_interpret_data.h"
#include "byte_interpret_parse.h"
#include "byte_interpret_builder.h"
#include "byte_interpret_compute_expression.h"
#include "byte_interpret.h"
#include "byte_interpret_build_types_read_token.h"
#include "T_expression.h"

void    build_types_begin (T_type_definitions  & type_definitions);

void    string_to_words (const string          & str,
                               vector<string>  & words,
                         const E_parser_cfg      parser_cfg);

bool    read_data (      T_frame_data  & in_out_frame_data,
                         void          * P_value_param,
                   const char          * TYPE_NAME,
                   const size_t          TYPE_BIT_SIZE,
                   const char          * TYPE_IMPL_STR,
                   const size_t          TYPE_IMPL_BIT_SIZE,
				   const bool            must_invert_bytes,
				   const bool            is_signed_integer);

//*****************************************************************************
//*****************************************************************************
#if 0
#include <time.h> 
#include <sys/times.h> 
void   text_unix_time()
{
	cout << "CLOCKS_PER_SEC = " << CLOCKS_PER_SEC << endl;  // gives 1000000
//	cout << "CLK_TCK        = " << CLK_TCK        << endl;
	cout << "clock() = " << clock() << endl;
	cout << "times() = " << times(NULL) << endl;
	sleep(1);
	cout << "clock() = " << clock() << endl;
	cout << "times() = " << times(NULL) << endl;
}
#endif

//*****************************************************************************
// test_bitfield_C
// Not a test, I just want to verify the position of the bits.
// a) From less significant bit to most significant bit
// b) take care of byte order
// Note that there is NO standard about this.
//*****************************************************************************

void    test_bitfield_C()
{
	struct {
		  unsigned field1  :4;
		  unsigned field2  :3;
		  unsigned field3  :1;
		  unsigned field4  :6;
		  unsigned field5  :2;
		  unsigned field6  :8;
		  unsigned field7  :8;
	} full_of_fields;

	memset(&full_of_fields, 0, sizeof(full_of_fields));
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field1 = 3;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field2 = 4;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field3 = 1;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field4 = 3;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field5 = 3;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field6 = 85;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	full_of_fields.field7 = 255;
	cout << "bitf = " << hex << *(unsigned *)&full_of_fields << endl;

	cout << dec;
}

//*****************************************************************************
// test_size
// Not a test.
//*****************************************************************************
C_value  string_count_cpp_to_fdesc(const string::size_type  cpp_count);

void    test_size()
{
#define M_OUT_SIZEOF(PARAM)    \
	cout << "sizeof(" << #PARAM << ") = " << sizeof(PARAM) << endl

	M_OUT_SIZEOF(void*);
	M_OUT_SIZEOF(int);
	M_OUT_SIZEOF(long);
	M_OUT_SIZEOF(long long);
	M_OUT_SIZEOF(double);
	M_OUT_SIZEOF(size_t);
	M_OUT_SIZEOF(string::size_type);
	M_OUT_SIZEOF(string);
	M_OUT_SIZEOF(C_value);
	M_OUT_SIZEOF(T_attribute_value);
	M_OUT_SIZEOF(T_interpret_value);

#define M_OUT_VAL(PARAM)    \
	cout << "value(" << #PARAM << ") = " << PARAM << endl

	M_OUT_VAL(string::npos);
	M_OUT_VAL(string_count_cpp_to_fdesc(string::npos));
}

//*****************************************************************************
// test_cast
// Not a test.
//*****************************************************************************

void    test_cast()
{
#define M_OUT_VAL(PARAM)    \
	cout << "value(" << #PARAM << ") = " << PARAM << endl

	unsigned short  valu  = 40000;
	short           vals  = (int)valu;
	unsigned short  valu2 = (unsigned int)vals;

	M_OUT_VAL(valu);
	M_OUT_VAL(vals);
	M_OUT_VAL(valu2);
}

//*****************************************************************************
// C_perf_chrono
//*****************************************************************************

#include "T_perf_time.h"

class C_perf_chrono
{
public:
	C_perf_chrono(	    T_interpret_data    & interpret_data,
				  const char                * in_input_string,
				  const size_t                in_sizeof_bytes);
	~C_perf_chrono();

	static void    end();

private:

	static void    output(
		const long          ms_diff_ctor,
		const int           nb_bytes,
		const string      & decode_function,
		const bool          debug,
		const string      & what);

	T_interpret_data    & A_interpret_data;
	const char  * A_input_string;
	const size_t  A_sizeof_bytes;
	T_perf_time   A_perf_time_ctor;

	static long   A_total_ms;
	static long   A_total_ms_decode;
	static long   A_total_ms_no_decode;
	static long   A_total_sizeof_bytes;
	static long   A_total_sizeof_bytes_decode;
	static long   A_total_sizeof_bytes_no_decode;
};

long    C_perf_chrono::A_total_ms = 0;
long    C_perf_chrono::A_total_ms_decode = 0;
long    C_perf_chrono::A_total_ms_no_decode = 0;
long    C_perf_chrono::A_total_sizeof_bytes = 0;
long    C_perf_chrono::A_total_sizeof_bytes_decode = 0;
long    C_perf_chrono::A_total_sizeof_bytes_no_decode = 0;

C_perf_chrono::C_perf_chrono(      T_interpret_data    & interpret_data,
							 const char                * in_input_string,
							 const size_t                in_sizeof_bytes)
	:A_interpret_data(interpret_data),
	 A_input_string(in_input_string),
	 A_sizeof_bytes(in_sizeof_bytes)
{
}

C_perf_chrono::~C_perf_chrono()
{
	T_perf_time  perf_time_val;

	const long          ms_diff_ctor = perf_time_diff_ms(perf_time_val, A_perf_time_ctor);
	T_interpret_data    interpret_data = A_interpret_data;
	const string      & decode_function = interpret_data.get_decode_function();
	const string        decode = decode_function.empty() ? "      " : "decode" ;

	if (get_debug() == false)
	{
		A_total_ms           += ms_diff_ctor;
		A_total_sizeof_bytes += A_sizeof_bytes;

		if (decode_function.empty())
		{
			A_total_ms_no_decode           += ms_diff_ctor;
			A_total_sizeof_bytes_no_decode += A_sizeof_bytes;
		}
		else
		{
			A_total_ms_decode           += ms_diff_ctor;
			A_total_sizeof_bytes_decode += A_sizeof_bytes;
		}
	}

	output(ms_diff_ctor, A_sizeof_bytes, decode, get_debug(), A_input_string);
}

void
C_perf_chrono::output(
		const long          ms_diff_ctor,
		const int           nb_bytes,
		const string      & decode,
		const bool          debug,
		const string      & what)
{
	cout.width(5);
	cout << ms_diff_ctor << " ms  ";

	cout.width(5);
	cout << nb_bytes << " bytes  ";

	cout << decode;
	cout << " ";

	if (debug == false)
	{
		cout << "     ";
	}
	else
	{
		cout << "debug";
	}

	cout << "  for " << what;
	cout << endl;
}

void
C_perf_chrono::end()
{
	output(A_total_ms_no_decode, A_total_sizeof_bytes_no_decode, "      ", false, "sum of all times (do not compare with decode time)");
	output(A_total_ms_decode, A_total_sizeof_bytes_decode, "decode", false, "sum of all times");
	output(A_total_ms, A_total_sizeof_bytes, "all   ", false, "sum of all times");
}

//*****************************************************************************
// test_scoped_copyable_ptr
//*****************************************************************************

class C_scoped_copyable_ptr_object
{
	C_scoped_copyable_ptr_object &  operator=(const C_scoped_copyable_ptr_object & rhs);

public:
	C_scoped_copyable_ptr_object()
		:id (++id_counter),
		 generation (1)
	{
	}
	C_scoped_copyable_ptr_object(const C_scoped_copyable_ptr_object & rhs)
		:id (rhs.id),
		 generation (rhs.generation + 1)
	{
	}
	~C_scoped_copyable_ptr_object()
	{
		last_deleted_id = id;
		last_deleted_generation = generation;
	}

	static int    id_counter;
	static int    last_deleted_id;
	static int    last_deleted_generation;
	int           id;
	int           generation;
};
int   C_scoped_copyable_ptr_object::id_counter = 0;
int   C_scoped_copyable_ptr_object::last_deleted_id = 0;
int   C_scoped_copyable_ptr_object::last_deleted_generation = 0;

void    test_scoped_copyable_ptr()
{
	// Empty
	scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_a;

	M_TEST_EQ(scptr_a.get(), NULL);

	// Empty reset 
	scptr_a.reset(NULL);
	M_TEST_EQ(scptr_a.get(), NULL);

	// reset initialize
	scptr_a.reset(new C_scoped_copyable_ptr_object);
	M_TEST_NE(scptr_a.get(), NULL);
	M_TEST_EQ(scptr_a->id, 1);
	M_TEST_EQ((*scptr_a).id, 1);
	M_TEST_EQ(scptr_a->generation, 1);

	// Create not empty
	scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_b(new C_scoped_copyable_ptr_object);

	M_TEST_NE(scptr_b.get(), NULL);
	M_TEST_EQ(scptr_b->id, 2);
	M_TEST_EQ(scptr_b->generation, 1);

	// Assign

	// Do not compile : forbidden
//	scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_c = new C_scoped_copyable_ptr_object;

	scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_c(NULL);
	M_TEST_EQ(scptr_c.get(), NULL);

	scptr_c = scptr_a;    // deep copy

	M_TEST_NE(scptr_c.get(), NULL);
	M_TEST_EQ(scptr_c->id, 1);
	M_TEST_EQ(scptr_c->generation, 2);    // deep copy

	M_TEST_NE(scptr_a.get(), NULL);
	M_TEST_EQ(scptr_a->id, 1);
	M_TEST_EQ(scptr_a->generation, 1);

	// Copy
	{
		scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_d(scptr_c);    // deep copy

		M_TEST_NE(scptr_d.get(), NULL);
		M_TEST_EQ(scptr_d->id, 1);
		M_TEST_EQ(scptr_d->generation, 3);    // deep copy
	}

	// Out of scope destruction
	M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_id, 1);
	M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_generation, 3);

	// swap
	swap(scptr_c, scptr_b);

	M_TEST_NE(scptr_b.get(), NULL);
	M_TEST_EQ(scptr_b->id, 1);
	M_TEST_EQ(scptr_b->generation, 2);

	M_TEST_NE(scptr_c.get(), NULL);
	M_TEST_EQ(scptr_c->id, 2);
	M_TEST_EQ(scptr_c->generation, 1);

	// swap on itself
	swap(scptr_c, scptr_c);

	M_TEST_NE(scptr_c.get(), NULL);
	M_TEST_EQ(scptr_c->id, 2);
	M_TEST_EQ(scptr_c->generation, 1);

	// Destruction reset 
	scptr_a.reset(NULL);
	M_TEST_EQ(scptr_a.get(), NULL);

	M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_id, 1);
	M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_generation, 1);

	// Destruction reset initialize
	scptr_a.reset(new C_scoped_copyable_ptr_object);

	M_TEST_NE(scptr_a.get(), NULL);
	M_TEST_EQ(scptr_a->id, 3);
	M_TEST_EQ(scptr_a->generation, 1);

	// Really bad idea : NO deep copy here !!! BAD, BAD, BAD
//	scptr_a.reset(scptr_b.get());
	scptr_a.reset(new C_scoped_copyable_ptr_object);

	M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_id, 3);
	M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_generation, 1);

	M_TEST_NE(scptr_a.get(), NULL);
	M_TEST_EQ(scptr_a->id, 4);
	M_TEST_EQ(scptr_a->generation, 1);
}

//*****************************************************************************
// test_string_to_words
//*****************************************************************************

void    test_string_to_words()
{
	{
		vector<string>    words;
		string_to_words("Hello world! \"How are you today ?\" Seems fine.", words);

		M_TEST_EQ(words.size(), 5);
		M_TEST_EQ(words[0], "Hello");
		M_TEST_EQ(words[1], "world!");
		M_TEST_EQ(words[2], "\"How are you today ?\"");
		M_TEST_EQ(words[3], "Seems");
		M_TEST_EQ(words[4], "fine.");
	}

	{
		vector<string>    words;
		string_to_words("  string(12)  str;  uint16{q=34.23}{max=1234.68}[number_of_integer]  int ;", words);

		M_TEST_EQ(words.size(), 6);
		M_TEST_EQ(words[0], "string(12)");
		M_TEST_EQ(words[1], "str");
		M_TEST_EQ(words[2], ";");
		M_TEST_EQ(words[3], "uint16{q=34.23}{max=1234.68}[number_of_integer]");
		M_TEST_EQ(words[4], "int");
		M_TEST_EQ(words[5], ";");
	}

	{
		vector<string>    words;
		string_to_words("  while (field < 12*3) { uint12  int ; }", words);

		M_TEST_EQ(words.size(), 7);
		M_TEST_EQ(words[0], "while");
		M_TEST_EQ(words[1], "(field < 12*3)");
		M_TEST_EQ(words[2], "{");
		M_TEST_EQ(words[3], "uint12");
		M_TEST_EQ(words[4], "int");
		M_TEST_EQ(words[5], ";");
		M_TEST_EQ(words[6], "}");
	}

	{
		vector<string>    words;
		string_to_words("  (field < 12*3)  ", words, K_parser_cfg_C);

		M_TEST_EQ(words.size(), 1);
		M_TEST_EQ(words[0], "(field < 12*3)");
	}

	{
		vector<string>    words;
		string_to_words("field < 12-3*(3.4 * 345)", words, K_parser_cfg_C);

		M_TEST_EQ(words.size(), 7);
		M_TEST_EQ(words[0], "field");
		M_TEST_EQ(words[1], "<");
		M_TEST_EQ(words[2], "12");
		M_TEST_EQ(words[3], "-");
		M_TEST_EQ(words[4], "3");
		M_TEST_EQ(words[5], "*");
		M_TEST_EQ(words[6], "(3.4 * 345)");
	}

	{
		vector<string>    words;
		string_to_words("\"Hello \" + \"world!\" == \"Hello world!\"", words, K_parser_cfg_C);

		M_TEST_EQ(words.size(), 5);
		M_TEST_EQ(words[0], "\"Hello \"");
		M_TEST_EQ(words[1], "+");
		M_TEST_EQ(words[2], "\"world!\"");
		M_TEST_EQ(words[3], "==");
		M_TEST_EQ(words[4], "\"Hello world!\"");
	}

	{
		vector<string>    words;
		string_to_words("\"Hello \"+\"world!\"==\"Hello world!\"", words, K_parser_cfg_C);

		M_TEST_EQ(words.size(), 5);
		M_TEST_EQ(words[0], "\"Hello \"");
		M_TEST_EQ(words[1], "+");
		M_TEST_EQ(words[2], "\"world!\"");
		M_TEST_EQ(words[3], "==");
		M_TEST_EQ(words[4], "\"Hello world!\"");
	}

	{
		vector<string>    words;
		string_to_words("  \"Unknow msg identifier (%d) or %s\",Type1,Type2  ", words, K_parser_cfg_parameters);

		M_TEST_EQ(words.size(), 3);
		M_TEST_EQ(words[0], "\"Unknow msg identifier (%d) or %s\"");
		M_TEST_EQ(words[1], "Type1");
		M_TEST_EQ(words[2], "Type2");
	}

	{
		vector<string>    words;
		string_to_words("  \"Unknow msg identifier (%d) or %s\"  ,  Type1  ,  Type2  ", words, K_parser_cfg_parameters);

		M_TEST_EQ(words.size(), 3);
		M_TEST_EQ(words[0], "\"Unknow msg identifier (%d) or %s\"");
		M_TEST_EQ(words[1], "Type1");
		M_TEST_EQ(words[2], "Type2");
	}

	{
		vector<string>    words;
		string_to_words("  12.3 * 2 + 17.1, \"par2\"  ", words, K_parser_cfg_parameters);

		M_TEST_EQ(words.size(), 2);
		M_TEST_EQ(words[0], "12.3 * 2 + 17.1");
		M_TEST_EQ(words[1], "\"par2\"");
	}

	{
		vector<string>    words;
		string_to_words("  12.3 * 2 + 17.1, \"par2\" + \"par1\"  ", words, K_parser_cfg_parameters);

		M_TEST_EQ(words.size(), 2);
		M_TEST_EQ(words[0], "12.3 * 2 + 17.1");
		M_TEST_EQ(words[1], "\"par2\" + \"par1\"");
	}
}

//*****************************************************************************
// test_read_token_word_cplx
//*****************************************************************************

void    test_read_token_word_cplx()
{
	string    str_result;

	// Words and strings
	{
		istringstream  iss("word + string - 1234.45 * 0xdf / -067 % \"a string\" ");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "word");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "+");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "string");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "-");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "1234.45");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "*");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "0xdf");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "/");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "-067");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "%");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "\"a string\"");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), false);
		M_TEST_EQ(str_result, "");

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// Comments and words with blocks
	{
		istringstream  iss("word + # comment \n type( (al) lo ) type[idx - 3] type{d=bin} ");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "word");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "+");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "type( (al) lo )");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "type[idx - 3]");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "type{d=bin}");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), false);
		M_TEST_EQ(str_result, "");

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// ;
	{
		istringstream  iss("word + type( al;lo ) begin;end type[idx ;- 3] type{d=bi;n} ");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "word");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "+");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "type( al;lo )");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "begin");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, ";");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "end");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "type[idx ;- 3]");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_normal), true);
		M_TEST_EQ(str_result, "type{d=bi;n}");

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// K_parser_cfg_parameters permits to extract function parameters
	// E.g: <param1> , <param2> , ...
	{
		istringstream  iss("wo\"r,d\" + type,( al;lo ) begin,end type[idx ,;- 3] t,ype{d=b,i;n} ");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
		M_TEST_EQ(str_result, "wo\"r,d\" + type");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
		M_TEST_EQ(str_result, "( al;lo ) begin");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
		M_TEST_EQ(str_result, "end type[idx ,;- 3] t");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_parameters), true);
		M_TEST_EQ(str_result, "ype{d=b,i;n}");

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// K_parser_cfg_expression : expression until ;
	{
		istringstream  iss("print (...); lhs + rhs; a_value == 0 ? (((a_value * 2) / a_value) + 2) : 5 ; ");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
		M_TEST_EQ(str_result, "print (...)");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
		M_TEST_EQ(str_result, ";");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
		M_TEST_EQ(str_result, "lhs + rhs");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
		M_TEST_EQ(str_result, ";");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
		M_TEST_EQ(str_result, "a_value == 0 ? (((a_value * 2) / a_value) + 2) : 5");

		str_result = "";
		M_TEST_EQ(read_token_word_cplx(iss, str_result, K_parser_cfg_expression), true);
		M_TEST_EQ(str_result, ";");

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}
}

//*****************************************************************************
// test_read_token
//*****************************************************************************

void    test_read_token()
{
	string    str_result;

	// Field with (...)
	{
		istringstream  iss(""
			"  raw(val-3)[val+2]"
			"  toto ; ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "raw(val-3)[val+2]");

		str_result = "";
		M_TEST_EQ(read_token_field_name(iss, str_result), true);
		M_TEST_EQ(str_result, "toto");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// Field with (...) and spaces
	{
		istringstream  iss(""
			"  raw( val - 3 )[ val + 2 ]"
			"  \"\" ; ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "raw( val - 3 )[ val + 2 ]");

		str_result = "";
		M_TEST_EQ(read_token_field_name(iss, str_result), true);
		M_TEST_EQ(str_result, "");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// Field with {...}
	{
		istringstream  iss(" hide  var"
			"  uint13{ns=0}{q=12.2:o=-47.2}{min=-43.2:max=12.3}{d=bin}[val+2]"
			"  toto; ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "hide");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "var");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "uint13{ns=0}{q=12.2:o=-47.2}{min=-43.2:max=12.3}{d=bin}[val+2]");

		str_result = "";
		M_TEST_EQ(read_token_field_name(iss, str_result), true);
		M_TEST_EQ(str_result, "toto");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// Field with {...} and spaces
	{
		istringstream  iss(" hide  var"
			"  uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=hello world}[ val + 2 ]"
			"  toto; ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "hide");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "var");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=hello world}[ val + 2 ]");

		str_result = "";
		M_TEST_EQ(read_token_field_name(iss, str_result), true);
		M_TEST_EQ(str_result, "toto");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// return  expression
	{
		istringstream  iss("	return  var_str_array[var_idx+1] + \" et quelques ...\"; ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "return");

		str_result = "";
		M_TEST_EQ(read_token_expression_any(iss, str_result), true);
		M_TEST_EQ(str_result, "var_str_array[var_idx+1] + \" et quelques ...\"");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// return  expression function
	{
		istringstream  iss(" return  print (var[var_idx+1] + \" et quelques ...\", 2); ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "return");

		str_result = "";
		M_TEST_EQ(read_token_expression_any(iss, str_result), true);
		M_TEST_EQ(str_result, "print (var[var_idx+1] + \" et quelques ...\", 2)");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}

	// return  expression function
	{
		istringstream  iss(" print (\"int24[0] = %s\", int24[0]); ");

		str_result = "";
		M_TEST_EQ(read_token_left_any(iss, str_result), true);
		M_TEST_EQ(str_result, "print");

		str_result = "";
		M_TEST_EQ(read_token_expression_any(iss, str_result), true);
		M_TEST_EQ(str_result, "(\"int24[0] = %s\", int24[0])");

		read_token_end_of_statement(iss);

		str_result = "";
		iss >> str_result;
		M_TEST_EQ(str_result, "");
	}
}

//*****************************************************************************
// test_build_field
//*****************************************************************************

void    test_build_field ()
{
	T_type_definitions    type_definitions;

	// Field 
	{
		string         first_word = "uint3";
		istringstream  iss("toto;follow");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "follow");

		M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
		M_TEST_EQ(field_type_name.must_show(), false);                      // show
		M_TEST_EQ(field_type_name.is_a_variable(), false);                  // var
		M_TEST_EQ(field_type_name.type, "uint3");                           // type
		M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
		M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
		M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
		M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 0);
		M_TEST_EQ(field_type_name.name, "toto");
		M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "");
	}

	// Field var float
	{
		string         first_word = "var";
		istringstream  iss("float32  val = 12;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
		M_TEST_EQ(field_type_name.must_show(), false);                      // show
		M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
		M_TEST_EQ(field_type_name.type, "float32");                         // type
		M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
		M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
		M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 0);
		M_TEST_EQ(field_type_name.name, "val");
		M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12");
		M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_int(), 12);
	}

	// Field var float
	{
		string         first_word = "var";
		istringstream  iss("float32  val = 12.3;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
		M_TEST_EQ(field_type_name.must_show(), false);                      // show
		M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
		M_TEST_EQ(field_type_name.type, "float32");                         // type
		M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
		M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
		M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 0);
		M_TEST_EQ(field_type_name.name, "val");
		M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12.3");
		M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_flt(), 12.3);
	}

	// Field var float
	{
		string         first_word = "var";
		istringstream  iss("float32  val = 12.3e2;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), false);                      // hide
		M_TEST_EQ(field_type_name.must_show(), false);                      // show
		M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
		M_TEST_EQ(field_type_name.type, "float32");                         // type
		M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
		M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
		M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 0);
		M_TEST_EQ(field_type_name.name, "val");
		M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "12.3e2");
		M_TEST_EQ(field_type_name.get_var_expression().compute_expression_static(type_definitions).get_flt(), 1230.0);
	}

	// Field with {...} and spaces
	{
		string         first_word = "hide";
		istringstream  iss(" var"
			" uint3{ns=0}{q=.2:o=-.2}{min=-3.2:max=2.3}{de=\"hello world\"}[ val + 2 ]"
			" toto = 4;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
		M_TEST_EQ(field_type_name.is_a_variable(), true);                   // var
		M_TEST_EQ(field_type_name.type, "uint3");                           // type
		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
		M_TEST_EQ(field_type_name.transform_quantum.get_flt(), 0.2);        // q
		M_TEST_EQ(field_type_name.transform_offset.get_flt(), -0.2);        // o
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
		M_TEST_EQ(field_type_name.constraints[0].min.get_flt(), -3.2);      // min
		M_TEST_EQ(field_type_name.constraints[0].max.get_flt(), 2.3);       // max
		M_TEST_EQ(field_type_name.str_display_expression, "\"hello world\"");
		M_TEST_EQ(field_type_name.str_arrays.size(), 1);
		M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
		M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
		M_TEST_EQ(field_type_name.name, "toto");
		M_TEST_EQ(field_type_name.get_var_expression().get_original_string_expression(), "4");
	}

	// Field 
	{
		string         first_word = "uint3{de='hello' + ' world'}";
		istringstream  iss("toto;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.type, "uint3");                           // type
		M_TEST_EQ(field_type_name.str_display_expression, "'hello' + ' world'");
		M_TEST_EQ(field_type_name.name, "toto");
	}

	// Field with transform expression
	{
		string         first_word = "hide";
		istringstream  iss(" "
			" uint3{ns=0}{tef=.2*this-.2}{min=-3.2:max=2.3}[ val + 2 ]"
			" toto;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
		M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
		M_TEST_EQ(field_type_name.type, "uint3");                           // type
		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
		M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
		M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
		M_TEST_EQ(field_type_name.transform_expression.is_defined(), true);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
		M_TEST_EQ(field_type_name.constraints[0].min.get_flt(), -3.2);      // min
		M_TEST_EQ(field_type_name.constraints[0].max.get_flt(), 2.3);       // max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 1);
		M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
		M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
		M_TEST_EQ(field_type_name.name, "toto");
		M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
	}

	// Field with size and specifications before and after
	{
		string         first_word = "hide";
		istringstream  iss(" "
			" string{decoder=decode_utf8}(50){d=string is %s}[ val + 2 ]"
			" toto;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
		M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
		M_TEST_EQ(field_type_name.type, "string");                          // type
		M_TEST_EQ(field_type_name.no_statement.as_string(), "");            // ns
		M_TEST_EQ(field_type_name.transform_quantum.as_string(), "");       // q
		M_TEST_EQ(field_type_name.transform_offset.as_string(), "");        // o
		M_TEST_EQ(field_type_name.transform_expression.is_defined(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), false);
		M_TEST_EQ(field_type_name.constraints.size(), 0);                   // min & max
		M_TEST_EQ(field_type_name.str_decoder_function, "decode_utf8");
		M_TEST_EQ(field_type_name.str_size_or_parameter, "50");
		M_TEST_EQ(field_type_name.str_display, "string is %s");
		M_TEST_EQ(field_type_name.str_arrays.size(), 1);
		M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
		M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + 2 ");
		M_TEST_EQ(field_type_name.name, "toto");
		M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
	}

	// Invalid syntax field name []
	{
		string         first_word = "float32";
		istringstream  iss(" x[6];");

		T_field_type_name    field_type_name;
		M_TEST_CATCH_EXCEPTION(
			build_field(iss, type_definitions, first_word, field_type_name),
			C_byte_interpret_exception);
	}

	// Invalid syntax field name {}
	{
		string         first_word = "uint32";
		istringstream  iss(" x{ns=0};");

		T_field_type_name    field_type_name;
		M_TEST_CATCH_EXCEPTION(
			build_field(iss, type_definitions, first_word, field_type_name),
			C_byte_interpret_exception);
	}

	// Invalid syntax field name ()
	{
		string         first_word = "string";
		istringstream  iss(" x(50);");

		T_field_type_name    field_type_name;
		M_TEST_CATCH_EXCEPTION(
			build_field(iss, type_definitions, first_word, field_type_name),
			C_byte_interpret_exception);
	}

	// Invalid syntax field name ::
	{
		string         first_word = "uint32";
		istringstream  iss(" konst::one_hundred;");

		T_field_type_name    field_type_name;
		M_TEST_CATCH_EXCEPTION(
			build_field(iss, type_definitions, first_word, field_type_name),
			C_byte_interpret_exception);
	}

	// Invalid syntax field name ::
	{
		string         first_word = "var";
		istringstream  iss("uint32 konst::one_hundred = 100;");

		T_field_type_name    field_type_name;
		M_TEST_CATCH_EXCEPTION(
			build_field(iss, type_definitions, first_word, field_type_name),
			C_byte_interpret_exception);
	}

	// Valid const syntax field name
	{
		string         first_word = "konst";
		istringstream  iss("uint32 konst::one_hundred = 100;");

		T_field_type_name    field_type_name;
		M_TEST_CATCH_EXCEPTION(
			build_field(iss, type_definitions, first_word, field_type_name),
			C_byte_interpret_exception);
	}


	// Add some types
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

	// Use const and enum (::) in many places
	{
		string         first_word = "hide";
		istringstream  iss(" "
			" uint3"
			"{ns=T_enum3::value0}"
			"{q=konst::int_enum:o=T_enum4::Val13}"
			"{tef=.2*this-.2+T_enum3::value0}"
			"{min=konst::int_enum:max=T_enum4::Val13}"
			"[ val + T_enum2::enu2 ]"
			" toto;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
		M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
		M_TEST_EQ(field_type_name.type, "uint3");                           // type
		M_TEST_EQ(field_type_name.no_statement.get_int(), 0);               // ns
		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 0);          // q
		M_TEST_EQ(field_type_name.transform_offset.get_int(), 13);          // o
		M_TEST_EQ(field_type_name.transform_expression.is_defined(), true);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
		M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
		M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 1);
		M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
		M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + T_enum2::enu2 ");
		M_TEST_EQ(field_type_name.name, "toto");
		M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
	}

	// Use condition ? xxx : yyy in many places
	{
		string         first_word = "hide";
		istringstream  iss(" "
			" uint3"
			"{ns=T_enum3::value0 == 0 ? 1 : 2}"
			"{q=T_enum4::Val13 != 13 ? konst::int_enum : 12:o=(konst::int_enum != 0 ? T_enum3::value0 : T_enum4::Val13)}"
			"{tef=.2*this-.2+(konst::int_enum == 0 ? T_enum3::value0 : T_enum4::Val13)}"
			"{min=T_enum4::Val13 == 13 ? konst::int_enum : 12:max=(konst::int_enum == 0 ? T_enum4::Val13 : T_enum3::value0)}"
			"[ val + (T_enum3::value0 != 0 ? 1 : 2) ]"
			" toto;");

		T_field_type_name    field_type_name;

		M_TEST_EQ(build_field(iss, type_definitions, first_word, field_type_name), "");

		M_TEST_EQ(field_type_name.must_hide(), true);                       // hide
		M_TEST_EQ(field_type_name.is_a_variable(), false)                   // var
		M_TEST_EQ(field_type_name.type, "uint3");                           // type
		M_TEST_EQ(field_type_name.no_statement.get_int(), 1);               // ns
		M_TEST_EQ(field_type_name.transform_quantum.get_int(), 12);         // q
		M_TEST_EQ(field_type_name.transform_offset.get_int(), 13);          // o
		M_TEST_EQ(field_type_name.transform_expression.is_defined(), true);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_int(), false);
		M_TEST_EQ(field_type_name.must_force_manage_as_biggest_float(), true);
		M_TEST_EQ(field_type_name.constraints.size(), 1);                   // min & max
		M_TEST_EQ(field_type_name.constraints[0].min.get_int(), 0);         // min
		M_TEST_EQ(field_type_name.constraints[0].max.get_int(), 13);        // max
		M_TEST_EQ(field_type_name.str_display_expression, "");
		M_TEST_EQ(field_type_name.str_arrays.size(), 1);
		M_TEST_EQ(field_type_name.str_arrays[0].size_type, T_field_type_name::T_array::E_size_normal);
		M_TEST_EQ(field_type_name.str_arrays[0].size_expression.get_original_string_expression(), " val + (T_enum3::value0 != 0 ? 1 : 2) ");
		M_TEST_EQ(field_type_name.name, "toto");
		M_TEST_EQ(field_type_name.get_var_expression().is_defined(), false);
	}
}

//*****************************************************************************
// test_get_number
//*****************************************************************************

void    test_get_number()
{
	long long    int_number = 0;
	double       flt_number = 0.0;

#define M_TEST_NOT_OK(STR)                                                  \
	M_TEST_EQ(get_number(STR, int_number), false);                          \
	M_TEST_EQ(get_number(STR, flt_number), false)

#define M_TEST_INT(STR,VAL_INT)                                             \
	M_TEST_EQ(get_number(STR, int_number), true);                           \
	M_TEST_EQ(int_number, VAL_INT);                                         \
	M_TEST_EQ(get_number(STR, flt_number), false)

#define M_TEST_FLT(STR,VAL_FLT)                                             \
	M_TEST_EQ(get_number(STR, int_number), false);                          \
	M_TEST_EQ(get_number(STR, flt_number), true);                           \
	M_TEST_EQ(flt_number, VAL_FLT)

#define M_TEST_INT_FLT(STR,VAL_INT,VAL_FLT)                                 \
	M_TEST_EQ(get_number(STR, int_number), true);                           \
	M_TEST_EQ(int_number, VAL_INT);                                         \
	M_TEST_EQ(get_number(STR, flt_number), true);                           \
	M_TEST_EQ(flt_number, VAL_FLT)

	M_TEST_NOT_OK("aze");
	M_TEST_NOT_OK("12g");
	M_TEST_NOT_OK("g12");
	M_TEST_NOT_OK("12a");
	M_TEST_NOT_OK("a12");

	M_TEST_FLT(  " 32.56",   32.56);
	M_TEST_FLT(" -132.56", -132.56);
	M_TEST_FLT( " 3.56e3", 3.56e3);
	M_TEST_FLT( " 3.56e+3", 3.56e3);
	M_TEST_FLT(" -3.56e-13", -3.56e-13);

	M_TEST_INT_FLT(  "32",   32,   32.0);
	M_TEST_INT_FLT("-132", -132, -132.0);
	M_TEST_INT_FLT(" 32",   32,   32.0);
	M_TEST_INT_FLT(" -132", -132, -132.0);

#if defined WIN32
	// VC++ strtod reject hexadecimal
	M_TEST_INT(  "0xa32",  0xa32);
	M_TEST_INT( " 0Xa32",  0xa32);
	M_TEST_INT("-0Xfa32",  -0xfa32);
#else
	// gcc strtod accept hexadecimal
	M_TEST_INT_FLT(  "0xa32",  0xa32, 2610.0);
	M_TEST_INT_FLT( " 0Xa32",  0xa32, 2610.0);
	M_TEST_INT_FLT("-0Xfa32",  -0xfa32, -64050.0);
#endif

	M_TEST_INT(   "032",   032);
	M_TEST_INT( "-0777", -0777);

#undef  M_TEST_NOT_OK
}

//*****************************************************************************
// test_get_before_separator_after
//*****************************************************************************

void    test_get_before_separator_after()
{
    string    str_left;
	string    str_right;

#define M_TEST_NOT_OK(STR,SEP)                                              \
	M_TEST_EQ(get_before_separator_after(STR, SEP, str_left, str_right), E_rc_not_found)

#define M_TEST_OK(STR,SEP,LEFT,RIGHT)                                       \
	M_TEST_EQ(get_before_separator_after(STR, SEP, str_left, str_right), E_rc_ok);  \
	M_TEST_EQ(str_left, LEFT);                                              \
	M_TEST_EQ(str_right, RIGHT)

	M_TEST_NOT_OK("aqsdfghjk", ':');

	M_TEST_OK("aqsdfghjk", 'd', "aqs", "fghjk");
	M_TEST_OK("aqs:dfg:hjk", ':', "aqs", "dfg:hjk");    // takes the 1st one
	M_TEST_OK(":aqs:dfg:hjk", ':', "", "aqs:dfg:hjk");  // no warranty : accept empty left

	M_TEST_OK("aqs::fghjk", "::", "aqs", "fghjk");

#undef  M_TEST_NOT_OK
#undef  M_TEST_OK
}

//*****************************************************************************
// test_decompose_type_sep_value_sep
//*****************************************************************************

void    test_decompose_type_sep_value_sep()
{
    string    str_left;
	string    str_right;

#define M_TEST_NOT_OK(STR,SEP_LEFT,SEP_RIGHT)                                              \
	M_TEST_EQ(decompose_type_sep_value_sep(STR, SEP_LEFT, SEP_RIGHT, str_left, str_right), E_rc_not_found)

#define M_TEST_OK(STR,SEP_LEFT,SEP_RIGHT,LEFT,RIGHT)                                                  \
	M_TEST_EQ(decompose_type_sep_value_sep(STR, SEP_LEFT, SEP_RIGHT, str_left, str_right), E_rc_ok);  \
	M_TEST_EQ(str_left, LEFT);                                                                        \
	M_TEST_EQ(str_right, RIGHT)

	M_TEST_NOT_OK("aqsdfghjk", '(', ')');
	M_TEST_NOT_OK("aqsdfghj)", '(', ')');
	M_TEST_NOT_OK("aq(dfghjk", '(', ')');
	M_TEST_NOT_OK("aq(dfgh)k", '(', ')');
	M_TEST_NOT_OK("(aqdfghk)", '(', ')');  // do NOT accept empty left

	M_TEST_OK("asaqs(dfghj)", '(', ')', "asaqs", "dfghj");
	M_TEST_OK("{aqs}{dfghj}", '{', '}', "{aqs}", "dfghj");
	M_TEST_OK("a[a][f[gh]k]", '[', ']', "a[a]", "f[gh]k");
	M_TEST_OK("a[a][f[ghk[]", '[', ']', "a[a][f[ghk", "");

	M_TEST_NOT_OK("a[a][f_gh]k]", '[', ']');             // missing 1 [
	M_TEST_OK("a[a][f[]g[h]k]", '[', ']', "a[a]", "f[]g[h]k");


#undef  M_TEST_NOT_OK
#undef  M_TEST_OK
}

//*****************************************************************************
// test_decompose_left_sep_middle_sep_right
//*****************************************************************************

void    test_decompose_left_sep_middle_sep_right()
{
    string    str_left;
    string    str_middle;
	string    str_right;

#define M_TEST_NOT_OK(STR,SEP_LEFT,SEP_RIGHT)                                              \
	M_TEST_EQ(decompose_left_sep_middle_sep_right(STR, SEP_LEFT, SEP_RIGHT, str_left, str_middle, str_right), E_rc_not_found)

#define M_TEST_OK(STR,SEP_LEFT,SEP_RIGHT,LEFT,MIDDLE,RIGHT)                                                       \
	M_TEST_EQ(decompose_left_sep_middle_sep_right(STR, SEP_LEFT, SEP_RIGHT, str_left, str_middle, str_right), E_rc_ok);  \
	M_TEST_EQ(str_left, LEFT);                                                                                    \
	M_TEST_EQ(str_middle, MIDDLE);                                                                                \
	M_TEST_EQ(str_right, RIGHT)

	M_TEST_NOT_OK("aqsdfghjk", '(', ')');
	M_TEST_NOT_OK("aqsdfghj)", '(', ')');
	M_TEST_NOT_OK("aq(dfghjk", '(', ')');

	M_TEST_OK("aq(dfgh)k", '(', ')', "aq", "dfgh", "k");
	M_TEST_OK("(aqdfghk)", '(', ')', "", "aqdfghk", "");
	M_TEST_OK("asaqs(dfghj)", '(', ')', "asaqs", "dfghj", "");
	M_TEST_OK("{aqs}{dfghj}", '{', '}', "", "aqs", "{dfghj}");
	M_TEST_OK("a[a][f[gh]k]", '[', ']', "a", "a", "[f[gh]k]");
	M_TEST_OK("[]a[a][f[ghk", '[', ']', "", "", "a[a][f[ghk");

	M_TEST_NOT_OK("a[a[f[_gh]k]du", '[', ']');             // missing 1 ]
	M_TEST_OK("a[a[f[]g[h]k]]du", '[', ']', "a", "a[f[]g[h]k]", "du");


#undef  M_TEST_NOT_OK
#undef  M_TEST_OK
}

//*****************************************************************************
// test_frame_data
//*****************************************************************************

void    test_frame_data()
{
	const unsigned char  bytes[] = { 0, 0, 0, 0, 1, 0, 1, 2, 3, 4, 0xff, 0xaa, 0x37 };
	const long           initial_sizeof_bytes = sizeof(bytes)/sizeof(bytes[0]);

	T_frame_data    frame_data(bytes, 0, initial_sizeof_bytes * 8);

	// Position and movements.

	M_TEST_EQ(frame_data.get_bit_offset(), 0);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), true);
	M_TEST_EQ(frame_data.get_P_bytes(), bytes);
	M_TEST_EQ(frame_data.get_remaining_bits(), initial_sizeof_bytes*8);
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	M_TEST_EQ(frame_data.can_move_1_byte_forward(), true);
	frame_data.move_1_byte_forward();
	M_TEST_EQ(frame_data.get_bit_offset(), 8);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), true);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes-1)*8);
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	M_TEST_EQ(frame_data.can_move_forward(2, 1), true);
	frame_data.move_forward(2, 1);
	M_TEST_EQ(frame_data.get_bit_offset(), 25);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	M_TEST_EQ(frame_data.can_move_bit_forward(3), true);
	frame_data.move_bit_forward(3);
	M_TEST_EQ(frame_data.get_bit_offset(), 28);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	M_TEST_EQ(frame_data.can_move_bit(-3), true);
	frame_data.move_bit(-3);
	M_TEST_EQ(frame_data.get_bit_offset(), 25);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	M_TEST_EQ(frame_data.can_move(-2, -1), true);
	frame_data.move(-2, -1);
	M_TEST_EQ(frame_data.get_bit_offset(), 8);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), true);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes-1)*8);
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	frame_data.set_offset(3, 7);
	M_TEST_EQ(frame_data.get_bit_offset(), 31);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-3-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);

	frame_data.set_bit_offset(37);
	M_TEST_EQ(frame_data.get_bit_offset(), 37);
	M_TEST_EQ(frame_data.is_physically_at_beginning_of_byte(), false);
	M_TEST_EQ(frame_data.get_remaining_bits(), (initial_sizeof_bytes*8)-frame_data.get_bit_offset());
	M_TEST_EQ(frame_data.get_remaining_entire_bytes(), initial_sizeof_bytes-4-1);
	M_TEST_EQ(frame_data.get_initial_sizeof_bits(), initial_sizeof_bytes * 8);


#define M_TEST_READ_EQ(READ_OPER,VALUE)                          \
{                                                                \
	const long  value_read = READ_OPER;                          \
	M_TEST_EQ(value_read, VALUE);                                \
}

	// Read bytes on byte position.
	frame_data.set_bit_offset(0);

	M_TEST_EQ(frame_data.read_1_byte(), 0);

	{
		unsigned char  bytes_read[4];
		const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

		frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

		T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 1);
		M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
	}
	{
		unsigned char  bytes_read[5];
		const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

		frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

		T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 1);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 2);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 3);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 4);
		M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
	}
	{
		unsigned char  bytes_read[3];
		const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

		frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

		T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0xff);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0xaa);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0x37);
		M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
	}
	M_TEST_EQ(frame_data.can_move_bit_forward(1), false);


	// Read byte on bit position.
	frame_data.set_bit_offset(0);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 32);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

	M_TEST_READ_EQ(frame_data.read_1_byte(), 64);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 128);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 192+1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 63);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 128+64+32+ 0+8+0+2+0);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 128+ 0+ 0+ 0+8+4+0+1);

	M_TEST_EQ(frame_data.can_move_bit_forward(2), true);
	M_TEST_EQ(frame_data.can_move_bit_forward(3), false);

	frame_data.move_bit(-6);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0x37);


	// Read bytes on bit position.
	frame_data.set_bit_offset(0);

	frame_data.move_bit_forward(1);
	M_TEST_READ_EQ(frame_data.read_1_byte(), 0);

	{
		unsigned char  bytes_read[4];
		const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

		frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

		T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 2);
		M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
	}
	{
		unsigned char  bytes_read[5];
		const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

		frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

		T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 2);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 4);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 6);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 8+1);
		M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
	}
	{
		unsigned char  bytes_read[2];
		const long     initial_sizeof_bytes_read = sizeof(bytes_read)/sizeof(bytes_read[0]);

		frame_data.read_n_bytes(initial_sizeof_bytes_read, bytes_read);

		T_frame_data    frame_data_read(bytes_read, 0, initial_sizeof_bytes_read * 8);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0xff);
		M_TEST_READ_EQ(frame_data_read.read_1_byte(), 0x54);
		M_TEST_READ_EQ(frame_data_read.can_move_bit_forward(1), false);
	}
	M_TEST_EQ(frame_data.can_move_bit_forward(7),  true);
	M_TEST_EQ(frame_data.can_move_bit_forward(8), false);

	// Read bits.
	frame_data.set_bit_offset(0);

	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 4);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 1);

	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 1);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 64+32);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 64+ 0+ 0+8+4+2+1);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7), 64+32+16+8+4+0+1);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(7),  0+32+ 0+8+0+0+0);
	M_TEST_READ_EQ(frame_data.read_less_1_byte(6),    32+16+0+4+2+1);
	M_TEST_EQ(frame_data.can_move_bit_forward(1), false);
}

//*****************************************************************************
// test_frame_data_write
//*****************************************************************************

void    test_frame_data_write()
{
	unsigned char  bytes[200];
	long           initial_sizeof_bytes = sizeof(bytes)/sizeof(bytes[0]);

	unsigned char  data[] = { 0xe0, 0xd7, 0xc3, 0x20, 0x31, 0x40, 0x51, 0x72, 0x03, 0x04, 0xff, 0xaa, 0x37 };
	long           initial_sizeof_data = sizeof(data)/sizeof(data[0]);

	// write_1_byte
	{
		memset(bytes, 0, sizeof(bytes));
		T_frame_data_write    frame_data_write(bytes, 0, initial_sizeof_data * 8);
		T_frame_data          frame_data      (bytes, 0, initial_sizeof_data * 8);

		for (int  idx = 0; idx < initial_sizeof_data; ++idx)
		{
			frame_data_write.write_1_byte(data[idx]);
			M_TEST_EQ(frame_data.read_1_byte(), data[idx]);
		}

		M_TEST_EQ(frame_data_write.get_remaining_bits(), 0);
		M_TEST_EQ(frame_data.get_remaining_bits(), 0);
	}

	// write_less_1_byte
	{
		memset(bytes, 0, sizeof(bytes));
		T_frame_data_write    frame_data_write(bytes, 0, initial_sizeof_data * 8);
		T_frame_data          frame_data      (bytes, 0, initial_sizeof_data * 8);

		for (int  idx = 0; idx < initial_sizeof_data; ++idx)
		{
			const short  n_bits = 1 + (idx % 7);
			frame_data_write.write_less_1_byte(data[idx], n_bits);
			M_TEST_EQ(frame_data.read_less_1_byte(n_bits), bit_erase_left(data[idx], 8-n_bits));
		}

		M_TEST_EQ(frame_data_write.get_bit_offset(), frame_data.get_bit_offset());
	}

	// A tester : write_n_bytes
	// A tester : write_n_bits
}

//*****************************************************************************
// test_read_data
//*****************************************************************************

void    test_read_data()
{
	bool            is_signed_integer = false;

	// Float.
	is_signed_integer = false;
	{
		double          frame_data_value = -23655869876.44864;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		double          read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"float64", 64,
							"double", 64,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}
	{
		float           frame_data_value = -23655869876.44864e32;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		float           read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"float32", 32,
							"float", 32,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}

	// Signed byte integers.
	is_signed_integer = true;
	{
		signed long long      frame_data_value = -123456789012345LL;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		signed long long    read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"int64", 64,
							"signed long long", 64,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}
	{
		signed int      frame_data_value = -23456789;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		signed int    read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"int32", 32,
							"signed int", 32,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}
	{
		signed int      frame_data_value = -3456789;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);  // intel only

		signed int      read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"int24", 24,
							"signed int", 32,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}
	{
		signed short    frame_data_value = -26789;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		signed short    read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"int16", 16,
							"signed short", 16,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}
	{
		signed char     frame_data_value = -119;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		signed char     read_value = 0;
		M_TEST_EQ(read_data(frame_data,
							&read_value,
							"int8", 8,
							"signed char", 8,
							false,
							is_signed_integer), true);
		M_TEST_EQ(read_value, frame_data_value);
	}

	// Signed bit integers.
	{
		signed char     frame_data_value = -128;
		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		{
			signed char     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int7", 7,
								"signed char", 8,
								false,
								is_signed_integer), true);
			M_TEST_EQ(read_value, -64);
		}
		frame_data.set_bit_offset(0);
		{
			signed char     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int4", 4,
								"signed char", 8,
								false,
								is_signed_integer), true);
			M_TEST_EQ(read_value, -8);
		}
		frame_data.set_bit_offset(0);
		{
			signed char     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int2", 2,
								"signed char", 8,
								false,
								is_signed_integer), true);
			M_TEST_EQ(read_value, -2);
		}
		frame_data.set_bit_offset(0);
		{
			signed short     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int7", 7,
								"signed short", 16,
								false,
								is_signed_integer), true);
			M_TEST_EQ(read_value, -64);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int4", 4,
								"signed int", 32,
								false,
								is_signed_integer), true);
			M_TEST_EQ(read_value, -8);
		}
		frame_data.set_bit_offset(0);
		{
			signed long long     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int2", 2,
								"signed long long", 64,
								false,
								is_signed_integer), true);
			M_TEST_EQ(read_value, -2);
		}
	}
	{
		signed int      frame_data_value = -128;    // 0xffffff80 -> 80 ff ff ff

		T_frame_data    frame_data(&frame_data_value, 0, sizeof(frame_data_value) * 8);

		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int4", 4,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)000 -> 7 +1
			M_TEST_EQ(read_value, -8);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int7", 7,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)000000 -> 63 +1
			M_TEST_EQ(read_value, -64);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int8", 8,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)0000000 -> 127 +1
			M_TEST_EQ(read_value, -128);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int9", 9,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1) 00000001 -> 254 +1
			M_TEST_EQ(read_value, -255);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int10", 10,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)1 00000010 -> 253 +1
			M_TEST_EQ(read_value, -254);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int11", 11,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)11 00000100 -> 251 +1
			M_TEST_EQ(read_value, -252);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int15", 15,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)111111 01000000 -> 191 +1
			M_TEST_EQ(read_value, -192);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int16", 16,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)1111111 10000000 -> 127 +1
			M_TEST_EQ(read_value, -128);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int17", 17,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)1111111 00000001 00000001 -> 65278 +1
			M_TEST_EQ(read_value, -65279);
		}
		frame_data.set_bit_offset(0);
		{
			signed int     read_value = 0;
			M_TEST_EQ(read_data(frame_data,
								&read_value,
								"int31", 31,
								"signed int", 32,
								false,
								is_signed_integer), true);    // bits read = (1)111111 1..1 01111111 01000000 ->  +1
			M_TEST_EQ(read_value, -32960);
		}
	}
}

//*****************************************************************************
// test_read_values
//*****************************************************************************

void    test_read_values()
{
	T_interpret_read_values    read_values;

	// Tests d'existence et de valeurs

	M_TEST_EQ(read_values.is_read_variable("toto"), false);
	M_TEST_EQ(read_values.is_read_variable("tata"), false);
	M_TEST_EQ(read_values.is_read_variable("titi"), false);
	M_TEST_EQ(read_values.is_read_variable("tutu"), false);
	M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

	read_values.add_read_variable("NOT used", "toto", 11);

	M_TEST_EQ(read_values.is_read_variable("toto"), true);
	M_TEST_EQ(read_values.is_read_variable("tata"), false);
	M_TEST_EQ(read_values.is_read_variable("titi"), false);
	M_TEST_EQ(read_values.is_read_variable("tutu"), false);
	M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

	read_values.add_read_variable("NOT used", "tata", 22);

	M_TEST_EQ(read_values.is_read_variable("toto"), true);
	M_TEST_EQ(read_values.is_read_variable("tata"), true);
	M_TEST_EQ(read_values.is_read_variable("titi"), false);
	M_TEST_EQ(read_values.is_read_variable("tutu"), false);
	M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

	read_values.add_read_variable("NOT used", "titi", 33);
	read_values.add_read_variable("NOT used", "tutu", 44);

	M_TEST_EQ(read_values.is_read_variable("toto"), true);
	M_TEST_EQ(read_values.is_read_variable("tata"), true);
	M_TEST_EQ(read_values.is_read_variable("titi"), true);
	M_TEST_EQ(read_values.is_read_variable("tutu"), true);
	M_TEST_EQ(read_values.is_read_variable("titi.tutu"), false);

	M_TEST_EQ(read_values.get_P_value_of_read_variable("toto")->get_int(), 11);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("tata")->get_int(), 22);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("titi")->get_int(), 33);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("tutu")->get_int(), 44);

	read_values.read_variable_group_begin("titi");
	read_values.add_read_variable("NOT used", "tutu", 55);
	read_values.add_read_variable("NOT used", "allo", 111);
	read_values.add_read_variable("NOT used", "bonjour", 222);
	read_values.add_read_variable("NOT used", "c_est_moi", 333);
	read_values.add_read_variable("NOT used", "comment_ca_va", 444);
	read_values.add_read_variable("NOT used", "bien_et_toi", 444);
	read_values.add_read_variable("NOT used", "ping", "pong");

	M_TEST_EQ(read_values.is_read_variable("titi.tutu"), true);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("tutu")->get_int(), 55);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("titi.tutu")->get_int(), 55);

	read_values.read_variable_group_end();

	M_TEST_EQ(read_values.get_P_value_of_read_variable("tutu")->get_int(), 55);     // NAMERULE
	M_TEST_EQ(read_values.get_P_value_of_read_variable("titi.tutu")->get_int(), 55);

	// Test mulitple values
	T_interpret_read_values::T_var_name_P_values    name_values;

	name_values.clear();
	read_values.get_multiple_P_attribute_value_of_read_variable("titi.*", name_values);
	M_TEST_EQ(name_values.size(), 7);

	name_values.clear();
	read_values.get_multiple_P_attribute_value_of_read_variable("titi.a*", name_values);
	M_TEST_EQ(name_values.size(), 1);

	name_values.clear();
	read_values.get_multiple_P_attribute_value_of_read_variable("titi.c*", name_values);
	M_TEST_EQ(name_values.size(), 2);

	// Test array
	read_values.add_read_variable("NOT used", "an_array[0]", 1000);
	read_values.add_read_variable("NOT used", "an_array[1]", 1001);
	read_values.add_read_variable("NOT used", "an_array[2]", 1002);
	read_values.add_read_variable("NOT used", "an_array[3]", 1003);

	M_TEST_EQ(read_values.is_read_variable("an_array[0]"), true);
	M_TEST_EQ(read_values.is_read_variable("an_array[1]"), true);
	M_TEST_EQ(read_values.is_read_variable("an_array[2]"), true);
	M_TEST_EQ(read_values.is_read_variable("an_array[3]"), true);
	M_TEST_EQ(read_values.is_read_variable("an_array[4]"), false);

	M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[0]")->get_int(), 1000);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[1]")->get_int(), 1001);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[2]")->get_int(), 1002);
	M_TEST_EQ(read_values.get_P_value_of_read_variable("an_array[3]")->get_int(), 1003);
}

//*****************************************************************************
// test_expression_perf_operation
//*****************************************************************************

void    test_expression_perf_operation(
				const T_type_definitions  & type_definitions)
{
	T_interpret_data                  interpret_data;

	//-------------------------------------------------------------------------
	// perf operation expression
	//-------------------------------------------------------------------------
	const string     perf_expression =
		"((31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
#if 0		// Too long for win64 (ok for win32 or linux)
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
		" (31*(2**27)+24*3600*1000) -"
		" (31*(2**27)+24*3600*1000) +"
#endif
		" (31*(2**27)+24*3600*1000))";
	const long long  perf_expression_result =  4247149568LL;

	{
		C_value    value;
		{
			C_perf_chrono  perf_chrono(interpret_data, "perf integer expression operation", 0);
			value = compute_expression_no_io (type_definitions, interpret_data, perf_expression);
		}
		M_TEST_EQ(value.get_int(), perf_expression_result);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, perf_expression);
		C_value    value;
		{
			value = expression.compute_expression_no_io (type_definitions, interpret_data);
			// x  1 =  0 ms
			// x 10 = 62 ms !!!
			C_perf_chrono  perf_chrono(interpret_data, "perf integer expression operation T_expression x 10", 0);
			for (int idx = 0; idx < 10; ++idx)
			{
				expression.compute_expression_no_io (type_definitions, interpret_data);
			}
		}
		M_TEST_EQ(value.get_int(), perf_expression_result);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, perf_expression);
		expression.pre_compute_expression(type_definitions);
		C_value    value;
		{
			value = expression.compute_expression_no_io (type_definitions, interpret_data);
			// x    10 =  0 ms
			// x 40000 = 66 ms
			C_perf_chrono  perf_chrono(interpret_data, "perf integer expression operation T_expression x 40000 pre_compute", 0);
			for (int idx = 0; idx < 40000; ++idx)
			{
				expression.compute_expression_no_io (type_definitions, interpret_data);
			}
		}
		M_TEST_EQ(value.get_int(), perf_expression_result);
		value.as_string();
	}

	//-------------------------------------------------------------------------
	// operation expression with unexistent_variable
	//-------------------------------------------------------------------------
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "2 + unexistent_variable");
		expression.pre_compute_expression(type_definitions);

		M_TEST_CATCH_EXCEPTION(
			C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data),
			C_byte_interpret_exception);
	}
}

//*****************************************************************************
// test_expression_perf_function
//*****************************************************************************

void    test_expression_perf_function(
				const T_type_definitions  & type_definitions)
{
	T_interpret_data                  interpret_data;

	//-------------------------------------------------------------------------
	// perf expression
	//-------------------------------------------------------------------------
	const string     perf_expression = "perf(1,4201)";
	const long long  perf_expression_result =  4201LL;

	{
		C_value    value;
		{
			C_perf_chrono  perf_chrono(interpret_data, "perf integer expression function", 0);
			value = compute_expression_no_io (type_definitions, interpret_data, perf_expression);
		}
		M_TEST_EQ(value.get_int(), perf_expression_result);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, perf_expression);
		C_value    value;
		{
			value = expression.compute_expression_no_io (type_definitions, interpret_data);
			C_perf_chrono  perf_chrono(interpret_data, "perf integer expression function T_expression x 1", 0);
			for (int idx = 0; idx < 1; ++idx)
			{
				expression.compute_expression_no_io (type_definitions, interpret_data);
			}
		}
		M_TEST_EQ(value.get_int(), perf_expression_result);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, perf_expression);
		expression.pre_compute_expression(type_definitions);
		C_value    value;
		{
			value = expression.compute_expression_no_io (type_definitions, interpret_data);
			// x    10 =  0 ms
			// x 40000 = 66 ms
			C_perf_chrono  perf_chrono(interpret_data, "perf integer expression function T_expression x 1 pre_compute", 0);
			for (int idx = 0; idx < 1; ++idx)
			{
				expression.compute_expression_no_io (type_definitions, interpret_data);
			}
		}
		M_TEST_EQ(value.get_int(), perf_expression_result);
		value.as_string();
	}

	//-------------------------------------------------------------------------
	// function expression with unexistent_variable
	//-------------------------------------------------------------------------
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "perf_value(1,4201)");
		expression.pre_compute_expression(type_definitions);
#if 0
		M_TEST_CATCH_EXCEPTION(
			C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data),
			C_byte_interpret_exception);
#endif
	}
}

//*****************************************************************************
// get_int_value_bit_pos_size
//*****************************************************************************

C_value  get_int_value_bit_pos_size(long long  basic_value,
									int        bit_offset,
									int        bit_size)
{
	C_value  value(basic_value);
	value.set_bit_position_offset_size(bit_offset, bit_size);
	return  value;
}

//*****************************************************************************
// get_flt_value_bit_pos_size
//*****************************************************************************

C_value  get_flt_value_bit_pos_size(double     basic_value,
									int        bit_offset,
									int        bit_size)
{
	C_value  value(basic_value);
	value.set_bit_position_offset_size(bit_offset, bit_size);
	return  value;
}

//*****************************************************************************
// test_expression
//*****************************************************************************

void    test_expression()
{
	T_type_definitions    type_definitions;
	{
		istrstream  iss(
			"enum16 T_Operation {  CREATION 0   MODIFICATION 1   DELETION 2 }" "\n"

			"function any  addition (in any lhs, in any rhs)" "\n"
			"{" "\n"
			"  return  (lhs + rhs) ;" "\n"
			"}" "\n"

			"function int64  perf (in int64 incr, in int64 limit)" "\n"
			"{" "\n"
			"  hide var int64  value = 0;" "\n"
			"  while (value < limit)" "\n"
			"  {" "\n"
			"    set value = value + incr;" "\n"
			"  }" "\n"
			"  return  value ;" "\n"
			"}" "\n"

			"function int64  perf_value (in int64 incr, in int64 limit)" "\n"
			"{" "\n"
			"  while (value < limit)" "\n"
			"  {" "\n"
			"    set value = value + incr;" "\n"
			"  }" "\n"
			"  return  value ;" "\n"
			"}" "\n"

			"" "\n");

		string    last_word_not_treated = build_types (iss, type_definitions);
		M_FATAL_IF_NE(last_word_not_treated, "");
	}
	{
		istrstream  iss(
			"const int32    k::int = -17;" "\n"
			"const float32  k::flt = -234.9;" "\n"
			"const string   k::str = \"hello world\";" "\n"

			"" "\n");

		string    last_word_not_treated = build_types (iss, type_definitions);
		M_FATAL_IF_NE(last_word_not_treated, "");
	}

	//-------------------------------------------------------------------------
	// integer static expression
	//-------------------------------------------------------------------------
	{
		struct T_test_expr_int_result
		{
			const char  * expr;
			long long     expected_result;
		};

		const T_test_expr_int_result    expressions[] = {
			{ "27", 27 },
			{ "-234", -234 },
			{ "23+4", 27 },
			{ "-1345-65", -1410 },
			{ "234 * 2 - 5 + 1", 464 },
			{ "(234 * 2) - 5 + 1", 464 },
			{ "(234 * 2) - (5 + 1)", 462 },
			{ "234 - 2 * 5 + 1", 225 },
			{ "-(2**31)", -2147483648LL },
			{ "-2**31", -2147483648LL },
			{ "-2**2", 4 },
			{ "(2**31)-1", 2147483647LL },
			{ "2**31-1", 2147483647LL },
			{ "31*(2**27)+24*3600*1000", 4247149568LL },
			{ "31*2**27+24*3600*1000", 4247149568LL },
			{ "2**32-1", 4294967295LL },
			{ "3*2**2", 12 },
			{ "36/2**2", 9 },
			{ "7%2**2", 3 },
			{ "0x045", 69 },
			{ "-0Xdf", -223 },
			{ "0x045 * 1", 69 },
			{ "-0Xdf / 1", -223 },
			{ "-0Xdf / -1", 223 },
			{ "-0Xdf * -1", 223 },
			{ "-0Xdf + -(-1+2)", -0Xe0 },
			{ "007", 7 },
			{ "03456", 1838 },
			{ "007 / 0x7", 1 },
			{ "03456/1838", 1 },
			{ "007 % 0x5", 2 },
			// const
			{ "136 + k::int", 119 },
			// bit operations
			{ "007 & 0x5", 5 },
			{ "007 & 0x9", 1 },
			{ "007 | 0x5", 7 },
			{ "007 | 0x9", 15 },
			{ "007 ^ 0x5", 2 },
			{ "007 ^ 0x9", 14 },
			// condition
			{ "7 ? 2 : 0x5", 2 },
			{ "0 ? 2 : 0x5", 5 },
			// complex
			{ "(007 | 0x5) > (27 - 4 * 5) ? 2 - 2 : 31*(2**27)+24*3600*1000", 4247149568LL },
			// function
			{ " addition (3, 7) ", 10 }
		};
		for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
		{
			const char       * expr            = expressions[idx].expr;
			const long long    expected_result = expressions[idx].expected_result;

			cout << "int expression : ";
			cout.width(25);
			cout << expr;
#if 1
			C_value    value = compute_expression_static (type_definitions, expr);
#else
			C_value    value;
			{
				T_interpret_data  interpret_data;
				C_perf_chrono  perf_chrono(interpret_data, expr, 0);
				value = compute_expression_static (type_definitions, expr);
			}
#endif
			cout << "    gives " << value.as_string();
			M_TEST_EQ(value.get_int(), expected_result);
			M_TEST_EQ(value.get_bit_position_offset(), -1);
			M_TEST_EQ(value.get_bit_position_size(), -1);

			T_expression    expression;
			expression.build_expression(type_definitions, expr);
			C_value    value_new = expression.compute_expression_static(type_definitions);
			if (value_new.get_int() != expected_result)
			{
				cout << "    and " << value_new.as_string();
			}
			M_TEST_EQ(value_new.get_int(), expected_result);
			M_TEST_EQ(value_new.get_bit_position_offset(), -1);
			M_TEST_EQ(value_new.get_bit_position_size(), -1);

			cout << endl;
		}
	}

	//-------------------------------------------------------------------------
	// bool static expression
	//-------------------------------------------------------------------------
	{
		struct T_test_expr_int_result
		{
			const char  * expr;
			long long     expected_result;
		};

		const T_test_expr_int_result    expressions[] = {
			{ "007 <  0x7", 0 },
			{ "007 <  0x8", 1 },
			{ "007 <= 0x6", 0 },
			{ "007 <= 0x7", 1 },
			{ "007 >  0x7", 0 },
			{ "007 >  0x6", 1 },
			{ "007 >= 0x8", 0 },
			{ "007 >= 0x7", 1 },
			{ "007 == 0x5", 0 },
			{ "007 == 0x7", 1 },
			{ "007 != 0x7", 0 },
			{ "007 != 0x5", 1 },
			{ "007 != 0x5 && 007 != 0x7", 0 },
			{ "007 != 0x5 && 007 == 0x7", 1 },
			{ "007 == 0x5 || 007 != 0x7", 0 },
			{ "007 == 0x5 || 007 == 0x7", 1 },
			{ "1   == 0x1", 1 }
		};
		for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
		{
			const char       * expr            = expressions[idx].expr;
			const long long    expected_result = expressions[idx].expected_result;

			cout << "int expression : ";
			cout.width(25);
			cout << expr;
			C_value    value = compute_expression_static (type_definitions, expr);
			cout << "    gives " << value.as_string();
			M_TEST_EQ(value.get_bool(), expected_result);
			M_TEST_EQ(value.get_bit_position_offset(), -1);
			M_TEST_EQ(value.get_bit_position_size(), -1);

			T_expression    expression;
			expression.build_expression(type_definitions, expr);
			C_value    value_new = expression.compute_expression_static(type_definitions);
			if (value_new.get_int() != expected_result)
			{
				cout << "    and " << value_new.as_string();
			}
			M_TEST_EQ(value_new.get_bool(), expected_result);
			M_TEST_EQ(value_new.get_bit_position_offset(), -1);
			M_TEST_EQ(value_new.get_bit_position_size(), -1);

			cout << endl;
		}
	}

	//-------------------------------------------------------------------------
	// float static expression
	//-------------------------------------------------------------------------
	{
		struct T_test_expr_flt_result
		{
			const char  * expr;
			double        expected_result;
		};

		const T_test_expr_flt_result    expressions[] = {
			{ "27.1", 27.1 },
			{ "-234.9", -234.9 },
			{ "-234.9E3", -234900.0 },
			{ "-234.9E-1", -23.49 },     // exception
			{ "-234.9E+1", -2349.0 },    // exception
			{ "23+4.0", 27.0 },
			{ "-1345.0-65", -1410.0 },
			{ "234 * 2.003 - 5 + 1", 464.702 },
			{ "0x045 + 1.0", 70.0 },
			{ "-0Xdf / 1.0", -223.0 },
			{ "-0Xdf // 1", -223.0 },      // // always gives float
			{ "-0Xde / 2.0", -111.0 },
			{ "-0Xde // 2", -111.0 },      // // always gives float
			{ "(7 / 3) // 1", 2.0 },       // // always gives float
			{ "007 * 1.0", 7.0 },
			{ "03456 - 1.0", 1837.0 },
			{ "235.9 + k::flt", 1 },			// const
			{ " addition (3.0, 7) ", 10}
		};
		for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
		{
			const char   * expr            = expressions[idx].expr;
			const double   expected_result = expressions[idx].expected_result;

			cout << "flt expression : ";
			cout.width(25);
			cout << expr;
			if ((idx == 3) || (idx == 4)) 
			{
				M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static (type_definitions, expr));
				continue;
			}
			C_value    value = compute_expression_static (type_definitions, expr);
			cout << "    gives " << value.as_string();
			M_TEST_EQ(value.get_flt(), expected_result);
			M_TEST_EQ(value.get_bit_position_offset(), -1);
			M_TEST_EQ(value.get_bit_position_size(), -1);

			T_expression    expression;
			expression.build_expression(type_definitions, expr);
			C_value    value_new = expression.compute_expression_static(type_definitions);
			if (value_new.get_flt() != expected_result)
			{
				cout << "    and " << value_new.as_string();
			}
			M_TEST_EQ(value_new.get_flt(), expected_result);
			M_TEST_EQ(value_new.get_bit_position_offset(), -1);
			M_TEST_EQ(value_new.get_bit_position_size(), -1);

			cout << endl;
		}
	}

	//-------------------------------------------------------------------------
	// string static expression
	//-------------------------------------------------------------------------
	{
		struct T_test_expr_str_result
		{
			const char  * expr;
			const char  * expected_result;
		};

		const T_test_expr_str_result    expressions[] = {
			{ "\"salut\"", "salut" },
			{ "\"Hello world!\"", "Hello world!" },
			{ "\"Hello \" + \"world!\"", "Hello world!" },
			{ "\"Hello \"+ \"world!\"", "Hello world!" },
			{ "\"Hello \" +\"world!\"", "Hello world!" },
			{ "\"Hello \"+\"world!\"", "Hello world!" },
			{ "(\"Hello \" + \"world!\")", "Hello world!" },
			{ "(\"Hello \"+ \"world!\")", "Hello world!" },
			{ "(\"Hello \" +\"world!\")", "Hello world!" },
			{ "(\"Hello \"+\"world!\")", "Hello world!" },
			{ "\"last test\"", "last test" },
			{ "\"136\" + k::str", "136hello world" },			// const
			{ " addition (\"3.0\", \"7\") ", "3.07"}
		};
		for (int  idx = 0; idx < sizeof(expressions)/sizeof(expressions[0]); ++idx)
		{
			const char  * expr            = expressions[idx].expr;
			const string  expected_result = expressions[idx].expected_result;

			cout << "str expression : ";
			cout.width(25);
			cout << expr;
			C_value    value = compute_expression_static (type_definitions, expr);
			cout << "    gives : " << value.as_string();
			M_TEST_EQ(value.get_str(), expected_result);
			M_TEST_EQ(value.get_bit_position_offset(), -1);
			M_TEST_EQ(value.get_bit_position_size(), -1);

			T_expression    expression;
			expression.build_expression(type_definitions, expr);
			C_value    value_new = expression.compute_expression_static(type_definitions);
			if (value_new.get_str() != expected_result)
			{
				cout << "    and " << value_new.as_string();
			}
			M_TEST_EQ(value_new.get_str(), expected_result);
			M_TEST_EQ(value_new.get_bit_position_offset(), -1);
			M_TEST_EQ(value_new.get_bit_position_size(), -1);

			cout << endl;
		}
	}

	//-------------------------------------------------------------------------
	// enum static expression
	//-------------------------------------------------------------------------
	{
		C_value    value = compute_expression_static (type_definitions, "T_Operation::MODIFICATION");
		M_TEST_EQ(value.get_int(), 1);
		M_TEST_EQ(value.get_bit_position_offset(), -1);
		M_TEST_EQ(value.get_bit_position_size(), -1);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "T_Operation::MODIFICATION");
		C_value    value = expression.compute_expression_static (type_definitions);
		M_TEST_EQ(value.get_int(), 1);
		M_TEST_EQ(value.get_bit_position_offset(), -1);
		M_TEST_EQ(value.get_bit_position_size(), -1);
		value.as_string();
	}

	//-------------------------------------------------------------------------
	// NOT static expression
	//-------------------------------------------------------------------------
	T_interpret_data                  interpret_data;

	interpret_data.read_variable_group_begin("niv");
	interpret_data.add_read_variable("niv.salut", "salut", get_int_value_bit_pos_size(-32,12,7));
	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut");
		M_TEST_EQ(value.get_int(), -32);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 7);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "salut");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), -32);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 7);
		value.as_string();
	}

	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "niv.salut");
		M_TEST_EQ(value.get_int(), -32);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 7);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "niv.salut");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), -32);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 7);
		value.as_string();
	}

	interpret_data.read_variable_group_begin("e2");
	interpret_data.add_read_variable("niv.e2.toi", "toi", get_int_value_bit_pos_size(-3450000,19,15));
	interpret_data.read_variable_group_end();
	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut * e2.toi");
		M_TEST_EQ(value.get_int(), 110400000);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 22);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "salut * e2.toi");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 110400000);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 22);
		value.as_string();
	}

	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut * e2.toi + T_Operation::DELETION");
		M_TEST_EQ(value.get_int(), 110400000 + 2);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 22);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "salut * e2.toi + T_Operation::DELETION");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 110400000 + 2);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 22);
		value.as_string();
	}

	interpret_data.read_variable_group_begin("eau");
	interpret_data.add_read_variable("niv.eau.la", "la", get_int_value_bit_pos_size(3402340,30,31));
	interpret_data.read_variable_group_end();
	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "salut * e2.toi / eau.la");
		M_TEST_EQ(value.get_int(), 32);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 49);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "salut * e2.toi / eau.la");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 32);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 49);
		value.as_string();
	}

	interpret_data.add_read_variable("niv.matelas", "matelas", get_flt_value_bit_pos_size(45.3465,67,2));
	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "niv.salut * niv.e2.toi / niv.eau.la - niv.matelas");
		M_TEST_EQ_APPROX(value.get_flt(), -13.3465);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 57);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "niv.salut * niv.e2.toi / niv.eau.la - niv.matelas");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ_APPROX(value.get_flt(), -13.3465);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 57);
		value.as_string();
	}

	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "addition(niv.salut * niv.e2.toi, niv.eau.la / niv.matelas)");
		M_TEST_EQ_APPROX(value.get_flt(), 110475607.5556);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 57);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "addition(niv.salut * niv.e2.toi, niv.eau.la / niv.matelas)");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ_APPROX(value.get_flt(), 110475607.5556);
		M_TEST_EQ(value.get_bit_position_offset(), 12);
		M_TEST_EQ(value.get_bit_position_size(), 57);
		value.as_string();
	}

	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "perf(niv.eau.la, 3402341)");
		M_TEST_EQ(value.get_int(), 6804680);
		M_TEST_EQ(value.get_bit_position_offset(), 30);
		M_TEST_EQ(value.get_bit_position_size(), 31);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "perf(niv.eau.la, 3402341)");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 6804680);
		M_TEST_EQ(value.get_bit_position_offset(), 30);
		M_TEST_EQ(value.get_bit_position_size(), 31);
		value.as_string();
	}

	//-------------------------------------------------------------------------
	// Test array
	//-------------------------------------------------------------------------
	interpret_data.add_read_variable("NOT used", "an_array[0]", 1000);
	interpret_data.add_read_variable("NOT used", "an_array[1]", 1001);
	interpret_data.add_read_variable("NOT used", "an_array[2]", 1002);
	interpret_data.add_read_variable("NOT used", "an_array[3]", 1003);
	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[0]");
		M_TEST_EQ(value.get_int(), 1000);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "an_array[0]");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 1000);
		value.as_string();
	}

	interpret_data.add_read_variable("NOT used", "idx", 1);
	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[idx]");
		M_TEST_EQ(value.get_int(), 1001);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "an_array[idx]");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 1001);
		value.as_string();
	}

	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[idx-1]");
		M_TEST_EQ(value.get_int(), 1000);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "an_array[idx-1]");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 1000);
		value.as_string();
	}

	{
		C_value   value = compute_expression_no_io (type_definitions, interpret_data, "an_array[idx+2]");
		M_TEST_EQ(value.get_int(), 1003);
		value.as_string();
	}
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "an_array[idx+2]");
		C_value    value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 1003);
		value.as_string();
	}

	//-------------------------------------------------------------------------
	// NOT static expression build and compute
	//-------------------------------------------------------------------------
	{
		T_expression    expression;
		expression.build_expression(type_definitions, "another_value == 0 ? 4 : 6");

		C_value    value;

		interpret_data.add_read_variable("NOT used", "another_value", 1);
		value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 6);
		value.as_string();

		interpret_data.add_read_variable("NOT used", "another_value", 0);
		value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 4);
		value.as_string();

		interpret_data.add_read_variable("NOT used", "another_value", 2);
		value = expression.compute_expression_no_io (type_definitions, interpret_data);
		M_TEST_EQ(value.get_int(), 6);
		value.as_string();
	}

	//-------------------------------------------------------------------------
	// perf expression
	//-------------------------------------------------------------------------
	test_expression_perf_operation(type_definitions);
	test_expression_perf_function(type_definitions);
}

//*****************************************************************************
// 
//*****************************************************************************
T_type_definitions  * SP_type_definitions = NULL;
T_interpret_data    * SP_interpret_data = NULL;

long long  compute_expression_static_int(
				const string              & expression_str)
{
	T_expression    expression;
	expression.build_expression(*SP_type_definitions, expression_str);

	C_value    value = expression.compute_expression_static(*SP_type_definitions);

	return  value.get_int();
}
long long  compute_expression_no_io_int(
				const string              & expression_str)
{
	T_expression    expression;
	expression.build_expression(*SP_type_definitions, expression_str);

	C_value    value = expression.compute_expression_no_io(*SP_type_definitions, *SP_interpret_data);

	return  value.get_int();
}

double  compute_expression_static_flt(
				const string              & expression_str)
{
	T_expression    expression;
	expression.build_expression(*SP_type_definitions, expression_str);

	C_value    value = expression.compute_expression_static(*SP_type_definitions);

	return  value.get_flt();
}
double  compute_expression_no_io_flt(
				const string              & expression_str)
{
	T_expression    expression;
	expression.build_expression(*SP_type_definitions, expression_str);

	C_value    value = expression.compute_expression_no_io(*SP_type_definitions, *SP_interpret_data);

	return  value.get_flt();
}

string  compute_expression_static_str(
				const string              & expression_str)
{
	T_expression    expression;
	expression.build_expression(*SP_type_definitions, expression_str);

	C_value    value = expression.compute_expression_static(*SP_type_definitions);

	return  value.get_str();
}
string  compute_expression_no_io_str(
				const string              & expression_str)
{
	T_expression    expression;
	expression.build_expression(*SP_type_definitions, expression_str);

	C_value    value = expression.compute_expression_no_io(*SP_type_definitions, *SP_interpret_data);

	return  value.get_str();
}

//*****************************************************************************
// test_function_call
//*****************************************************************************

void    test_function_call()
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);
	SP_type_definitions = &type_definitions;

	T_interpret_data      interpret_data;
    SP_interpret_data = &interpret_data;

	M_TEST_EQ(compute_expression_static_int("call_me_maybe_u32(1234)"), 32);
	M_TEST_EQ(compute_expression_static_int("call_me_maybe_u24(1234)"), 24);
	M_TEST_EQ(compute_expression_static_int("call_me_maybe_u16(1234)"), 16);
	M_TEST_EQ(compute_expression_static_int("call_me_maybe_u8 ( 234)"),  8);

	// 1234 gives u8 overflow
    M_TEST_CATCH_EXCEPTION(compute_expression_static_int("call_me_maybe_u8 (1234)"), C_byte_interpret_exception);

	// Currently, parameter must be an entire byte size
	M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u48(1234)"));
	M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u30(1234)"));
	M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u20(1234)"));
	M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u10( 934)"));
	M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u2 (   3)"));
	M_TEST_EXCEPTION_ALREADY_KNOWN(compute_expression_static_int("call_me_maybe_u1 (   1)"));
}

//*****************************************************************************
// test_builtin_functions
//*****************************************************************************

void    test_builtin_functions()
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);
	SP_type_definitions = &type_definitions;

	T_interpret_data      interpret_data;
    SP_interpret_data = &interpret_data;
	interpret_data.add_read_variable("NOT used", "int", 0);
	interpret_data.add_read_variable("NOT used", "flt", 0.0);
	interpret_data.add_read_variable("NOT used", "str", "");

    // to_string
	M_TEST_EQ(compute_expression_static_str("to_string(1234)"), "1234");
	M_TEST_EQ(compute_expression_static_str("to_string (1234) + \"5\""), "12345");
	M_TEST_EQ(compute_expression_static_str("to_string (12.34)"), "12.34");
	interpret_data.set_read_variable("int", -136);
	interpret_data.set_read_variable("flt", -136.2);
	M_TEST_EQ(compute_expression_no_io_str("to_string (int+5) + \"5\""), "-1315");
	M_TEST_EQ(compute_expression_no_io_str("to_string(flt+5)"), "-131.2");
  
    // to_numeric
	M_TEST_EQ(compute_expression_static_int("to_numeric (\"12\" + \"34\")"), 1234);
	M_TEST_EQ(compute_expression_static_int("to_numeric (\"12\" + \"3\") * 3"), 369);
	interpret_data.set_read_variable("str", "123");
	M_TEST_EQ(compute_expression_no_io_int("to_numeric (str + \"3\")"), 1233);

	M_TEST_EQ(compute_expression_static_flt("to_numeric (\"1.2\" + \"34\")"), 1.234);
	M_TEST_EQ(compute_expression_static_flt("to_numeric (\"1.2\" + \"3\") / 2"), 0.615);
	interpret_data.set_read_variable("str", "123");
	M_TEST_EQ(compute_expression_no_io_flt("to_numeric (str + \".2\" + \"3\")"), 123.23);

    // to_integer
	M_TEST_EQ(compute_expression_static_int("to_integer (\"12\" + \"34\")"), 1234);
	M_TEST_EQ(compute_expression_static_int("to_integer (\"12\" + \"3\") * 3"), 369);
	interpret_data.set_read_variable("str", "123");
	M_TEST_EQ(compute_expression_no_io_int("to_integer (str + \"3\")"), 1233);
    // to_integer from float (can loose data) inside string or not
	M_TEST_EQ(compute_expression_static_int("to_integer (\"1.2\" + \"34\")"), 1);   //crash
	M_TEST_EQ(compute_expression_static_int("to_integer (1.2 + 34)"), 35);

    // to_float
	M_TEST_EQ(compute_expression_static_flt("to_float (\"1.2\" + \"34\")"), 1.234);
	M_TEST_EQ(compute_expression_static_flt("to_float (\"1.2\" + \"3\") / 2"), 0.615);
	interpret_data.set_read_variable("str", "123");
	M_TEST_EQ(compute_expression_no_io_flt("to_float (str + \".2\" + \"3\")"), 123.23);
	// to_float from int (can loose precision) inside string or not
	M_TEST_EQ(compute_expression_static_flt("to_float (\"12\" + \"34\")"), 1234.0);
	M_TEST_EQ(compute_expression_static_flt("to_float (12 + 34)"), 46.0);

	// getenv
	compute_expression_static_str("getenv (\"USERNAME\")");

    // string.length
	M_TEST_EQ(compute_expression_static_int("string.length (\"1234\")"), 4);
	M_TEST_EQ(compute_expression_static_int("string.length (\"abcde\")"), 5);
	interpret_data.set_read_variable("str", "123");
	M_TEST_EQ(compute_expression_no_io_int("string.length (str)"), 3);

    // string.substr
	M_TEST_EQ(compute_expression_static_str("string.substr (\"12345678\", 2)"), "345678");
	M_TEST_EQ(compute_expression_static_str("string.substr (\"abcdefgh\", 1, 3)"), "bcd");
	interpret_data.set_read_variable("str", "abcdefgh");
	interpret_data.set_read_variable("int", 1);
	M_TEST_EQ(compute_expression_no_io_str("string.substr (str, int, 3)"), "bcd");

    // string.erase
	M_TEST_EQ(compute_expression_static_str("string.erase (\"12345678\", 2)"), "12");
	M_TEST_EQ(compute_expression_static_str("string.erase (\"abcdefgh\", 1, 3)"), "aefgh");
	interpret_data.set_read_variable("str", "abcdefgh");
	interpret_data.set_read_variable("int", 1);
	M_TEST_EQ(compute_expression_no_io_str("string.erase (str, int, 3)"), "aefgh");

    // string.insert
	M_TEST_EQ(compute_expression_static_str("string.insert (\"12345678\", 2, \"yo\")"), "12yo345678");
	interpret_data.set_read_variable("str", "yo");
	interpret_data.set_read_variable("int", 2);
	M_TEST_EQ(compute_expression_no_io_str("string.insert (\"12345678\", int, str)"), "12yo345678");

    // string.replace
	M_TEST_EQ(compute_expression_static_str("string.replace (\"abcdefgh\", 1, 3, \"123\")"), "a123efgh");
	interpret_data.set_read_variable("str", "123");
	M_TEST_EQ(compute_expression_no_io_str("string.replace (\"abcdefgh\", 1, 3, str)"), "a123efgh");

    // string.replace_all
	M_TEST_EQ(compute_expression_static_str("string.replace_all (\"abcdefgh - abcdefgh\", \"bc\", \"xyz\")"), "axyzdefgh - axyzdefgh");
	interpret_data.set_read_variable("str", "abcdefgh - abcdefgh");
	M_TEST_EQ(compute_expression_no_io_str("string.replace_all (str, \" \", \"\")"), "abcdefgh-abcdefgh");

    // string.find
	M_TEST_EQ(compute_expression_static_int("string.find (\"12345678\", \"2\")"), 1);
	M_TEST_EQ(compute_expression_static_int("string.find (\"abcdefgh\", \"ef\")"), 4);
	M_TEST_EQ(compute_expression_static_int("string_find (\"abcdefgh\", \"ef\")"), 4);
	M_TEST_EQ(compute_expression_static_int("string_found(\"abcdefgh\", \"ef\")"), true);
	M_TEST_EQ(compute_expression_static_int("string.find (\"abcdefgh\", \"fe\")"), compute_expression_static_int("string::npos"));
	M_TEST_EQ(compute_expression_static_int("string_find (\"abcdefgh\", \"fe\")"), compute_expression_static_int("string::npos"));
	M_TEST_EQ(compute_expression_static_int("string_found(\"abcdefgh\", \"fe\")"), false);
	interpret_data.set_read_variable("str", "abcdefgh");
	M_TEST_EQ(compute_expression_no_io_int("string.find (str, \"ef\")"), 4);


	// date.get_string_from_days
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,   0)"), "2012/01/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,   1)"), "2012/01/02");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  31)"), "2012/02/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  59)"), "2012/02/29");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  60)"), "2012/03/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012,  91)"), "2012/04/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 121)"), "2012/05/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 152)"), "2012/06/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 182)"), "2012/07/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 213)"), "2012/08/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 244)"), "2012/09/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 274)"), "2012/10/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 305)"), "2012/11/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 335)"), "2012/12/01");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_days (2012, 366)"), "2013/01/01");

	// date.get_string_from_seconds
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 0)"), "1970/01/01 00:00:00");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 4133980800)"), "2101/01/01 00:00:00");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 4134067199)"), "2101/01/01 23:59:59");
	M_TEST_EQ(compute_expression_static_str("date.get_string_from_seconds (1970, 13601174399)"), "2401/01/01 23:59:59");
}

//*****************************************************************************
// test_value_format
//*****************************************************************************

void    test_value_format()
{
	C_value            value;
	string             format;
	
#define M_TEST_OK(VALUE,RESULT)                                        \
	value = C_value(VALUE);                                            \
	M_TEST_EQ(value.as_string(), RESULT)

	M_TEST_OK( 123456789101112,     "123456789101112");
	M_TEST_OK( 1234567891011121314, "1234567891011121314");
	M_TEST_OK( -4427611715, "-4427611715");


#undef  M_TEST_OK

#define M_TEST_OK(VALUE,FORMAT,RESULT)                                 \
	value = C_value(VALUE);                                            \
	format = FORMAT;                                                   \
	promote_printf_string_to_64bits(format);                           \
	value.format(format);                                              \
	M_TEST_EQ(value.as_string(), RESULT)


	M_TEST_OK( 123, "hex", "0x7b");
	M_TEST_OK( 123, "oct", "0173");
	M_TEST_OK( 123, "bin", "b1111011");

//	M_TEST_OK(-123, "hex", "-0x7b");     // donne 0xffffff85
//	M_TEST_OK(-123, "oct", "-0173");     // donne 037777777605
//	M_TEST_OK(-123, "bin", "-b1111011"); // donne b1111111111111111111111111111111111111111111111111111111110000101

	M_TEST_OK( 123, "augmentation = %d%% (non consolide)", "augmentation = 123% (non consolide)");
	M_TEST_OK( 123, "augmentation = %s%% (non consolide)", "augmentation = 123% (non consolide)");

	M_TEST_OK( 123456789101112    , "%d", "123456789101112");
	M_TEST_OK( 1234567891011121314, "%d", "1234567891011121314");
	M_TEST_OK( -4427611715        , "%d", "-4427611715");

#undef  M_TEST_OK
}

//*****************************************************************************
// test_value_printf
//*****************************************************************************

void    test_value_printf()
{
	C_value            value;
	vector<C_value>    values;
	string             format;

#define M_TEST_OK(FORMAT,VALUE,RESULT)                                 \
	values.clear();                                                    \
	values.push_back(C_value(VALUE));                                  \
	format = FORMAT;                                                   \
	promote_printf_string_to_64bits(format);                           \
	value = C_value::sprintf_values(format,values);                    \
	M_TEST_EQ(value.as_string(), RESULT)


	M_TEST_OK("%d",   123, "123");
	M_TEST_OK("0x%x", 123, "0x7b");
	M_TEST_OK("0%o",  123, "0173");
	M_TEST_OK("%s",   123, "123");
	M_TEST_OK("%f",   123.0, "123.000000");     // NB: do not care about precise format
#if defined WIN32
	M_TEST_OK("%e",   123.0, "1.230000e+002");  // NB: do not care about precise format
#else
	M_TEST_OK("%e",   123.0, "1.230000e+02");  // NB: do not care about precise format
#endif

	M_TEST_OK("%d",   -123, "-123");
//	M_TEST_OK("0x%x", -123, "-0x7b");     // donne 0xffffff85
//	M_TEST_OK("0%o",  -123, "-0173");     // donne 037777777605

	M_TEST_OK("%s",   -123, "-123");
	M_TEST_OK("%f",   -123.0, "-123.000000");     // NB: do not care about precise format
#if defined WIN32
	M_TEST_OK("%e",   -123.0, "-1.230000e+002");  // NB: do not care about precise format
#else
	M_TEST_OK("%e",   -123.0, "-1.230000e+02");  // NB: do not care about precise format
#endif

	M_TEST_OK("augmentation = %d%% (non consolide)", 123, "augmentation = 123% (non consolide)");
	M_TEST_OK("augmentation = %s%% (non consolide)", 123, "augmentation = 123% (non consolide)");

	M_TEST_OK("%d",  123456789101112    , "123456789101112");
	M_TEST_OK("%d",  1234567891011121314, "1234567891011121314");
	M_TEST_OK("%d",  -4427611715        , "-4427611715");

#undef  M_TEST_OK
}

//*****************************************************************************
// ut_interpret_bytes_base ****************************************************
//*****************************************************************************

bool    ut_interpret_bytes_base (const T_type_definitions  & type_definitions,
				                 const T_byte_vector       & in_byte_vector,
						         const string              & in_input_string,
								       ostream             & os_out,
							           ostream             & os_err,
									   T_interpret_data    & interpret_data)
{
	string    conditions;
	if (interpret_data.get_decode_function().empty())
	{
		conditions += "      ";
	}
	else
	{
		conditions += "decode";
	}

	conditions += " ";

	if (get_debug() == false)
	{
		conditions += "     ";
	}
	else
	{
		conditions += "debug";
	}


	const T_byte    * P_bytes = in_byte_vector.empty() ? NULL : &in_byte_vector[0];
	size_t            sizeof_bytes = in_byte_vector.size();

	bool  result = false;
	M_STATE_WARNING("enter ut_interpret_bytes_base " << conditions << "  for " << in_input_string);
	get_state_ostream() << flush;
	{
		C_perf_chrono  perf_chrono(interpret_data, in_input_string.c_str(), sizeof_bytes);
		result = interpret_bytes (type_definitions,
									P_bytes,
									sizeof_bytes,
									in_input_string,
									os_out,
									os_err,
									interpret_data);
	}
	M_STATE_WARNING("leave ut_interpret_bytes_base " << conditions << "  for " << in_input_string);

	M_TEST_EQ(result, true);
	M_TEST_EQ(sizeof_bytes, 0);

	return  result;
}

//*****************************************************************************
// ut_interpret_bytes *********************************************************
//*****************************************************************************

bool  S_ut_interpret_bytes_decode_nothing = true;
bool  S_ut_interpret_bytes_debug          = false;

void ut_interpret_bytes (const T_type_definitions  & type_definitions,
                         const T_byte_vector       & in_byte_vector,
                         const string              & in_input_string,
							   T_interpret_data    & interpret_data,
						 const char                * output_expected = NULL)
{
	// Save original interpret_data (because of global data).
	T_interpret_data  interpret_data_save = interpret_data;

	string    output_str_normal;

	for (int  idx_decode = 0; idx_decode <= S_ut_interpret_bytes_decode_nothing; ++idx_decode)
	for (int  idx_debug  = 0; idx_debug  <= S_ut_interpret_bytes_debug; ++idx_debug)
	{
		// Restore original interpret_data (because of global data).
		interpret_data = interpret_data_save;

		ostringstream   oss;
		ostream       & os_out = oss;
		ostream       & os_err = oss;

		if (idx_decode > 0)  { interpret_data.set_decode_function("decode_stream_nothing"); }
		if (idx_debug > 0)   { set_debug(! get_debug()); }
		bool  result = ut_interpret_bytes_base (type_definitions,
										in_byte_vector,
										in_input_string,
										os_out,
										os_err,
										interpret_data);
		if (idx_debug > 0)   { set_debug(! get_debug()); }
		if (idx_decode > 0)  { interpret_data.set_decode_function(""); }
		if (result != true)
		{
			return;
		}

		if (idx_decode == 0)
		{
			// Without decoder, save output.
			output_str_normal = oss.str();

			// Compare to expected output.
			if (output_expected != NULL)
			{
				if (output_str_normal != output_expected)
				{
					string  output_expected_str = output_expected;
					string  put_a_breakpoint_here;
				}

				M_TEST_EQ(output_str_normal, output_expected);
			}
		}
		else
		{
			// With decoder
			string    output_str_decoder = oss.str();

			// Check output is identic to normal output
			M_TEST_EQ(output_str_decoder, output_str_normal);
		}
	}
}

//*****************************************************************************
// ut_interpret_bytes *********************************************************
//*****************************************************************************

void ut_interpret_bytes (const T_type_definitions  & type_definitions,
                         const char                * in_hexa_str_param,
                         const string              & in_input_string,
							   T_interpret_data    & interpret_data,
						 const char                * output_expected = NULL)
{
	string           in_hexa_str = in_hexa_str_param;
	if (in_hexa_str.find(' ') != string::npos)
	{
		mod_replace_all(in_hexa_str, " ", "");
	}

	T_byte_vector    byte_vector;
	string_hexa_to_frame(in_hexa_str, byte_vector);

	ut_interpret_bytes (type_definitions, byte_vector, in_input_string, interpret_data, output_expected);
}

//*****************************************************************************
// test_interpret_simple
//*****************************************************************************

void    test_interpret_simple()
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

	T_interpret_data      interpret_data;

#define K_eol  "\n"
#define M_TEST_SIMPLE(BIN_DATA,TO_READ,EXPECTED)    \
	ut_interpret_bytes(type_definitions, BIN_DATA, TO_READ, interpret_data, EXPECTED K_eol)


	// global and pinfo must be instantiated at the beginning
	interpret_data.global_variable_group_begin();
	M_TEST_SIMPLE("", " var uint32  msg_counter = 0 ;", "msg_counter = 0");
	interpret_data.global_variable_group_end();

	interpret_data.pinfo_variable_group_begin();
	M_TEST_SIMPLE("", " var uint32  msg_counter = 0 ;", "msg_counter = 0");
	M_TEST_SIMPLE("", " var uint32  dstport = 133 ;"  , "dstport = 133");
	M_TEST_SIMPLE("", " var uint32  srcport = 5000 ;" , "srcport = 5000");
	interpret_data.read_variable_group_begin("fd");
	M_TEST_SIMPLE("", " var string  pipo = \"str\" ;" , "pipo = str");
	interpret_data.read_variable_group_end();
	interpret_data.pinfo_variable_group_end();


	interpret_data.set_big_endian();
	M_TEST_EQ(interpret_data.is_little_endian(), false);

	// char test
M_TEST_ERROR_ALREADY_KNOWN__OPEN(3535660, "char are displayed as integer")
	M_TEST_SIMPLE("41", "char  char ;", "char = A");
	M_TEST_SIMPLE("81", "char  char ;", "char = ü");
	M_TEST_SIMPLE("81", "uchar  char ;", "char = ü");
}
	M_TEST_SIMPLE("41", "char  char ;", "char = 65");
	M_TEST_SIMPLE("81", "char  char ;", "char = -127");
	M_TEST_SIMPLE("81", "uchar  char ;", "char = 129");

	// 2010/08/16
	// no decode time = 0 ms
	//    decode time = 2 ms
	// 2010/10/16       0 ms
	M_TEST_SIMPLE("ad", "uint8  uint8 ;", "uint8 = 173");
	M_TEST_SIMPLE("ad", " int8   int8 ;", "int8 = -83");

	M_TEST_SIMPLE("c23f", "uint16  val ;", "val = 49727");
	M_TEST_SIMPLE("c23f", " int16  val ;", "val = -15809");

	M_TEST_SIMPLE("c23fde", "uint24  val ;", "val = 12730334");
	M_TEST_SIMPLE("c23fde", " int24  val ;", "val = -4046882");

	M_TEST_SIMPLE("e23f6a77", "uint32  val ;", "val = 3795806839");
	M_TEST_SIMPLE("e23f6a77", " int32  val ;", "val = -499160457");

	M_TEST_SIMPLE("e23f6a77cb", "uint40  val ;", "val = 971726550987");
	M_TEST_SIMPLE("e23f6a77cb", " int40  val ;", "val = -127785076789");

	M_TEST_SIMPLE("e23f6a77cbf3", "uint48  val ;", "val = 248761997052915");
	M_TEST_SIMPLE("e23f6a77cbf3", " int48  val ;", "val = -32712979657741");

	// 2010/08/16
	// no decode time =  0 ms
	//    decode time = 18 ms
	// 2010/09/25       14 ms
	// 2010/10/14       13 ms
	// 2010/10/16        0 ms
	M_TEST_SIMPLE("e23f6a77cbf367a9", "int64  val ;", "val = -2143877834849687639");

	// ATTENTION : NOT really checked
#if defined WIN32
	M_TEST_SIMPLE("e23f6a77", "float32  val ;", "val = -8.8275e+020");
#else
	M_TEST_SIMPLE("e23f6a77", "float32  val ;", "val = -8.8275e+20");
#endif
	M_TEST_SIMPLE("e23f6a77cbf367a9", "float64  val ;", "val = -1.80912e+165");


	// local byte order
	M_TEST_SIMPLE("776a3fe2", "uint32{byte_order=little_endian}  val ;", "val = 3795806839");
	M_TEST_SIMPLE("776a3fe2", " int32{byte_order=little_endian}  val ;", "val = -499160457");

	M_TEST_EQ(interpret_data.is_little_endian(), false);


	interpret_data.set_little_endian();
	M_TEST_EQ(interpret_data.is_little_endian(), true);

	// integer
	M_TEST_SIMPLE("ad", "uint8  uint8 ;", "uint8 = 173");
	M_TEST_SIMPLE("ad", " int8   int8 ;", "int8 = -83");

	M_TEST_SIMPLE("3fc2", "uint16  val ;", "val = 49727");
	M_TEST_SIMPLE("3fc2", " int16  val ;", "val = -15809");

	M_TEST_SIMPLE("de3fc2", "uint24  val ;", "val = 12730334");
	M_TEST_SIMPLE("de3fc2", " int24  val ;", "val = -4046882");

	M_TEST_SIMPLE("776a3fe2", "uint32  val ;", "val = 3795806839");
	M_TEST_SIMPLE("776a3fe2", " int32  val ;", "val = -499160457");

	M_TEST_SIMPLE("cb776a3fe2", "uint40  val ;", "val = 971726550987");
	M_TEST_SIMPLE("cb776a3fe2", " int40  val ;", "val = -127785076789");

	M_TEST_SIMPLE("f3cb776a3fe2", "uint48  val ;", "val = 248761997052915");
	M_TEST_SIMPLE("f3cb776a3fe2", " int48  val ;", "val = -32712979657741");

	M_TEST_SIMPLE("a967f3cb776a3fe2", "int64  val ;", "val = -2143877834849687639");

	// float
	// ATTENTION : NOT really checked
#if defined WIN32
	M_TEST_SIMPLE("776a3fe2", "float32  val ;", "val = -8.8275e+020");
#else
	M_TEST_SIMPLE("776a3fe2", "float32  val ;", "val = -8.8275e+20");
#endif
	M_TEST_SIMPLE("a967f3cb776a3fe2", "float64  val ;", "val = -1.80912e+165");

	// string
	M_TEST_SIMPLE("4249472d5245515545535453", "string(12)  val ;", "val = BIG-REQUESTS");
	M_TEST_SIMPLE("424947005245515545535453", "string(12)  val ;", "val = BIG");
	M_TEST_SIMPLE(""                        , "string(0)   val ;", "val = ");
	M_TEST_SIMPLE("42494700"                , "string      val ;", "val = BIG");
	M_TEST_SIMPLE("424947005245515545535453",
				  "string      val ; string(8)  val2 ;",
				  "val = BIG" K_eol "val2 = REQUESTS");
	M_TEST_SIMPLE("424947005245515545535400",
				  "string()    val ; string()  val2 ;",
				  "val = BIG" K_eol "val2 = REQUEST");
	M_TEST_SIMPLE("424947005245515545535400",
				  "string()    val ; string(0)  val2 ; string()  val3 ;",
				  "val = BIG" K_eol "val2 = " K_eol "val3 = REQUEST");
	// string at bit position
	M_TEST_SIMPLE("b4249472d5245515545535453e",
				  "uint4  begin; string(12)  val ; uint4  end;",
				  "begin = 11" K_eol
				  "val = BIG-REQUESTS" K_eol
				  "end = 14");

	// raw
	M_TEST_SIMPLE("4249472d5245515545535453",
				  "raw(12)  val ;",
				  "val = " K_eol
				  "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    ");
	M_TEST_SIMPLE("4249472d5245515545535453",
				  "raw(*)  val ;",
				  "val = " K_eol
				  "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    ");
	M_TEST_SIMPLE("4249472d5245515545535453",
				  "raw(2)  val ; raw(*)  val2 ;",
				  "val = " K_eol
				  "00000000 : 42 49                                            - BI              " K_eol
  				  "val2 = " K_eol
				  "00000000 : 47 2d 52 45 51 55 45 53 54 53                    - G-REQUESTS      ");
	M_TEST_SIMPLE("4249472d5245515545535453",
				  "raw(0)  val ; raw(*)  val2 ;",
				  "val2 = " K_eol
				  "00000000 : 42 49 47 2d 52 45 51 55 45 53 54 53              - BIG-REQUESTS    ");

	// int2 ...
	M_TEST_SIMPLE("f3", "uint4  val ; uint4  val2 ;",
				  "val = 15" K_eol
				  "val2 = 3");
	M_TEST_SIMPLE("f3", "int4  val ; int4  val2 ;",
				  "val = -1" K_eol
				  "val2 = 3");
	M_TEST_SIMPLE("f3cb",
				  "uint1  val ; uint2  val2; uint3  val3; uint4  val4; uint5  val5; uint1  val6;",
				  "val = 1" K_eol
				  "val2 = 3" K_eol
				  "val3 = 4" K_eol
				  "val4 = 15" K_eol
				  "val5 = 5" K_eol
				  "val6 = 1");
//	M_TEST_SIMPLE("f3cb776a3fe2", "uint48  val ;", "val = 248761997052915");

	// enum
	M_TEST_SIMPLE("de3fc2", "T_enum24                         val ;", "val = val12730334 (12730334)");
	M_TEST_SIMPLE("c23fde", "T_enum24{byte_order=big_endian}  val ;", "val = val12730334 (12730334)");

	M_TEST_SIMPLE("f3cb",
				  "T_enum1  val ; T_enum2  val2; T_enum3  val3; T_enum4  val4; T_enum5  val5; T_enum1  val6;",
				  "val = value1 (1)" K_eol
				  "val2 = enu3 (3)" K_eol
				  "val3 = val4 (4)" K_eol
				  "val4 = Val15 (15)" K_eol
				  "val5 = val5 (5)" K_eol
				  "val6 = value1 (1)");

	// ICIOA ...

	// transform quantum/offset
	M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}    val ;", "val = 99467 (49727)");
	M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.0}  val ;", "val = -31605 (-15809)");
	M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.1}  val ;", "val = -31604.9 (-15809)");
	M_TEST_SIMPLE("3fc2", " int16{q=1+1:o=10.1+1*3}  val ;", "val = -31604.9 (-15809)");
	M_TEST_SIMPLE("3fc2", " int16{q=konst::int+0:o=konst::flt+15.7}  val ;", "val = -31604.9 (-15809)");
	M_TEST_SIMPLE("02"  , "T_enum8{q=2:o=11}    val ;", "val = Val15 (2)");

	// transform expression
	M_TEST_SIMPLE("3fc2", "uint16{tei=2*this+13}    val ;", "val = 99467 (49727)");
	M_TEST_SIMPLE("3fc2", " int16{tef=2*this+13.0}  val ;", "val = -31605 (-15809)");
	M_TEST_SIMPLE("3fc2", " int16{tef=2*this+13.1}  val ;", "val = -31604.9 (-15809)");
	M_TEST_SIMPLE("3fc2", " int16{tef=addition(2*this,13.1)}  val ;", "val = -31604.9 (-15809)");
	M_TEST_SIMPLE("02"  , "T_enum8{tei=2*this+11}    val ;", "val = Val15 (2)");

	// decoder
	M_TEST_SIMPLE("3fc2", "uint16{decoder=nil}  val ;", "val = 49727");
	M_TEST_SIMPLE("3fc2", " int16{decoder=nil}  val ;", "val = -15809");
	M_TEST_SIMPLE("3fc2", "uint16{decoder=decode_stream_nothing}  val ;", "val = 49727");
	M_TEST_SIMPLE("3fc2", " int16{decoder=decode_stream_nothing}  val ;", "val = -15809");
	// 0x3f | 0xC2 = 0xFF
	// 0x3f & 0xC2 = 0x02
	// 0x02ff = 767
	M_TEST_SIMPLE("3fc2", "uint16{decoder=decode_stream_test16}  val ;", "val = 767");
	M_TEST_SIMPLE("3fc2", " int16{decoder=decode_stream_test16}  val ;", "val = 767");

	// stringUtf8, stringUtf16Le & stringUtf16Be using decoder
	// string original = abcde\r\n12345\r\n&é"'(-è_çà)=\r\n€\r\n^$ù*,;:!\r\n¨£%µ?./§
	// Characters not directly transformable to ascii are replaced by "."
	M_TEST_SIMPLE("61626364650D0A31323334350D0A26C3A92227282DC3A85FC3A7C3A0293D0D0AE282AC0D0A5E24C3B92A2C3B3A210D0AC2A8C2A325C2B53F2E2FC2A7",
		" stringUtf8     val ;",
		"val = abcde\r\n12345\r\n&.\"'(-._..)=\r\n.\r\n^$.*,;:!\r\n..%.?./.");
	M_TEST_SIMPLE("610062006300640065000D000A00310032003300340035000D000A002600E9002200270028002D00E8005F00E700E00029003D000D000A00AC200D000A005E002400F9002A002C003B003A0021000D000A00A800A3002500B5003F002E002F00A700",
		" stringUtf16Le  val ;",
		"val = abcde\r\n12345\r\n&é\"'(-è_çà)=\r\n.\r\n^$ù*,;:!\r\n¨£%µ?./§");
	M_TEST_SIMPLE("00610062006300640065000D000A00310032003300340035000D000A002600E9002200270028002D00E8005F00E700E00029003D000D000A20AC000D000A005E002400F9002A002C003B003A0021000D000A00A800A3002500B5003F002E002F00A7",
		" stringUtf16Be  val ;",
		"val = abcde\r\n12345\r\n&é\"'(-è_çà)=\r\n.\r\n^$ù*,;:!\r\n¨£%µ?./§");


	// min/max ok
	M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}{min=99000}    val ;", "val = 99467 (49727)");
	M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.6}{max=-30000} val ;", "val = -31604.4 (-15809)");
	M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}{min=99000:max=99467}       val ;", "val = 99467 (49727)");
	M_TEST_SIMPLE("3fc2", " int16{q=2:o=13.6}{min=-31604.5:max=-30000} val ;", "val = -31604.4 (-15809)");

	// min/max ERROR
	M_TEST_SIMPLE("3fc2", "uint16{q=2:o=13}{min=99468}    val ;", "val = 99467 (49727)\tERROR is < to 99468");

	// display
	M_TEST_SIMPLE("3fc2", "uint16{d=hex}  val ;", "val = 0xc23f (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=oct}  val ;", "val = 0141077 (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=bin}  val ;", "val = b1100001000111111 (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=%s}   val ;", "val = 49727");
	M_TEST_SIMPLE("3fc2", "uint16{d=%dmeters}   val ;", "val = 49727meters (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=%d meters}   val ;", "val = 49727 meters (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=%.1f}   val ;", "val = 49727.0 (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=%07d} val ;", "val = 0049727 (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=%7d}  val ;", "val =   49727 (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{d=%03d} val ;", "val = 49727");
	M_TEST_SIMPLE("3fc2", "uint16{d=%3d}  val ;", "val = 49727");
	M_TEST_SIMPLE("de3fc2", "T_enum24{d=%d and ...}  val ;", "val = 12730334 and ... (12730334)");

	// display expression
	M_TEST_SIMPLE("3fc2", "uint16{de=\"123\"}  val ;", "val = 123 (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{de=\"HelloWorld\"}  val ;", "val = HelloWorld (49727)");
	M_TEST_SIMPLE("3fc2", "uint16{de=\"hello world\"}  val ;", "val = hello world (49727)");  // space NOT ok
	M_TEST_SIMPLE("3fc2", "uint16{de=to_string(this)}  val ;", "val = 49727");
	M_TEST_SIMPLE("3fc2", "uint16{de=print_ip(this)}  val ;", "val = 0.0.194.63 (49727)");
	M_TEST_SIMPLE("776a3fe2", "uint32{de=print_ip(this)}  val ;", "val = 226.63.106.119 (3795806839)");
	M_TEST_SIMPLE("de3fc2", "T_enum24{de=\"hello world\"}        val ;", "val = hello world (12730334)");
	M_TEST_SIMPLE("de3fc2", "T_enum24{de=print(\"%s 0x%x %d\", this, this, this)}  val ;", "val = val12730334 0xc23fde 12730334 (12730334)");

	// struct
	M_TEST_SIMPLE("3fc2", "struct { uint8  val1; uint8 val2; }  str ;", "str.val1 = 63" K_eol "str.val2 = 194");
	M_TEST_SIMPLE("776a3fc2", "struct { uint16  val1; uint16 val2; }  str ;", "str.val1 = 27255" K_eol "str.val2 = 49727");
	M_TEST_SIMPLE("776a3fc2", "T_struct_16_16                         str ;", "str.val1 = 27255" K_eol "str.val2 = 49727");

	// struct with decoder
	M_TEST_SIMPLE("776a3fc2", "T_struct_16_16{decoder=decode_stream_nothing} str ;", "str.val1 = 27255" K_eol "str.val2 = 49727");
	// 0x77 | 0x6a = 0x7F
	// 0x77 & 0x6a = 0x62
	// 0x627F = 25215
	M_TEST_SIMPLE("776a3fc2", "T_struct_16_16{decoder=decode_stream_test16}  str ;", "str.val1 = 25215" K_eol "str.val2 = 767");

	// array
	M_TEST_SIMPLE("4249472d5245515545535453",
				  "uint8[12]  val ;",
				  "val[0] = 66" K_eol
				  "val[1] = 73" K_eol
				  "val[2] = 71" K_eol
				  "val[3] = 45" K_eol
				  "val[4] = 82" K_eol
				  "val[5] = 69" K_eol
				  "val[6] = 81" K_eol
				  "val[7] = 85" K_eol
				  "val[8] = 69" K_eol
				  "val[9] = 83" K_eol
				  "val[10] = 84" K_eol
				  "val[11] = 83");
	M_TEST_SIMPLE("4249472d5245515545535453" "4249472d5245515545535453",
				  "uint8[24]  val ;",
				  "val[0] = 66" K_eol
				  "val[1] = 73" K_eol
				  "val[2] = 71" K_eol
				  "val[3] = 45" K_eol
				  "val[4] = 82" K_eol
				  "val[5] = 69" K_eol
				  "val[6] = 81" K_eol
				  "val[7] = 85" K_eol
				  "val[8] = 69" K_eol
				  "val[9] = 83" K_eol
				  "val[10] = 84" K_eol
				  "val[11] = 83" K_eol
				  "val[12] = 66" K_eol
				  "val[13] = 73" K_eol
				  "val[14] = 71" K_eol
				  "val[15] = 45" K_eol
				  "val[16] = 82" K_eol
				  "val[17] = 69" K_eol
				  "val[18] = 81" K_eol
				  "val[19] = 85" K_eol
				  "val[20] = 69" K_eol
				  "val[21] = 83" K_eol
				  "val[22] = 84" K_eol
				  "val[23] = 83");
	// Loop which show an acumulative problem
	// 2010/08/16
	// no decode time =    6,    7,    7,    7,    8,     8,     9,     8,     9,    10 ms
	//    decode time = 2078, 3546, 5110, 6715, 8062, 10077, 12275, 14037, 16014, 17724 ms  -> +1600 ms at each test
	// 2010/09/18
	// removed "more" (a string always empty) inside T_attribute_value
	// no decode time =    6,    7,    7,    7,    8,     9,     8,     9,     9,     9 ms
	//    decode time = 2233, 3551, 5134, 6608, 8164,  9784, 11657, 13382, 15225, 16900 ms
	// -> win ~600 ms on final time (17724)
	// --> memory load is a problem
	// --> vector<T_interpret_value> into T_interpret_read_values is a problem
	// --> to not really erase a variable into T_interpret_read_values::sup_read_variable  is a problem
	// --> to not erase local variable of a function is a bigger problem
	// 2010/09/18
	// removed 48 bytes inside T_attribute_value & T_interpret_value
	// -> win between ~600 ms and zero on final time
	// --> memory load is NOT a problem ?
	// 2010/09/24
	// erase done into T_interpret_read_values::sup_read_variable
	// -> win ~3000 ms on final time (13550)
	// 2010/09/25
	// erase all data created during a function (behavior modification)
	// -> win ~11690 ms on final time (1860)
	//    acumulative problem still here
	// 2010/10/09
	// modif T_interpret_read_values::get_P_attribute_value_of_read_variable
	// -> win ~190 ms on final time (1670)
	// 2010/10/13  1622 ms
	// 2010/10/13  1654 ms  avec le nouveau code de compute_expression !!!
	// 2010/10/16
	// Into get_complex_value,
	//  move get_value_of_read_variable in 4th place (instead 1st)
	// -> final time = 358 - 395 ms
	//    no acumulative problem visible
	// 2010/10/29 265 - 280 ms  T_expression var set call
	// 2010/10/29 141 - 156 ms  T_expression condition return
	// 2010/11/10 148 - 170 ms  T_expression pre_compute value & operation

	for (int  idx_tst = 0; idx_tst < 10; ++idx_tst)
	{
		M_STATE_WARNING("msg.size=" << interpret_data.DEBUG_get_msg().size());
  	ut_interpret_bytes(type_definitions,
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453",
  					   "uint8[240]  val ;",
					   interpret_data,
  					   NULL);
  }
  
	interpret_data.read_variable_group_begin("A_current_path_not_empty");
	for (int  idx_tst = 0; idx_tst < 40; ++idx_tst)
	{
		// global
		ut_interpret_bytes(type_definitions, "", " set global.msg_counter = 0 ;", interpret_data, "");
		ut_interpret_bytes(type_definitions, "", " set global.msg_counter = global.msg_counter + 1 ;", interpret_data, "");

		// pinfo
		ut_interpret_bytes(type_definitions, "", " set pinfo.msg_counter = 0 ;", interpret_data, "");
		ut_interpret_bytes(type_definitions, "", " set pinfo.msg_counter = pinfo.msg_counter + 1 ;", interpret_data, "");
		ut_interpret_bytes(type_definitions, "", " set pinfo.srcport = pinfo.dstport ;", interpret_data, "");
		ut_interpret_bytes(type_definitions, "", " set pinfo.fd.pipo = \"a new string\" ;", interpret_data, "");
	}
	interpret_data.read_variable_group_end();
	
	type_definitions = T_type_definitions();
    build_types ("unitary_tests_example_with_capture.fdesc",
                 type_definitions);
}

//*****************************************************************************
// test_interpret_simple_int64
//*****************************************************************************

void    test_interpret_simple_int64()
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

	T_interpret_data      interpret_data;

	interpret_data.set_big_endian();

	M_TEST_SIMPLE("e23f6a77cbf367a9", "int64  val ;", "val = -2143877834849687639");
}

//*****************************************************************************
// test_interpret_simple_uint8_array
//*****************************************************************************

void    test_interpret_simple_uint8_array(const int  max_iter = 1)
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

	T_interpret_data      interpret_data;

	interpret_data.set_little_endian();

	for (int  idx_tst = 0; idx_tst < max_iter; ++idx_tst)
	{
		M_STATE_WARNING("msg.size=" << interpret_data.DEBUG_get_msg().size());
  	ut_interpret_bytes(type_definitions,
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453"
  					   "4249472d5245515545535453" "4249472d5245515545535453",
  					   "uint8[240]  val ;",
					   interpret_data,
  					   NULL);
  }
}

//*****************************************************************************
// test_interpret_simple_internal_frame
//*****************************************************************************

void    test_interpret_simple_internal_frame()
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

	T_interpret_data      interpret_data;
	interpret_data.set_big_endian();

	//*************************************************************************
	// internal_frame
	//*************************************************************************

	// No real data, data is read from internal_frame.
	M_TEST_SIMPLE("",
				  "call frame_append_hexa_data (internal_frame, \" e2 3f 6a 77 \") ; "
				  "uint32{d=hex}  val ;",
				  "val = 0xe23f6a77 (3795806839)");

	// First, data is read from internal_frame, then from real data.
	M_TEST_SIMPLE("de",
				  "call frame_append_hexa_data (internal_frame, \" e2 3f 6a 77 \") ; "
				  "uint16{d=hex}  val1 ; "
				  "uint4{d=hex}   val2 ; "
				  "uint4{d=hex}   val3 ; "
				  "uint8{d=hex}   val4 ; "
				  "uint8{d=hex}   val5 ; ",
				  "val1 = 0xe23f (57919)" K_eol
				  "val2 = 0x6 (6)" K_eol
				  "val3 = 0xa (10)" K_eol
				  "val4 = 0x77 (119)" K_eol
				  "val5 = 0xde (222)");

	//*************************************************************************
	// Decoder which append more data than asked.
	// Data goes into internal_frame.
	//*************************************************************************
	interpret_data.set_decode_function("decode_invert_4_bytes");  // read 4 bytes and invert them

	const bool  save_S_ut_interpret_bytes_decode_nothing = S_ut_interpret_bytes_decode_nothing;
	S_ut_interpret_bytes_decode_nothing = false;

	// Bytes inverted by decoder
	M_TEST_SIMPLE("776a3fe2",    // e23f6a77
				  "uint8{d=hex}  val1 ; uint8{d=hex}  val2 ; uint8{d=hex}  val3 ; uint8{d=hex}  val4 ;",
				  "val1 = 0xe2 (226)" K_eol
				  "val2 = 0x3f (63)" K_eol
				  "val3 = 0x6a (106)" K_eol
				  "val4 = 0x77 (119)");

	// Bytes inverted by decoder
	M_TEST_SIMPLE("776a3fe2", "uint32  val ;", "val = 3795806839");
	M_TEST_SIMPLE("776a3fe2", " int32  val ;", "val = -499160457");

	// val2 is over first 4 bytes and second 4 bytes
	M_TEST_SIMPLE("776a3fe2 01020304",    // e23f6a77 04030201
				  "uint24{d=hex}  val1 ; uint16{d=hex}  val2 ; uint8{d=hex}  val3 ; uint16{d=hex}  val4 ;",
				  "val1 = 0xe23f6a (14827370)" K_eol
				  "val2 = 0x7704 (30468)" K_eol
				  "val3 = 0x3 (3)" K_eol
				  "val4 = 0x201 (513)");

	// bits
	M_TEST_SIMPLE("776a3fe2 01020304",    // e23f6a77 04030201
		"uint16{d=hex}  val1 ; uint4{d=hex}  val2; uint8{d=hex}  val3; uint16{d=hex}  val4 ; uint8{d=hex}  val5 ; uint12{d=hex}  val6 ;",
				  "val1 = 0xe23f (57919)" K_eol
				  "val2 = 0x6 (6)" K_eol
				  "val3 = 0xa7 (167)" K_eol
				  "val4 = 0x7040 (28736)" K_eol
				  "val5 = 0x30 (48)" K_eol
				  "val6 = 0x201 (513)");

	// string fixed size
	M_TEST_SIMPLE("4249472d 52455155 45535453",    // 2d474942 55514552 53545345    -GIB UQER STSE
				  "string(12)  val ;",
				  "val = -GIBUQERSTSE");  // BIG-REQUESTS if not inverted
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "string(3)  val1; string(3)  val2; string(3)  val3; string(3)  val4; ",
				  "val1 = -GI" K_eol
				  "val2 = BUQ" K_eol
				  "val3 = ERS" K_eol
				  "val4 = TSE");
	// string fixed size at bit position
	M_TEST_SIMPLE("94 74 d4 b2    55 14 55 25    34 45 35 25      56 34 12 5e",  // b 4249472d 52455155 45535453 e ...
				  "uint4  begin; string(12)  val ; uint4  end; uint24{d=hex}  end2;",
				  "begin = 11" K_eol
				  "val = -GIBUQERSTSE" K_eol
				  "end = 14" K_eol
				  "end2 = 0x123456 (1193046)");

	// string unknow size
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "string  val ;",
				  "val = -GIBUQERSTSE");  // BIG-REQUESTS if not inverted
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "string(3)  val1; string  val2; ",
				  "val1 = -GI" K_eol
				  "val2 = BUQERSTSE");
	M_TEST_SIMPLE("4249472d 52005155 45535453",
				  "string(3)  val1; string  val2; string  val3; ",
				  "val1 = -GI" K_eol
				  "val2 = BUQ" K_eol
				  "val3 = RSTSE");
	// string unknow size at bit position
	M_TEST_SIMPLE("94 74 d4 b2  55 14 55 25  30 45 35 25  56 34 12 0e",  // b2 d4 74 94   25 55 14 55   25 35 45 30  0e 12 34 56
				  "uint4  begin; string  val ; uint4  end; uint24{d=hex}  end2;",
				  "begin = 11" K_eol
				  "val = -GIBUQERSTS" K_eol
				  "end = 14" K_eol
				  "end2 = 0x123456 (1193046)");
	M_TEST_SIMPLE("94 74 d4 b2  55 04 50 25  30 45 35 25  56 34 12 0e",  // b2 d4 74 94   25 50 04 55   25 35 45 30  0e 12 34 56
				  "uint4  begin; string  val1 ; string  val2 ; uint4  end; uint24{d=hex}  end2;",
				  "begin = 11" K_eol
				  "val1 = -GIBU" K_eol
				  "val2 = ERSTS" K_eol
				  "end = 14" K_eol
				  "end2 = 0x123456 (1193046)");

	// raw fixed size
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "raw(12)  val ;",
				  "val = " K_eol
				  "00000000 : 2d 47 49 42 55 51 45 52 53 54 53 45              - -GIBUQERSTSE    ");
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "raw(3)  val1; raw(3)  val2; raw(3)  val3; raw(3)  val4; ",
				  "val1 = " K_eol
				  "00000000 : 2d 47 49                                         - -GI             " K_eol
				  "val2 = " K_eol
				  "00000000 : 42 55 51                                         - BUQ             " K_eol
				  "val3 = " K_eol
				  "00000000 : 45 52 53                                         - ERS             " K_eol
				  "val4 = " K_eol
				  "00000000 : 54 53 45                                         - TSE             ");

	// raw unknow size
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "raw(*)  val ;",
				  "val = " K_eol
				  "00000000 : 2d 47 49 42 55 51 45 52 53 54 53 45              - -GIBUQERSTSE    ");
	M_TEST_SIMPLE("4249472d 52455155 45535453",
				  "raw(3)  val1; raw(*)  val2;",
				  "val1 = " K_eol
				  "00000000 : 2d 47 49                                         - -GI             " K_eol
				  "val2 = " K_eol
				  "00000000 : 42 55 51 45 52 53 54 53 45                       - BUQERSTSE       ");


	S_ut_interpret_bytes_decode_nothing = save_S_ut_interpret_bytes_decode_nothing;
}

//*****************************************************************************
// test_interpret_forget
//*****************************************************************************

void    test_interpret_forget()
{
	T_type_definitions    type_definitions;
    build_types ("unitary_tests_basic.fdesc",
                 type_definitions);

	T_interpret_data      interpret_data;

	M_TEST_SIMPLE("", "        var uint32  value = 7 ;", "value = 7");
	M_TEST_SIMPLE("", "        print (\"%d\", value);", "7");
	M_TEST_SIMPLE("", " forget var uint32  value = 3457 ;", "value = 3457");
	M_TEST_SIMPLE("", "        print (\"%d\", value);", "7");
	M_TEST_SIMPLE("", "        var uint32  value = 3 ;", "value = 3");
	M_TEST_SIMPLE("", "        print (\"%d\", value);", "3");

	// Test with struct
	M_TEST_SIMPLE("", "    var int32  struct_counter = 1000 ;", "struct_counter = 1000");
	M_TEST_SIMPLE("", "        T_forget      st1 ;", "st1.struct_counter = 1001");
	M_TEST_SIMPLE("", " forget T_forget      st2 ;", "st2.struct_counter = 1002");
	M_TEST_SIMPLE("", "        T_forget      st3 ;", "st3.struct_counter = 1002");
	// Test with array : each item of the array is forgot
	M_TEST_SIMPLE("", " forget T_forget[3]   sta ;", "sta[0].struct_counter = 1003" K_eol
		                                             "sta[1].struct_counter = 1003" K_eol
												     "sta[2].struct_counter = 1003");
	M_TEST_SIMPLE("", "        T_forget      st7 ;", "st7.struct_counter = 1003");
}

//*****************************************************************************
// test_interpret_msg
//*****************************************************************************

void    test_interpret_msg(int   msg_to_test = -1)
{
//	S_ut_interpret_bytes_decode_nothing = false;  // ICIOA temp

	// Messages which come from example_with_capture.pcap
	// The number are the packet's number.
	T_byte_vector    msg_1_frame;
	T_byte_vector    msg_2_frame;
	T_byte_vector    msg_4_frame;
	T_byte_vector    msg_35_frame;
	{
		const char  * msg_1_str = "6c000b000000000000000000";
		const char  * msg_2_str = "01000b000000f400807468020000a000ffff1f00000000001800ffff01070000202008ff8072470054686520584672656538362050726f6a6563742c20496e630101200000000000040820000000000008082000000000000f102000000000001010200000000000182020000000000020202000000000005700000020000000ffffff000000000000001a0090061a0439026401010001002200000000001807182720005ede020022000000040800010000ff0000ff0000ff000000d2ea020023000000040800010000ff0000ff0000ff000000d2ea020024000000040800010000ff0000ff0000ff000000d2ea020025000000040800010000ff0000ff0000ff000000d2ea020026000000040800010000ff0000ff0000ff000000d2ea020027000000040800010000ff0000ff0000ff000000d2ea020028000000040800010000ff0000ff0000ff000000d2ea020029000000040800010000ff0000ff0000ff000000d2ea02002a000000040800010000ff0000ff0000ff000000d2ea02002b000000040800010000ff0000ff0000ff000000d2ea02002c000000040800010000ff0000ff0000ff000000d2ea02002d000000040800010000ff0000ff0000ff000000d2ea02002e000000040800010000ff0000ff0000ff000000d2ea02002f000000040800010000ff0000ff0000ff000000d2ea020030000000040800010000ff0000ff0000ff000000d2ea020031000000040800010000ff0000ff0000ff000000d2ea020032000000040800010000ff0000ff0000ff000000d2ea020033000000040800010000ff0000ff0000ff000000d2ea020034000000040800010000ff0000ff0000ff000000d2ea020035000000040800010000ff0000ff0000ff000000d2ea020036000000040800010000ff0000ff0000ff000000d2ea020037000000040800010000ff0000ff0000ff000000d2ea020038000000040800010000ff0000ff0000ff000000d2ea020039000000040800010000ff0000ff0000ff000000d2ea02003a000000040800010000ff0000ff0000ff000000d2ea02003b000000040800010000ff0000ff0000ff000000d2ea02003c000000040800010000ff0000ff0000ff000000d2ea02003d000000040800010000ff0000ff0000ff000000d2ea02003e000000040800010000ff0000ff0000ff000000d2ea02003f000000040800010000ff0000ff0000ff000000d2ea020040000000040800010000ff0000ff0000ff000000d2ea020041000000040800010000ff0000ff0000ff000000d2ea0200012700005ede0200042700005ede0200082700005ede02000f2700005ede0200102700005ede0200202700005ede0200";
		const char  * msg_4_str = "010002000000000001870000000000000000000000000000280000005bc00100";
		const char  * msg_35_str = "370005000000a0005700000008000000ffffff00620005000c0000004249472d524551554553545387000100";
		string_hexa_to_frame(msg_1_str, msg_1_frame);
		string_hexa_to_frame(msg_2_str, msg_2_frame);
		string_hexa_to_frame(msg_4_str, msg_4_frame);
		string_hexa_to_frame(msg_35_str, msg_35_frame);
	}

    T_type_definitions    type_definitions;
    build_types ("unitary_tests_example_with_capture.fdesc",
                 type_definitions);

	if ((msg_to_test < 0) || (msg_to_test == 1))
	{
		T_interpret_data  interpret_data;
		interpret_data.global_variable_group_begin();
		interpret_data.add_read_variable("global.msg_counter", "msg_counter" ,0);
		interpret_data.global_variable_group_end();

		ut_interpret_bytes(type_definitions,
						   msg_1_frame,
						   "T_msg_cs_start         msg_1;",
						   interpret_data,
						   "msg_1.Header.Type = cs_start (7077899)" K_eol
						   "msg_1.Header.Size = 8" K_eol
						   "msg_1.id = 0" K_eol
						   "msg_1.acronym = " K_eol);
	}
	if ((msg_to_test < 0) || (msg_to_test == 2))
	{
		T_interpret_data  interpret_data;
		interpret_data.global_variable_group_begin();
		interpret_data.add_read_variable("global.msg_counter", "msg_counter" ,1);
		interpret_data.global_variable_group_end();

		// 2010/10/15         2013 ms
		// 2010/10/16  1513 - 1545 ms
		// 2010/10/29         1190 ms  T_expression var set call
		// 2010/10/29          655 ms  T_expression condition return
		// 2010/11/10          675 ms  T_expression pre_compute value & operation
		ut_interpret_bytes(type_definitions,
						   msg_2_frame,
						   "T_msg_sc_start_ack         msg_2;",
						   interpret_data);
	}
	if ((msg_to_test < 0) || (msg_to_test == 4))
	{
		T_interpret_data  interpret_data;
		interpret_data.global_variable_group_begin();
		interpret_data.add_read_variable("global.msg_counter", "msg_counter" ,2);
		interpret_data.global_variable_group_end();

		ut_interpret_bytes(type_definitions,
						   msg_4_frame,
						   "T_msg_sc_ctx_data         msg_4;",
						   interpret_data);
	}
	if ((msg_to_test < 0) || (msg_to_test == 35))
	{
		T_interpret_data  interpret_data;
		interpret_data.global_variable_group_begin();
		interpret_data.add_read_variable("global.msg_counter", "msg_counter" ,3);
		interpret_data.global_variable_group_end();

		ut_interpret_bytes(type_definitions,
						   msg_35_frame,
						   "T_msg_cs_request        msg_35;",
						   interpret_data);
	}
}

//*****************************************************************************
// test_build_types_and_interpret_bytes
//*****************************************************************************

void    test_build_types_and_interpret_bytes()
{
	T_byte_vector    msg_1_frame;
	{
		const char  * msg_1_str = "6c000b000000000000000000";
		string_hexa_to_frame(msg_1_str, msg_1_frame);
	}

	const T_byte  * P_bytes = &msg_1_frame[0];
	size_t          sizeof_bytes = msg_1_frame.size();
	istringstream   iss("uint8[12]  uint8;");
	ostringstream   oss;

    bool  result = build_types_and_interpret_bytes (
		P_bytes,
		sizeof_bytes,
		iss,
		oss,
		oss);

	M_TEST_EQ(result, true);
	M_TEST_EQ(sizeof_bytes, 0);
}

//*****************************************************************************
// test_library
//*****************************************************************************

void    test_library()
{
    T_type_definitions    type_definitions;
    build_types ("unitary_tests_library.fdesc",
                 type_definitions);

	T_interpret_data      interpret_data;

	// ascii_strdown_inplace & ascii_strup_inplace
	M_TEST_SIMPLE("", "var string  original = \"Hello WORLD !\";",          "original = Hello WORLD !");
	M_TEST_SIMPLE("", "var string  before = original;",                       "before = Hello WORLD !");
	M_TEST_SIMPLE("", "var string  ret    = ascii_strdown_inplace (before);",    "ret = hello world !");
	M_TEST_SIMPLE("", "var string  after  = before;",                          "after = hello world !");
	M_TEST_SIMPLE("", "var string  ret    = ascii_strup_inplace (before);",      "ret = HELLO WORLD !");
	M_TEST_SIMPLE("", "var string  after  = before;",                          "after = HELLO WORLD !");
	M_TEST_SIMPLE("", "var string  notmodified = original;",             "notmodified = Hello WORLD !");


	// crc is not inside wsutil before wireshark 18X
#if WIRESHARK_VERSION_NUMBER >= 10800
	build_types ("unitary_tests_library_part2.fdesc",
                 type_definitions);

	// crc32c_calculate_byte
#define DATA  "6c 00 0b 00 00 00 00 00 00 00 00 00"

#define M_TEST_CRC32(POS,OFFSET,SEED,RESULT)  \
	M_TEST_SIMPLE(DATA,  \
		"var uint32  crc32 = crc32c_calculate_byte (" #POS "," #OFFSET "," #SEED "); move_position_bytes 12;",  \
		"crc32 = " #RESULT)

	M_TEST_CRC32( 0, 0,   0,            0);
	M_TEST_CRC32( 2, 0, 136,          136);
	M_TEST_CRC32( 0, 4,   0,   1483871832);
	M_TEST_CRC32( 4, 4,   0,            0);
	M_TEST_CRC32( 0, 4, 136,   4010883408);
	M_TEST_CRC32( 4, 4, 136,   3076730632);
	M_TEST_CRC32( 8, 4,   0,            0);
//	M_TEST_CRC32( 9, 4,   0,            0);    // out of bounds
//	M_TEST_CRC32(12, 4,   0,            0);    // out of bounds
#endif

	// base64 is not inside wsutil before wireshark 112X
#if WIRESHARK_VERSION_NUMBER >= 11200
	build_types ("unitary_tests_library_part3.fdesc",
                 type_definitions);

#define M_TEST_BASE64(INPUT,RESULT)  \
	M_TEST_SIMPLE("",  \
		"hide var string  input_output = " #INPUT "; "  \
		"hide var int64   size = ws_base64_decode_inplace (input_output); "  \
		"print (\"%s\", input_output); ",  \
		RESULT)

	M_TEST_BASE64("V3NnZA==", "Wsgd");
	M_TEST_BASE64("SSBoYXZlIGEgYmFzZTY0IGVuY29kZWQgc3RyaW5n", "I have a base64 encoded string");
	M_TEST_BASE64("T3V0a2FzdCAvIEhleSBZYQ==", "Outkast / Hey Ya");
#endif
}

//*****************************************************************************
// main
//*****************************************************************************

int   main(const int         argc,
		   const char * const  argv[])
{
	// Traces
	string      trace_file_name = string(argv[0]) + ".traces.txt";
	ofstream    ofs(trace_file_name.c_str());
	set_state_ostream(ofs);

	int   arg_idx = 1;

	if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-debug") == 0))
	{
		set_debug(true);
		++arg_idx;
	}

#define M_TEST_FCT(TEST_NAME)                                           \
	if ((arg_idx >= argc) || (strcmp(argv[arg_idx], #TEST_NAME) == 0))  \
	{                                                                   \
		TEST_NAME();                                                    \
	}

	if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-int64") == 0))
	{
		test_interpret_simple_int64();
	}
	else if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-uint8_array") == 0))
	{
		test_interpret_simple_uint8_array(1);
	}
	else if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-uint8_array_10") == 0))
	{
		test_interpret_simple_uint8_array(10);
	}
	else if ((arg_idx < argc) && (strcmp(argv[arg_idx], "-msg_2") == 0))
	{
		test_interpret_msg(2);
	}
	else
	{
		M_TEST_FCT(test_bitfield_C);
		M_TEST_FCT(test_scoped_copyable_ptr);
		M_TEST_FCT(test_string_to_words);
		M_TEST_FCT(test_read_token_word_cplx);
		M_TEST_FCT(test_read_token);
		M_TEST_FCT(test_build_field);
		M_TEST_FCT(test_get_number);
		M_TEST_FCT(test_get_before_separator_after);
		M_TEST_FCT(test_decompose_type_sep_value_sep);
		M_TEST_FCT(test_expression);
		M_TEST_FCT(test_function_call);
		M_TEST_FCT(test_builtin_functions);
		M_TEST_FCT(test_frame_data);
		M_TEST_FCT(test_frame_data_write);
		M_TEST_FCT(test_read_data);
		M_TEST_FCT(test_read_values);
		M_TEST_FCT(test_value_format);
		M_TEST_FCT(test_value_printf);
		M_TEST_FCT(test_interpret_simple);
		M_TEST_FCT(test_interpret_simple_int64);
		M_TEST_FCT(test_interpret_simple_uint8_array);
		M_TEST_FCT(test_interpret_simple_internal_frame);
		M_TEST_FCT(test_interpret_forget);
		M_TEST_FCT(test_interpret_msg);
		M_TEST_FCT(test_build_types_and_interpret_bytes);
#ifdef BYTE_INTERPRET_USE_LIBRARY_DYNCALL
		M_TEST_FCT(test_library);
#endif
		M_TEST_FCT(test_size);
	}

	C_perf_chrono::end();
	M_TEST_RETURN_OF_MAIN();
}

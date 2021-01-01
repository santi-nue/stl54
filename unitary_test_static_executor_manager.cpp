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

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"
#include "T_static_executor_manager.h"


//*****************************************************************************
// test_static_executor_manager_filters_empty
//*****************************************************************************

M_TEST_FCT(test_static_executor_manager_filters_empty)
{
    T_static_executor_manager::T_execution_filters  filters = T_static_executor_manager::build_execution_filters(7, nullptr, 7);
    M_TEST_EQ(filters.filter_file_name_ends.size(), 0);
    M_TEST_EQ(filters.filter_function_names.size(), 0);
    M_TEST_EQ(filters.filter_function_name_starts.size(), 0);

    T_static_executor executor(nullptr, "file_name.cpp", "fct_name");
    M_TEST_EQ(filters.must_execute(executor), true);
}

//*****************************************************************************
// test_static_executor_manager_filters_file_name_end
//*****************************************************************************

M_TEST_FCT(test_static_executor_manager_filters_file_name_end)
{
    const char* argv[] = { "ut.exe", "file_name.cpp", "_name3.cpp" };
    T_static_executor_manager::T_execution_filters  filters = T_static_executor_manager::build_execution_filters(3, argv, 1);
    M_TEST_EQ(filters.filter_file_name_ends.size(), 2);
    M_TEST_EQ(filters.filter_file_name_ends[0], "file_name.cpp");
    M_TEST_EQ(filters.filter_file_name_ends[1], "_name3.cpp");
    M_TEST_EQ(filters.filter_function_names.size(), 0);
    M_TEST_EQ(filters.filter_function_name_starts.size(), 0);

    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name.cpp", "fct_name")), true);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "the_file_name.cpp", "fct_name")), true);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name2.cpp", "fct_name")), false);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name3.cpp", "fct_name")), true);
}

//*****************************************************************************
// test_static_executor_manager_filters_function_name
//*****************************************************************************

M_TEST_FCT(test_static_executor_manager_filters_function_name)
{
    const char* argv[] = { "ut.exe", "fct_name", "fct_name3" };
    T_static_executor_manager::T_execution_filters  filters = T_static_executor_manager::build_execution_filters(3, argv, 1);
    M_TEST_EQ(filters.filter_file_name_ends.size(), 0);
    M_TEST_EQ(filters.filter_function_names.size(), 2);
    M_TEST_EQ(filters.filter_function_names[0], "fct_name");
    M_TEST_EQ(filters.filter_function_names[1], "fct_name3");
    M_TEST_EQ(filters.filter_function_name_starts.size(), 0);

    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name.cpp", "fct_name")), true);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name2.cpp", "fct_name2")), false);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name3.cpp", "fct_name3")), true);
}

//*****************************************************************************
// test_static_executor_manager_filters_function_name_start
//*****************************************************************************

M_TEST_FCT(test_static_executor_manager_filters_function_name_start)
{
    const char* argv[] = { "ut.exe", "fct_na*", "fct3_*" };
    T_static_executor_manager::T_execution_filters  filters = T_static_executor_manager::build_execution_filters(3, argv, 1);
    M_TEST_EQ(filters.filter_file_name_ends.size(), 0);
    M_TEST_EQ(filters.filter_function_names.size(), 0);
    M_TEST_EQ(filters.filter_function_name_starts.size(), 2);
    M_TEST_EQ(filters.filter_function_name_starts[0], "fct_na");
    M_TEST_EQ(filters.filter_function_name_starts[1], "fct3_");

    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name.cpp", "fct_name")), true);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name2.cpp", "fctname2")), false);
    M_TEST_EQ(filters.must_execute(T_static_executor(nullptr, "file_name3.cpp", "fct3_name3")), true);
}


#include <iostream>
#include <algorithm>

#include "T_static_executor.h"
#include "T_static_executor_manager.h"


bool ends_with(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}
bool ends_with_case_insensitive(const std::string& str, const std::string& suffix)
{
    if (suffix.size() > str.size())
    {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin(),
        [](const char a, const char b)
        {
            return tolower(a) == tolower(b);
        }
    );
}
bool starts_with(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}



T_static_executor_manager& T_static_executor_manager::getInstance()
{
    static T_static_executor_manager instance;
    return instance;
}

void T_static_executor_manager::add(T_static_executor& staticExecutor)
{
    m_static_executors.push_back(staticExecutor);
}

int T_static_executor_manager::execute(const int           argc,
                                       const char * const  argv[],
                                       const int           arg_idx)
{
    T_execution_filters  execution_filters = build_execution_filters(argc, argv, arg_idx);

    int execution_counter = 0;
    for (T_static_executor& executor : m_static_executors)
    {
        if (execution_filters.must_execute(executor))
        {
            ++execution_counter;
            executor.execute();
        }
    }

    if (execution_counter <= 0)
    {
        std::cerr << "!!! No function executed !!!" << std::endl;
    }

    return execution_counter;
}

T_static_executor_manager::T_execution_filters
T_static_executor_manager::build_execution_filters(const int           argc,
                                                   const char * const  argv[],
                                                         int           arg_idx)
{
    T_execution_filters  execution_filters;

    for (; arg_idx < argc; ++arg_idx)
    {
        std::string  filter = argv[arg_idx];
        if (ends_with(filter, ".cpp"))
        {
            execution_filters.filter_file_name_ends.push_back(filter);
        }
        else if (ends_with(filter, "*"))
        {
            filter.pop_back();
            execution_filters.filter_function_name_starts.push_back(filter);
        }
        else
        {
            execution_filters.filter_function_names.push_back(filter);
        }
    }

    return execution_filters;
}

bool
T_static_executor_manager::T_execution_filters::must_execute(const T_static_executor& executor) const
{
    bool must_match = false;   // will be true if any filter is specified

    if (!filter_function_names.empty())
    {
        must_match = true;
        if (find(begin(filter_function_names), end(filter_function_names), executor.get_fct_name()) != end(filter_function_names))
        {
            return  true;
        }
    }

    if (!filter_function_name_starts.empty())
    {
        must_match = true;
        for (const auto& filter_function_name_start : filter_function_name_starts)
        {
            if (starts_with(executor.get_fct_name(), filter_function_name_start))
            {
                return  true;
            }
        }
    }

    if (!filter_file_name_ends.empty())
    {
        must_match = true;
        for (const auto& filter_file_name_end : filter_file_name_ends)
        {
            if (ends_with_case_insensitive(executor.get_file_name(), filter_file_name_end))
            {
                return  true;
            }
        }
    }

    return  !must_match;
}

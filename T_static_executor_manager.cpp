
#include <iostream>

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
    std::string  filter_file_name;
    std::string  filter_function_name;
    std::string  filter_function_name_start;

    if (arg_idx < argc)
    {
        const std::string  filter = argv[arg_idx];
        if (ends_with(filter, ".cpp"))
        {
            filter_file_name = filter;
        }
        else if (ends_with(filter, "*"))
        {
            filter_function_name_start = filter;
            filter_function_name_start.pop_back();
        }
        else
        {
            filter_function_name = filter;
        }
    }

    int execution_counter = 0;
    for (auto iter  = m_static_executors.begin();
              iter != m_static_executors.end();
            ++iter)
    {
        T_static_executor& executor = *iter;

        if (filter_function_name != "")
        {
            if (executor.get_fct_name() != filter_function_name)
                continue;
        }
        else if (filter_function_name_start != "")
        {
            if (!starts_with(executor.get_fct_name(), filter_function_name_start))
                continue;
        }
        if (filter_file_name != "")
        {
            if (!ends_with_case_insensitive(executor.get_file_name(), filter_file_name))
                continue;
        }

        ++execution_counter;
        executor.execute();
    }

    if (execution_counter <= 0)
    {
        std::cerr << "Aucun fonction éxécutée" << std::endl;
    }

    return execution_counter;
}


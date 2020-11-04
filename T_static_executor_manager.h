#pragma once

#include <vector>
#include "T_static_executor.h"


class T_static_executor_manager
{
public:
    static T_static_executor_manager& getInstance();

    void add(T_static_executor& static_executor);

    // Return the number of executions done
    int execute(const int           argc,
                const char * const  argv[],
                const int           arg_idx);

private:
    std::vector<T_static_executor> m_static_executors;
};

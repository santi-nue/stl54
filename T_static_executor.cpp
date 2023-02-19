
#include "T_static_executor.h"
#include "T_static_executor_manager.h"


void T_static_executor::do_register()
{
    T_static_executor_manager::getInstance().add(*this);
}

void T_static_executor_base::execute()
{
    m_test_fct();
}

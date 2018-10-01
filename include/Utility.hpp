#ifndef GVIEW_UTILITY_HPP
#define GVIEW_UTILITY_HPP
#include <future>

template<class T>
bool ready(T &future)
{
    return future.wait_for(std::chrono::duration<double>(0)) == std::future_status::ready;
}

#endif // GVIEW_UTILITY_HPP
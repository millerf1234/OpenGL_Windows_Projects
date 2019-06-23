
#pragma once

#ifndef FORCE_BEGIN_ASYNC_TASK_H_
#define FORCE_BEGIN_ASYNC_TASK_H_

#include <future>

//Helper function to launch an asynchronous task with the launch policy that ensures the task
//begins executing right away. The default behavior of std::async allows the task scheduler to
//optionally delay launching the task until a request is made using 'get' or 'wait' on the future.
//This default behavior means if the returned future never has 'get' or 'wait' called on it, it
//could be that it is possible that the task never launches at all. Seeing as this class is perhaps 
//going to be saving data to files asynchronously, it is very desirable to be able to guarantee that
//every 'save data' task is carried out without delay. 
//This example is based directly from Scott Meyer's Effective Modern C++ Item 36
template<typename F, typename ... Ts>
inline auto forceBeginAsyncTask(F&& f, Ts&& ... parameters) {
    return std::async(std::launch::async,
                      std::forward<F>(f),
                      std::forward<Ts>(parameters)...);
}


#endif //FORCE_BEGIN_ASYNC_TASK_H_ 
#pragma once

#include <string>
#include <iostream>
#include <tuple>

struct trace_obj                                                                   
{                                                                               
    std::string m_func_name;
    static inline int m_indent{0};
    trace_obj(std::string func_name);
                                                                                
    ~trace_obj();
};                                                                              


#if 0

#define TRACE(func_name, ...)                                                       \
    trace_obj tr{func_name};                                                        \
                                                                                    \
    [](auto&&... args)                                                              \
    {                                                                               \
       const     auto t = std::make_tuple(std::forward<decltype(args)>(args)...);   \
       constexpr auto N = std::tuple_size<decltype(t)>::value;                      \
                                                                                    \
       if constexpr ( N != 0 ) {                                                     \
           ((std::cout << ' ' << std::forward<decltype(args)>(args)), ...);           \
       }                                                                            \
                                                                                    \
        std::cout << '\n';                                                          \
                                                                                    \
    }(__VA_ARGS__)


#else
#define TRACE(...) {} 
#endif


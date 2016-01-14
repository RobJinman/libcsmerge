#ifndef __UTIL_HPP__
#define __UTIL_HPP__


#include <iostream>


#ifdef DEBUG
#    define DBG_STREAM std::cout

#    define DBG_OUT(x) \
         DBG_STREAM << x;
#else
#    define DBG_OUT(x)
#endif

#define NON_FATAL(x) std::cerr << "Warning: " << x << std::endl;


#endif

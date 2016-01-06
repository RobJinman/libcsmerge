#ifndef __UTIL_HPP__
#define __UTIL_HPP__


#ifdef DEBUG
#    include <iostream>

#    define DBG_STREAM std::cout

#    define DBG_OUT(x) \
         DBG_STREAM << x;
#else
#    define DBG_OUT(x)
#endif


#endif

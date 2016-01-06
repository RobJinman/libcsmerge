#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__


#include <stdexcept>


namespace csmerge {


class CsMergeException : public std::runtime_error {
    public:
        CsMergeException(const std::string& msg)
            : runtime_error(msg) {}
};


}


#endif

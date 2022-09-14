#ifndef _INNER_EXCEPTOIN
#define _INNER_EXCEPTION

#include <exception>

struct empty_stack: std::exception {
    const char* what() const throw() { return "empty stack!!!"; }
};

#endif

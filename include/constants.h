#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string_view>

constexpr std::string_view LOG_FMT{
    "[%Y-%m-%d %H:%M:%S.%e] "
    "[%P] "
    "[%^%l%$] "
    "%v"
};

#endif // CONSTANTS_H
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string_view>

constexpr std::string_view LOG_FMT{
    "[%Y-%m-%d %H:%M:%S.%e] "
    "[%P] "
    "[%^%l%$] "
    "%v"
};

constexpr int kMaxRetries = 50;
constexpr useconds_t kRetryDelayUs = 10000;

#endif // CONSTANTS_H
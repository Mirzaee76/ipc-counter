#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "constants.h"

int main()
{
    spdlog::set_pattern(std::string{LOG_FMT});
    spdlog::set_level(spdlog::level::trace);

    spdlog::info("Application started");

    return 0;
}
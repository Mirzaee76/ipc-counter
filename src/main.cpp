#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "constants.h"
#include "core/cli_parser.h"

int main(int argc, char** argv)
{
    spdlog::set_pattern(std::string{LOG_FMT});
    spdlog::set_level(spdlog::level::trace);

    try
    {
        auto options = counter::core::CliParser::parse(argc, argv);
        spdlog::info("IPC Type = {}, Max Value = {}", static_cast<int>(options.ipc), options.maxValue);
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatal error in main: {}", e.what());
        return 1;
    }

    return 0;
}
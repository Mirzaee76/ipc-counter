#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <chrono>

#include "constants.h"
#include "core/cli_parser.h"
#include "core/application.h"

int main(int argc, char** argv)
{
    spdlog::set_pattern(std::string{LOG_FMT});
    spdlog::set_level(spdlog::level::trace);

    const auto start = std::chrono::steady_clock::now();
    int result = EXIT_SUCCESS;

    try
    {
        auto options = counter::core::CliParser::parse(argc, argv);
        if (options.action == counter::core::CliAction::ExitSuccess)
            return EXIT_SUCCESS;

        counter::core::Application app(options);
        result = app.run();
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatal error in main: {}", e.what());
        result = EXIT_FAILURE;
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    spdlog::info("Execution time: {} ns", elapsed.count());

    return result;
}
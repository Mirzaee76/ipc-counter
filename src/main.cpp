#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "constants.h"
#include "core/cli_parser.h"
#include "core/application.h"

int main(int argc, char** argv)
{
    spdlog::set_pattern(std::string{LOG_FMT});
    spdlog::set_level(spdlog::level::trace);

    try
    {
        auto options = counter::core::CliParser::parse(argc, argv);
        counter::core::Application app(options);
        return app.run();
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatal error in main: {}", e.what());
        return 1;
    }
}
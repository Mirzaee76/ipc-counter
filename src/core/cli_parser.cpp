#include "core/cli_parser.h"

namespace counter::core
{

CliOptions CliParser::parse(int argc, char** argv)
{
    CliOptions options;

    auto print_help = []()
    {
        std::cout <<
            "Usage: app [options]\n"
            "\n"
            "Options:\n"
            "  --ipc <pipe|socket|shm>   Set IPC type\n"
            "  --max-value <int>         Set max counter value (default: 10)\n"
            "  --version                 Show application version\n"
            "  --help                    Show this help message\n";
    };

    for (int i = 1; i < argc; ++i)
    {
        std::string_view arg = argv[i];

        // help
        if (arg == "--help")
        {
            print_help();
            std::exit(0);
        }

        // version
        else if (arg == "--version")
        {
            std::cout
                << APP_VER_MAJOR << "."
                << APP_VER_MINOR << "."
                << APP_VER_PATCH
                << std::endl;

            std::exit(0);
        }

        // IPC type
        if (arg == "--ipc")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("missing value for --ipc");

            std::string_view value = argv[++i];

            if (value == "pipe")
                options.ipc = IpcType::Pipe;
            else if (value == "socket")
                options.ipc = IpcType::UnixSocket;
            else if (value == "shm")
                options.ipc = IpcType::SharedMemory;
            else
                throw std::runtime_error("invalid ipc type");
        }

        // max value
        else if (arg == "--max-value")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("missing value for --max-value");

            std::string_view value = argv[++i];

            try
            {
                options.maxValue = std::stoi(std::string(value));
            }
            catch (...)
            {
                throw std::runtime_error("invalid integer for --max-value");
            }
        }
    }

    return options;
}

}
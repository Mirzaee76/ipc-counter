#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <iostream>
#include <string>
#include "types.h"
#include "version.h"

namespace counter::core
{

enum class CliAction{
    Continue,
    ExitSuccess
};

struct CliOptions
{
    IpcType ipc{IpcType::Pipe};
    int maxValue{10};

    CliAction action{CliAction::Continue};
};

class CliParser
{
public:
    static CliOptions parse(int argc, char** argv);
};
}
#endif // CLI_PARSER_H
#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <cstdlib>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include "ipc/ipc_factory.h"
#include "ipc/ipc_channel.h"
#include "types.h"
#include "core/cli_parser.h"

namespace counter::core
{

class Application
{
public:
    explicit Application(core::CliOptions options);
    int run();
private:
    void runInitiator();
    void runReceiver();
    void createPipesIfNeeded();

    core::CliOptions options_;
    counter::ipc::Context ipcContext_;
};

}

#endif // APPLICATION_H
#include "core/application.h"
#include "spdlog/spdlog.h"
#include "types.h"

namespace counter::core
{
Application::Application(core::CliOptions options)
    : options_(options)
{}

int Application::run()
{
    spdlog::info("application started");

    createPipesIfNeeded();

    const auto pid = ::fork();
    if (pid < 0)
        throw std::runtime_error("fork failed");

    if (pid == 0)
    {
        try
        {
            runReceiver();
            std::exit(EXIT_SUCCESS);
        }
        catch (const std::exception& exception)
        {
            spdlog::error("receiver failed: {}", exception.what());
            std::exit(EXIT_FAILURE);
        }
    }

    runInitiator();

    int status{};
    if (::waitpid(pid, &status, 0) < 0)
        throw std::runtime_error("waitpid failed");
    if (WIFEXITED(status))
        spdlog::info("Parent process: child exited with code {}", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        spdlog::error("Parent process: child terminated by signal {}", WTERMSIG(status));

    spdlog::info("application finished");
    return EXIT_SUCCESS;
}

void Application::runInitiator()
{
    spdlog::info("Initiator started");
    auto channel = ipc::IpcFactory::create(options_.ipc, ipcContext_);
    channel->open(ProcessRole::Initiator);

    Message msg;
    channel->send(msg);
    spdlog::info("Initiator: sent value={}", msg.value);

    while (true)
    {
        msg = channel->receive();
        spdlog::info("Initiator: recv value={}", msg.value);

        if (msg.value >= options_.maxValue)
            break;

        msg.value++;
        channel->send(msg);
        spdlog::info("Initiator: sent value={}", msg.value);

        if (msg.value >= options_.maxValue)
            break;
    }

    spdlog::info("Initiator finished");
}

void Application::runReceiver()
{
    spdlog::info("Receiver started");
    auto channel = ipc::IpcFactory::create(options_.ipc, ipcContext_);
    channel->open(ProcessRole::Receiver);

    while (true)
    {
        auto msg = channel->receive();
        spdlog::info("Receiver : recv value={}", msg.value);

        if (msg.value >= options_.maxValue)
            break;

        msg.value++;
        channel->send(msg);
        spdlog::info("Receiver : sent value={}", msg.value);

        if (msg.value >= options_.maxValue)
            break;
    }

    spdlog::info("Receiver finished");
}

void Application::createPipesIfNeeded()
{
    if (options_.ipc != IpcType::Pipe)
        return;

    if (::pipe(ipcContext_.parentToChild.data()) < 0)
        throw std::runtime_error("failed to create parentToChild pipe");

    if (::pipe(ipcContext_.childToParent.data()) < 0)
        throw std::runtime_error("failed to create childToParent pipe");

    spdlog::info("pipes created");
}

} // namespace counter::core
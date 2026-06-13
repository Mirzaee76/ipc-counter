#ifndef PIPE_CHANNEL_H
#define PIPE_CHANNEL_H

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "ipc/ipc_channel.h"

namespace counter::ipc
{

class PipeChannel final : public IpcChannel
{
public:
    PipeChannel(const std::array<int,2>& parentToChild,const std::array<int,2>& childToParent);
    ~PipeChannel() override;

    void open(core::ProcessRole role) override;
    void send(const counter::core::Message& message) override;
    counter::core::Message receive() override;
    void close() override;

private:
    std::array<int,2> parentToChild_{};
    std::array<int,2> childToParent_{};
    int readFd_{-1};
    int writeFd_{-1};

    bool isOpen_{false};
};

} // namespace counter::ipc

#endif // PIPE_CHANNEL_H
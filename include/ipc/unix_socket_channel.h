#ifndef UNIX_SOCKET_CHANNEL_H
#define UNIX_SOCKET_CHANNEL_H

#include <string>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ipc/ipc_channel.h"
#include "constants.h"

namespace counter::ipc
{

class UnixSocketChannel final : public IpcChannel
{
public:
    UnixSocketChannel();
    ~UnixSocketChannel() noexcept override;

    UnixSocketChannel(const UnixSocketChannel&) = delete;
    UnixSocketChannel& operator=(const UnixSocketChannel&) = delete;

    void open(core::ProcessRole role) override;
    void send(const counter::core::Message& message) override;
    counter::core::Message receive() override;
    void close() override;

private:
    void setupServer();
    void setupClient();

    void sendAll(int fd, const void* data, std::size_t size);
    void recvAll(int fd, void* data, std::size_t size);

    int socketFd_{-1};
    int clientFd_{-1};

    core::ProcessRole role_;
    std::string socketPath_{"/tmp/counter_ipc.sock"};
};

} // namespace counter::ipc

#endif // UNIX_SOCKET_CHANNEL_H
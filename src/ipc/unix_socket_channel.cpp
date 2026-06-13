#include "ipc/unix_socket_channel.h"

namespace counter::ipc
{

UnixSocketChannel::UnixSocketChannel() = default;

UnixSocketChannel::~UnixSocketChannel()
{
    close();
}

void UnixSocketChannel::open(core::ProcessRole role)
{
    role_ = role;

    if (role_ == core::ProcessRole::Initiator)
        setupClient();
    else
        setupServer();
}

void UnixSocketChannel::setupServer()
{
    socketFd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd_ < 0)
        throw std::runtime_error("socket creation failed");

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath_.c_str(), sizeof(addr.sun_path) - 1);

    ::unlink(socketPath_.c_str());

    if (::bind(socketFd_, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");

    if (::listen(socketFd_, 1) < 0)
        throw std::runtime_error("listen failed");

    clientFd_ = ::accept(socketFd_, nullptr, nullptr);
    if (clientFd_ < 0)
        throw std::runtime_error("accept failed");
}

void UnixSocketChannel::setupClient()
{
    socketFd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd_ < 0)
        throw std::runtime_error("socket creation failed");

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath_.c_str(), sizeof(addr.sun_path) - 1);

    for (int i = 0; i < 50; ++i)
    {
        if (::connect(socketFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0)
            return;
        ::usleep(10000); // 10 ms
    }

    throw std::runtime_error("connect failed");
}

void UnixSocketChannel::send(const counter::core::Message& message)
{
    int fd = (role_ == core::ProcessRole::Initiator) ? socketFd_ : clientFd_;
    const auto bytesWritten = ::send(fd, &message, sizeof(message), 0);

    if (bytesWritten != sizeof(message))
        throw std::runtime_error("socket send failed");
}

counter::core::Message UnixSocketChannel::receive()
{
    int fd = (role_ == core::ProcessRole::Initiator) ? socketFd_ : clientFd_;
    counter::core::Message message{};
    const auto bytesRead = ::recv(fd, &message, sizeof(message), 0);

    if (bytesRead != sizeof(message))
        throw std::runtime_error("socket receive failed");

    return message;
}

void UnixSocketChannel::close()
{
    if (clientFd_ != -1)
        ::close(clientFd_);

    if (socketFd_ != -1)
        ::close(socketFd_);

    clientFd_ = -1;
    socketFd_ = -1;

    ::unlink(socketPath_.c_str());
}

} // namespace counter::ipc
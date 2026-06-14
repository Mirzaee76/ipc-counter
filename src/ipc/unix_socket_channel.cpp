#include "ipc/unix_socket_channel.h"

namespace counter::ipc
{

UnixSocketChannel::UnixSocketChannel() = default;

UnixSocketChannel::~UnixSocketChannel() noexcept
{
    close();
}

void UnixSocketChannel::open(core::ProcessRole role)
{
    role_ = role;

    if (socketFd_ >= 0)
        throw std::runtime_error("UnixSocketChannel already opened");

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

    for (int i = 0; i < kMaxRetries; ++i)
    {
        if (::connect(socketFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0)
            return;
        ::usleep(kRetryDelayUs);
    }

    throw std::runtime_error("connect failed");
}

void UnixSocketChannel::send(const counter::core::Message& message)
{
    int fd = (role_ == core::ProcessRole::Initiator) ? socketFd_ : clientFd_;
    if (fd < 0)
        throw std::runtime_error("socket not opened");

    sendAll(fd, &message, sizeof(message));
}

counter::core::Message UnixSocketChannel::receive()
{
    int fd = (role_ == core::ProcessRole::Initiator) ? socketFd_ : clientFd_;
    if (fd < 0)
        throw std::runtime_error("socket not opened");

    counter::core::Message message{};
    recvAll(fd, &message, sizeof(message));
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

    if (role_ == core::ProcessRole::Receiver)
        ::unlink(socketPath_.c_str());

}

void UnixSocketChannel::sendAll(int fd, const void* data, std::size_t size)
{
    const char* buffer = static_cast<const char*>(data);

    std::size_t totalSent = 0;
    while (totalSent < size)
    {
        const auto bytesSent = ::send(fd, buffer + totalSent, size - totalSent, 0);

        if (bytesSent <= 0)
            throw std::runtime_error("socket send failed");

        totalSent += static_cast<std::size_t>(bytesSent);
    }

    if (totalSent != size)
        throw std::runtime_error("socket send failed");
}

void UnixSocketChannel::recvAll(int fd, void* data, std::size_t size)
{
    char* buffer = static_cast<char*>(data);

    std::size_t totalRead = 0;
    while (totalRead < size)
    {
        const auto bytesRead = ::recv(fd, buffer + totalRead, size - totalRead, 0);

        if (bytesRead == 0)
            throw std::runtime_error("socket closed (EOF)");

        if (bytesRead < 0)
            throw std::runtime_error("socket receive failed");

        totalRead += static_cast<std::size_t>(bytesRead);
    }

    if (totalRead != size)
        throw std::runtime_error("socket receive failed");
}

} // namespace counter::ipc
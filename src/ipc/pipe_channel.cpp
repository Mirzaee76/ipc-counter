#include "ipc/pipe_channel.h"

namespace counter::ipc
{

PipeChannel::PipeChannel(const std::array<int,2>& parentToChild,const std::array<int,2>& childToParent) : 
    parentToChild_(parentToChild),
    childToParent_(childToParent)
{}

PipeChannel::~PipeChannel()
{
    close();
}

void PipeChannel::open(core::ProcessRole role)
{
    switch (role)
    {
        case core::ProcessRole::Initiator:
        {
            ::close(parentToChild_[0]);
            ::close(childToParent_[1]);

            writeFd_ = parentToChild_[1];
            readFd_  = childToParent_[0];

            break;
        }
        case core::ProcessRole::Receiver:
        {
            ::close(parentToChild_[1]);
            ::close(childToParent_[0]);

            readFd_  = parentToChild_[0];
            writeFd_ = childToParent_[1];

            break;
        }
        default:
        {
            throw std::runtime_error("Unsupported process role");
        }
    }

    isOpen_ = true;
}

void PipeChannel::send(const counter::core::Message& message)
{
    const auto bytesWritten = ::write(writeFd_, &message, sizeof(message));

    if (bytesWritten != sizeof(message))
        throw std::runtime_error("failed to write to pipe");
}

counter::core::Message PipeChannel::receive()
{
    counter::core::Message message{};
    const auto bytesRead = ::read(readFd_, &message, sizeof(message));

    if (bytesRead == 0)
        throw std::runtime_error("pipe closed (EOF)");

    if (bytesRead != sizeof(message))
        throw std::runtime_error("failed to read from pipe");

    return message;
}

void PipeChannel::close()
{
    if (!isOpen_)
        return;

    if (readFd_ >= 0)
    {
        ::close(readFd_);
        readFd_ = -1;
    }

    if (writeFd_ >= 0)
    {
        ::close(writeFd_);
        writeFd_ = -1;
    }

    isOpen_ = false;
}

} // namespace counter::ipc
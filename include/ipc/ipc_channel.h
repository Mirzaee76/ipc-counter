#ifndef IPC_CHANNEL_H
#define IPC_CHANNEL_H

#include "types.h"

namespace counter::ipc
{

class IpcChannel
{
public:

    virtual ~IpcChannel() = default;

    virtual void open(core::ProcessRole role) = 0;
    virtual void send(const counter::core::Message& message) = 0;
    virtual counter::core::Message receive() = 0;
    virtual void close() = 0;
};

} // namespace counter::ipc

#endif // IPC_CHANNEL_H
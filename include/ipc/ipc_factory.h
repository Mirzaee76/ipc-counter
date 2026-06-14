#ifndef IPC_FACTORY_H
#define IPC_FACTORY_H

#include <memory>
#include <stdexcept>

#include "ipc/ipc_channel.h"
#include "ipc/pipe_channel.h"
#include "ipc/unix_socket_channel.h"
#include "ipc/shm_channel.h"

namespace counter::ipc
{

struct Context
{
    std::array<int, 2> parentToChild{};
    std::array<int, 2> childToParent{};
};

class IpcFactory
{
public:
    static std::unique_ptr<IpcChannel> create(core::IpcType type, const Context& context);
};

}

#endif // IPC_FACTORY_H
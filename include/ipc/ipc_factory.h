#ifndef IPC_FACTORY_H
#define IPC_FACTORY_H

#include <memory>
#include <stdexcept>

#include "ipc/ipc_channel.h"
#include "ipc/pipe_channel.h"

namespace counter::ipc
{

class IpcFactory
{
public:
    static std::unique_ptr<IpcChannel> create(core::IpcType type,
                                            const std::array<int,2>& parentToChild,
                                            const std::array<int,2>& childToParent);
};

}

#endif // IPC_FACTORY_H
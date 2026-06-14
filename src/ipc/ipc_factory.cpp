#include "ipc/ipc_factory.h"

namespace counter::ipc
{

std::unique_ptr<IpcChannel> IpcFactory::create(core::IpcType type, const Context& context)
{
    switch (type)
    {
        case core::IpcType::Pipe:
            return std::make_unique<PipeChannel>(context.parentToChild, context.childToParent);
        case core::IpcType::UnixSocket:
            return std::make_unique<UnixSocketChannel>();
        case core::IpcType::SharedMemory:
            return std::make_unique<SharedMemoryChannel>();
    }

    throw std::runtime_error("invalid ipc type");
}
}
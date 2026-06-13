#include "ipc/ipc_factory.h"

namespace counter::ipc
{

std::unique_ptr<IpcChannel> IpcFactory::create(core::IpcType type,
                                            const std::array<int,2>& parentToChild,
                                            const std::array<int,2>& childToParent)
{
    switch (type)
    {
        case core::IpcType::Pipe:
            return std::make_unique<PipeChannel>(parentToChild, childToParent);
    }

    throw std::runtime_error("invalid ipc type");
}
}
#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace counter::core
{

enum class IpcType
{
    Pipe,
    UnixSocket,
    SharedMemory
};

enum class ProcessRole
{
    Initiator,
    Receiver
};

struct Message
{
    std::int32_t value = 0;
};

}

#endif // TYPES_H
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

static_assert(std::is_trivially_copyable_v<Message>, "Message must be trivially copyable");

}

#endif // TYPES_H
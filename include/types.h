#ifndef TYPES_H
#define TYPES_H

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

}

#endif // TYPES_H
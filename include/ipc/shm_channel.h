#ifndef SHM_CHANNEL_H
#define SHM_CHANNEL_H

#include <cstdint>
#include <string>
#include <semaphore.h>
#include <stdexcept>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ipc/ipc_channel.h"

namespace counter::ipc
{

class SharedMemoryChannel final : public IpcChannel
{
public:
    SharedMemoryChannel();
    ~SharedMemoryChannel() override;

    void open(core::ProcessRole role) override;
    void send(const counter::core::Message& message) override;
    counter::core::Message receive() override;
    void close() override;

private:
    void createResources();
    void openResources();

    core::ProcessRole role_{};

    int shmFd_{-1};

    counter::core::Message* shared_{nullptr};

    sem_t* parentSem_{SEM_FAILED};
    sem_t* childSem_{SEM_FAILED};

    std::string kShmName   = "/counter_ipc_shm";
    std::string kParentSem = "/counter_parent_sem";
    std::string kChildSem  = "/counter_child_sem";
};

}

#endif // SHM_CHANNEL_H
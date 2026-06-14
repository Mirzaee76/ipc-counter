#include "ipc/shm_channel.h"

namespace counter::ipc
{

SharedMemoryChannel::SharedMemoryChannel() = default;

SharedMemoryChannel::~SharedMemoryChannel() noexcept
{
    close();
}

void SharedMemoryChannel::open(core::ProcessRole role)
{
    role_ = role;
    if (shmFd_ >= 0)
        throw std::runtime_error("SharedMemoryChannel already opened");

    if (role == core::ProcessRole::Initiator)
        createResources();
    else
        openResources();
}

void SharedMemoryChannel::send(const counter::core::Message& message)
{
    if (shared_ == nullptr || shared_ == MAP_FAILED)
        throw std::runtime_error("shared memory not opened");

    std::memcpy(shared_, &message, sizeof(message));
    if (role_ == core::ProcessRole::Initiator)
    {
        if (sem_post(childSem_) == -1)
            throw std::runtime_error("sem_post failed");
    }
    else
    {
        if (sem_post(parentSem_) == -1)
            throw std::runtime_error("sem_post failed");
    }
}

counter::core::Message SharedMemoryChannel::receive()
{
    if (shared_ == nullptr || shared_ == MAP_FAILED)
        throw std::runtime_error("shared memory not opened");

    if (role_ == core::ProcessRole::Initiator)
    {
        if (sem_wait(parentSem_) == -1)
            throw std::runtime_error("sem_wait failed");
    }
    else
    {
        if (sem_wait(childSem_) == -1)
            throw std::runtime_error("sem_wait failed");
    }

    counter::core::Message msg;
    std::memcpy(&msg, shared_, sizeof(msg));
    return msg;
}

void SharedMemoryChannel::close()
{
    if (shared_ != nullptr && shared_ != MAP_FAILED)
    {
        munmap(shared_, sizeof(counter::core::Message));
        shared_ = nullptr;
    }

    if (shmFd_ != -1)
    {
        ::close(shmFd_);
        shmFd_ = -1;
    }

    if (parentSem_ != SEM_FAILED)
    {
        sem_close(parentSem_);
        parentSem_ = SEM_FAILED;
    }

    if (childSem_ != SEM_FAILED)
    {
        sem_close(childSem_);
        childSem_ = SEM_FAILED;
    }

    if (role_ == core::ProcessRole::Initiator)
    {
        sem_unlink(kParentSem.c_str());
        sem_unlink(kChildSem.c_str());
        shm_unlink(kShmName.c_str());
    }
}

void SharedMemoryChannel::createResources()
{
    shm_unlink(kShmName.c_str());
    sem_unlink(kParentSem.c_str());
    sem_unlink(kChildSem.c_str());

    shmFd_ = shm_open(kShmName.c_str(), O_CREAT | O_RDWR, 0666);
    if (shmFd_ == -1)
        throw std::runtime_error("shm_open failed");

    if (ftruncate(shmFd_, sizeof(counter::core::Message)) == -1)
        throw std::runtime_error("ftruncate failed");

    shared_ = static_cast<counter::core::Message*>(mmap(nullptr, sizeof(counter::core::Message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd_, 0));
    if (shared_ == MAP_FAILED)
        throw std::runtime_error("mmap failed");

    parentSem_ = sem_open(kParentSem.c_str(), O_CREAT, 0666, 0);
    childSem_  = sem_open(kChildSem.c_str(), O_CREAT, 0666, 0);

    if (parentSem_ == SEM_FAILED || childSem_ == SEM_FAILED)
        throw std::runtime_error("sem_open failed");
}

void SharedMemoryChannel::openResources()
{
    for (int i = 0; i < kMaxRetries; ++i)
    {
        shmFd_ = shm_open(kShmName.c_str(), O_RDWR, 0666);
        if (shmFd_ != -1)
            break;
        ::usleep(kRetryDelayUs);
    }
    if (shmFd_ == -1)
        throw std::runtime_error("shm_open failed");

    shared_ = static_cast<counter::core::Message*>(mmap(nullptr, sizeof(counter::core::Message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd_, 0));
    if (shared_ == MAP_FAILED)
        throw std::runtime_error("mmap failed");

    for (int i = 0; i < kMaxRetries; ++i)
    {
        parentSem_ = sem_open(kParentSem.c_str(), 0);
        if (parentSem_ != SEM_FAILED)
            break;
        ::usleep(kRetryDelayUs);
    }

    for (int i = 0; i < kMaxRetries; ++i)
    {
        childSem_  = sem_open(kChildSem.c_str(), 0);
        if (childSem_ != SEM_FAILED)
            break;
        ::usleep(kRetryDelayUs);
    }

    if (parentSem_ == SEM_FAILED || childSem_ == SEM_FAILED)
        throw std::runtime_error("sem_open failed");
}
}
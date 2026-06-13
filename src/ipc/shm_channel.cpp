#include "ipc/shm_channel.h"

namespace counter::ipc
{

SharedMemoryChannel::SharedMemoryChannel() = default;

SharedMemoryChannel::~SharedMemoryChannel()
{
    close();
}

void SharedMemoryChannel::open(core::ProcessRole role)
{
    role_ = role;
    if (role == core::ProcessRole::Initiator)
        createResources();
    else
        openResources();
}

void SharedMemoryChannel::send(const counter::core::Message& message)
{
    shared_->value = message.value;
    if (role_ == core::ProcessRole::Initiator)
        sem_post(childSem_);
    else
        sem_post(parentSem_);
}

counter::core::Message SharedMemoryChannel::receive()
{
    if (role_ == core::ProcessRole::Initiator)
        sem_wait(parentSem_);
    else
        sem_wait(childSem_);

    counter::core::Message msg;
    msg.value = shared_->value;
    return msg;
}

void SharedMemoryChannel::close()
{
    if (shared_)
        munmap(shared_, sizeof(counter::core::Message));

    if (shmFd_ != -1)
        ::close(shmFd_);

    if (parentSem_ != SEM_FAILED)
        sem_close(parentSem_);

    if (childSem_ != SEM_FAILED)
        sem_close(childSem_);

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

    new (shared_) counter::core::Message();

    parentSem_ = sem_open(kParentSem.c_str(), O_CREAT, 0666, 0);
    childSem_  = sem_open(kChildSem.c_str(), O_CREAT, 0666, 0);

    if (parentSem_ == SEM_FAILED || childSem_ == SEM_FAILED)
        throw std::runtime_error("sem_open failed");
}

void SharedMemoryChannel::openResources()
{
    for (int i = 0; i < 50; ++i)
    {
        shmFd_ = shm_open(kShmName.c_str(), O_RDWR, 0666);
        if (shmFd_ != -1)
            break;
        ::usleep(10000);
    }
    if (shmFd_ == -1)
        throw std::runtime_error("shm_open failed");

    shared_ = static_cast<counter::core::Message*>(mmap(nullptr, sizeof(counter::core::Message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd_, 0));
    if (shared_ == MAP_FAILED)
        throw std::runtime_error("mmap failed");

    parentSem_ = sem_open(kParentSem.c_str(), 0);
    childSem_  = sem_open(kChildSem.c_str(), 0);

    if (parentSem_ == SEM_FAILED || childSem_ == SEM_FAILED)
        throw std::runtime_error("sem_open failed");
}
}
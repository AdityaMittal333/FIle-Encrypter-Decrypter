#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "Task.hpp"
#include <memory>
#include <mutex>
#include <atomic>
#include <semaphore.h>

class ProcessManagement {
public:
    ProcessManagement();
    ~ProcessManagement();

    bool submitToQueue(std::unique_ptr<Task> task);
    void executeTask();

private:
    struct SharedMemory {
        std::atomic<int> size;
        char tasks[1000][512];  // Task string buffer
        int front;
        int rear;
    };

    SharedMemory* sharedMem;
    int shmFd;
    const char* SHM_NAME = "/my_queue";

    sem_t* itemsSemaphore;
    sem_t* emptySlotsSemaphore;
    std::mutex queueLock;
};

#endif // PROCESS_MANAGEMENT_HPP

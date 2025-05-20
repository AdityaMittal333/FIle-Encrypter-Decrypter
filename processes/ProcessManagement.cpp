#include "ProcessManagement.hpp"
#include "../encryptDecrypt/CaesarCipher.hpp"
#include "../encryptDecrypt/RSA.hpp"
#include "../encryptDecrypt/XOR.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sstream>

#include "../encryptDecrypt/CaesarCipher.hpp"
#include "../encryptDecrypt/RSA.hpp"
#include "../encryptDecrypt/XOR.hpp"

ProcessManagement::ProcessManagement() {
    itemsSemaphore = sem_open("/items_semaphore", O_CREAT, 0666, 0);
    emptySlotsSemaphore = sem_open("/empty_slots_semaphore", O_CREAT, 0666, 1000);
    shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shmFd, sizeof(SharedMemory));
    sharedMem = static_cast<SharedMemory*>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    sharedMem->front = 0;
    sharedMem->rear = 0;
    sharedMem->size.store(0);
}

ProcessManagement::~ProcessManagement() {
    munmap(sharedMem, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
    sem_close(itemsSemaphore);
    sem_close(emptySlotsSemaphore);
    sem_unlink("/items_semaphore");
    sem_unlink("/empty_slots_semaphore");
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    sem_wait(emptySlotsSemaphore);
    std::unique_lock<std::mutex> lock(queueLock);

    if (sharedMem->size.load() >= 1000) {
        lock.unlock();
        sem_post(emptySlotsSemaphore);
        return false;
    }

    strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
    sharedMem->rear = (sharedMem->rear + 1) % 1000;
    sharedMem->size.fetch_add(1);

    lock.unlock();
    sem_post(itemsSemaphore);

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed!" << std::endl;
        return false;
    } else if (pid == 0) {
        executeTask();
        _exit(0);
    }
    return true;
}

void ProcessManagement::executeTask() {
    sem_wait(itemsSemaphore);
    std::unique_lock<std::mutex> lock(queueLock);

    char taskStr[512];
    strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
    sharedMem->front = (sharedMem->front + 1) % 1000;
    sharedMem->size.fetch_sub(1);

    lock.unlock();
    sem_post(emptySlotsSemaphore);

    std::string task(taskStr);

    // Parse algorithm name from the task string (expected format: "filepath|ALGORITHM|ACTION")
    std::istringstream iss(task);
    std::string filePath, algoStr, actionStr;

    if (!std::getline(iss, filePath, '|') ||
        !std::getline(iss, algoStr, '|') ||
        !std::getline(iss, actionStr, '|')) {
        std::cerr << "Malformed task string: " << task << std::endl;
        return;
    }

    try {
        if (algoStr == "CAESAR") {
            executeCaesarCipher(task);
        } else if (algoStr == "RSA") {
            executeRSA(task);
        } else if (algoStr == "XOR") {
            executeXOR(task);
        } else {
            std::cerr << "Unknown algorithm: " << algoStr << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during task execution: " << e.what() << std::endl;
    }
}

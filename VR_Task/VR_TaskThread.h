#ifndef VR_TASK_VR_TaskThread_H_
#define VR_TASK_VR_TaskThread_H_

#include <atomic>
#include <memory>
#include <thread>

#include "VR_TaskQueue.h"


class VR_TaskThread {
public:

    VR_TaskThread(std::shared_ptr<VR_TaskQueue> taskQueue);
    ~VR_TaskThread();
    void start();
    bool isShutdown();

private:
    void processTasksLoop();

private:
    std::weak_ptr<VR_TaskQueue> m_taskQueue;
    std::atomic_bool m_shutdown;
    std::thread m_thread;
};

#endif

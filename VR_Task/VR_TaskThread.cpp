#include "VR_TaskThread.h"


VR_TaskThread::VR_TaskThread(std::shared_ptr<VR_TaskQueue> taskQueue) : m_taskQueue{taskQueue}, m_shutdown{false} {
}

VR_TaskThread::~VR_TaskThread() {
    m_shutdown = true;

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void VR_TaskThread::start() {
    m_thread = std::thread{std::bind(&VR_TaskThread::processTasksLoop, this)};
}

bool VR_TaskThread::isShutdown() {
    return m_shutdown;
}

void VR_TaskThread::processTasksLoop() {
    while (!m_shutdown) {
        auto m_actualTaskQueue = m_taskQueue.lock();

        if (m_actualTaskQueue && !m_actualTaskQueue->isShutdown()) {
            auto task = m_actualTaskQueue->pop();

            if (task) {
                task->operator()();
            }
        } else {
            m_shutdown = true;
        }
    }
}


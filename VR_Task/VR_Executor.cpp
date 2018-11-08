#include "VR_Executor.h"


VR_Executor::VR_Executor() :
        m_taskQueue{std::make_shared<VR_TaskQueue>()},
        m_loopThread{vr_make_unique<VR_TaskThread>(m_taskQueue)}
{
    m_loopThread->start();
}

VR_Executor::~VR_Executor() {
    shutdown();
}

void VR_Executor::waitForSubmittedTasks() {
    std::promise<void> flushedPromise;
    auto flushedFuture = flushedPromise.get_future();
    auto task = [&flushedPromise]() { flushedPromise.set_value(); };
    submit(task);
    flushedFuture.get();
}

void VR_Executor::shutdown() {
    m_taskQueue->shutdown();
    m_loopThread.reset();
}

bool VR_Executor::isShutdown() {
    return m_taskQueue->isShutdown();
}

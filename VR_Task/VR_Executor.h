#ifndef VR_TASK_VR_Executor_H_
#define VR_TASK_VR_Executor_H_

#include <future>
#include <utility>

#include "VR_TaskThread.h"
#include "VR_TaskQueue.h"

class VR_Executor {
public:
    VR_Executor();
    ~VR_Executor();

    template <typename Task, typename... Args>
    auto submit(Task task, Args&&... args) -> std::future<decltype(task(args...))>;
    template <typename Task, typename... Args>
    auto submitToFront(Task task, Args&&... args) -> std::future<decltype(task(args...))>;
    void waitForSubmittedTasks();
    void shutdown();
    bool isShutdown();

private:
    std::shared_ptr<VR_TaskQueue> m_taskQueue;
    std::unique_ptr<VR_TaskThread> m_loopThread;
};

template <typename Task, typename... Args>
auto VR_Executor::submit(Task task, Args&&... args) -> std::future<decltype(task(args...))> {
    return m_taskQueue->push(task, std::forward<Args>(args)...);
}

template <typename Task, typename... Args>
auto VR_Executor::submitToFront(Task task, Args&&... args) -> std::future<decltype(task(args...))> {
    return m_taskQueue->pushToFront(task, std::forward<Args>(args)...);
}


#endif

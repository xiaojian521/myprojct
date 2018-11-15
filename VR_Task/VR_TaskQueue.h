#ifndef VR_TASK_VR_TaskQueue_H_
#define VR_TASK_VR_TaskQueue_H_

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <utility>

class VR_TaskQueue {
public:
    VR_TaskQueue();
    //typename 定义的是类型 ， typename... Args指的是多个模板参数
    template <typename Task, typename... Args>
    //->表示返回值是什么，std::future可以从异步任务中获取结果，decltype返回参数的数据类型
    auto push(Task task, Args&&... args) -> std::future<decltype(task(args...))>;
    template <typename Task, typename... Args>
    //push的优先级更高
    auto pushToFront(Task task, Args&&... args) -> std::future<decltype(task(args...))>;
    //弹出任务
    std::unique_ptr<std::function<void()>> pop();
    //停止
    void shutdown();
    //判断是否停止
    bool isShutdown();

private:
    using Queue = std::deque<std::unique_ptr<std::function<void()>>>;

    //私有成员，提供接口
    template <typename Task, typename... Args>
    auto pushTo(bool front, Task task, Args&&... args) -> std::future<decltype(task(args...))>;

    Queue m_queue;
    //原子类型
    std::condition_variable m_queueChanged;
    std::mutex m_queueMutex;
    //原子类型
    std::atomic_bool m_shutdown;
};

//===================================================================================================

template <typename Task, typename... Args>
auto VR_TaskQueue::push(Task task, Args&&... args) -> std::future<decltype(task(args...))> {
    bool front = true;
    return pushTo(!front, std::forward<Task>(task), std::forward<Args>(args)...);
}

template <typename Task, typename... Args>
auto VR_TaskQueue::pushToFront(Task task, Args&&... args) -> std::future<decltype(task(args...))> {
    bool front = true;
    return pushTo(front, std::forward<Task>(task), std::forward<Args>(args)...);
}

template <typename T>
inline static void forwardPromise(std::shared_ptr<std::promise<T>> promise, std::future<T>* future) {
    promise->set_value(future->get());
}

template <>
inline void forwardPromise<void>(std::shared_ptr<std::promise<void>> promise, std::future<void>* future) {
    future->get();
    promise->set_value();
}

template <typename Task, typename... Args>
//push的时候要对参数进行解析
auto VR_TaskQueue::pushTo(bool front, Task task, Args&&... args) -> std::future<decltype(task(args...))> {
    //std::forward 常常需要将参数原封不动的转发给另外一个函数
    auto boundTask = std::bind(std::forward<Task>(task), std::forward<Args>(args)...);


    using PackagedTaskType = std::packaged_task<decltype(boundTask())()>;
    auto packaged_task = std::make_shared<PackagedTaskType>(boundTask);
    auto cleanupPromise = std::make_shared<std::promise<decltype(task(args...))>>();
    auto cleanupFuture = cleanupPromise->get_future();
    auto translated_task = [packaged_task, cleanupPromise]() mutable {
        packaged_task->operator()();
        auto taskFuture = packaged_task->get_future();
        packaged_task.reset();
        forwardPromise(cleanupPromise, &taskFuture);
    };
    packaged_task.reset();

    {
        std::lock_guard<std::mutex> queueLock{m_queueMutex};
        if (!m_shutdown) {
            m_queue.emplace(front ? m_queue.begin() : m_queue.end(), new std::function<void()>(translated_task));
        } else {
            using FutureType = decltype(task(args...));
            return std::future<FutureType>();
        }
    }

    m_queueChanged.notify_all();
    return cleanupFuture;
}

#endif

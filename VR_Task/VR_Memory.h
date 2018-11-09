#ifndef VR_TASK_MEMORY_H_
#define VR_TASK_MEMORY_H_

#include <memory>
#include <utility>

template <typename T, typename... Args>
std::unique_ptr<T> vr_make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif

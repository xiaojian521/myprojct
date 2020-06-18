#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <testbinder.h>

using namespace android;

//向Servicemanager注册testbinder服务
int main(int argc, char** argv) {
    /**
     * (1)打开/dev/bind设备,这就相当于与内核的Binder驱动有了交互的通道
     * (2)对返回的fd使用mmap,这样Binder驱动就会分配一快内存来接收数据
     * (3)由于Prcocess具有唯一性,因此一个进程只打开设备一次
     */
    sp<ProcessState> proc(ProcessState::self());
    /**
     * defaultServiceManager函数的实现在IServiceManager.cpp中完成.它会返回一个IServiceManager对象
     * 通过这个对象,我们可以与另一个进程ServiceManager进行交互
     */
    sp<IServiceManager> sm = defaultServiceManager();
    LOGI("ServiceManager:%p",sm.get());
    testbinder::instantiate();
    /**
     * 启动线程池
     */
    ProcessState::self()->startThreadPool();
    /**
     *调用joinThreadPool读取binder设备,查看是否有请求
     */
    IPCThreadState::self()->joinThreadPool();
    return 0;
}




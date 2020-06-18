#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <private/android_filesystem_config.h>
#include "DeleteFile.h"

using namespace android;
using namespace com::example::mortal;

int main(int argc, char** argv) {
    LOGD("main begine........");
    sp<ProcessState> proc(ProcessState::self());
    LOGD("[ %s (L : %d) ] \n",__FUNCTION__,__LINE__);
    sp<IServiceManager> sm = defaultServiceManager();//获得ServiceManager
    android::sp<DeleteFile> sp_deletefile = new DeleteFile();
    ProcessState::self()->startThreadPool();//启动缓冲池
    IPCThreadState::self()->joinThreadPool();//这里把服务添加到Binder闭合循环进程中
    LOGD("main end........");
}

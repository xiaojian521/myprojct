#include "DeleteFile.h"

#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cutils/log.h>

namespace com {

namespace example {

namespace mortal {

using namespace android;
using namespace com::example::mortal;

DeleteFile::DeleteFile() :
    mClientListener(NULL),
    mCallBack(NULL) {

    android::String16 serviceName = this->getInterfaceDescriptor();
    status_t r = defaultServiceManager()->addService(serviceName, IInterface::asBinder(this));
    if (NO_ERROR != r) {
        LOGW("[ %s (L : %d) ] Add Service Failed \n",__FUNCTION__,__LINE__);
    }
    LOGD("[ %s (L : %d) ] Add Service Success serviceName is : %s \n",__FUNCTION__,__LINE__,serviceName.string());

    mCallBack = new com::example::mortal::DeleteFile::CallBack();
}

DeleteFile::~DeleteFile() {
}

::android::binder::Status DeleteFile::dealDeleteFiles(const ::android::String16& path, int32_t* _aidl_return) {
    const char* path_char = android::String8(path).string();
    LOGD("[ %s (L : %d) ] path name is = %s \n",__FUNCTION__,__LINE__,path_char);
    char* cmd = (char*)malloc(16+strlen(path_char));
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "rm -rf %s", path_char);
    int result = system(cmd);
    free(cmd);
    return android::binder::Status::ok();
}


::android::binder::Status DeleteFile::registerLisntener(const ::android::sp<::com::example::mortal::IClientListener>& listener) {
    //调用listener的通知函数
    LOGD("[ %s (L : %d) ] Registed Lisntener To Server Success \n",__FUNCTION__,__LINE__);
    mClientListener = listener;
    android::String16 result = android::String16("Server notifyClientMessage result !");
    mClientListener->notifyClientMessage(result);

    mClientListener->registerCallBack(mCallBack);

    return android::binder::Status::ok();
}

//=================================================================================
DeleteFile::CallBack::~CallBack() {
    
}

::android::binder::Status DeleteFile::CallBack::replyServerMessage(const ::android::String16& msg) {
    const char* msg_char = android::String8(msg).string();
    LOGD("[ %s (L : %d) ] msg name is = %s \n",__FUNCTION__,__LINE__,msg_char);
    char* cmd = (char*)malloc(16+strlen(msg_char));
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "rm -rf %s", msg_char);
    int result = system(cmd);
    free(cmd);
    return android::binder::Status::ok();
}

} //namespace com

} // namespace example

} // namespace mortal
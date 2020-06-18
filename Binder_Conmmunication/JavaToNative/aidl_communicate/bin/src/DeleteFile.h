#ifndef _DELETE_FILE_H
#define _DELETE_FILE_H

#include <stdio.h>
#include <utils/RefBase.h>
#include <utils/threads.h>
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include <android/log.h>

#include <com/example/mortal/BnDeleteFile.h>
#include <com/example/mortal/BnServerCallBack.h>

#define LOG_TAG "xjtest"
#define DEBUG
#define ANDROID_PLATFORM

#ifdef DEBUG
    #ifdef ANDROID_PLATFORM
        #define LOGD(...)((void)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__))
        #define LOGI(...)((void)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__))
        #define LOGW(...)((void)__android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__))
        #define LOGE(...)((void)__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__))

    #else
        #define LOGD(fmt, ...)printf(fmt"\n",##__VA_ARGS__)
        #define LOGI(fmt, ...)printf(fmt"\n",##__VA_ARGS__)
        #define LOGW(fmt, ...)printf(fmt"\n",##__VA_ARGS__)
        #define LOGE(fmt, ...)printf(fmt"\n",##__VA_ARGS__)
    #endif
#endif

namespace com {

namespace example {

namespace mortal {

class DeleteFile : public BnDeleteFile {
    
public:
    DeleteFile();
    virtual ~DeleteFile();
    static int instantiate();

private:
    class CallBack : public BnServerCallBack {
    public:
        CallBack() = default;
        ~CallBack();

    public :
        virtual ::android::binder::Status replyServerMessage(const ::android::String16& msg) override;
    };


public:
    virtual ::android::binder::Status dealDeleteFiles(const ::android::String16& path, int32_t* _aidl_return) override;
    virtual ::android::binder::Status registerLisntener(const ::android::sp<::com::example::mortal::IClientListener>& listener) override;

private:
    android::sp<com::example::mortal::DeleteFile::CallBack> mCallBack;
    android::sp<com::example::mortal::IClientListener> mClientListener;
};



} //namespace com

} // namespace example

} // namespace mortal

#endif
#ifndef _DELETE_FILE
#define _Delete_FILE

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/threads.h>

#include <stdio.h>
#include <android/log.h>

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

namespace android{
class DeleteFile : public BBinder {
    
public:
    DeleteFile();
    virtual ~DeleteFile();
    virtual status_t onTransact(uint32_t, const Parcel&, Parcel*, uint32_t);

    static int instantiate();
};

}//namespace

#endif
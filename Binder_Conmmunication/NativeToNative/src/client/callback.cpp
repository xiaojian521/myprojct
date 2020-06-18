#include "callback.h"
#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <utils/String16.h>
#include <utils/threads.h>
#include <utils/Atomic.h>

//#include <cutils/bitops.h>
#include <cutils/properties.h>
#include <cutils/compiler.h>

namespace android {

int callback::notifyCallback(int a) {
    LOGD("TK----->>>>>>callback.cpp>>>callback:notifyCallback\n");
    return 1;
}

status_t callback::onTransact(uint32_t code,const Parcel& data,Parcel* reply,uint32_t flags) {
    LOGD("TK----->>>>>>callback.cpp>>>callback:onTransact\n");
    return Bncallback::onTransact(code,data,reply,flags);
}

}

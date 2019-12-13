#include "testbinder.h"
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
//#include <utils/Trace.h>
#include <binder/Parcel.h>
#include <utils/String16.h>
#include <utils/threads.h>
#include <utils/Atomic.h>

//#include <cutils/bitops.h>
#include <cutils/properties.h>
#include <cutils/compiler.h>

namespace android {
    int testbinder::testinterface(int a) {
        LOGD("TK----->>>>>>testbinder.cpp>>>testbinder::testinterface\n");
        sp<Icallback> c = mcallback;
        c->notifyCallback(2);
        return a+2;
    }

    int testbinder::setcallback(const sp<Icallback>& callback) {
        LOGD("TK----->>>>>>testbinder.cpp>>>testbinder::setcallback\n");
        mcallback = callback;
        return 1;
    }

    status_t testbinder::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
        LOGD("TK----->>>>>>testbinder.cpp>>>testbinder::onTransact\n");
        return Bntestbinder::onTransact(code,data,reply,flags);
    }
}

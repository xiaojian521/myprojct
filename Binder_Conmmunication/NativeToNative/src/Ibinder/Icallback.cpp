#include "Icallback.h"
#include "Itestbinder.h"
#include <binder/Parcel.h>
#include <binder/IInterface.h>

namespace android {
enum {
    NOTIFY_CALLBACK,
};

//客户端
class Bpcallback : public BpInterface<Icallback> {
public:
    Bpcallback(const sp<IBinder>& impl) : BpInterface<Icallback>(impl) {
    }
    virtual int notifyCallback(int a) {
        LOGD("=========================================================\n");
        LOGD("TK----->>>>>>Icallback.cpp>>>Bpcallback:notifyCallback\n");
        Parcel data,reply;
        data.writeInt32(a);
        remote()->transact(NOTIFY_CALLBACK,data,&reply);
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(callback, "android.test.Icallback");

//服务端
status_t Bncallback::onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags) {
    LOGD("TK----->>>>>>Icallback.cpp>>>Bpcallback:onTransact\n");
    switch(code) {
        case NOTIFY_CALLBACK:{
            LOGD("TK----->>>>>>Icallback.cpp>>>Bpcallback:onTransact>>NOTIFY_CALLBACK\n");
            //CHECK_INTERFACE(Itestbinder, data, reply);
            reply->writeInt32(notifyCallback((int) data.readInt32()) );
            return NO_ERROR;
        }break;
        default: {
            LOGD("TK----->>>>>>Icallback.cpp>>>Bpcallback:onTransact>>222\n");
            return BBinder::onTransact(code,data,reply,flags);
        }
    }
}
}

#include "Itestbinder.h"
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include "Icallback.h"

namespace android {
enum {
    TEST_INTERFACE,
    SET_CALLBACK
};

//客户端
class Bptestbinder : public BpINterface<Itestbinder> {
public :
    Bptestbinder(const sp<IBinder>& impl) : BpInterface<Itestbinder>(impl) {
    }
    virtual int testinterface(int a) {
        LOGD("=========================================================\n");
        LOGD("TK----->>>>>>Itestbinder.cpp>>>Bptestbinder:testinterface\n");
        Parcel data, reply;
        data.writeInt32(a);
        remote()->transact(TEST_INTERFACE,data,&reply);
        return reply.readInt32();
    }
    virtual int setcallback(const sp<Icallback>& callback) {
        LOGD("=========================================================\n");
        LOGD("TK----->>>>>>Itestbinder.cpp>>>Bptestbinder:setcallback\n");
        Parcel data,reply;
        data.writeStrongBinder(callback->asBinder());
        remote()->transact(SET_CALLBACK,data,&reply);
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(testbinder,"android.test.Itestbinder");

//服务端
status_t Bntestbinder::onTransact(uint32_t code,
                                  const Parcel& data,
                                  Parcel* reply, uint32_t flags) {
    LOGD("TK----->>>>>>Itestbinder.cpp>>>Bptestbinder:onTransact\n");
    switch (code) {
    case TEST_INTERFACE: {
        LOGD("TK----->>>>>>Itestbinder.cpp>>>Bptestbinder:onTransact>>TEST_INTERFACE\n");
        reply->writeInt32(testinterface((int)data.readInte32()));
        return NO_ERROR;
    }break;
    case SET_CALLBACK: {
        LOGD("TK----->>>>>>Itestbinder.cpp>>>Bptestbinder:onTransact>>SET_CALLBACK\n");
        sp<Icallback> callback = interface_cast<Icallback>(data,readStrongBinder());
        //int a = connect(Client);
        reply->writeInt32(setcallback(callback));
        return NO_ERROR;
    }
    default:
        LOGD("TK----->>>>>>Itestbinder.cpp>>>Bptestbinder:onTransact>>333\n");
        return BBinder::onTransact(code,data,reply,flags);
    }


}

}

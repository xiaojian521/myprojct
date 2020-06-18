#ifndef Itestbinder_H
#define Itestbinder_h

#include <binder/IInterface.h>
#include "Icallback.h"

namespace android {

//相当于binder类
class Itestbinder:public IInterface {
public:
    /**
     *#define DECLARE_META_INTERFACE(INTERFACE) \
      static const String16 descriptor; \
      static sp<I##INTERFACE> asInterface(const sp<IBinder>& obj); \
      virtual String16 getInterfaceDescriptor() const; \
     */
    DECLARE_META_INTERFACE(testbinder);

    /**
     * @brief testinterface
     * server transfer interface
     */
    virtual int testinterface(int a) = 0;
    /**
     * @brief setcallback
     * server transfer interface
     */
    virtual int setcallback(const sp<Icallback>& callback) = 0;

};

/**
 * @brief The Bntestbinder class
 * BnInterface是服务端的接口，BpInterface是客户端的代理接口
 * B: Binder
 * n: native
 * p: proxy
 */
class Bntestbinder : public BnInterface<Itestbinder> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags = 0);
};

}

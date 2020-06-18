#ifndef TESTBINDER_H
#define TESTBINDER_H

#include "Itestbinder.h"
#include "Icallback.h"
#include <binder/BinderService.h>

namespace android {
class testbinder : public BinderService<testbinder>, public Bntestbinder
{
friend class BinderService<testbinder>;
public:
    /**
     * @brief getServiceName
     * provide interface get service name
     */
    static const char* getServiceName() {
        return "test.Itestbinder";
    }

    /**
     * @brief testinterface
     * onTransact transfer testinterface function
     */
    virtual int testinterface(int a);
    /**
     * @brief testinterface
     * onTransact transfer setcallback function
     */
    virtual int setcallback(const sp<Icallback>& callback);

    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags);
protected:
    sp<Icallback> mcallback;
};

}

#endif // TESTBINDER_H

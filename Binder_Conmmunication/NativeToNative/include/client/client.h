#ifndef CLIENT_H
#define CLIENT_H

#include "../Ibinder/Itestbinder.h"

namespace android {

class client {
public:
    static const sp<Itestbinder>& get_test_binder();
    static sp<Itestbinder> gtestbinder;
};

}


#endif // CLIENT_H

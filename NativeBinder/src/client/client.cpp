#include "client.h"
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <stdio.h>

namespace android {

    sp<Itestbinder> client::gtestbinder;

    const sp<Itestbinder>& client::get_test_binder() {
        if(gtestbinder == 0) {
            sp<IServiceManager> sm = defaultServiceManager();
            sp<IBinder> binder;
            do {
                binder = sm->getService(String16("test.Itestbinder"));
                if(binder != 0) {
                    break;
                    printf("testbinder not published, waiting...");
                    usleep(500000);//5s
                }
            }while(true);
            gtestbinder = interface_cast<Itestbinder>(binder);
        }
        if(gtestbinder == 0)
            printf("no testbinder!?");
        return gtestbinder;
    }

}

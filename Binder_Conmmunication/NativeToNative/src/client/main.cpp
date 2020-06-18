#include "stdio.h"
#include "client.h"
#include "callback.h"

using namespace android;

int main(int args, char* argv[]) {
    client* myclient = new client();
    if(myclient == NULL)
        return 0;
    const sp<Itestbinder>& tb = myclient->get_test_binder();
    if(tb == NULL)
        return 0;
    sp<callback> c = new callback();
    ProcessState::self()->startThreadPool();//add by tank
    int a = tb->setcallback(c);
    a = tb->testinterface(3);
    LOGD("TK----------->>>retult is %d\n",a);
    delete myclient;
    return 0;
}

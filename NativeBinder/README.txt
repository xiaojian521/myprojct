Android Binder机制的Native应用—双向通信
https://blog.csdn.net/tankai19880619/article/details/21224151

D/ ( 565): ==========================================================
D/ ( 565): TK—->>>>>>Itestbinder.cpp>>>>Bptestbinder::setcallback
D/ ( 563): TK—->>>>>>testbinder.cpp>>>>testbinder::onTransact
D/ ( 563): TK—->>>>>>Itestbinder.cpp>>>>Bntestbinder::onTransact
D/ ( 563): TK—->>>>>>Itestbinder.cpp>>>>Bntestbinder::onTransact>>SET_CALLBACK
D/ ( 563): TK—->>>>>>testbinder.cpp>>>>testbinder::setcallback
D/ ( 565): ==========================================================
D/ ( 565): TK—->>>>>>Itestbinder.cpp>>>>Bptestbinder::testinterface
D/ ( 563): TK—->>>>>>testbinder.cpp>>>>testbinder::onTransact
D/ ( 563): TK—->>>>>>Itestbinder.cpp>>>>Bntestbinder::onTransact
D/ ( 563): TK—->>>>>>Itestbinder.cpp>>>>Bntestbinder::onTransact>>TEST_INTERFACE
D/ ( 563): TK—->>>>>>testbinder.cpp>>>>testbinder::testinterface
D/ ( 563): ==========================================================
D/ ( 563): TK—->>>>>>Icallback.cpp>>>>Bpcallback::notifyCallback
D/ ( 565): TK—->>>>>>callback.cpp>>>>callback::onTransact
D/ ( 565): TK—->>>>>>Icallback.cpp>>>>Bncallback::onTransact
D/ ( 565): TK—->>>>>>Icallback.cpp>>>>Bncallback::onTransact>>NOTIFY_CALLBACK
D/ ( 565): TK—->>>>>>callback.cpp>>>>callback::notifyCallback
D/ ( 565): TK——–>>>result is 5
服务端先启动->客户端启动->获取服务端Itestbinder对象->将callback注册到服务端->
(binder客户端)TK—->>>>>>Itestbinder.cpp>>>>Bptestbinder::setcallback->
(服务端)TK—->>>>>>testbinder.cpp>>>>testbinder::onTransact->
(binder服务端)TK—->>>>>>Itestbinder.cpp>>>>Bntestbinder::onTransact->
(binder服务端)TK—->>>>>>Itestbinder.cpp>>>>Bntestbinder::onTransact>>SET_CALLBACK->
(服务端)TK—->>>>>>testbinder.cpp>>>>testbinder::setcallback->调用成功
服务端回调客户端callback(此时的服务端相当于客户端,服务端相当于客户端)->
(binder客户端)TK—->>>>>>Icallback.cpp>>>>Bpcallback::notifyCallback->
(callback服务端)TK—->>>>>>callback.cpp>>>>callback::onTransact()->
(binder服务端)TK—->>>>>>Icallback.cpp>>>>Bncallback::onTransact->
(binder服务端)TK—->>>>>>Icallback.cpp>>>>Bncallback::onTransact>>NOTIFY_CALLBACK->
(callback服务端)TK—->>>>>>callback.cpp>>>>callback::notifyCallback->调用成功
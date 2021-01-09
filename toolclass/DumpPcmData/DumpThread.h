#ifndef DUMPTHREAD_H
#define DUMPTHREAD_H

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "CommonDefine.h"

class DumpThread {

public:
    DumpThread();

    ~DumpThread();

    void Start();

    bool PostMessage(const MessageInfo& message);

    void Stop();

private:
    void RunOperation(const MessageInfo& message);

    void Run();

private:
    FILE*                         mFdout;
    FILE*                         mFdin;
    volatile long                 mOffsetout;
    volatile long                 mOffsetin;
    bool                          mThreadStatus;
    std::mutex                    m_mtx;
    std::thread*                  td;
    std::condition_variable       m_cv;
    std::list<MessageInfo>        mMsgList;
};

#endif
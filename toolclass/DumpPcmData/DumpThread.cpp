#include <DumpThread.h>
#include <thread>
#include <fstream>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#define LOG_TAG "DumpThread"

DumpThread::DumpThread():mFdout(nullptr),
    mFdin(nullptr),
    mOffsetout(0),
    mOffsetin(0),
    mThreadStatus(false),
    td(nullptr) {
}

DumpThread::~DumpThread() {
    Stop();
}

void DumpThread::Start() {
    if(nullptr == td) {
        mThreadStatus = false;
        td = new std::thread(&DumpThread::Run,this);
    }
}

void DumpThread::Stop() {
    if(nullptr != td) {
        mThreadStatus = true;
        if (nullptr != td) {
            m_cv.notify_all();
            if (td->joinable()) {
                td->join();
            }
            delete td;
            td = nullptr;
        }
        {
            std::unique_lock <std::mutex> lck(m_mtx); 
            mMsgList.clear();
        }
    }
    if(nullptr != mFdout) {
        fflush(mFdout);
        fclose(mFdout);
        mFdout = nullptr;
        mOffsetout = 0;
    }
    if(nullptr != mFdin) {
        fflush(mFdin);
        fclose(mFdin);
        mFdin = nullptr;
        mOffsetin = 0;
    }
}

void DumpThread::RunOperation(const MessageInfo& message) {
    if(strcmp(message.type,DUMP_TYPE_OUT) == 0) {
        if(access(DUMP_OUT_FILE_PATH,0) != 0) {
            ALOGD("pa:%s not have",DUMP_OUT_FILE_PATH);
            mFdout = nullptr;
            mOffsetout = 0;
        }
        if(nullptr == mFdout) {
            mFdout = fopen(DUMP_OUT_FILE_PATH,"ab+");
            if(nullptr == mFdout) {
                ALOGD("file is not open path:%s",DUMP_OUT_FILE_PATH);
                mOffsetout = 0;
                return;
            }
        }
        if(mOffsetout >= (long)(message.filesize)*1024*1024) {
            fflush(mFdout);
            fclose(mFdout);
            mFdout = nullptr;
            mOffsetout = 0;
            char command[92];
            sprintf(command, "setprop %s 0", DUMP_OUT_PROPERTY);
            system(command);
            return;
        }
        fwrite((void*)message.buffer,message.count,1,mFdout);
        mOffsetout += message.count;
        fflush(mFdout);
    }
    else if (strcmp(message.type,DUMP_TYPE_IN) == 0) {
        if(access(DUMP_IN_FILE_PATH,0) != 0) {
            ALOGD("pa:%s not have",DUMP_IN_FILE_PATH);
            mFdout = nullptr;
            mOffsetout = 0;
        }
        if(nullptr == mFdin) {
            mFdin=fopen(DUMP_IN_FILE_PATH,"ab+");
            ALOGD("pcm::dump_data() file is not open");
            if(nullptr == mFdin) {
                mOffsetin = 0;
                return;
            }
        }
        if(mOffsetin >= (long)(message.filesize)*1024*1024) {
            fflush(mFdin);
            fclose(mFdin);
            mFdin = nullptr;
            mOffsetin = 0;
            char command[92];
            sprintf(command, "setprop %s 0", DUMP_IN_PROPERTY);
            system(command);
            return;
        }
        fwrite((void*)message.buffer,message.count,1,mFdin);
        mOffsetout += message.count;
        fflush(mFdin);
    }
}

void DumpThread::Run() {
    while (true) {
        if (mMsgList.empty()) {
            std::unique_lock <std::mutex> lck_run(m_mtx);
            m_cv.wait(lck_run);
        }
        if (true == mThreadStatus) {
            break;
        }
        while (!mMsgList.empty()) {
            if (true == mThreadStatus) {
                break;
            }
            MessageInfo msg;
            {
                std::unique_lock <std::mutex> lck(m_mtx);
                if (!mMsgList.empty()) {
                    msg = mMsgList.front();
                    mMsgList.pop_front();
                }
                else {
                    continue;
                }
            }
            RunOperation(msg);
        }
    }
}

bool DumpThread::PostMessage(const MessageInfo& message) {
    {
        std::unique_lock <std::mutex> lck(m_mtx);
        mMsgList.push_back(message);
    }
    m_cv.notify_all();

    return true;
}
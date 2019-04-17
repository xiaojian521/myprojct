#ifndef VR_OPERATIONFILECONTROL_H
#define VR_OPERATIONFILECONTROL_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif
#include <list>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>

class VR_ControllerBase;

class VR_OperationFileControl

{
public:
    VR_OperationFileControl();

    ~VR_OperationFileControl();

    bool Initialize(VR_ControllerBase* controller);
    //create thread
    bool Start();

    //add message to thread
    bool PostMessage(const std::string& message);

    //stop thread
    void Stop();

private:
    //operation file
    void RunOperationFile(const std::string &message);

    //start Thread
    void Run();

private:
    struct VR_MsgInfo
    {
        std::string strMsg;
    };

    std::list<VR_MsgInfo>         m_msglist;      ///< Message list
    bool                          m_bThreadQuit;
    std::mutex                    m_mtx;
    std::thread*                  td;
    std::condition_variable       m_cv;
    VR_ControllerBase*            m_controller;
};

#endif // VR_OPERATIONFILECONTROL_H

#include "VR_OperationFileControl.h"

#ifndef VR_LOG_H
#    include "VR_Log.h"
#endif

#ifndef VR_XMLPARSER_H
#    include "VR_XmlParser.h"
#endif

#ifndef VR_CONTROLLERBASE_H
#    include "VR_ControllerBase.h"
#endif

#include <thread>
#include <fstream>

VR_OperationFileControl::VR_OperationFileControl()
: m_msglist()
, td(nullptr)
, m_bThreadQuit(false)
, m_controller(nullptr)

{

}

bool VR_OperationFileControl::Initialize(VR_ControllerBase* controller)
{
    m_controller = controller;
    return true;
}

VR_OperationFileControl::~VR_OperationFileControl()
{
    if(nullptr != td) {
        Stop();
    }
}

bool VR_OperationFileControl::Start()
{
    if(nullptr == td) {
        m_bThreadQuit = false;
        td = new std::thread(&VR_OperationFileControl::Run,this);
    }
    return true;
}

void VR_OperationFileControl::Stop()
{
    m_bThreadQuit = true;
    if (nullptr != td) {
        m_cv.notify_all();
        if (td->joinable()) {
            td->join();
        }
        delete td;
        td = nullptr;
    }
}

void VR_OperationFileControl::RunOperationFile(const std::string &message)
{
    VR_LOGD_FUNC();
    std::fstream webappfile;
    std::ofstream webappfile_test;
    std::string line = "";
    VR_XmlParser parser(message);
    std::string strActionKey = parser.getXmlKey();
    std::string strOp = parser.getValueByKey("op");
    std::string strappid = parser.getValueByKey("appid");
    std::string stroperation = parser.getValueByKey("operation");

    std::string f_op = "";
    std::string f_packageid = "";

    //read file
    if("server_status" == strOp) {
        webappfile.open("/data/webapplist.txt", std::ios::in | std::ios::out | std::ios::app);
        if (true == webappfile.is_open()) {
            //read file
            while (std::getline(webappfile, line)) {
                std::string deMsg = "<WEBFILE>";
                deMsg += line;
                deMsg += "</WEBFILE>";
                if(nullptr != m_controller) {
                    m_controller->PostMessage(deMsg);
                }
            }
        }
        else {
            VR_LOGI("open file failed");
        }
        webappfile.close();
    }
    else if ("script_operation" == strOp) {
       webappfile.open("/data/webapplist.txt", std::ios::in | std::ios::out | std::ios::app);
       //recive baidupoc engine message delete file
       webappfile_test.open("/data/webapplist-test.txt", std::ios::app);
       if (true == webappfile.is_open() && true == webappfile_test.is_open()) {
           //read file
           while (std::getline(webappfile, line)) {
               VR_XmlParser parserfile(line);
               f_op = parserfile.getValueByKey("op");
               f_packageid = parserfile.getValueByKey("packageid");
               if(f_op == stroperation && f_packageid == strappid) {
                   continue;
               }
               webappfile_test<<line<<std::endl;
           }
       }
       else {
           VR_LOGI("open file failed");
       }
       webappfile.close();
       remove("/data/webapplist.txt");
       rename("/data/webapplist-test.txt", "/data/webapplist.txt" );
       webappfile_test.flush();
       webappfile_test.close();
    }
    else if ("webvrapp" == strActionKey) {
        webappfile.open("/data/webapplist.txt", std::ios::in | std::ios::out | std::ios::app);
        if (true == webappfile.is_open()) {
            webappfile<<message<<std::endl;
            webappfile.flush();
            webappfile.close();
        }
        else {
            VR_LOGI("open file failed");
        }
        webappfile.close();
    }
}

void VR_OperationFileControl::Run() {
    VR_LOGD_FUNC();
    while (true) {
        if (m_msglist.empty()) {
            std::unique_lock <std::mutex> lck_run(m_mtx);
            m_cv.wait(lck_run);
        }
        if (true == m_bThreadQuit) {
            break;
        }
        while (!m_msglist.empty()) {
            if (true == m_bThreadQuit) {
                break;
            }
            VR_MsgInfo msgInfo;
            {
                std::unique_lock <std::mutex> lck(m_mtx);
                if (!m_msglist.empty()) {
                    msgInfo = m_msglist.front();
                    m_msglist.pop_front();
                    RunOperationFile(msgInfo.strMsg);
                }
                else {
                    continue;
                }
            }
        }
    }
}

bool VR_OperationFileControl::PostMessage(const std::string& message)
{
    VR_LOGD_FUNC();
    VR_MsgInfo msgInfo;
    msgInfo.strMsg = message;
    {
        std::unique_lock <std::mutex> lck(m_mtx);
        m_msglist.push_back(msgInfo);
    }
    m_cv.notify_all();

    return true;
}



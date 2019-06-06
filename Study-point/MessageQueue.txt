C++线程消息队列
1.设计思想
创建线程类继承ControllerBase类
只含有virtual bool PostMessage(const std::string& message, int actionSeqId = -1) = 0;
优点对外使用只提供父类接口对子类具体实现进行隐藏,增加安全性,外部只能调用PoseMessage添加消息函数

在Intialize中传入处理对象
VR_DEProcessor类
只含有virtual void ProcessMessage(const std::string& message, int actionSeqId  = -1) = 0;
优点需要处理的类继承VR_DEProcessor类重写处理函数通过线程类的Initialize传入就可以进行调用

线程类:
public:
    bool Initialize(VR_DEProcessor* processor);
    bool Start();
    //add message to thread
    bool PostMessage(const std::string& message);
    //stop thread
    void Stop();
private:
    //Thread Run
    void Run();
private:
    bool                          m_bThreadQuit;
    std::mutex                    m_mtx;
    std::thread*                  td;
    std::condition_variable       m_cv;
    std::list<std::string>        m_msglist;
具体实现:
bool Start() {
    if(nullptr == td) {
        m_bThreadQuit = false;
        td = new std::thread(&类名::Run,this);
    }
    return true;
}

bool PostMessage(const std::string& message) {
    //这里一定要加{}作用域
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_msglist.push_back(message);
    }
    m_cv.notify_all();
}

bool PostFrontMessage(const std::string& message){}

void Run() {
    while(true) {
        if(true == m_msglist.empty()) {
            std::unique_lock <std::mutex> lck_run(m_mtx);
            lck.wait(lck_run);
        }
        if(true == m_bThreadQuit) {
            break;
        }
        while(true != m_msglist.empty()) {
            if(true == m_bThreadQuit) {
                break;
            }
            std::string msginfo;
            {
                std::unique_lock<std::mutex> lck_run(m_mtx);
                if(!m_msglist.empty()) {
                    msginfo = m_msglist.front();
                    m_msglist.pop_front();
                } else {
                    continue;
                }
            }
            //process message
            processor->ProcessMessage();
        }
    }
}

void ClearMessage()                                       
{
    std::unique_lock<std::mutex> lck_run(m_mtx);
    m_msglist.clear();
}

void Stop() {
    m_bThreadQuit = true;
    if(nullptr != td) {
        m_cv.notify_all();
        if(true == td.joinable()) {
            td->join();
        }
        delete td;
        td = nullptr;
    }
}

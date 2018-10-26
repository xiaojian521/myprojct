#include<iostream>
#include<map>
#include<algorithm>
//=====================================================================
#include"config.h"
enum {
    VR_STOP,
    VR_START_SESSION,
    VR_START_AISLE,
    VR_START_ENGIN,
    VR_CLOSE_AISLE,
    VR_CLOSE_SESSION,
    VR_CLOSE_ENGIN
};

//=====================================================================
#include"state.h"
class State
{
public:
    virtual void Operation(Work* work) = 0;
};

//=====================================================================

#include"work.h"
class Work
{
private:
    int m_currentState;  //当前状态
    int m_lastState;     //记录上一个状态
    std::map<int,State*> m_stateManager;
public:
    Work():m_currentState(0), m_lastState(0)
    {
    }
    ~Work(){}
public:
    void init()
    {
        m_stateManager[0] = new StopState;
        m_stateManager[1] = new StartSessionState;
        m_stateManager[2] = new StartAisleState;
        m_stateManager[3] = new StartEnginState;
        m_stateManager[4] = new CloseAisleState;
        m_stateManager[5] = new CloseSessionState;
        m_stateManager[6] = new CloseEnginState;
        (m_stateManager[0])->Operation(this);
    }
    void uninit()
    {
        auto it = work->m_stateManager.begin();
        for(it ; it != end(); it++) {
            delete it->second;
            it.erase();
        }
    }
    int GetCurrentState()
    {
        return m_currentevent;
    }
    int GetLastState()
    {
        return m_lastState;
    }
    void SetState(int stateId)
    {
        if(stateId == m_currentState) {
            std::cout<<"curent is same  stateId"<<std::endl;
            return ;
        } else {
            int temp = m_lastState;
            swap(m_currentState,m_lastState);
            auto it = m_stateManager.find(stateId);
            if(it != m_stateManager.end()) {
                if(!(it->second)->Operation(this)) {
                    swap(m_currentState,m_lastState);
                    swap(m_lastState,temp);
                    std::cout<<"next failed"<<std::cout;
                }
            } 
        }
    }
}

//=====================================================================
/**
 * 打开session
 * */
class StartSessionState:public State
{
public:
    StartSessionState(){}
    ~StartSessionState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_STOP && work -> GetCurrentState() == VR_START_SESSION) {
            //do something
            std::cout<<"state : "<<VR_START_SESSION<<"打开session"<<std::endl;
            return ture;
        } 
        return false;
    }
}
/**
 * 打开通道
 * */
class StartAisleState:public State
{
public:
    StartAisleState(){}
    ~StartAisleState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_START_SESSION && work -> GetCurrentState() == VR_START_AISLE) {
            //do something
            std::cout<<"state : "<<VR_START_AISLE<<"open aisle"<<std::endl;
            return true;
        } 
        return false;
    }
}
/**
 * 打开引擎
 * */
class StartEnginState:public State
{
public:
    StartEnginState(){}
    ~StartEnginState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_START_AISLE && work -> GetCurrentState() == VR_START_ENGIN) {
            //do something
            std::cout<<"state : "<<VR_START_ENGIN<<"open engin"<<std::endl;
            return true;
        } 
        return false;
    }
}
/**
 * 关闭通道
 * */
class CloseAisleState:public State
{
public:
    CloseAisleState(){}
    ~CloseAisleState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_START_ENGIN && work -> GetCurrentState() == VR_CLOSE_AISLE) {
            //do something
            std::cout<<"state : "<<VR_CLOSE_AISLE<<"close aisle"<<std::endl;
            return true;
        } 
        return false;
    }
}
/**
 * 关闭会话
 * */
class CloseSessionState:public State
{
public:
    CloseSessionState(){}
    ~CloseSessionState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_CLOSE_AISLE && work -> GetCurrentState() == VR_CLOSE_SESSION) {
            //do something
            std::cout<<"state : "<<VR_CLOSE_SESSION<<"close session"<<std::endl;
            return true;
        } 
        return false;
    }
}
/**
 * 关闭引擎
 * */
class CloseEnginState:public State
{
public:
    CloseEnginState(){}
    ~CloseEnginState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_CLOSE_SESSION && work -> GetCurrentState() == VR_CLOSE_ENGIN) {
            //do something
            std::cout<<"state : "<<VR_CLOSE_ENGIN<<"close engin"<<std::endl;
            return true;
        } 
        return false;
    }
}
/**
 * 停止
 * */
class StopState:public State
{
public:
    StopState(){}
    ~StopState(){}
public:
    virtual bool Operation(Work* work)
    {
        if(work -> GetLastState() == VR_CLOSE_ENGIN && work -> GetCurrentState() == VR_STOP) {
            //do something
            std::cout<<"state : "<<VR_CLOSE_ENGIN<<"stop"<<std::endl;
            }
            return true;
        } 
        return false;
    }
}

//=====================================================================

int main()
{
    
    return 0;
}
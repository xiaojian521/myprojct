#include<iostream>
#include<map>

using namespace std;

//=================================================

enum StateStatus {
    startsessionstate = 0, //开始状态
    startenginstate
};

enum Event {
    startsessionevent = 5 //触发事件
};

//=================================================

class Notify;
class State
{
public:
    virtual void Operation(Notify* notify) = 0;
};           

//=================================================

class Manager
{
public:
    //对应的事件找到对应的子类对象
    map<int, State*> m_listState;
public:
    //子类状态对象，触发它需要的事件
    void AddState(State* state, int event)
    {
        m_listState.insert(pair<int,State*>(event,state));
    }
    void DeleteState()
    {
        auto it = m_listState.begin();
        for(; it!= m_listState.end(); it++) {
            delete it->second;
            m_listState.erase(it);
        }
    }
};

//=================================================

class Notify
{
public:
    int m_event;
    int m_state;
    Manager m_manager;
public:
    Notify(Manager manager):m_manager(manager)
    {}
    void SetEventState(int event, int state)
    {
        m_event = event;
        m_state = state;
    }
    void TrigerEvent()
    {
        map<int, State*>::iterator iter;
        iter = m_manager.m_listState.find(m_event);
        if( iter != m_manager.m_listState.end()) {
            (m_manager.m_listState[m_event])->Operation(this);
        } else {
            cout<<"no this event"<<endl;
        }
    }
};

//=================================================

class StartSessionState: public State
{
private:
    int m_currentstate;
    int m_laststate;
public:
    StartSessionState():m_currentstate(startsessionstate)
    {}
public:
    void Operation(Notify* notify)
    {
        if(notify->m_event == startsessionevent && notify->m_state == startsessionstate) {
            m_laststate = m_currentstate;
            m_currentstate = notify->m_state;
            cout<<"state 1"<<endl;
        } else {
            cout<< "failed to change"<<endl;
        }
    }
};

//=================================================
int main(int argc, char const *argv[])
{
    //派生类状态
    State* p = new StartSessionState;
    //创建管理者，将派生类状态放入到map进行管理
    Manager manager;
    manager.AddState(p,startsessionevent);
    //创建管理者，设置触发事件和当前状态，发送通知
    Notify aa(manager);
    aa.SetEventState(startsessionevent,startsessionstate);
    aa.TrigerEvent();
    
    return 0;
}

#include<iostream>
#include<list>
using namespace std;

class Observer
{
public:
    //观察者
    virtual void Update(int) = 0;
};

class Subject
{
public:
    //添加观察者对象
    virtual void Attach(Observer*) = 0;
    //删除观察者对象
    virtual void Detach(Observer*) = 0;
    //通知事件信息
    virtual void Notify() = 0;
};

class ConcreteObsercer: public Observer
{
public:
    //
    ConcreteObsercer(Subject* pSubject) : m_pSubject(pSubject)
    {}
    void Update(int value)
    {
        cout<<"ConcreteObserver get the update. New State:"<<value<<endl;
    }
private:
    Subject* m_pSubject;
};

class ConcreteObsercer2 : public Observer
{
public:
    ConcreteObsercer2(Subject* pSubject): m_pSubject(pSubject)
    {}
    void Update(int value)
    {
        cout << "ConcreteObserver2 get the update. New State:" << value << endl;
    }
private:
    Subject* m_pSubject;
};

class ConcreteSubject : public Subject
{
public:
    void Attach(Observer *pObserver);
    void Detach(Observer *pObserver);
    void Notify();

    void SetState(int State)
    {
        m_iState = State;
    }
private:
    std::list<Observer*> m_ObserverList;
    int m_iState;
};
void ConcreteSubject::Attach(Observer *pObserver)
{
    m_ObserverList.push_back(pObserver);
}
 
void ConcreteSubject::Detach(Observer *pObserver)
{
    m_ObserverList.remove(pObserver);
}

void ConcreteSubject::Notify()
{
    std::list<Observer *>::iterator it = m_ObserverList.begin();
    while (it != m_ObserverList.end())
    {
        (*it)->Update(m_iState);
        ++it;
    }
}

int main(int argc, char const *argv[])
{
    // 创建管理者
    ConcreteSubject *pSubject = new ConcreteSubject();
 
    // 创建两个观察者
    Observer *pObserver = new ConcreteObsercer(pSubject);
    Observer *pObserver2 = new ConcreteObsercer2(pSubject);
    
    // 改变状态
    pSubject->SetState(2);
 
    // 将观察者加入到任务列表中
    pSubject->Attach(pObserver);
    pSubject->Attach(pObserver2);
    
    //遍历所有观察者将改变的信息传入到所有的对象里面
    pSubject->Notify();
 
    // 删除观察者1
    pSubject->Detach(pObserver);
    
    //改变状态
    pSubject->SetState(3);
    //继续通知
    pSubject->Notify();
    //回收内存
    delete pObserver;
    delete pObserver2;
    delete pSubject;
    return 0;
}
//运行结果
// ConcreteObserver get the update. New State:2
// ConcreteObserver2 get the update. New State:2
// ConcreteObserver2 get the update. New State:3
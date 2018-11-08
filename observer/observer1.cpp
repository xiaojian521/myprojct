 /*
  * 观察者模式
  * 情景：高数课，ABCD四位同学，A是好学生，去上课，B在寝室睡觉，C在网吧打游戏，D在学校外陪女友逛街
  * 他们约定，如果要点名了，A在QQ群里吼一声，他们立刻赶到教室去。
  * 采用观察者模式实现这个情景的应用。
  */

#include<iostream>
#include<string>
#include<list>

//==================================================================

//观察者，为那些在目标发生改变时需获得通知的对象定义一个更新接口
class Observer;

//目标，知道它的观察者，提供注册和删除观察者对象的接口
class Subject 
{
public:
    virtual ~Subject(){};
    //注册Obsvr对象
    virtual void registerObsvr(Observer* obsvr) = 0;
    //删除Obsvr对象
    virtual void removeObsvr(Observer* obsvr) = 0;
    //通知Obsvr对象
    virtual void notifyObsvr(const std::string& msg) = 0;
};
//==================================================================
class Observer
{
public:
    virtual ~Observer() {};
    virtual void Update(const std::string& msg) = 0;
protected:
    Observer(){};
};
//==================================================================
class QQGroup : public Subject
{
public:
    QQGroup() { _observers = new std::list<Observer*>(); }
    void registerObsvr(Observer* obsvr);
    void removeObsvr(Observer* obsvr);
    void notifyObsvr(const std::string& msg);
private:
    std::list<Observer*> *_observers;
};

void QQGroup::registerObsvr(Observer* obsvr) 
{
    _observers->push_back(obsvr);
}

void QQGroup::removeObsvr(Observer* obsvr)
{
    if(_observers->size() > 0)
        _observers->remove(obsvr);
}

void QQGroup::notifyObsvr(const std::string& msg)
{
    std::cout<<"群消息" << msg <<std::endl;
    std::list<Observer*>::iterator iter = _observers->begin();
    for( ; iter != _observers->end(); iter++) {
        (*iter)->Update(msg);
    }
}
//==================================================================
class RoomMate : public Observer
{
public:
    RoomMate(std::string name, std::string now, std::string action)
    {
        _name = name;
        _action = action;
        _now = now;
    }
    void Update(const std::string& msg);
    std::string getName();
private:
    std::string _name;
    std::string _action;
    std::string _now;
};

std::string RoomMate::getName()
{
    return _name;
}

void RoomMate::Update(const std::string& msg)
{
    std::cout<<"this is "<<_name <<std::endl;
    if(msg == "点名了") {
        std::cout<<"Action: "<<_action<<std::endl<<std::endl;
    } else {
        std::cout <<"Go on : "<<_now<<std::endl<<std::endl;
    }
}

//测试代码
int main()
{ 
    RoomMate* B = new RoomMate("B",
          "sleeping",
          "get dressed and run to classroom");
      RoomMate* C = new RoomMate("C",
          "playing games",
          "pay the fee and run to classroom");
      RoomMate* D = new RoomMate("D",
          "shopping with girl friend","go back to school and be worried about girl friend's angry");
 
    QQGroup* qqgroup = new QQGroup();
    qqgroup->registerObsvr(B);
    qqgroup->registerObsvr(C);
    qqgroup->registerObsvr(D);

    qqgroup->notifyObsvr("目前没点名");
    qqgroup->notifyObsvr("点名了");

    //system("Pause");
    return 0;
}
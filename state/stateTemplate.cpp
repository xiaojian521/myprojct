#ifndef STATEMACHINE_H
#define STATEMACHINE_H
 
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
 
//typename指定StateType，和EventType是一个类型
//StateType是状态类型，EventType是事件类型
template <typename StateType, typename EventType>
class StateMachine
{
public:
    typedef boost::function<bool()> GuardFun;
    typedef boost::function<void()> ActionFun;
    
    struct Item {
        StateType currState, nextState;  //当前状态，和下一个状态
        EventType triger;  //事件类型
        GuardFun  guard;  
        ActionFun action;  //执行函数
 
        Item(StateType cs, StateType ns, EventType t, GuardFun gf, ActionFun af)
            : currState(cs), nextState(ns), triger(t), guard(gf), action(af) {}
    };
    //初始化给一个起始的状态
    StateMachine(StateType startState) : m_currState(startState) {}
    //追加
    void append(StateType cs, StateType ns, EventType t, GuardFun gf, ActionFun af) {
        m_stateTable.push_back(Item(cs, ns, t, gf, af));
    }
    //bind函数绑定参数如果是没有绑定的是通过引用传递可以改变值，如果绑定的固定的值则传递时是值传递
    void append(StateType cs, StateType ns, EventType t, GuardFun gf) {
        m_stateTable.push_back(Item(cs, ns, t, gf, boost::bind(&StateMachine::default_action, this)));
    }
    void append(StateType cs, StateType ns, EventType t, int, ActionFun af) {
        m_stateTable.push_back(Item(cs, ns, t, boost::bind(&StateMachine::default_guard, this), af));
    }
    void append(StateType cs, StateType ns, EventType t) {
        m_stateTable.push_back(Item(cs, ns, t, boost::bind(&StateMachine::default_guard, this),
                                    boost::bind(&StateMachine::default_action, this)));
    }
    //获得当前状态
    StateType currentState() { return m_currState; }
    //事件通知
    bool eventTriger(EventType triger)
    {
        bool isStateChanged = false;
 
        typename std::vector<Item>::iterator iter = m_stateTable.begin();
        for ( ; iter != m_stateTable.end(); ++iter) {
            //遍历vector改变状态
            if (iter->currState == m_currState && iter->triger == triger) {
                if (iter->guard()) {
                    m_currState = iter->nextState;
                    iter->action();
                    isStateChanged = true;
                }
                break;
            }
        }
        return isStateChanged;
    }
 
protected:
    bool default_guard() { return true; }
    void default_action() {}
 
private:
    std::vector<Item> m_stateTable;
    StateType m_currState;
};
 
#endif // STATEMACHINE_H
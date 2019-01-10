#ifndef _STRATEGY_H_
#define _STRATEGY_H_

class Strategy {
public:
    ~Strategy();
    virtual void AlgrithmInterface() = 0;
protected:
    Strategy();
private: 
};

class ConcreteStrategyA : public Strategy {
public:
    ConcreteStrategyA();
    ~ConcreteStrategyA();
    virtual void AlgrithmInterface();
protected:
private:
};

class ConcreteStrategyB : public Strategy {
public:
    ConcreteStrategyB();
    ~ConcreteStrategyB();
    virtual void AlgrithmInterface();
protected:
private:
};

/*
这个类是Strategy模式的关键,也是Strategy模式和Template模式的根本区别所在.
Strategy通过"组合"(委托)凡是实现算法(实现)的异构,而Template模式则采取的是继承的方式,这两个模式的区别也是继承和组合两种实现接口重用的方式的区别
*/
class Context {
public:
    Context(Strategy*);
    ~Context();
    void DoAction();
private:
    Strategy* _strategy;
};

#endif
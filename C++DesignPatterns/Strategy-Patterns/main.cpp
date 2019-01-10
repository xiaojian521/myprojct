#include "Strategy.h"

int main(int argc, char const *argv[])
{
    /*
    Strategy模式和Template模式实际是实现一个抽象接口的两种方式:继承和组合之间的区别,要实现一个抽象接口,继承是一种方式:我们将抽象接口声明在基类中,将具体的实现放在具体子类中.组合(委托)是另外一种方式:我们将借口的实现放在被组合对象中,将抽象借口放在组合类中
    */
   Strategy* pStr = new ConcreteStrategyA();
   Context* pcon = new Context(pStr);
   pcon->DoAction();
   pStr = new ConcreteStrategyB();
   pcon = new Context(pStr);
   pcon->DoAction();
    return 0;
}

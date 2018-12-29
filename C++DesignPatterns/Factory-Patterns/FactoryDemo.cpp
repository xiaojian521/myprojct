#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <memory>
/**
 * 抽象工厂模式 创建一个工厂类进行对象的创建
 */
class Operation {
private:
    double A, B;
public:
    double GetA() const {return A;}
    double GetB() const {return B;}
    void SetA(double x) {A = x;}
    void SetB(double y) {B = y;}
    double virtual GetResult() {return 0;}
    Operation():A(0), B(0){}
};
class Add : public Operation
{
public: 
    double GetResult()
    {
        return GetA() + GetB();
    }
};
class Sub : public Operation
{
    public:
        double GetResult()
        {
            return GetA()-GetB();
        }
};

class Mul : public Operation
{
    public:
        double GetResult()
        {
            return GetA()*GetB();
        }
};

class Div : public Operation
{
    public:
        double GetResult()
        {
            return GetA()/GetB();
        }
};

class SimpleFactory
{
public:
    static Operation* CreateOperation(char ch)
    {
        Operation* p = nullptr;
        switch(ch)
        {
            case '+':
                p = new Add();
                break;
            case '-':
                p = new Sub();
                break;
            case '*':
                p = new Mul();
                break;
            case '/':
                p = new Div();
                break;
        }
        return p;
    }
};

int main(int argc, char const *argv[])
{
    double A = 0;
    double B = 0;
    char ch = '\0';
    std::cin>>A>>ch>>B;
    std::shared_ptr<Operation> op(SimpleFactory::CreateOperation(ch));
    op->SetA(A);
    op->SetB(B);
    std::cout<<op->GetResult()<<std::endl; 

    return 0;
}

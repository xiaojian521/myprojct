#include <iostream>

/**
 * 统一的抽象借口IFactory
 */
class IFactory
{
public:
    IFactory(){}
    ~IFactory(){}
    virtual void makeProduc() = 0;
};

class PhoneFactory : IFactory {
    PhoneFactory(){}
    void makeProduc() {
        std::cout<<"生产手机"<<std::endl;
    }
};

class FoxconnProxy : IFactory
{
public:
    FoxconnProxy(IFactory* factory) {
        m_real = factory;
    }
    ~FoxconnProxy(){}

    void makeProduc() {
        m_real->makeProduc();
    }
private:
    IFactory* m_real;

};


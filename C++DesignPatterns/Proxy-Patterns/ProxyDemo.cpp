#include <iostream>

/**
 * 继承相同的接口,把对象传给代理类进行管理
 */
class IFactory
{
public:
    IFactory(){}
    ~IFactory(){}
    virtual void makeProduc() = 0;
};

class PhoneFactory : public IFactory {
public:
    PhoneFactory(){}
    ~PhoneFactory(){}
    void makeProduc() {
        std::cout<<"生产手机"<<std::endl;
    }
};

class FoxconnProxy : public IFactory
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

int main(int argc, char const *argv[])
{
    IFactory* factory = new PhoneFactory();
    FoxconnProxy* proxy = new FoxconnProxy(factory);
    proxy->makeProduc();
    return 0;
}

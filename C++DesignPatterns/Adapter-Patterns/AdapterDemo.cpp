#include <iostream>

class Adaptee
{
public:
    Adaptee(){}
    ~Adaptee(){}

    void specificRequest()
    {
       std::cout<<"this is specificRequest"<<std::endl;
    }
};

class Target
{
public:
    Target(){}
    ~Target(){}
    virtual void request() = 0;
};

class Adapter : public Target {
public: 
    Adapter() {
        adptee = new Adaptee(); 
    }
    ~Adapter(){}

    void request() {
        adptee -> specificRequest();
    }

private:
    Adaptee* adptee;
};

int main(int argc, char const *argv[])
{
    Target* tar = new Adapter();
    tar->request();
    return 0;
}

#include <iostream>
#include "Adaptee.h"

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
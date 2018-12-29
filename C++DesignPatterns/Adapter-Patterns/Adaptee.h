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
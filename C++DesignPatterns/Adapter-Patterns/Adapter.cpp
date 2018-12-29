#include "Adapter.h"
#include <iostream>

Target::Target(){}

Target::~Target(){}

void Target::Request()
{
    std::cout<<"Target::Request()"<<std::endl;
}

Adaptee::Adaptee(){}

Adaptee::~Adaptee(){}

void Adaptee::SpecificRequest() {
    std::cout<<"Adaptee::SpecificRequest()"<<std::endl;
}

//类模式的Adapter
Adapter::Adapter() {}

Adapter::~Adapter() {}

void Adapter::Request() {
    std::cout<<"Adapter::Request()" <<std::endl;
    this->SpecificRequest();
    std::cout<<"----------------------------"<<std::endl;
}

//对象模式的Adapter
Adapter1::Adapter1():_adaptee(new Adaptee) {}

Adapter1::~Adapter1() {}

void Adapter1::Request() {
    std::cout<<"Adapter1::Request()"<<std::endl;
    this->_adaptee->SpecificRequest();
    std::cout<<"----------------------------"<<std::endl;
}
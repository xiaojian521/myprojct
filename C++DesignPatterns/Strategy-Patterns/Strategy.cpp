#include "Strategy.h"
#include <iostream>

using namespace std;

Strategy::Strategy() {

}

Strategy::~Strategy() {

}

ConcreteStrategyA::ConcreteStrategyA()
{}

ConcreteStrategyA::~ConcreteStrategyA()
{}

void ConcreteStrategyA::AlgrithmInterface()
{
    cout << "ConcreteStrategyA::AlgrithmInterface" << endl;
}

ConcreteStrategyB::ConcreteStrategyB()
{}

ConcreteStrategyB::~ConcreteStrategyB()
{}

void ConcreteStrategyB::AlgrithmInterface()
{
    cout << "ConcreteStrategyB::AlgrithmInterface" << endl;
}

Context::Context(Strategy* strategy)
{
    this->_strategy = strategy;
}

Context::~Context()
{
    delete this->_strategy;
}

void Context::DoAction()
{
    this->_strategy->AlgrithmInterface();
}
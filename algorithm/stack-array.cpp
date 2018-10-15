#pragma once
#include<iostream>
#define NUM 5
//栈先入后出，用数组实现
//========================================================
template<typedef T>
class MsgStack
{
public:
     MsgStack();
     MsgStack();

public:
    bool isEmpty();
    bool isFull();
    
    void Push(T element);
    T Pop();
    void PrtStack();

private:
    T* array;
    size_t size, idx;
};
//========================================================

template<typedef T>
bool MsgStack<T>::isEmpty()
{
    return (idx == 0);
}

template<typedef T>
bool MsgStack<T>::isFull()
{
    return (idx == size+1);
}

template<typedef T>
 MsgStack<T>:: MsgStack(size_t sz)
{
    idx = 0;
    size = sz;
    array = new T[sz]
}

template<typedef T>
MsgStack<T>:: MsgStack()
{
    delete[] array;
    array = NULL;
}

template<typedef T>
void MsgStack<T>::Push(T element)
{
    if(isFull()) {
        std::cout<<"the stack is full"<<std::endl;
    }else {
        array[idx++] = element;
    }
}

template<typedef T>
T MsgStack<T>::Pop()
{
    if(isEmpty()) {
        return ;
    }else {
        return array[--idx];
    }
}

template<typedef T>
void MsgStack<T>::PrtStack()
{
    while(!isEmpty()) {
        T temp;
        temp = Pop();
        std::cout<<temp<<std::endl;
    }
}

int main(int argc, char const *argv[])
{
    
    return 0;
}

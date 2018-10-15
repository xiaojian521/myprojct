#pragma once
//基于数组的循环队列，rear那个数组空间会空着，为了判断是否为NULL
#include<iostream>
//========================================================
template<typename T>
class MsgQueue 
{
public:
    MsgQueue(size_t sz);
    ~MsgQueue();
public:
    bool isEmpty(); //判断队列是否为空
    bool isFull(); //判断队列是否为满

    void Push(T &val); //入队操作
    T Pop(); //出队操作

    T getFront();
    T getRear();

    size_t getRealSize();
    void prtQueue();

private:
    //@param front 头的位置 rear 结尾位置 size 申请数组大小
    size_t front, rear, size;
    T* data;
};
//========================================================
template<typename T>
MsgQueue<T>::MsgQueue(size_t sz)
{
    rear = front = 0;
    size = sz + 1;
    data = new T[size]; //申请循环队列空间
}

template<typename T>
MsgQueue<T>::~MsgQueue()
{
    delete[] data;
    data == NULL;
}

template<typename T>
bool MsgQueue<T>::isEmpty()
{
    return (front == rear);
}

template<typename T>
bool MsgQueue<T>::isFull()
{
    return (((rear+1) % size) == front);
} 

template<typename T>
bool MsgQueue<T>::Push(T &val)
{
    if(!isFull()) {
        data[rear] = cal;
        rear = (rear + 1) % size;
    } else {
        std::cout<<"The queue is full!"<<std::endl;
    }
}

template<typename T>
T MsgQueue<T>::Pop(void)
{
    T temp；
    if(!isEmpty()) {
        temp = data[front];
        data[front] = NULL;
        front = (front + 1) % size;
        return data[temp];
    } else {
        std::cout<<"The queue is empty"<<std::endl;
        return ;
    }
}

template<typename T>
T MsgQueue<T>::getFront()
{
    if(isEmpty()) {
        std::cout<<"The queue is empty"<<std::endl;
    }
    return data[front];
}

template<typename T>
T MsgQueue<T>::getRear()
{
    if(isEmpty()) {
        std::cout<<"The queue is empty"<<std::endl;
    }
    return data[front];
}

template<typename T>
size_t MsgQueue<T>::getRealSize()
{
    size_t realSize = (rear - front + size) % size;
}

template<typename T>
void MsgQueue<T>::prtQueue()
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
};

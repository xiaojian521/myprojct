#ifndef READDATA_H
#define READDATA_H

#include <iostream>
#include <string.h>

typedef struct Node {
    char* bufferdata;  //buffer str
    int buffersize;  //buffer size
}BufferDate;

class ReadData
{
public:
    ReadData(int size);
    ~ReadData();
public:
    //Adjust the data order
    BufferDate GetBufferData();
    //write to buffer
    bool Writedata(char* data,int length);
private: 
    BufferDate buffer; //buffer struct
    int m_pend; //buffer data end
    int m_ptop; //buffer data top
    const int m_size; //buffer size
    bool first; //是否是第一次写入
    char* ptemp; //辅助空间
};

#endif // READDATA_H

#include "readdata.h"

ReadData::ReadData(int size):m_size(size)
{
    first= false;
     buffer.bufferdata=nullptr;
     buffer.buffersize=size;
    if( size <= 0 ) {
            std::cout<<"size is incorrect"<<std::endl;
        }
        buffer.bufferdata = new char[m_size];
        ptemp = new char[m_size];
       if( ! buffer.bufferdata ) {
           std::cout<<"buffer create failed"<<std::endl;
       }
       m_pend=0;
       m_ptop=0;
}

ReadData::~ReadData()
{
    if(! buffer.bufferdata) {
            delete[]  buffer.bufferdata;
             buffer.bufferdata = nullptr;
        }
    if(! ptemp) {
            delete[]  ptemp;
             ptemp = nullptr;
        }
}

BufferDate ReadData::GetBufferData()
{
    if(m_ptop == 0)
        return buffer;
    else {
        int b=m_ptop;
         strcpy(ptemp,buffer.bufferdata);
        for(int a=0; a<m_size; ++a) {
             buffer.bufferdata[a] = ptemp[b++];
            if(b >= m_size)
                b = 0;
        }
        return buffer;
    }
}

bool ReadData::Writedata(char* data,int length)
{
        if( NULL==data || 0>=length) {
            std::cout<<"data is null"<<std::endl;
            return false;
        }
        if(length < m_size) {
            int ptemp=m_pend;
             for(int a =0; a<length; ++a) {
                  buffer.bufferdata[ptemp++] = data[a];
                  if(first == true)
                      m_ptop=ptemp;
                  if(ptemp >= m_size){
                        first=true;
                        ptemp = 0;
                        m_ptop = 0;
                      }
             }
             m_pend = ptemp;
             return true;
        }else {
            int ptemp = length - m_size;
            for(int a =0; a <m_size; ++a)
                 buffer.bufferdata[a] = data[ptemp++];
            m_ptop =0 ;
            return true;
        }
        return false;
}


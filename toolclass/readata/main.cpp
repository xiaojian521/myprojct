#include"readdata.h"

int main()
{
    ReadData a(11);
     a.Writedata("xiaojian",8);
     a.Writedata("HAHA",4);
     a.Writedata("ainio",5);
     a.Writedata("youlihaile",10);
     std::cout<<a.GetBufferData().bufferdata<<std::endl;
     std::cout<<a.GetBufferData().buffersize<<std::endl;
    return 0;
}


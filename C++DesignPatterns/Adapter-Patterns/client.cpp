#include <iostream>
#include "Target.cpp"



int main(int argc, char const *argv[])
{
    Target* tar = new Adapter();
    tar->request();
    return 0;
}

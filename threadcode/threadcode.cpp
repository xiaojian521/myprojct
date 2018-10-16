//==========================================================
//一、线程基本用法
#include<iostream>
#include<thread>
#include<stdio.h>
using namespace std;
void show(const char* str, const int id)
{
    cout<<"thread "<<id+1<<":" <<str<<endl;
}

int main()
{
    std::thread t1(&show,"hello cplisplus!",0);
    t1.join();
    std::thread t2(&show,"你好，C++！",1);
    t2.join();
    std::thread t3(&show,"hello!",2);
    t3.join();
    return 0;
}

//==========================================================
#include<iostream>
#include<functional>
#include<thread>
using namespace std;
void sayHello()
{
 std::cout<<"hello"<<std::endl;
}

void show()
{
    std::cout<<"hello cplusplus!"<<std::endl;
}
void method(int &a)//ref
{
    a += 5;
    std::cout<<a<<std::endl;
}

int main(int argc, char const *argv[])
{
    int a = 0;
    std::thread th1(&method, ref(a));//ref !!
    //move constructor, th1被转移到th1_move后th1被销毁，类似于剪切
    thread th1_move = thread(move(th1));
    th1_move.join();
    thread t11(&sayHello);
    t11.join();
    std::cout<<"world"<<std::endl;
    //栈上
    thread t1(show);//根据函数初始化执行
    thread t2(show);
    thread t3(show);
    //线程数组
    thread th[3]{thread(show), thread(show),thread(show)};
    //堆上
    thread *pt1(new thread(show));
    thread *pt2(new thread(show));
    thread *pt3(new thread(show));
    //线程指针数组
    thread *pth(new thread[3]{thread(show), thread(show), thread(show)});
    return 0;
}
==========================================================
//二、多线程传递参数
#include<iostream>
#include<thread>
using namespace std;

void show(const char str[], const int id)
{
    cout<<"thread"<< id + 1 <<str<<endl;
}

int main()
{
    thread t1(&show, "hello cplusplus!", 0);
    t1.join();
    thread t2(&show, "你好， C++", 1);
    t2.join();
    thread t3(&show, "hello!", 2);
    t3.join();
    return 0;
}
//==========================================================
//三、线程的join和detach
#include<iostream>
#include<thread>
#include<array>

using namespace std;
void show()
{
    cout<<"hello cplusplus"<<endl;
}

int main()
{
    thread th = thread(show);
    //脱离主线程的绑定，主线程挂了，子线程不报错，子线程执行完自动退出
    th.detach();
    //detach以后，子线程会成为孤儿线程，线程之间将无法通信
    cout<<th.joinable()<<endl;
    //th.join();//error
    array<thread, 3> threads = {thread(show), thread(show), thread(show)};
    for(int i = 0; i < 3; i++) {
        //判断线程是否可以join
        cout<<threads[i].joinable()<<endl;
        threads[i].join();//主线程等待当前线程执行完成再退出
    }
    return 0;
}
//==========================================================
//四、线程交换
#include<iostream>
#include<thread>
using namespace std;

int main()
{
    thread t1([]()
    {
        cout<<"thread1"<<endl;
    });
    thread t2([]()
    {
        cout<<"thread2"<<endl;
    });
    cout << "'thread1' id is" << t1.get_id() <<endl;
    cout << "'thread2' id is" << t2.get_id() <<endl;
    cout <<"swap after:"<<endl;
    swap(t1, t2);//线程交换
    cout << "'thread1' id is" << t1.get_id() <<endl;
    cout << "'thread2' id is" << t2.get_id() <<endl;
    t1.join();
    t2.join();
    return 0;
}
//==========================================================
//五、线程移动
#include<iostream>
#include<thread>
using namespace std;

int main()
{
    thread t1([]()
    {
        cout << "thread1" << endl;
    });
    cout << "thread1' id is " << t1.get_id() << endl;
    thread t2 = move(t1);
    cout << "thread2' id is " << t2.get_id() << endl;
    t2.join();
    return 0;
}

//==========================================================
//六、类内函数使用线程
#include<iostream>
#include<string>
#include<thread>
using namespace std;

class TT
{
public:
    TT(){}
    ~TT(){}
public:
    void show(string file, int num);
    void showTwo();
};

void TT::show(string file, int num)
{
    cout<<"thread 1"<<":"<<file<<","<<num<<endl;
}

void TT::showTwo()
{
    thread th1(&TT::show,this,"xiaojian",1);
    th1.join();
}

int main()
{
    TT a;
    a.showTwo();
    return 0;
}
#include<iostream>

//栈先进后出
struct Node;
typedef struct Node * PtrToNode;
typedef PtrToNode Stack;
struct Node 
{
    int ele;
    PtrNode Next;
}

//判断栈是否为空栈
int IsEmpty(Stack S)
{
    return S->Next == NULL;
}

//创建一个空栈(链表实现)
Stack CreateStack(void)
{
    Stack S;
    S = malloc(sizeof(struct Node));
    if(S == NULL) {
        std::cout<<"Out of space!!!"<<std::endl;
        S->Next == NULL;
        MakeEmpty(S);
        return S;
    }
}
void MakeEmpty(Stack S)
{
    if(S == NULL) {
        std::cout<<"Must use CreateStack first"<<std::endl;
    }
    else {
        while(!IsEmpty(S))
            Pop(S);//弹出栈顶元素
    }
}

//从栈弹出元素
void Pop(Stack S)
{
    PtrToNode FirstCell;
    if(IsEmpty(S)) {
        std::cout<<"Empty stack"<<std::endl;
    }else {
        FirstCell = S->Next;
        S->Next = S->Next->Next;
        free(FirstCell);
    }
}

//返回栈顶元素
ElementType Top(Stack S)
{
    if(!IsEmpty(S))
        return S->Next->Element;
    cou
}
//Push进栈
void Push(int X, Stack S)
{
    PtrToNode TmpCell;
    TmpCell = malloc(sizeof(struct Node));
    if(TmpCell == NULL)
        std::cout<<"Empty stack"<<std::endl;
    else {
        TmpCell->Element = X;
        TmpCell->Next = S->Next;
        S->Next = TmpCell;
    }
}


int main(int argc, char const *argv[])
{
    
    return 0;
}
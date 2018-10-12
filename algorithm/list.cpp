#include<iostream>

struct Node;
typedef struct Node * PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
struct Node
{
    int ele;
    Position Next;
}

//判断一个链表是否为空表
int Is Empyty(List L)
{
    return L->next == NULL;
}

//判断当前位置是否是链表的末尾的函数
Position Find(int X, List L)
{
    //return position of X in L;NULL if not found
    Position P;
    P = L->Next;
    while(P != NULL && P->Element != X) {
        P = P->Next;
    }
    return P;
}

//删除链表中的元素
void Delete(int X, List L)
{
    Position P, TmpCell;
    //找到要删除元素的前一个
    p = FindPrevious(X, L);
    if(!IsLast(P, L)) {
        TmpCell = P->Next;
        P->Next = TmpCell->Next;
        free(TmpCell); 
    }
}
Position FndPrevous(int X, List L)
{
    Position P;
    P=L;
    while(P->Next != NULL && P->Next->Element != X) {
        P = P->Next;
    }
    return P;
}

//链表的插入(头插入)
void Insert(int X, List L, Position P)
{
    Position TmpCell;
    TmpCell = malloc(sizeof(strcut Node));
    if(TmpCell == NULL) {
        cout<<"Out of space!!!"<<endl;
    }
    TmpCell->Element = X;
    TmpCell->Next = P->Next;
    P->Next = TmpCell;
}

//删除表
void DeleteList(List L)
{
    P = L->Next;
    L->Next = NULL;
    while(p != NULL) {
        Tmp = P->Next;
        free(P);
        P = Tmp;
    }
}

int main(int argc, char const *argv[])
{

    return 0;
}

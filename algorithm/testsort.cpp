#include <iostream>
#include <cstdio>
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <ctime>
using namespace std;
//----------------插入排序--------------------------
void insertSort(int a[],int len){
    for(int i=1;i<len;i++){
        int tmp=a[i];
        int j=i-1;
        while(j>=0 && tmp<a[j]){
            a[j+1]=a[j];
            j--;
        }
        a[j+1]=tmp;
    }
}
//----------------选择排序--------------------------
void selectSort(int a[],int len){
    for(int i=0;i<len-1;i++){
        int minidx=i;
        for(int j=i+1;j<len;j++){
            if(a[j]<a[minidx]){
                minidx=j;
            }
        }
        if(a[i]>a[minidx]){
            int tmp=a[i];
            a[i]=a[minidx];
            a[minidx]=tmp;
        }
    }
}
//----------------冒泡排序--------------------------
void bubbleSort(int a[],int len){
    int tmp;
    for(int i=0;i<len-1;i++){
        bool flag=true;
        for(int j=0;j<len-i-1;j++){
            if(a[j]>a[j+1]){
                tmp=a[j+1];
                a[j+1]=a[j];
                a[j]=tmp;
                flag=false;
            }
        }
        if(flag)
            break;
    }
}
//----------------归并排序O(NlgN)--------------------------
#define INF 0x3f3f3f3f
/*
将[l,mid]和[mid+1,r]两个区间进行合并，每次取两个开头最小的那个
*/
void merges(int a[],int l,int mid,int r){
    int len1=mid-l+1;
    int len2=r-mid;
    int L[len1+1],R[len2+1];
    for(int i=0;i<len1;i++)
        L[i]=a[l+i];
    for(int i=0;i<len2;i++)
        R[i]=a[mid+1+i];
    L[len1]=INF;
    R[len2]=INF;
    int left=0,right=0;
    for(int k=l;k<=r;k++){
        if(L[left]<=R[right]){
            a[k]=L[left];
            left++;
        }
        else{
            a[k]=R[right];
            right++;
        }
    }
}
//对区间[l,r]进行归并排序
void mergeSort(int a[],int l,int r){
    if(l<r){
        int mid=(r+l)/2;
        mergeSort(a,l,mid);
        mergeSort(a,mid+1,r);
        merges(a,l,mid,r);
    }
}
//----------------快速排序O(NlgN)--------------------------
/*
以最后a[r]为划分点，将数组a划分成两个部分
前部分<=a[r],后部分>a[r]
最后返回a[r]的索引
*/
int quick_partition(int a[],int l,int r){
    int x=a[r];
    int i=l-1;
    for(int j=l;j<r;j++){
        if(a[j]<=x){
            i++;
            swap(a[i],a[j]);
        }
    }
    swap(a[i+1],a[r]);
    return i+1;

}
void quickSort(int a[],int l,int r){
    if(l<r){
        int mid=quick_partition(a,l,r);
        quickSort(a,l,mid-1);
        quickSort(a,mid+1,r);
    }
}


int main()
{
    int num=10;
    int a[num];
    srand((unsigned)time(NULL));
    for(int i=0;i<num;i++){
        a[i]=rand()%20;
        printf("%d ",a[i]);
    }
    printf("\n");
    quickSort(a,0,num-1);
    for(int i=0;i<num;i++){
        printf("%d ",a[i]);
    }
    return 0;
}
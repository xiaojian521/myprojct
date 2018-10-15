//=====================================================
//1.插入排序
void insertSort(int a[],int len){        //最差时间复杂度：O(n^2)
    for(int i=1;i<len;i++){              //最优时间复杂度：O(n)
        int tmp=a[i];                    //平均时间复杂度：O(n^2)
        int j=i-1;                       //空间复杂度：O(1)
        while(j>=0 && tmp<a[j]){         //稳定性：稳定
            a[j+1]=a[j];
            j--;
        }
        a[j+1]=tmp;
    }
}
//=====================================================
//2.选择排序
void selectSort(int a[],int len){
    for(int i=0;i<len-1;i++){
        int minidx=i;
        for(int j=i+1;j<len;j++){       //最差时间复杂度：O(n^2) 
            if(a[j]<a[minidx]){         //最优时间复杂度：O(n^2)
                minidx=j;               //平均时间复杂度：O(n^2)
            }                           //空间复杂度：O(1)
        }                               //稳定性：不稳定
        if(a[i]>a[minidx]){
            int tmp=a[i];
            a[i]=a[minidx];
            a[minidx]=tmp;
        }
    }
}
//=====================================================
//3.冒泡排序
void bubbleSort(int a[],int len){
    int tmp;
    for(int i=0;i<len-1;i++){
        bool flag=true;                 //最差时间复杂度：O(n^2) 
        for(int j=0;j<len-i-1;j++){     //最优时间复杂度：O(n)
            if(a[j]>a[j+1]){            //平均时间复杂度：O(n^2)
                tmp=a[j+1];             //空间复杂度：O(1)
                a[j+1]=a[j];            //稳定性：稳定
                a[j]=tmp;
                flag=false;
            }
        }
        if(flag)
            break;
    }
}
//=====================================================
//4.归并排序
#define INF 0x3f3f3f3f
/*
将[l,mid]和[mid+1,r]两个区间进行合并，每次取两个开头最小的那个
*/
void merges(int a[],int l,int mid,int r){
    int len1=mid-l+1;
    int len2=r-mid;
    int L[len1+1],R[len2+1];            //最差时间复杂度：O(nlog2n) 
    for(int i=0;i<len1;i++)             //最优时间复杂度：O(nlog2n)
        L[i]=a[l+i];                    //平均时间复杂度：O(nlog2n)
    for(int i=0;i<len2;i++)             //空间复杂度：O(n)
        R[i]=a[mid+1+i];                //稳定性：稳定
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
//=====================================================
//5.快速排序
/*
以最后a[r]为划分点，将数组a划分成两个部分
前部分<=a[r],后部分>a[r]
最后返回a[r]的索引
*/
int quick_partition(int a[],int l,int r){
    int x=a[r];
    int i=l-1;                      //最差时间复杂度：O(n^2)
    for(int j=l;j<r;j++){           //最优时间复杂度：O(nlog2n)
        if(a[j]<=x){                //平均时间复杂度：O(nlog2n)
            i++;                    //空间复杂度：O(nlog2n)
            swap(a[i],a[j]);        //稳定性：不稳定
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
//=====================================================
//6.最大堆排序
//----------------堆排序O(NlgN)最大堆的实现--------------------------
/*
将最大堆的根节点和末尾交换后，可能破坏了最大堆的性质，所以要进行更新
*/
void heap_update(int a[],int i,int heap_size){
    int l=i*2+1;
    int r=i*2+2;
    int large=i;
    if(l<heap_size && a[l]>a[large])     //最差时间复杂度：O(nlog2n)
        large=l;                         //最优时间复杂度：O(nlog2n)
    if(r<heap_size && a[r]>a[large])     //平均时间复杂度：O(nlog2n)
        large=r;                         //空间复杂度：O(1)
    if(large!=i){                        //稳定性：不稳定
        swap(a[i],a[large]);
        heap_update(a,large,heap_size);
    }
}
/*
将val插入到数组a的末尾，并进行最大堆的维护
*/
void heap_insert(int a[],int val,int heap_size){
    heap_size++;
    a[heap_size-1]=val;
    int idx=heap_size-1;
    while(idx>0 && a[idx/2]<a[idx]){
        swap(a[idx/2],a[idx]);
        idx=idx>>1;
    }
}
void heapSort(int a[],int len){
    //构建最大堆
    for(int i=0;i<len;i++)
        heap_insert(a,a[i],i);
    for(int i=len-1;i>=0;i--){
        swap(a[0],a[i]);
        heap_update(a,0,i);  //每次heap_size减小1的
    }
}
//=====================================================
//7.希尔排序
void shellsort(int a[],int len){
    //划分成gap个组
    for(int gap=len/2;gap>0;gap/=2){
        //从第gap个元素，逐个对其所在组进行插入排序操作
        //而不需要for每个组进行操作
        for(int i=gap;i<len;i++){
            int j=i-gap;
            //插入排序采用交换法即可
            while(j>=0 && a[j]>a[j+gap]){  //最差时间复杂度：O(n^2)
                swap(a[j],a[j+gap]);       //最优时间复杂度：O(n)
                j-=gap;                    //平均时间复杂度：O(nlog2n)
            }                              //空间复杂度：O(1)
        }                                  //稳定性：不稳定
        /*
        printf("gap:%d\n",gap);
        for(int i=0;i<len;i++)
            printf("%d ",a[i]);
        printf("\n");
        */
    }
}
//=====================================================
//8.排序最省力的就是C++中的自定义排序了，直接用algorithm中的sort即可
#include <algorithm>
bool cmp(int a,int b){
    return a>b; 
}

sort(a,a+n); //a为数组，n为数组的长度，默认是从小到大排序
sort(a,a+n,cmp);//cmp即为自定义比较函数，此处是从大到小排序。
//=====================================================
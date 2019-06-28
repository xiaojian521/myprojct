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
//==================自己从新写的===================================

//冒泡排序
void MPSort(int* aary, int lenght) {
    if(nullptr == aary || 0 >= lenght )
        return ;
    for(int i = 1; i < lenght; i++) {
        for(int j = lenght-1; j >= i; --j) {
            if(aary[j-1] > aary[j]) {
                int tmp = aary[j];
                aary[j] = aary[j-1];
                aary[j-1] = tmp;
            }
        }
    }
}

//选择排序
void XZSort(int* aary, int lenght) {
    if(nullptr == aary || 0 >= lenght )
        return ;
    for(int i = 0; i < lenght-1; ++i) {
        int min = i;
        for(int j = i+1; j < lenght; j++) {
            if(aary[min] > aary[j]) {
                min = j;
            }
        }
        if(min != i) {
            int tmp = aary[i];
            aary[i] = aary[min];
            aary[min] = tmp;
        }
    }
}

//插入排序
void CRSort(int* aar, int lenght) {
    if(nullptr == aar || 0 >= lenght )
        return ;
    int i; //有序数组的最后一个
    int j; //无序数组的第一个
    //遍历无序数组
    for(j = 1; j < lenght; ++j) {
        int temp = aar[j];
        i = j - 1;
        while(i>=0 && temp < aar[i]) {
            aar[i+1] = aar[i];
            --i;
        }
        aar[i+1] = temp;
    }
}

//希尔排序
void ShellSort(int* aar, int lenght) {
    if(nullptr == aar || lenght <= 0) return;
    //分段
    for(int Gap = lenght/2; Gap >= 1; Gap/=2) {
        //对分段数组进行遍历
        for(int i = 0; i < Gap; i++) {
            //插入排序
            //遍历无序数组
            for(int j = i+Gap; j < lenght; j += Gap) {
                int temp = aar[j]; //记录无序数组的第一个
                int k = j- Gap;//有序数组的最后一个
                while(k >= i && temp < aar[k]) {
                    //插入进去
                    aar[k+Gap] = aar[k];
                    k -= Gap;
                }
                aar[k+Gap] = temp;
            }
        }
    }
}

//堆排序
#define LEFT 2*(rootNode)+1
#define RIGHT 2*(rootNode)+2

void Adjust(int* aar, int lenght, int rootNode) {
    if(nullptr == aar || lenght <=0 || rootNode > lenght/2-1) return ;

    //向下循环
    for(int MAX = LEFT; MAX < lenght; MAX = LEFT) {
        //先判断左右子树的大小取最大的
        if(RIGHT < lenght) {
            if(aar[MAX] < aar[RIGHT]) {
                MAX = RIGHT;
            }
        }

        //将大值赋给父节点
        if(aar[MAX] > aar[rootNode]) {
            aar[MAX] = aar[rootNode] ^ aar[MAX];
            aar[rootNode] = aar[rootNode] ^ aar[MAX];
            aar[MAX] = aar[rootNode] ^ aar[MAX];
            rootNode = MAX;
        } else {
            break;
        }
    }

}

void HeapSort(int* aar, int lenght) {
    //从最后一个父节点开始调整
    //将数组调整为堆排序
    for(int i = lenght/2-1; i >= 0; --i) {
        Adjust(aar,lenght,i);
    }

    for(int j = lenght - 1; j > 0; --j) {
        aar[j] = aar[j] ^ aar[0];
        aar[0] = aar[j] ^ aar[0];
        aar[j] = aar[j] ^ aar[0];
        Adjust(aar,j,0);
    }
}

//归并排序

void Merge(int* aar, int low, int high) {
    int begin1 = low;
    int end1 = (low+high)/2;
    int begin2 = end1 + 1;
    int end2 = high;
    //创建一个数组保存临时结果
    int* ptemp = new int[high-low+1];
    int i = 0;
    //谁小把谁放进去
    while(begin1 <= end1 && begin2 <= end2) {
        if(aar[begin1] < aar[begin2]) {
            ptemp[i] = aar[begin1];
            begin1++;
            i++;
        } else {
            ptemp[i] = aar[begin2];
            begin2++;
            i++;
        }
    }
    //将剩余的元素放进数组
    while(begin1 <= end1) {
        ptemp[i] = aar[begin1];
        begin1++;
        i++;
    }
    while(begin2 <= end2) {
        ptemp[i] = aar[begin2];
        begin2++;
        i++;
    }

    //将临时数组中的元素全部拷贝到aar数组中
    for(int i = low ;i < high-low+1; i++) {
        aar[i] = ptemp[i];
    }
    delete[] ptemp;
    ptemp= nullptr;

}

void MergeSort(int* aar, int low, int high) {
    if(nullptr == aar || low >= high) return ;
    int mid = (low + high)/2;
    MergeSort(aar, low, mid);
    MergeSort(aar, mid+1, high);

    //合并
    Merge(aar,low,high);
}

//快速排序
int Sort(int* aar, int low, int high) {
    //指向小区域最后的指针
    int small = low -1;
    //遍历数组
    for(low; low < high; ++low) {
        if(aar[low] < aar[high]) {
            if(++small != low) {
                aar[small] = aar[small] ^ aar[low];
                aar[low] = aar[small] ^ aar[low];
                aar[small] = aar[small] ^ aar[low];
            }
        }
    }
    //将标准值放在合适的位置
    if(++small != high) {
        aar[high] = aar[high] ^ aar[small];
        aar[small] = aar[high] ^ aar[small];
        aar[high] = aar[high] ^ aar[small];
    }
    return small;
}

void QuickSort(int* aar, int low, int high) {
    if(nullptr == aar || low >= high) return;
    int standard = Sort(aar, low, high);
    QuickSort(aar, low, standard-1);
    QuickSort(aar, standard+1, high);
}

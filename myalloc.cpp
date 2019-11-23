/*
 * @Author: Kingtous
 * @Date: 2019-11-15 23:37:21
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-11-16 09:31:33
 * @Description: Kingtous' Code
 */
// （1）用C语言分别实现采用首次适应算法和最佳适应算法的动态分区分配过程alloc( )和回收过程free( )。其中，空闲分区通过空闲分区链来管理：在进行内存分配时，系统优先使用空闲区低端的空间。
// （2）假设初始状态下，可用的内存空间为640KB，并有下列的请求序列：
// •作业1申请130KB。
// •作业2申请60KB。
// •作业3申请100KB。
// •作业2释放60KB。
// •作业4申请200KB。
// •作业3释放100KB。
// •作业1释放130KB。
// •作业5申请140KB。
// •作业6申请60KB。
// •作业7申请50KB。
// •作业6释放60KB。
// 请分别采用首次适应算法和最佳适应算法，对内存块进行分配和回收，要求每次分配和回收后显示出空闲分区链的情况。
#include <iostream>
#include "cdef.h"
// 配置算法，FF为首次适应算法，BF为最佳适应算法
#define ALGORITHM BF
#define MAXSIZE 640
using namespace std;

int job[11][3]={
    {1,130,ACT_ALLOC},
    {2,60,ACT_ALLOC},
    {3,100,ACT_ALLOC},
    {2,60,ACT_FREE},
    {4,200,ACT_ALLOC},
    {3,100,ACT_FREE},
    {1,130,ACT_FREE},
    {5,140,ACT_ALLOC},
    {6,60,ACT_ALLOC},
    {7,50,ACT_ALLOC},
    {6,60,ACT_FREE}
};

int main(){
    Memory memory(ALGORITHM,MAXSIZE);
    memory.dump();
    for (int i = 0; i < 11; i++)
    {
        memory.act(job[i][0],job[i][1],job[i][2]);   
        memory.dump();
    }
}
/*
 * @Author: Kingtous
 * @Date: 2019-11-22 22:07:11
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-11-23 17:04:44
 * @Description: Kingtous' Code
 */
#include <iostream>
#include "cdef.h"

#define JOB_INS_NUM 20

int main(int argc, char const *argv[])
{
    // 随机化种子
    srand((long)time(0));
    MemoryManager mManager(JOB_INS_NUM);
    int * serial = mManager.simulate();
    SwapAnalyzer sAnalyzer(serial,JOB_INS_NUM);
//     OPT
    sAnalyzer.analyze(ALGORITHM_OPT);
    // FIFO
    sAnalyzer.analyze(ALGORITHM_FIFO);
    // LRU
    sAnalyzer.analyze(ALGORITHM_LRU);
    return 0;
}

// 2、实验内容
// （1）假设每个页面中可存放10条指令，分配给一作业的内存块数为4。
// （2）用C语言模拟一作业的执行过程。该作业共有320条指令，即它的地址空间为32页，目前它的所有页都还未调入内存。在模拟过程中，如果所访问的指令已经在内存中，则显示其物理地址，并转下一条指令。如果所访问的指令还未装入内存，则发生缺页，此时需记录缺页的次数，并将相应页调入内存。如果4个内存块中均已装入该作业，则需进行页面置换。最后显示其物理地址，并转下一条指令。在所有320条指令执行完毕后，请计算并显示作业运行过程中发生的缺页率。
// （3）置换算法：请分别考虑OPT、FIFO和LRU算法。
// （4）作业中指令的访问次序按下述原则生成：
// •50%的指令是顺序执行的。
// •25%的指令是均匀分布在前地址部分。
// •25%的指令时均匀分布在后地址部分。
// 具体的实施办法是：
// ①	在[0，319]之间随机选取一条起始执行指令，其序号为m；
// ②	顺序执行下一条指令，即序号为m+1的指令；
// ③	通过随机数，跳转到前地址部分[0，m-1]中的某条指令处，其序号为m1；
// ④	顺序执行下一条指令，即序号为m1+1的指令；
// ⑤	通过随机数，跳转到后地址部分[m1+2，319]中的某条指令处，其序号为m2；
// ⑥	顺序执行下一条指令，即序号为m2+1的指令；
// ⑦	重复跳转到前地址部分、顺序执行、跳转到后地址部分、顺序执行的过程，直至执行320条指令。
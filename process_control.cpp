/*
 * @Author: Kingtous
 * @Date: 2019-11-02 14:09:12
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-11-15 23:43:07
 * @Description: Kingtous' Code
 */
// 实验1
#include <iostream>
#include "cdef.h"
using namespace std;

int main(){
    // PCB
    PCB p;
    // 添加进程
    p.add_process(Process(0,9,0,3,2,3,STATUS_RDY));
    p.add_process(Process(1,38,0,3,-1,0,STATUS_RDY));
    p.add_process(Process(2,30,0,6,-1,0,STATUS_RDY));
    p.add_process(Process(3,29,0,3,-1,0,STATUS_RDY));
    p.add_process(Process(4,0,0,4,-1,0,STATUS_RDY));
    // 执行
    p.simulate();
}
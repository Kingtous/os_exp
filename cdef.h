/*
 * @Author: Kingtous
 * @Date: 2019-11-02 14:18:34
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-11-23 09:24:17
 * @Description: Kingtous' Code
 */
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>
#include <set>
//实验1

#define STATUS_RUN 1
#define STATUS_RDY 2
#define STATUS_BLK 3
#define STATUS_OK 0

#define int_run -3
#define int_rdy 1
#define int_time 1
using namespace std;

// 运行态  -3
// 就绪态  +1
// 阻塞态  大于BlockTime进入就绪状态

class Process{

public:

    int id;
    int priority;
    int cputime;
    int alltime;
    int startBlock;
    int blocktime;
    int state;

    Process(){}

    Process(int i,int p,int c,int a,int sb,int b,int st){
        id = i;
        priority = p;
        cputime = c;
        alltime = a;
        startBlock = sb;
        blocktime = b;
        state = st;
    }

    void timeby(){
        switch (state)
        {
            case STATUS_RUN:
                priority += int_run;
                cputime += int_time;
                alltime -= int_time;
                if (startBlock > 0)
                {
                    startBlock -= int_time;
                }
                break;
            case STATUS_RDY:
                priority += int_rdy;
                break;
            case STATUS_BLK:
                if(blocktime > 0){
                    blocktime -= int_time;
                }
        }
    }  

    void switch_rdy(){
        state = STATUS_RDY;
    }

    void switch_run(){
        state = STATUS_RUN;
    }

    void output(){
        cout<<setw(15)<<id<<setw(15)<<priority<<setw(15)<<cputime<<setw(15)<<alltime<<setw(15)<<startBlock<<setw(15)<<blocktime<<setw(15)<<state<<endl;
    }
};


typedef struct PCBNode{
    Process p;
    PCBNode* next = nullptr;
}PCBNode;

class PCB{

public:

    // 时间片
    int time = 0;
    int num = 0;
    // 链表
    PCBNode* p_run = nullptr;
    PCBNode* p_rdy = nullptr;
    PCBNode* p_rdy_tail= nullptr;
    PCBNode* p_blk = nullptr;
    PCBNode* p_blk_tail = nullptr;

    PCBNode* tmpPre = nullptr;

    int num_p_done = 0;
    PCBNode* p_done = nullptr;
    PCBNode* p_done_tail = nullptr;

    void add_process(Process s){
        // 添加进 run 链表
        PCBNode* node = new PCBNode();
        node->p = s;
        node->next = nullptr;
        if (p_rdy == nullptr){
            p_rdy = node;
            p_rdy_tail = p_rdy;
        } else{
            p_rdy_tail->next = node;
            p_rdy_tail = p_rdy_tail -> next;
            p_rdy_tail->next = nullptr;
        }
        num += 1;
    }

    void add_to_done(PCBNode* p){
        p->p.state = STATUS_OK;
        num_p_done += 1;
        if (p_done == nullptr){
            p_done = p;
            p_done_tail = p;
        }
        else{
            p_done_tail->next = p;
            p_done_tail = p_done_tail->next;
            p_done_tail->next = nullptr;
        }
    }

    void add_to_blk(PCBNode* p){
        p->p.state = STATUS_BLK;
        if (p_blk == nullptr){
            p_blk = p;
            p_blk_tail = p;
        }
        else{
            p_blk_tail->next = p;
            p_blk_tail = p_blk_tail->next;
            p_blk_tail->next = nullptr;
        }
    }

    void add_to_rdy(PCBNode* p){
        p->p.state = STATUS_RDY;
        if (p_rdy == nullptr){
            p_rdy = p;
            p_rdy_tail = p;
        }
        else{
            p_rdy_tail->next = p;
            p_rdy_tail = p_rdy_tail->next;
            p_rdy_tail->next = nullptr;
        }
    }

    void add_to_run(PCBNode* p){
        p->p.state = STATUS_RUN;
        p_run = p;
        p->next = nullptr;
    }

    void delete_node_in_rdy(PCBNode* p){
        PCBNode * t = p_rdy;
        if(t == nullptr){
            return;
        }
        if(p_rdy == p){
            // 删除队首
            p_rdy = p_rdy ->next;
        }
        else{
            while(t->next != p){
                t = t->next;
            }
            t ->next = p->next;
        }
    }

    void delete_node_in_blk(PCBNode* p){
        PCBNode * t = p_blk;
        if(t == nullptr){
            return;
        }
        if(p_blk == p){
            // 删除队首
            p_blk = p_blk ->next;
        }
        else{
            while(t->next != p){
                t = t->next;
            }
            t ->next = p->next;
        }
    }

    void simulate(){
        while (num_p_done<num){
            // time pass
            // 执行态，可能的情况 1.执行完，2.阻塞
            // 另：执行态一时只有一个，所以直接赋值 nullptr 即可

            // 经过一个时间片
            time += 1;
            PCBNode* p ;
            //开始处理
            p = p_run;
            tmpPre = p;
            while (p!= nullptr){
                if (p->p.alltime == 0){
                    //执行完，添加到完成队列
                    add_to_done(p);
                    p_run = nullptr;
                }
                else if (p->p.startBlock == 0 && p->p.blocktime != -1){
                    // 需要阻塞了
                    add_to_blk(p);
                    p_run = nullptr;
                }
                p = p-> next;
            }
            // 阻塞态，可能的情况 1.阻塞时间完进入就绪 2.继续阻塞
            p = p_blk;
            tmpPre = p;
            while (p!= nullptr){
                if (p->p.blocktime == 0){
                    // 阻塞完
                    p->p.startBlock = -1;
                    add_to_rdy(p);
                    delete_node_in_blk(p);
                }
                p = p->next;
            }
            // 从就绪态选择优先权最高的进行执行
            p = p_rdy;
            PCBNode* maxNode= p_rdy;
            while (p != nullptr){
                if (maxNode->p.priority < p->p.priority){
                    maxNode = p;
                }
                p = p->next;
            }
            if (p_run != nullptr) {
                if (maxNode != nullptr && p_run->p.priority < maxNode->p.priority) {
                    add_to_rdy(p_run);
                    delete_node_in_rdy(maxNode);
                    add_to_run(maxNode);
                }
            }
            else{
                if (maxNode != nullptr){
                    delete_node_in_rdy(maxNode);
                    add_to_run(maxNode);
                }
            }

            p = p_run;
            while (p!= nullptr){
                p->p.timeby();
                p = p->next;
            }
            p = p_rdy;
            while (p!= nullptr){
                p->p.timeby();
                p = p->next;
            }
            p = p_blk;
            while (p!= nullptr){
                p->p.timeby();
                p = p->next;
            }
            output();
        }
    }

    void output(){
        cout<<"===================================================="<<endl;
        cout<<"当前时间："<<time<<endl;
        cout<<"正在运行：";
        if (p_run != nullptr){
            cout<<p_run->p.id;
        }
        cout<<endl;
        cout<<"就绪队列：";
        PCBNode* p = p_rdy;
        tmpPre = p;
        while (p != nullptr){
            cout<<"=>"<<p->p.id;
            tmpPre = p;
            p = p->next;
        }
        cout<<endl;
        cout<<"阻塞队列：";
        p = p_blk;
        tmpPre = p;
        while (p != nullptr){
            cout<<"=>"<<p->p.id;
            tmpPre = p;
            p = p->next;
        }
        cout<<endl;
        cout<<"完成队列：";
        p = p_done;
        tmpPre = p;
        while (p != nullptr){
            cout<<"=>"<<p->p.id;
            tmpPre = p;
            p = p->next;
        }
        cout<<endl;
        cout<<setw(15)<<"ID"<<setw(15)<<"Priority"<<setw(15)<<"CPUTime"<<setw(15)<<"AllTime"<<setw(15)<<"StartBlock"<<setw(15)<<"BlockTime"<<setw(15)<<"State"<<endl;

        for (int i = 0; i < num; ++i) {
            PCBNode* p = findPCBNodeByID(i);
            p->p.output();
        }


        cout<<"===================================================="<<endl;
    }

    PCBNode* findPCBNodeByID(int id){
        PCBNode* p = p_run;
        tmpPre = p;
        while (p != nullptr){
            if (p->p.id == id){
                return p;
            }
            tmpPre = p;
            p = p->next;
        }
        p = p_rdy;
        tmpPre = p;
        while (p != nullptr){
            if (p->p.id == id){
                return p;
            }
            tmpPre = p;
            p=p->next;
        }
        p = p_blk;
        tmpPre = p;
        while (p != nullptr){
            if (p->p.id == id){
                return p;
            }
            tmpPre = p;
            p=p->next;
        }
        p = p_done;
        tmpPre = p;
        while (p != nullptr){
            if (p->p.id == id){
                return p;
            }
            tmpPre = p;
            p=p->next;
        }
        return nullptr;
    }

};

//实验2
#define FF 0 //首次适应算法
#define BF 1 //最佳首次算法
#define ACT_FREE 0
#define ACT_ALLOC 1

typedef int ALGORITHM_ID;

typedef struct MemorySlice{
    bool isUsed;
    int jobID;
    int size;
    MemorySlice * former;
    MemorySlice * next;
}MemorySlice;

class Memory{
public:
    Memory(ALGORITHM_ID id,int INITIALMEMORYSIZE){
        if (id == FF || id == BF)
        {
            alloc = AllocAlgo[id];
        }
        memory = getInitSlice(INITIALMEMORYSIZE);
    }

    ~Memory(){
        destory();
    }

    // 变量
    MemorySlice * memory;
    void (Memory::*alloc)(int,int);

    // 函数
    MemorySlice * getInitSlice(int SIZE){
        MemorySlice * p = (MemorySlice *)malloc(sizeof(MemorySlice));
        p->isUsed = false;
        p->jobID = 0;
        p->size = SIZE;
        p->former = nullptr;
        p->next = nullptr;
        return p;
    }

    void destory(){
        while(memory!=nullptr){
            MemorySlice * t = memory;
            memory = memory -> next;
            free(t);
        }
        cout<<endl<<"内存清空完成"<<endl;
    }

    // 算法
    void (Memory::*AllocAlgo[2])(int,int)={&Memory::FFAlloc,&Memory::BFAlloc};
    
    // 首次适应算法
    void FFAlloc(int job,int num){
        MemorySlice* p = memory;
        while (p != nullptr)
        {
            if (!p->isUsed && p->size > num)
            {
                break;
            }
            p = p->next;
        }
        // 在 p 指向的地方分配
        startAlloc(p,job,num);
    }
    // 最佳适应算法
    void BFAlloc(int job,int num){
        MemorySlice* best=nullptr;
        MemorySlice* p = memory;
        while (p != nullptr)
        {
            if (!p->isUsed && p->size > num)
            {
                if(best==nullptr || best->size > p->size){
                    best = p;
                }
            }
            p=p->next;
        }
        startAlloc(best,job,num);
    }

    void startAlloc(MemorySlice* p,int job,int num){
        if(p==nullptr){
            cout<<"job "<<job<<"申请分配"<<num<<"KB失败"<<endl;
            return;
        }
        else{
            MemorySlice* ms = (MemorySlice*)malloc(sizeof(MemorySlice));
            ms->former = p->former;
            ms->size = num;
            ms->jobID = job;
            ms->isUsed = true;
            ms->next = p;
            if(ms->former != nullptr){
                ms->former->next = ms;
            }
            else{
                // 首部
                memory = ms;
            }
            // 修改 p
            p->former = ms;
            p->size -= ms->size;
        }
    }
    // 释放内存
    void Free(int job,int num){
        MemorySlice* p = findMemorySliceByJob(job);
        if(p==nullptr){
            cout<<"未找到job "<<job<<"分配的内存"<<endl;
            return;
        }
        else{
            // free
            MemorySlice* former = p->former;
            MemorySlice* next = p->next;
            if (former != nullptr && former->isUsed == false)
            {
                // 和former合并 , p-> former
                former->size += p->size;
                former->next = p->next;
                if (p->next != nullptr)
                {
                    p->next->former = former;
                }
                p = former;
            }
            if (next != nullptr && next->isUsed == false)
            {
                // p和 next合并
                p->size += next->size;
                p->next = next->next;
                if(next->next != nullptr){
                    next->next->former = p;
                }
            }
            p->isUsed = false;
        }
    }

    MemorySlice* findMemorySliceByJob(int num){
        MemorySlice* p = memory;
        while (p!=nullptr)
        {
            if (p->jobID == num)
            {
                return p;
            }
            p = p->next;
        }
        return nullptr;
    }

    // 动作
    void act(int job,int num,int ACTION){
        if (ACTION == ACT_FREE)
        {
            cout<<"释放job "<<job<<"的"<<num<<"KB内存"<<endl;
            Free(job,num);
        }
        else if(ACTION == ACT_ALLOC){
            cout<<"job "<<job<<"申请"<<num<<"KB内存"<<endl;
            (this->*alloc)(job,num);
        } 
        else{
            cout<<"未知操作符 "<<ACTION<<endl;
        }
    }

    void dump(){
        cout<<"---------------------"<<endl;
        MemorySlice* p = memory;
        int i = 0;
        while (p!=nullptr)
        {
            cout<<"内存块："<<i<<endl;
            if (p->isUsed)
            {
                cout<<"内存状态："<<"已占用"<<endl<<"作业名："<<p->jobID<<endl;
            }
            else{
                cout<<"内存状态："<<"未使用"<<endl;   
            }
            cout<<"空间大小："<<p->size<<"KB"<<endl;
            p = p->next;
            i += 1;
            cout<<endl;
        }
        cout<<"---------------------"<<endl;
    }
};

// 实验3：页面调度

#define INS_PER_PAGE 10
#define MEM_BLOCK_PER_JOB 3
#define STATUS_ERROR -1
#define MAXINT 2147483647
set<int> exp3_s;

int getRandom(int minLap,int maxLap){
    int n = rand()%(maxLap-minLap+1)+minLap;
    // 记录不能走的
    set<int> tmp;
    tmp.clear();
    while (exp3_s.find(n) != exp3_s.end())
    {
        // 找到了,证明这个数不可取
        tmp.insert(n);
        if(tmp.size() == maxLap - minLap + 1){
            // 找完了
            break;
        }
        n = rand()%(maxLap-minLap+1)+minLap;
    }
    if(tmp.size() == maxLap - minLap + 1){
        //cout<<"警告：无符合的block块"<<endl;
        return -1;
    }
    else{
        exp3_s.insert(n);
        //cout<<n<<endl;
        return n;
    }
}

class MemoryManager{
public: 
    int block_num;
    int * ins = nullptr;
    int block_index = 0;
    int up=0,down = 0,force=0; // 向前跳和向后跳的次数

    MemoryManager(int ins){
        this->block_num = ins/2;
        this->ins = (int *)malloc(sizeof(int)*this->block_num);
    }

    ~MemoryManager(){
        if(ins != nullptr)
            free(ins);
    }

    void add(int num){
        ins[block_index] = num;
        block_index++;
    }

    int chooseBlock(int blockNum){
        // 找一个相对较好的Block
        if (up>down){
            // 优先向下走
            if(blockNum+2<=block_num){
                //没有越界，可用向下走
                int t = getRandom(blockNum+2,block_num);
                if(t != STATUS_ERROR){
                    down ++;
                    return t;
                }
            }
            // 不能向下走，只能向上走
            if (blockNum != 1){
                // 能向上走
               int t = getRandom(1,blockNum-1);
                if(t != STATUS_ERROR){
                    up ++;
                    return t;
                }
            }
        }
        // 优先向上走
        else{
            if (blockNum != 1){
                // 向上走
                int t = getRandom(1,blockNum-1);
                if(t != STATUS_ERROR){
                    up ++;
                    return t;
                }
            }
            else{
                // 只能向下走
                if(blockNum+2<=block_num) {
                    //没有越界，可用向下走
                    int t = getRandom(blockNum + 2, block_num);
                    if (t != STATUS_ERROR) {
                        down++;
                        return t;
                    }
                }
            }
        }
        // 不能向上走，也不能向下+1走，说明只剩下跳转到下一个block了
        if (blockNum+1 <= block_num){
            // 没到底
            int t = getRandom(blockNum+1,blockNum+1);
            if(t != STATUS_ERROR){
                force ++;
                return t;
            }
        }
        return STATUS_ERROR;
    }

    //动作
    int * simulate(){
        int times = 0;
        cout<<"开始生成随机指令序列..."<<endl;
        while (true)
        {
            // 初始化一些记录变量,尽量保证向前跳和向后跳次数相等
            block_index = 0;up = 0;down = 0;force = 0;exp3_s.clear();
            // 自动随机
            int current_b = getRandom(1,block_num);
            add(current_b);
            //cout<<current_b;
            // step.1 生成起始block块,从1开始计数
            while ((current_b = chooseBlock(current_b)) != STATUS_ERROR){
                add(current_b);
                //cout<<"->"<<current_b;
            }
            if (block_index == block_num){
                // 一个序列完成
                cout<<"生成完成：顺序执行"<<block_num<<"次，向前"<<up<<"次，向后"<<down<<"次，最后强制顺序执行："<<force<<"次"<<endl;
                for (int i = 0; i < block_index; ++i) {
                    cout<<2*ins[i]-1<<"->"<<2*ins[i]<<"->";
                }
                cout<<endl;
                cout<<"是否接受？接受请输入y并回车，其他任意键继续生成"<<endl;
                int c = getchar();
                if (c == 'y'){
                    break;
                }
                else{
                    cout<<"继续随机生成"<<endl;
                }
            } else{
                //cout<<"警告："<<endl;
            }
            ++times;
        }
        // 返回一个可用的序列，转换为页面而不是指令
        int* ss=(int*)malloc(sizeof(int)*block_num*2);
        int index =0;
        for (int j = 0; j < block_num; ++j) {
            ss[index] = (ins[j]*2-1-(ins[j]*2-1)%INS_PER_PAGE)/INS_PER_PAGE;
            ss[index+1] = (ins[j]*2-(ins[j]*2)%INS_PER_PAGE)/INS_PER_PAGE;
            index+=2;
        }
        return ss;
    }
};

// 注意：页数是从0-9才是10
#include <queue>

class SwapAnalyzer{
public:

    int *serial;
    int length;

    int mem[MEM_BLOCK_PER_JOB];
    int total_size,lack_size;

    SwapAnalyzer(int *serial, int length) : serial(serial), length(length) {reset();}


    void mem_pop(){
        for (int i = 0; i < MEM_BLOCK_PER_JOB-1; ++i){
            int page = mem[i];
            mem[i]=mem[i+1];
        }
    }

    void mem_exchange(int old_page_id,int new_page_id){
        int page = mem[old_page_id];
        mem[old_page_id] = mem[new_page_id];
        mem[new_page_id] = page;
    }

    void mem_in(int index,int page){
        mem[index] = page;
    }

    void mem_insert_tail(int new_page){
        mem[MEM_BLOCK_PER_JOB-1]=new_page;
    }

    int mem_find(int page){
        int index = STATUS_ERROR;
        for (int i = 0; i < MEM_BLOCK_PER_JOB ; ++i) {
            if (mem[i] == STATUS_ERROR){
                continue;
            }
            if(mem[i]==page){
                index = i;
                break;
            }
        }
        return index;
    }

    int isFull(){
        for (int i = 0; i < MEM_BLOCK_PER_JOB; ++i) {
            if (mem[i]==STATUS_ERROR){
                return i;
            }
        }
        return STATUS_ERROR;
    }

    int getNextDistance(int beginIndex,int value){
        int dis = 1;
        for (int i = beginIndex; i < length; ++i) {
            if (serial[i]==value){
                return dis;
            }
            else{
                dis++;
            }
        }
        return STATUS_ERROR;
    }

    void opt(){
        reset();
        cout<<">>> OPT算法"<<endl;
        for (int i = 0; i < length; ++i) {
            //对应每一条访存页
            int page = serial[i];
            int point = isFull();
            int result = mem_find(page);
            if(result == STATUS_ERROR){
                if (point != STATUS_ERROR){
                    // point空着
                    mem_in(point,page);
                }
                else{
                    // 没空
                    // 需要置换
                    // 选择一个最久未使用的
                    int maxIndex=STATUS_ERROR;
                    int maxDis= -1;
                    for (int j = 0; j < MEM_BLOCK_PER_JOB; ++j) {
                        int dis = getNextDistance(i+1,mem[j]);
                        if (dis>maxDis || dis == STATUS_ERROR){
                            maxIndex = j;
                            maxDis = dis;
                        }
                        if(dis == STATUS_ERROR){
                            break;
                        }
                    }
                    // 置换maxIndex
                    mem_in(maxIndex,page);
                }
                lack_size++;
            }
            total_size++;
        }
        cout<<"总指令数："<<total_size<<"，缺页次数："<<lack_size<<"，缺页率："<<(float)lack_size/total_size*100.0<<"%"<<endl;

    }

    void fifo(){
        reset();
        cout<<">>> FIFO算法"<<endl;
        for (int i = 0; i < length; ++i) {
            //对应每一条访存页
            int page = serial[i];
            int point = isFull();
            int result = mem_find(page);
            if(result == STATUS_ERROR){
                if (point != STATUS_ERROR){
                    // point空着
                    mem_in(point,page);
                    lack_size++;
                }
                else{
                    // 没空
                    // 需要置换
                    mem_pop();
                    mem_insert_tail(page);
                    lack_size++;
                }
            }
            total_size++;
        }
        cout<<"总指令数："<<total_size<<"，缺页次数："<<lack_size<<"，缺页率："<<(float)lack_size/total_size*100.0<<"%"<<endl;
    }

    void lru(){
        reset();
        cout<<">>> LRU算法"<<endl;
        for (int i = 0; i < length; ++i) {
            //对应每一条访存页
            int page = serial[i];
            int point = isFull();
            int result = mem_find(page);
            if(result == STATUS_ERROR){
                if (point != STATUS_ERROR){
                    // point空着
                    mem_in(point,page);
                    lack_size++;
                }
                else{
                    // 没空
                    // 需要置换
                    mem_pop();
                    mem_insert_tail(page);
                    lack_size++;
                }
            }
            else{
                // LRU置换新 result号放至最后
                int endi = (point == STATUS_ERROR)?MEM_BLOCK_PER_JOB-1:point-1;
                for (int j = result; j < endi; ++j) {
                    mem_exchange(j,j+1);
                }
            }
            total_size++;
        }
        cout<<"总指令数："<<total_size<<"，缺页次数："<<lack_size<<"，缺页率："<<(float)lack_size/total_size*100.0<<"%"<<endl;
    }

private:
    void reset(){
        for (int &i : mem) {
            i = STATUS_ERROR;
        }
        lack_size = 0;
        total_size = 0;
    }
};



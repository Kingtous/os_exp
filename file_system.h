/*
 * @Author: Kingtous
 * @Date: 2019-12-05 16:57:33
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-12-05 18:12:04
 * @Description: Kingtous' Code
 */
#include <utility>

#include <utility>

#ifndef UNTITLED_FILE_SYSTEM_H
#define UNTITLED_FILE_SYSTEM_H

// 简单文件系统
#include <string>
#define FILE 'F'
#define DIRECTORY 'D'
using namespace std;

// 文件
typedef struct FPCB{
    string fileName = "Untitiled";
    string content;
    char property = FILE;
    int firstBlock = -1;
    int length = 1;
    bool authority = false; // 权限
    FPCB* neigh = nullptr;
    FPCB* father = nullptr;
    FPCB* child = nullptr;
}FPCB;


// 文件表
typedef struct FAT{
    int nextb = -1;
    bool state = false;
}FAT;


class FBuffer{
    bool * buf;
    int length;
    int freeth;
public:

    FBuffer(int length){
        this->length = length;
        buf = new bool[length];
        for (int i = 0; i < length; ++i) {
            buf[i] = false;
        }
        this->freeth = length;
    }

    int getFreeBlock(){
        for (int i = 0; i < length; ++i) {
            if (buf[i]==false){
                // 选取
                buf[i]=true;
                return i;
            }
        }
        return -1;
    }

    void freeBlock(int index){
        buf[index] = false;
    }

};


class FATable{
public:

    int fatLength;
    FAT * fatTable;
    FBuffer *fBuffer;

    FATable(int length){
        fatLength = length;
        fBuffer = new FBuffer(length);
        fatTable = new FAT[length];
    }

    void addFCB(FPCB * pcb){
        int cnt = pcb->length;
        int priorblock = pcb->firstBlock;
        while(cnt--){
            int index = fBuffer->getFreeBlock();
            if(pcb->firstBlock == -1){
                pcb->firstBlock = index;
            }
            else{
                fatTable[priorblock].nextb = index;
            }
            // 指向下一块
            fatTable[index].state = true;
            fatTable[index].nextb = -1;
            priorblock = index;
        }
    }

    void delFCB(FPCB * fpcb){
        int block = fpcb->firstBlock;
        while (block != -1){
            fBuffer->freeBlock(block);
            fatTable[block].state = false;
            block = fatTable[block].nextb;
            fatTable[block].nextb = -1;
        }
    }

    void writeFCB(FPCB * fpcb,string input){
        if(fpcb->authority){
            fpcb->content = std::move(input);
            printf("\nWrite Success.\n");
        }
        else{
            fprintf(stderr,"\nWrite Error: Please Open First!\n");
        }
    }

    void openFCB(FPCB * fpcb){
        if (fpcb->authority){
            fprintf(stderr,"\nOpen Warning: Already Opened!\n");
        }
        else{
            fpcb->authority = true;
            printf("\nOpen Success.\n");
        }
    }

    void closeFCB(FPCB * fpcb){
        if (!fpcb->authority){
            fprintf(stderr,"\nOpen Warning: Already Closed!\n");
        }
        else{
            fpcb->authority = false;
            printf("\nClose Success.\n");
        }
    }


};


class FileSystem{
private:
    int length;
    FPCB * head= nullptr;
    FPCB * now= nullptr;
    FATable* table;

    FPCB* getFile(const string &fileName,char Type){
        FPCB *fpcb = now->child;
        while (fpcb != nullptr){
            if (fpcb->fileName == fileName && fpcb->property == Type){
                return fpcb;
            }
            fpcb = fpcb->neigh;
        }
        return nullptr;
    }

    FPCB* getTail(string fileName){
        FPCB *fpcb = now->child;
        if(fpcb == nullptr){
            // 目录下还是空的
            // 特许这里能这样写为空字符
            FPCB *fpcb_t = new FPCB();
            fpcb_t->fileName = "";
            fpcb_t->length = 1;
            now->child = fpcb_t;
            table->addFCB(fpcb_t);
            return fpcb_t;
        }else{
            while (fpcb->neigh != nullptr){
                if(fpcb->fileName == fileName){
                    return nullptr;
                }
                fpcb = fpcb->neigh;
            }
            fpcb->neigh = new FPCB();
            return fpcb->neigh;
        }
    }

    FPCB* getFileLeftNeigh(FPCB* f){
        FPCB* fpcb = now->child;
        while (fpcb != nullptr){
            if(fpcb->neigh == f){
                return fpcb;
            }
            fpcb = fpcb->neigh;
        }
        return nullptr;
    }

public:

    FileSystem(int length){
        table = new FATable(length);
        this->length= length;
        head = new FPCB();
        now  = head;
        head->fileName = "/";
        head->property = DIRECTORY;
        table->addFCB(head);
    }

    string getCurrentFolder(){
        return now->fileName;
    }

    FPCB* create(const string &fileName,int length,char property = FILE){
        FPCB *fpcb =new FPCB();
        fpcb->fileName = fileName;
        fpcb->length = length;
        fpcb->property = property;
        fpcb->father = now;
        FPCB * tail = getTail(fileName);
        *tail = *fpcb;
        return tail;
    }

    void format(){
        table = new FATable(length);
        this->length= length;
        head = new FPCB();
        now  = head;
        head->fileName = "/";
        head->property = DIRECTORY;
        table->addFCB(head);
        printf("\nFormat Success.\n");
    }

    void ls(){
        printf("目录名:%s",now->fileName.c_str());
        auto p = now->child;
        while (p!= nullptr){
            printf("\n%s\t%d\t%c",p->fileName.c_str(),p->length,p->property);
            p = p->neigh;
        }
    }

    void rm(const string &fileName){
        auto file = getFile(fileName,FILE);
        if(file){
            auto left = getFileLeftNeigh(file);
            if(left != nullptr)
                left->neigh = file->neigh;
            else{
                // 没有左边了
                file->father->child = file ->neigh;
            }
        }
        else{
            fprintf(stderr,"\nRM Error: File not exists.\n");
        }
    }

    void rmdir(const string &folderName){
        auto file = getFile(folderName,DIRECTORY);
        if(file){
            auto left = getFileLeftNeigh(file);
            if(left != nullptr)
                left->neigh = file->neigh;
            else{
                // 没有左边了
                file->father->child = file->neigh;
            }
        }
        else{
            fprintf(stderr,"\nRM Error: Directory not exists.\n");
        }
    }

    void write(const string &fileName,const string &content){
        auto file = getFile(fileName,FILE);
        if(file!= nullptr){
            if (file->authority)
                file->content = content;
            else{
                fprintf(stderr,"\nError: File not opened.\n");
            }
        }
        else{
            fprintf(stderr,"\nError: File Not Exists.\n");
        }
    }

    void read(const string &fileName){
        auto file = getFile(fileName,FILE);
        if(file!= nullptr){
            if(file->authority)
                printf("Content For %s:\n%s",file->fileName.c_str(),file->content.c_str());
            else{
                fprintf(stderr,"\nRead Error: File not opened.\n");
            }
        }
        else{
            fprintf(stderr,"\nError: File Not Exists.\n");
        }
    }

    void open(const string &fileName){
        auto file = getFile(fileName,FILE);
        if(file!= nullptr){
            table->openFCB(file);
        }
        else{
            fprintf(stderr,"\nError: File Not Exists.\n");
        }
    }

    void close(const string &fileName){
        auto file = getFile(fileName,FILE);
        if(file!= nullptr){
            table->closeFCB(file);
        }
        else{
            fprintf(stderr,"\nError: File Not Exists.\n");
        }
    }

    void mkdir(const string &folderName){
        create(folderName,1,DIRECTORY);
    }

    void cd(const string &folderName){
        if(folderName == ".."){
            if(now -> father != nullptr){
                now = now->father;
                printf("Switch to Folder %s",now->fileName.c_str());
            }
            else{
                fprintf(stderr,"\nCD Error: Folder is already the root of the system.\n");
            }
        }
        else{
            auto file = getFile(folderName,DIRECTORY);
            if(file!= nullptr){
                now = file;
                printf("Switch to Folder %s",now->fileName.c_str());
            }
            else{
                fprintf(stderr,"\nCD Error: Folder Not Exists.\n");
            }
        }
    }

};

//
#endif //UNTITLED_FILE_SYSTEM_H



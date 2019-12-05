/*
 * @Author: Kingtous
 * @Date: 2019-12-05 16:57:33
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-12-05 18:11:48
 * @Description: Kingtous' Code
 */
#include <iostream>
#include "file_system.h"

using namespace std;
int main(){
    FileSystem fileSystem(1024);
    string op,value,extra;
    while (true){
        printf("FileSystem: %s > ",fileSystem.getCurrentFolder().c_str());
        cin>>op>>value;
        if(op == "format"){
            fileSystem.format();
        }
        else if(op == "rm"){
            fileSystem.rm(value);
        }
        else if(op == "create"){
            int length;
            printf("File Length:");
            cin>>length;
            fileSystem.create(value,length);
        }
        else if(op == "write"){
            printf("Input Content:");
            cin>>extra;
            fileSystem.write(value,extra);
        }
        else if(op == "read"){
            fileSystem.read(value);
        }
        else if(op =="open"){
            fileSystem.open(value);
        }
        else if(op == "close"){
            fileSystem.close(value);
        }
        else if(op == "ls"){
            fileSystem.ls();
        }
        else if(op =="mkdir"){
            fileSystem.mkdir(value);
        }
        else if(op == "rmdir"){
            fileSystem.rmdir(value);
        }
        else if(op == "cd"){
            fileSystem.cd(value);
        }
        else if(op == "exit"){
            break;
        }
        else{
            fprintf(stderr,"\nUnknown Operator.\n");
        }
        cout<<endl;
    }
    return 0;
}
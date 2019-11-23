/*
 * @Author: Kingtous
 * @Date: 2019-11-23 18:04:40
 * @LastEditors: Kingtous
 * @LastEditTime: 2019-11-23 18:07:55
 * @Description: Kingtous' Code
 */
#include <stdio.h>
#define M 20
void Opt(int paper[]);
 
void print(int a[],int n)
{
    int i;
    for (i=0;i<=n;i++)
    {
        printf("%d ",a[i]);
    }
    printf("\n");
}
int main()
{
	int paper[M]={4,4,3,3,4,4,3,3,1,1,2,2,3,3,1,1,2,2,2,2};

    int i=0;
	//printf("请输入页面走向\n");
	/*for (i = 0; i < 12;i++)
	{
		scanf("%d", &paper[i]);
	}
	*/
 
	printf("内存中物理块中的页面如下\n");
	Opt(paper);
	return 0;
}
 
void Opt(int paper[])
{
    double count=3.0;
    int a[3]={0,0,0};
    int i=0,k=0,flag=0,temp;
    for (i=0;i<M;i++)
    {
        flag=0;
        //printf("%d",paper[i]);
        if (i<=2)
        {
            a[i]=paper[i];
            print(a,i);
            continue;
        }
 
        for (k=0;k<2;k++)
        {
            if (paper[i]==a[k])
            {flag=1;
 
            break;}
        }
        if (flag==0)//表示有置换
        {
            count++;
            a[0]=a[1];
            a[1]=a[2];
            a[2]=paper[i];
            print(a,2);
        }
        if (flag==1) //表示没置换
        {
            if (k==0)
            {
                temp=a[0];
                a[0]=a[1];
                a[1]=a[2];
                a[2]=temp;
            }
            if (k==1)
            {
                temp=a[1];
                a[1]=a[2];
                a[2]=temp;
            }
 
        }
 
 
    }
    printf("置换次数为%.0f,缺页率为%.2f",count,count/M);
}


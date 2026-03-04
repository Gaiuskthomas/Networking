#include<stdio.h>
#include<stdlib.h>
int nt=0;
int k=0,fl=0;
int left[10000] = {-1};
int frames[100],frame_size,window_size;
int input()
{
printf("\n\n Input \n\n");
for(int i = 1 ; i <= frame_size ; i++)
{
printf(" Enter Value For Frame[%d] : " , i);
scanf("%d",&frames[i]);
printf("\n");
}
printf("\n\n");
return 1;
}
int display()
{
printf("\n\n Display \n\n");
for(int i = 1 ; i <= frame_size ; i++)
{
printf(" Frame[%d] : %d " , i , frames[i]);
printf("\n");
}
printf("\n\n");
return 1;
}
void sender(int fr[],int q,int fl)
{
int flag = rand() % 2;
if(flag&&fl==0)
{
printf(" Frame[%d] with value %d send !!! \n\n", q , fr[q]);
printf(" Frame Acknowledged !!! \n\n");
nt++;
}
else if(flag&&fl==1)
{
printf(" Frame[%d] retransmitted with value %d !!! \n\n", q , fr[q]);
printf(" Frame Acknowledged !!! \n\n");
nt++;
}
else if(fl==1)
{
printf(" Frame[%d] retransimitted with value %d !!! \n\n", q , fr[q]);
printf(" Frame not Acknowledged !!! \n\n");
left[k++] = fr[q];
nt++;
}
else
{
printf(" Frame[%d] with value %d send !!! \n\n", q , fr[q]);
printf(" Frame not Acknowledged !!! \n\n");
left[k++] = fr[q];
nt++;
}
}
int selective_repeat()
{
int i ;
for(i = 1 ; i <= frame_size ; i++)
{
fl=0;
sender(frames,i,fl);
if(i % window_size == 0)
{
for(int x = 0 ; x < k ; x++)
{
fl=1;
sender(left,x,fl);
}
k = 0;
}
}
fl=1;
for(i = 0 ; i < k ; i++)
{
sender(left,i,fl);
}
printf(" Total Transmissions : %d \n\n", nt);
return 0;
}
int main()
{
printf("Enter Window size:");
scanf("%d",&window_size);
printf("Enter The Number of Frames to be Transmitted:");
scanf("%d",&frame_size);
input();
display();
selective_repeat();
return 0;
}


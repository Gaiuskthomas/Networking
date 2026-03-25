#include<stdio.h>
int main(){
int incoming, outgoing, buck_size, n, store = 0;
printf("Enter bucket size, outgoing rate and no of inputs: ");
scanf("%d %d %d", &buck_size, &outgoing, &n);
while (n != 0) {
printf("\nEnter the incoming packet size : ");
scanf("%d", &incoming);
if (incoming <= (buck_size - store)){
store += incoming;
printf("Bucket buffer size %d out of %d\n", store, buck_size);
} else {
printf("Dropped %d no of packets\n", incoming - (buck_size - store));
store = buck_size;
printf("Bucket buffer size %d out of %d\n", store, buck_size);
}
store = store - outgoing;
printf("After outgoing %d packets left out of %d in buffer\n", store, buck_size);
n--;
}
}

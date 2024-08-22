#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "data.h"

int main(){
    int id = 10;
    int fd = open("product.txt", O_RDWR|O_TRUNC);

    struct product products[10] = {
        {1,"Product_1",100.49,1},
        {2,"Product_2",10.49,2},
        {3,"Product_3",101.49,5},
        {4,"Product_4",102.49,100},
        {5,"Product_5",120.49,34},
        {6,"Product_6",103.49,25},
        {7,"Product_7",109.49,36},
        {8,"Product_8",110.49,24},
        {9,"Product_9",130.49,31},
        {10,"Product_10",150.49,76},
    };

    for(int i=0;i<id;i++)
    {
        write(fd, &products[i],sizeof(struct product));
    }
}
// Write a program to communicate between two machines using socket.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "data.h"


int main()
{
    int id = 10;
    int fd = open("product.txt", O_RDWR);

    int option;
    struct flock fl;
    char allow[20];
    printf("Do you want to many changes in the items list\n");
    scanf("%s", allow);
    if(strcmp(allow,"yes") == 0)
    {
        fl.l_type = F_WRLCK;
        fl.l_whence = 0;
        fl.l_len = 0;
        fl.l_pid = getpid(); 
        fcntl(fd,F_SETLKW,&fl);

        while(1)
        {               
            printf("choice option from\n 1. Add item \n 2. Delete item \n 3. Update item\n 4.End \n");
            printf("Enter option: ");
            scanf("%d", &option);
            if(option == 1)
            {
                struct product add_product;
                float cost; 
                char n[20];
                printf("Product name: ");
                scanf("%s", add_product.P_name);
                printf("Product cost: ");
                scanf("%f", &add_product.cost);
                printf("Product quantity: ");
                scanf("%d", &add_product.quantity);
                id++;
                add_product.P_Id = id;
                off_t endPos = lseek(fd, 0, SEEK_END);
                write(fd, &add_product,sizeof(struct product));
                
            }
            else if(option == 2)
            {
                struct product p;
                int indexToDelete=0,i=0;
                char buffer[1024];
                ssize_t num_bytes_read;
                char productToDelete[20];
                printf("Product name: ");
                scanf("%s", productToDelete);
                off_t startPos = lseek(fd, 0, SEEK_SET);
                while(read(fd, &p,sizeof(struct product))>0)
                {
                    // read(fd, &p,sizeof(struct product));
                    if(strcmp(p.P_name,productToDelete)==0)
                    {
                        num_bytes_read = read(fd, buffer, sizeof(buffer));
                        break;
                    }
                    i++;
                }
                off_t offset = lseek(fd, sizeof(struct product) * i, SEEK_SET);
                ssize_t num_bytes_written = write(fd, buffer, num_bytes_read);
                off_t endPos = lseek(fd, 0, SEEK_END);
                ftruncate(fd, endPos - sizeof(struct product));
                printf("Product %s deleted.\n", productToDelete);
                //  
                // else {
                // printf("Invalid index.\n");
                // 
            }
            else if(option == 3)
            {
                struct product p;
                ssize_t num_bytes_read;
                char buffer[1024];
                int c;
                char productToDelete[20];
                printf("Product name: ");
                scanf("%s", productToDelete);

                printf("what you want to change?? \n 1. Price\n 2. quantity\n");
                printf("Enter option: ");
                scanf("%d", &c);
                float new_price;
                int new_quantity,i=0;
                if(c==1)
                {
                    printf("Enter the new price: ");
                    scanf("%f", &new_price);
                }
                else if(c==2)
                {
                    printf("Enter the new quantity: ");
                    scanf("%d", &new_quantity);       
                }
                off_t startPos = lseek(fd, 0, SEEK_SET);
                while(read(fd, &p,sizeof(struct product))>0)
                {
                    // read(fd, &p,sizeof(struct product));
                    if(strcmp(p.P_name,productToDelete)==0)
                    {
                        if(c==1)
                        {
                            p.cost = new_price;
                        }
                        else if(c==2)
                        {
                            p.quantity = new_quantity;
                        }
                        lseek(fd, sizeof(struct product) * i, SEEK_SET);
                        write(fd, &p,sizeof(struct product));
                        break;
                    }
                    i++;
                }
            }
            
            else if (option == 4)
            {
                break;
            }
        }
    }

    fl.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&fl);

    struct product x;
    off_t startPos = lseek(fd, 0, SEEK_SET);
    while(read(fd, &x,sizeof(struct product))>0)
    {
        printf("%d %s %f %d\n", x.P_Id,x.P_name,x.cost,x.quantity);
    }
}
 
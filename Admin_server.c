// Write a program to communicate between two machines using socket.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "data.h"

struct message{
    char product_name[120];
    int quantity;
    char message[500];
};
struct message_only{
    char message[500];
};

void update(char product_name[],int quantity)
{
    int fd = open("product.txt", O_RDWR);
    struct product p;
    ssize_t num_bytes_read;
    char buffer[1024];
    int i=0;
    off_t startPos = lseek(fd, 0, SEEK_SET);
    while(read(fd, &p,sizeof(struct product))>0)
    {
        if(strcmp(p.P_name,product_name)==0)
        {
            p.quantity = p.quantity - quantity;
            lseek(fd, sizeof(struct product) * i, SEEK_SET);
            write(fd, &p,sizeof(struct product));
            break;
        }
        i++;
    }
}
int main()
{
    int fd = open("product.txt", O_RDWR);
    struct product cart[100];
    int cart_size = 0 ;
    
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    printf("Socket created successfully\n");
    printf("Socket id: %d\n", sockfd);

    // create a socket address structure
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = INADDR_ANY,
    };

    // bind the socket to the address
    int status = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (status == -1)
    {
        perror("bind");
        return 1;
    }

    printf("Socket binded successfully\n");

    // listen for connections
    status = listen(sockfd, 5);
    if (status == -1)
    {
        perror("listen");
        return 1;
    }

    printf("Listening for connections\n");

    // accept a connection
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
        int clientSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSockfd == -1)
        {
            perror("accept");
            return 1;
        }

        printf("Connection accepted\n");
        printf("Client socket id: %d\n", clientSockfd);
    while(1)
    {
        // read from the client

        char buffer[1024];
        status = read(clientSockfd, buffer, sizeof(buffer));
        if (status == -1)
        {
            perror("read");
            return 1;
        }

        if(strcmp(buffer,"Display all the products") == 0)
        {
            struct product x;
            off_t startPos = lseek(fd, 0, SEEK_SET);
            while(read(fd, &x,sizeof(struct product))>0)
            {
                printf("%d %s %f %d\n", x.P_Id, x.P_name, x.cost, x.quantity);
            }


        }
        else if(strcmp(buffer,"Display the Cart") == 0)
        {

            for(int i=0;i<cart_size;i++)
            {
                printf("%d %s %f %d\n", cart[i].P_Id, cart[i].P_name, cart[i].cost, cart[i].quantity);
            }
            if(cart_size==0)
            {
                printf("Cart is empty\n");
            }
        }
        else if(strcmp(buffer,"Add product to the cart") == 0)
        {
            struct product x;
            off_t startPos = lseek(fd, 0, SEEK_SET);

            char pr[100];
            int qu;
            read(clientSockfd, pr, sizeof(pr));
            read(clientSockfd, &qu, sizeof(int));


            int h=0;
            int i=0;
            struct flock fl;
            while(read(fd, &x,sizeof(struct product))>0)
            {
                if(strcmp(x.P_name,pr) == 0)
                {
                    fl.l_type = F_RDLCK;
                    fl.l_whence = 0;
                    fl.l_start = i*sizeof(struct product);
                    fl.l_len = sizeof(struct product);
                    fl.l_pid = getpid(); 
                    printf("Before locking..\n");
                    fcntl(fd,F_SETLKW,&fl);
                    printf("%d %d",fl.l_type,F_RDLCK);
                    printf("Entered the critical section...");
                    h++;
                    if(x.quantity >= qu)
                    {
                        struct product add;
                        add.P_Id = x.P_Id;
                        strcpy(add.P_name,pr);
                        add.cost = x.cost;
                        add.quantity = qu;
                        cart[cart_size] = add;
                        cart_size++;
                        update(pr,qu);
                        printf("Product %s added to the cart of quantity %d\n", pr,qu);

                    }
                    else
                    {
                        printf("Out of stock\n");

                    }
                }
                i++;
            }
            fl.l_type = F_UNLCK;
            fcntl(fd,F_SETLK,&fl);
            if(h==0)
            {
                printf("Product is not avaliable\n");

            }
        }
        else if(strcmp(buffer,"Edit the cart") == 0)
        {
            char mess[100];
            read(clientSockfd, mess, sizeof(mess));

            if(strcmp(mess,"Delete the product") == 0)
            {
                int flag = 0;
                 char pr[100];
                read(clientSockfd, pr, sizeof(pr));
                for(int i=0;i<cart_size;i++)
                {
                    if(strcmp(cart[i].P_name,pr) != 0)
                    {
                        cart[flag] = cart[i];
                        flag++;
                    }
                    else
                    {
                        update(cart[i].P_name, -cart[i].quantity);
                    }
                }
                printf("Product deleted from the cart\n");
                cart_size--;
            }
            else if(strcmp(mess,"Edit the quantity")==0)
            {
                char prx[100];
                int qu;
                read(clientSockfd, prx, sizeof(prx));
                read(clientSockfd, &qu, sizeof(int));
                for(int i=0;i<cart_size;i++)
                {
                    if(strcmp(cart[i].P_name,prx) == 0)
                    {
                        cart[i].quantity = qu;
                        update(prx, qu-cart[i].quantity);
                    }
                }
                printf("Quantity editted in the cart\n");
            }
        }
        else if(strcmp(buffer,"Payment receipt") == 0)
        {
            struct flock fl1;
            fl1.l_type = F_WRLCK;
            fl1.l_whence = 0;
            fl1.l_len = 0;
            fl1.l_pid = getpid(); 
            fcntl(fd,F_SETLKW,&fl1);
            float amt = 0;
            for(int i=0;i<cart_size;i++)
            {
                amt += cart[i].cost * cart[i].quantity;
            }
            write(clientSockfd, &amt, sizeof(int));

            float paid_amt;
            read(clientSockfd, &paid_amt, sizeof(int));
            float change = paid_amt - amt;

            for(int i=0;i<cart_size;i++)
            {
                float y = cart[i].cost * cart[i].quantity;
                printf("%s %f * %d = %f\n", cart[i].P_name, cart[i].cost, cart[i].quantity, y);
            }
            printf("Total amount = %f\n", amt);
            printf("Paid amount = %f\n", paid_amt);
            printf("Change = %f\n", change);
            fl1.l_type = F_UNLCK;
            fcntl(fd,F_SETLK,&fl1);
        }
        else if(strcmp(buffer,"Exit") == 0)
        {
            break;
        }
        printf("Message from client: %s\n", buffer);
        printf("\n\n");
    }
}
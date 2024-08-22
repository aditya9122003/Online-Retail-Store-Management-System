#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "data.h"



int main()
{
    // create a socket
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
        // ! inet_addr val = ip address of the server
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    // connect to the server
    int status = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (status == -1)
    {
        perror("connect");
        return 1;
    }

    printf("Connected to server\n");
    char message[50];
    printf("Choice the option: \n");
    while (1)
    {
        int option;
        printf("1. Displays all the products. \n 2. Displays the Cart.\n 3. You can choose a product and quantity to add into the cart. \n 4. You can edit the cart. \n 5. Payment page \n  6. Exit \n");
        printf("Enter option: ");
        scanf("%d", &option);
        if(option == 1)
        {
            write(sockfd,"Display all the products",sizeof("Display all the products"));
        }
        else if(option == 2)
        {   
            char mess[] = "Display the Cart";
            write(sockfd,mess,sizeof(mess));

        }
        else if(option == 3)
        {
          
            char mess[] = "Add product to the cart";
            write(sockfd,mess,sizeof(mess));
            char p[20];
            int q;
            printf("Enter the product name: ");
            scanf("%s",p);
            write(sockfd,&p,sizeof(p));
            printf("Enter amount of quntity: ");
            scanf("%d",&q);
            write(sockfd,&q,sizeof(q));
        }
        else if (option == 4)
        {
            char mes[] = "Edit the cart";
            write(sockfd,mes,sizeof(mes));

            int x,quan;
            char pro_n[20];
            printf("1. Delete the product \n 2. Edit the quantity\n");
            scanf("%d", &x);
            if(x == 1)
            {
                write(sockfd,"Delete the product",sizeof("Delete the product"));
                printf("Enter product name: ");
                scanf("%s", pro_n);
                write(sockfd, &pro_n,sizeof(pro_n));
            }
            else if(x == 2)
            {
                write(sockfd,"Edit the quantity",sizeof("Edit the quantity"));
                printf("Enter product name: ");
                scanf("%s", pro_n);
                write(sockfd,&pro_n,sizeof(pro_n));
                printf("Enter quantity: ");
                scanf("%d", &quan);
                write(sockfd,&quan,sizeof(int));
            }
        }
        else if(option == 5)
        {
            float amt;
            write(sockfd,"Payment receipt",sizeof("Payment receipt"));
            float total_amt;
            read(sockfd, &total_amt, sizeof(float));
            printf("Total Amount = %f\n", total_amt);
            printf("Enter the amount: ");
            scanf("%f", &amt);
            write(sockfd, &amt, sizeof(float));
        }
        else if(option ==6)
        {
            write(sockfd,"Exit",sizeof("Exit"));
            break;
        }
    }
    // send a message to the server
    strcpy(message,"Hello server!");
    status = write(sockfd, message, strlen(message));
    if (status == -1)
    {
        perror("write");
        return 1;
    }
    printf("Message sent to server\n");

    return 0;
}
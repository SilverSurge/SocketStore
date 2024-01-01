// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>      // for cart
#include <vector>   // for cart
#define PORT 8080

#include "Codes.hpp"
#include "SocketStoreDB.hpp"

void print_menu()
{
    printf("----------------------------------------\n");
    printf("0. Register\n");    // complete
    printf("1. Login\n");       // complete
    printf("2. Logout\n");      // complete
    printf("3. Exit\n");        // complete
    printf("4. View All Products\n");               // complete
    printf("5. View All Products Cheaper than\n");  // complete
    printf("6. Add to Cart\n");     // complete
    printf("7. View Cart\n");       // complete
    printf("8. Checkout [Everything is Free]\n");
    printf("9. Print Menu\n");  // complete
    printf("----------------------------------------\n");
}

int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0)
    {
        printf("Socket Failure\n");
        return 0;
    }

	struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

    int inet_pton_res = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if(inet_pton_res <= 0)
    {
        printf("Inet Pton Failure\n");
        return 0;
    }

    int connect_res = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(connect_res < 0)
    {
        printf("Connect Failure\n");
    }

    // write
    // read

    char buffer[128+1] = {0};
    char msg[] = "hello from client";


    // -----------------write read block--------------------- //
    print_menu();
    int fd = client_fd;
    int operation_type = -1;
    std::map<int,int> cart;
    bool has_login = false;
    while(true)
    {
        printf("enter operation type: ");
        scanf("%d", &operation_type);
        if(operation_type == 0)
        {
            if(has_login)
            {
                printf("Already Logged in!!\n");
                continue;
            }

            // register
            int code = REGISTRATION_REQUEST;
            write(client_fd, &code, sizeof(int));

            read(client_fd, &code, sizeof(int));    // OK

            // Taking Input
            char username[32] = {0}, password[32] = {0};
            printf("enter new username [<=30 characters]:");
            fgets(username, 32, stdin);
            fgets(username, 32, stdin);
            printf("enter new password [<=30 characters]:");
            fgets(password, 32, stdin);
            for(int i=0; i<32; i++)
            {
                if(username[i] == '\n')
                {
                    username[i] = 0;
                    break;
                }
            }
            for(int i=0; i<32; i++)
            {
                if(password[i] == '\n')
                {
                    password[i] = 0;
                    break;
                }
            }

            write(client_fd, username, sizeof(username));
            read(client_fd, &code, sizeof(int));            // OK
            write(client_fd, password, sizeof(password));
            read(client_fd, &code, sizeof(int));        // status of creation

            if(code == REGISTRATION_SUCCESSFUL)
                printf("Registration Successful\n");
            else 
                printf("Registration Failed !!\n");
        }
        else if(operation_type == 1)
        {   
            if(has_login)
            {
                printf("Already Logged in!!\n");
                continue;
            }
            // login
            int code = LOGIN_REQUEST;
            write(client_fd, &code, sizeof(int));
            
            read(client_fd, &code, sizeof(int));            

            // Taking Input
            char username[32] = {0}, password[32] = {0};
            printf("enter username [<=30 characters]: ");
            fgets(username, 32, stdin);
            fgets(username, 32, stdin);
            printf("enter password [<=30 characters]: ");
            fgets(password, 32, stdin);
            for(int i=0; i<32; i++)
            {
                if(username[i] == '\n')
                {
                    username[i] = 0;
                    break;
                }
            }
            for(int i=0; i<32; i++)
            {
                if(password[i] == '\n')
                {
                    password[i] = 0;
                    break;
                }
            }

            write(client_fd, username, sizeof(username));
            read(client_fd, &code, sizeof(int));
            write(client_fd, password, sizeof(password));
            read(client_fd, &code, sizeof(int));
            if(code == VALID_LOGIN_CREDENTIALS)
            {
                has_login = true;
                printf("Login Successful\n");
            }
            else    
            {
                has_login = false;
                printf("Login Failed !!\n");
            }
            
        }
        else if(operation_type == 2)
        {
            // logout
            if(has_login)
            {
                has_login = false;
                printf("Logout Successful\n");
            }
            else 
            {
                printf("You are not Logged in !!\n");
            }
        }
        else if(operation_type == 3)
        {
            // exit
            int code = CLOSE_REQUEST;
            write(client_fd, &code, sizeof(int));
            read(client_fd, &code, sizeof(int));
            if(code == CLOSE_RESPONSE)
                break;
            else
            {
                printf("Breaking out of the loop anyways\n");
                break;
            }
        }
        else if(operation_type == 4)
        {
            // retrive all products
            int code = ALL_PRODUCTS;
            write(client_fd, &code, sizeof(int));
            int num_records;
            read(client_fd, &num_records, sizeof(int));
            printf("num_records: %d\n", num_records);
            for(int i=0; i<num_records; i++)
            {
                int ok = OK;
                write(client_fd, &ok, sizeof(int));
                ProductSchema ps;
                read(client_fd, &ps, sizeof(ProductSchema));
                ps.display();
            }
        }
        else if(operation_type == 5)
        {
            int code = PRODUCT_LESS_THAN;
            write(client_fd, &code, sizeof(int));
            int ok = OK;
            read(client_fd, &ok, sizeof(int));
            printf("enter threshold: ");
            int threshold=-1;
            scanf("%d", &threshold);
            // printf("%d\n", threshold);
            write(client_fd, &threshold, sizeof(int));
            int num_records;
            read(client_fd, &num_records, sizeof(int));
            printf("num_records: %d\n", num_records);
            for(int i=0; i<num_records; i++)
            {
                int ok = OK;
                write(client_fd, &ok, sizeof(int));
                ProductSchema ps;
                read(client_fd, &ps, sizeof(ProductSchema));
                ps.display();
            }

        }
        else if(operation_type == 6)
        {
            if(!has_login)
            {
                printf("You are not Logged in!!\n");
                continue;
            }

            int item_id;
            printf("enter item_id: ");
            scanf("%d", &item_id);
            int quantity;
            printf("enter quantity: ");
            scanf("%d", &quantity);
            if(item_id < 0 || quantity < 0)
            {
                printf("invalid entries!!\n");
            }
            else
                cart[item_id] += quantity;
            // printf("%d, %d\n", item_id, quantity);
        }
        else if(operation_type == 7)
        {
            if(!has_login)
            {
                printf("You are not Logged in!!\n");
                continue;
            }
            // view cart
            printf("Cart {item_id, quantity}\n");
            for(auto p: cart)
            {
                if(p.second)
                    printf("{%d, %d}\n", p.first, p.second);
            }
        }
        else if(operation_type == 8)
        {
            if(!has_login)
            {
                printf("You are not Logged in!!\n");
                continue;
            }
            std::vector<int> item_ids, quantities;
            for(auto p: cart)
            {
                if(p.second)
                {
                    item_ids.push_back(p.first);
                    quantities.push_back(p.second);
                }
            }
            if(item_ids.size() == 0)
            {
                printf("Cart is Empty!!\n");
            }
            else
            {
                int code = CART_CHECKOUT;
                write(client_fd, &code, sizeof(int));
                int ok;
                read(client_fd, &ok, sizeof(int));
                int size = item_ids.size();
                write(client_fd, &size, sizeof(int));
                read(client_fd, &ok, sizeof(int));
                for(int i=0; i<size; i++)
                {
                    write(client_fd, &item_ids[i], sizeof(int));
                    read(client_fd, &ok, sizeof(int));
                    write(client_fd, &quantities[i], sizeof(int));
                    read(client_fd, &ok, sizeof(int));
                }
                write(client_fd, &ok, sizeof(int));
                int status;
                read(client_fd, &status, sizeof(status));
                if(status == CHECKOUT_SUCCESSFUL)
                {
                    printf("Checkout Successful\n");
                    cart.clear();
                }
                else
                {
                    printf("Checkout Failed!!\n");
                }
            }
        }
        else if(operation_type == 9)
        {
            // print menu
            print_menu();
        }

    }
    // write(client_fd, msg, sizeof(msg));
    // read(client_fd, buffer, 128);
    // printf("%s\n", buffer);

    // ------------------------------------------------------ //


    // close
    close(client_fd);
    return 0;
}

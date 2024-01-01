#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <pthread.h>

#include "SocketStoreDB.hpp"
#include "Codes.hpp"

#define PORT 8080

// creating relations
char authdb[] = "AuthDB";
Relation<AuthSchema> auth_relation(11, authdb);

char productdb[] = "ProductDB";
Relation<ProductSchema> product_relation(12, productdb);

// client handler function
void* client_handler(void* new_socket_ptr)
{
    int new_socket = *(int*)new_socket_ptr;
    char buffer[128+1] = {0};
    char msg[] = "hello from server";
    
    // -----------------read write block--------------------- //
    
    int code;
    while(true)
    {
        read(new_socket, &code, sizeof(int));
        if(code == CLOSE_REQUEST)
        {
            int res_code = CLOSE_RESPONSE;
            write(new_socket, &res_code, sizeof(int));
            break;
        }
        else if(code == LOGIN_REQUEST)
        {
            int res_code = OK;
            write(new_socket, &res_code, sizeof(int));

            char username[32] = {0}, password[32] = {0};
            read(new_socket, username, sizeof(username));
            write(new_socket, &res_code, sizeof(int));  // OK
            read(new_socket, password, sizeof(password));
            printf("username: %s\n", username);
            printf("password: %s\n", password);

            // try finding the user
            AuthSchema_Filter_Verifier verifier(username, password);
            Collection<AuthSchema> collection;
            auth_relation.retrieve_record(&verifier, &collection);

            if(collection.count() == 0)
                res_code = INVALID_LOGIN_CREDENTIALS;
            else
                res_code = VALID_LOGIN_CREDENTIALS;
            
            // DONE
            write(new_socket, &res_code, sizeof(int));  // OK -- change to status
        }

        else if(code == REGISTRATION_REQUEST)
        {
            int res_code = OK;
            write(new_socket, &res_code, sizeof(int));

            char username[32] = {0}, password[32] = {0};
            read(new_socket, username, sizeof(username));
            write(new_socket, &res_code, sizeof(int));  // OK
            read(new_socket, password, sizeof(password));
            printf("new username: %s\n", username);
            printf("new password: %s\n", password);

            // checking if the username exists
            AuthSchema_Filter_FindUser finduser(username);
            Collection<AuthSchema> collection;
            auth_relation.retrieve_record(&finduser, &collection);
            if(collection.count())
                res_code == REGISTRATION_UNSUCCESSFUL;
            else 
            {
                AuthSchema user(0, true, username, password);
                auth_relation.create_record(&user);
                auth_relation.commit();
                res_code = REGISTRATION_SUCCESSFUL;
            }
            write(new_socket, &res_code, sizeof(int)); // OK FOR NOW, change to the addition status
        }

        else if(code == ALL_PRODUCTS)
        {
            // write all products one by one.
            Filter_Allower<ProductSchema> allower;
            Collection<ProductSchema> collection;
            product_relation.retrieve_record(&allower, &collection);
            int num_records = collection.count();
            write(new_socket, &num_records, sizeof(int));
            printf("num_records: %d\n", num_records);
            
            ProductSchema_Applier_Sender sender(new_socket);
            collection.iterate(&sender);
        }

        else if(code == PRODUCT_LESS_THAN)
        {
            int ok = OK;
            write(new_socket, &ok, sizeof(int));
            int threshold;
            read(new_socket, &threshold, sizeof(int));

            ProductSchema_Filter_LessThan thresher(threshold);
            Collection<ProductSchema> collection;
            product_relation.retrieve_record(&thresher, &collection);
            int num_records = collection.count();
            write(new_socket, &num_records, sizeof(int));
            ProductSchema_Applier_Sender sender(new_socket);
            collection.iterate(&sender);
        }

        else if(code == CART_CHECKOUT)
        {
            int ok = OK;
            write(new_socket, &ok, sizeof(int));
            int size;
            read(new_socket, &size, sizeof(int));
            write(new_socket, &ok, sizeof(int));
            int ids[size], q[size];
            int flag = true;
            for(int i=0; i<size; i++)
            {
                int item_id;
                read(new_socket, &item_id, sizeof(int));
                write(new_socket, &ok, sizeof(int));// ok
                int quantity;
                read(new_socket, &quantity, sizeof(int));
                ProductSchema_Filter_QuantityCheck qc(item_id, quantity);
                Collection<ProductSchema> collection;
                product_relation.retrieve_record(&qc, &collection);
                write(new_socket, &ok, sizeof(int));// ok
                ids[i] = item_id;
                q[i] = quantity;
                if(collection.count() == 0)
                {
                    flag = false;
                    continue;
                }
                
            }
            read(new_socket, &ok, sizeof(int));

            int status = CHECKOUT_SUCCESSFUL;
            if(flag)
            {
                for(int i=0; i<size; i++)
                {
                    ProductSchema_Filter_QuantityCheck qc(ids[i], q[i]);
                    ProductSchema_Modifier_CartDeduction cd(ids[i], q[i]);
                    product_relation.update_record(&qc, &cd);
                }   
                product_relation.commit();
            }
            else{
                status = CHECKOUT_FAILURE;
            }
            printf("DEBUG\n");
            write(new_socket, &status, sizeof(int));
        }
    }
    
    // read(new_socket, buffer, 128);
    // printf("%s\n", buffer);
    // write(new_socket, msg, sizeof(msg));

    // ------------------------------------------------------ //

    // close
    close(new_socket);

    pthread_detach(pthread_self());
    return NULL;
}


int main()
{
    // connect the databases
    auth_relation.connect();
    product_relation.connect();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        printf("Socket Failure\n");
        return 0;
    }   


    int opt = 1;
    int sso = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if(sso)
    {
        printf("SetSocketOpt Failure\n");
        return 0;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

    int bind_val = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if(bind_val < 0)
    {
        printf("Bind Failure\n");
        return 0;
    }

    int listen_val = listen(server_fd, 3);
    if(listen_val < 0)
    {
        printf("Listen Failure\n");
        return 0;
    }

    while(true)
    {
        socklen_t addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if(new_socket < 0)
        {
            printf("Accept Failure\n");
            break;
        }
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, (void*)&new_socket);

    }

    // disconnect
    auth_relation.disconnect();
    product_relation.disconnect();

    // wait for all threads to complete
    pthread_exit(NULL);
    close(server_fd);
    return 0;
}


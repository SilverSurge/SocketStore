#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <pthread.h>


#define PORT 8080

void* client_handler(void* new_socket_ptr)
{
    int new_socket = *(int*)new_socket_ptr;
    char buffer[128+1] = {0};
    char msg[] = "hello from server";
    
    // -----------------read write block--------------------- //
    read(new_socket, buffer, 128);
    printf("%s\n", buffer);
    write(new_socket, msg, sizeof(msg));

    // ------------------------------------------------------ //

    // close
    close(new_socket);

    pthread_detach(pthread_self());
    return NULL;
}


int main()
{
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

    // wait for all threads to complete
    pthread_exit(NULL);
    close(server_fd);
    return 0;
}


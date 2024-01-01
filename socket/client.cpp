// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

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

    write(client_fd, msg, sizeof(msg));
    read(client_fd, buffer, 128);
    printf("%s\n", buffer);

    // ------------------------------------------------------ //


    // close
    close(client_fd);
    return 0;
}

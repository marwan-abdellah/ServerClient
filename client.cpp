#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc , char** argv)
{
    // The application socket that connects the client with the server
    int appSocket;

    // Server address
    struct sockaddr_in serverAddress;

    // Messages
    char message[1000];
    char serverReply[2000];
     
    // Create the clinet socket
    appSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (appSocket == -1)
        std::cout << "Could not create the client-side socket";
    else
        std::cout << "Client socket has been successfully created";

    // Fill in the address of the server
    // TODO: Change the hardcoded port number and the local host
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( 8888 );
 
    // Connect to remote server
    int errorCode = connect(appSocket,
                            (struct sockaddr *)&serverAddress,
                            sizeof(serverAddress));
    if (errorCode < 0)
    {
        std::cout << "Connection failed. Error, application terminates" << std::endl;
        return 1;
    }
    else
        std::cout << "Client is connected to the server" << std::endl;
     
    // Keep communicating with server until you drop the client
    while(1)
    {
        printf("Enter message to be sent to the server : ");
        scanf("%s" , message);
         
        // Send the message
        errorCode = send(appSocket, message, strlen(message), 0);
        if(errorCode < 0)
        {
            std::cout << "Sending the message failed" << std::endl;
            return 1;
        }
         
        // Receive a reply from the server
        errorCode = recv(appSocket , serverReply , 2000 , 0);
        if(errorCode < 0)
        {
            std::cout << "Receving the message from the server failed, Exiting" << std::endl;
            break;
        }
        else
        {
            std::cout << "Server reply : " << serverReply << std::endl;
        }
    }

    // If the loop terminates, then close the socket
    close(appSocket);
    return 0;
}

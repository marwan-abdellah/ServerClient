#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Shared.h"

#define ARRAY_SIZE 8


//the thread function
void *connectionHandler(void *);

int main(int argc , char *argv[])
{
    // Sokcet descriptor Id
    int socketDescriptor;

    // Client socket
    int clientSocket;

    // Size of the address structure
    int sizeAddress;

    // Fresh socket to be connected once a clinet is connected
    int* freshSocket;

    // Internet socket addresses for the server and the clinet
    struct sockaddr_in serverAddress , clientAddress;

    // Create an initial socket
    socketDescriptor = socket(AF_INET, SOCK_STREAM , 0);

    // If the socket fails, inform the user
    if (socketDescriptor == -1)
        std::cout << "Could not create the socket" << std::endl;
    else
        std::cout << "The initial socket has been successfully created" << std::endl;

    // Prepare the socket address structure
    // TODO: Change the fixed port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons( 8888 );

    // Bind the socket to the described address
    int errorCode = bind(socketDescriptor,
                         (struct sockaddr *)&serverAddress,
                         sizeof(serverAddress));
    if(errorCode < 0)
    {

        std::cout << "Binding the socket failed. Error, application will exit"
                  << std::endl;
        return 1;
    }
    else
        std::cout << "Binding the socket has been successfully done"
                  << std::endl;

    // Listen to any clinets
    // Prepare to accept connections on socket FD
    errorCode = listen(socketDescriptor , 3);

    if (errorCode < 0)
        std::cout << "Error happened during listening" << std::endl;
    else
    {
        // Accept and incoming connection
        std::cout << "Waiting for incoming connections ..." << std::endl;
    }

    sizeAddress = sizeof(struct sockaddr_in);

    // Check for the connection
    // Establish the connection
    while((clientSocket = accept(socketDescriptor,
                                 (struct sockaddr *)&clientAddress,
                                 (socklen_t*)&sizeAddress)))
    {
        std::cout << "The connection has been accepted" << std::endl;
        std::cout << "Creating a thread for this connection" << std::endl;

        // Create a new thread for this connection
        pthread_t connectionThread;

        // This socket is the client socket
        freshSocket = (int*) malloc(sizeof(int*));
        *freshSocket = clientSocket;

        // Create the thread
        errorCode = pthread_create(&connectionThread, NULL, connectionHandler,
                                   (void*) freshSocket);
        if(errorCode < 0)
        {
            std::cout << "Could not create a thread specific to the connection" << std::endl;
            return 1;
        }

        // Now join the thread , so that we dont terminate before the thread
        // pthread_join( connectionThread , NULL);
        std::cout << "Handler assigned" << std::endl;
    }

    // If the socket is not connected, FAIL.
    if (clientSocket < 0)
    {
        std::cout << "Accepting the connetion was failed" << std::cout;
        return 1;
    }

    return 0;
}



void display()
{
    /// this is the display thread,
    /// switch between this thread and the collector threads to make it
}


/// This will handle connection for each client
/// This is a threded function
void *connectionHandler(void *socketDescriptor)
{
    // Get the socket descriptor
    int socket = *(int*)socketDescriptor;

    // The size of the read message
    int readSize;


    float fMsg[ARRAY_SIZE];

    // Receive a message from clientAddress
    // Stay in this loop as long as you read a valid string

    while((readSize = recv(socket , fMsg , sizeof(float) * ARRAY_SIZE, 0)) > 0)
    {
        std::cout << "Read size" << readSize << std::endl;
        for (int i = 0; i < ARRAY_SIZE ; i++)
            std::cout << fMsg[i] << " ";


        // Fill the array
        // wait until the array is filled
        // send back to the clinet to resume
        // The server is waiting for me so write it


        // display thread
        // display();

        int commMessgae = 1;
        write(socket, &commMessgae, sizeof(int));
    }

    if(readSize == 0)
    {
        std::cout << "Client disconnected" << std::endl;
        fflush(stdout);
    }
    else if(readSize == -1)
    {
        std::cout << "Receiving failed" << std::endl;
    }

    // Free the socket pointer
    free(socketDescriptor);

    return 0;
}

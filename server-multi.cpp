#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include "Shared.h"

// Array size
#define ARRAY_SIZE 8

// A vector containing all the threads
// Keep all the threads in this table
std::vector <pthread_t> threadTable;

// Use these cline acknowledgments to synchronize between
// the collecting threads and the draw() thread.
int clinetAck_0 = 0;
int clinetAck_1 = 0;
int clinetAck_2 = 0;
int clinetAck_3 = 0;

// Mutex for the final image
pthread_mutex_t imageMutex;

// Condition variables for the Producer and the Consumer
pthread_cond_t drawCond;
pthread_cond_t clinetCond_1;
pthread_cond_t clinetCond_2;
pthread_cond_t clinetCond_3;
pthread_cond_t clinetCond_4;

float image[32];

// the thread function
void *connectionHandler(void *);


// Create the draw thread
pthread_t drawThread;


void *display(void*);

void *display(void* ptr)
{
    std::cout << "Display " << std::endl;

    // Lock the image array until the thread writes its corresponding
    // part into it
    pthread_mutex_lock(&imageMutex);

    // If the array is ready, then display its contents
    if (clinetAck_0 == 1 && clinetAck_1 == 1
            && clinetAck_2 == 1 && clinetAck_3 == 1)
    {
        for (int i = 0; i < 32; i++)
            std::cout << image[i];
        std::cout << std::endl;

        // Reset all the clinet acknowledgments for the next
        // iteration in the rendering loop
        clinetAck_0 = 0;
        clinetAck_1 = 0;
        clinetAck_2 = 0;
        clinetAck_3 = 0;

        for (int i = 0; i < 32; i++)
            image[i] = 0;
    }

    // resume the collecting threads

    // signal all the threads (for loop)
    // unlock the resource

    pthread_cond_signal(&clinetCond_1);
    pthread_cond_signal(&clinetCond_2);
    pthread_cond_signal(&clinetCond_3);
    pthread_cond_signal(&clinetCond_4);

    // Unlock the image array
    pthread_mutex_unlock(&imageMutex);


}

int main(int argc , char *argv[])
{
    // Initialize the image mutex
    pthread_mutex_init(&imageMutex, NULL);

    // Create the drawing thread
    pthread_create(&drawThread, NULL, display, NULL);

    // Initialize, the condition variables
    pthread_cond_init(&clinetCond_1, NULL);
    pthread_cond_init(&clinetCond_2, NULL);
    pthread_cond_init(&clinetCond_3, NULL);
    pthread_cond_init(&clinetCond_4, NULL);
    pthread_cond_init(&drawCond, NULL);

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

        // Add the thread to the thread table
        threadTable.push_back(connectionThread);

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

    // Destroy the image mutex
    pthread_mutex_destroy(&imageMutex);

    // Free up Consumer condition variable
    pthread_cond_destroy(&clinetCond_1);
    pthread_cond_destroy(&clinetCond_2);
    pthread_cond_destroy(&clinetCond_3);
    pthread_cond_destroy(&clinetCond_4);
    pthread_cond_destroy(&drawCond);

    return 0;
}

/// This will handle connection for each client
/// This is a threded function
void *connectionHandler(void *socketDescriptor)
{
    // Get the socket descriptor
    int socket = *(int*)socketDescriptor;

    // The size of the read message
    int readSize;


    Packet packet;

    // Receive a message from clientAddress
    // Stay in this loop as long as you read a valid string

    while((readSize = recv(socket , &packet , sizeof(Packet), 0)) > 0)
    {


        // lock the resource
        // make the display function wait
        // write to the array
        // make the flag that proves it have been written to 1



        // Lock the image array until the thread writes its corresponding
        // part into it
        pthread_mutex_lock(&imageMutex);

        // Wait until you handle the image array
        // pthread_cond_wait(&drawCond, &imageMutex);

        // std::cout << "Read size" << readSize
           //       <<  " from " <<  packet.clinetNumber << std::endl;



        int client = int (packet.clinetNumber);
        switch (client)
        {
        case 1:
            clinetAck_0 = 1;

            for (int i = 0; i < ARRAY_SIZE ; i++)
                image[i] = packet.imageData[i];
            break;
        case 2:
            clinetAck_1 = 1;

            for (int i = 0; i < ARRAY_SIZE ; i++)
                image[i + ARRAY_SIZE] = packet.imageData[i];
            break;
        case 3:
            clinetAck_2 = 1;

            for (int i = 0; i < ARRAY_SIZE ; i++)
                image[i + (2 * ARRAY_SIZE)] = packet.imageData[i];
            break;
        case 4:
            clinetAck_3 = 1;

            for (int i = 0; i < ARRAY_SIZE ; i++)
                image[i + (3 * ARRAY_SIZE)] = packet.imageData[i];
            break;
        default:
            break;
        }

        // Fill the array
        // wait until the array is filled
        // send back to the clinet to resume
        // The server is waiting for me so write it


        // Signal the draw() thread
        pthread_cond_signal(&drawCond);

        // Unlock the image array
        pthread_mutex_unlock(&imageMutex);



        // display thread
        // try the display thread if it can works
        void* ptr;
        display(ptr);

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

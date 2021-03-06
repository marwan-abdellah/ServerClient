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
#include <stdlib.h>
#include <stdio.h>

#include "Shared.h"

#define ARRAY_SIZE 8

int main(int argc , char** argv)
{
    int sCount = 1;
    if (argc < 2)
    {
        std::cout << "Enter a clinet number " << std::endl;
        return 0;
    }

    // Identify the client
    float clinetValue = atof(argv[1]);

    std::cout << "Clinet : " << clinetValue << std::endl;

    // The application socket that connects the client with the server
    int appSocket;

    // Server address
    struct sockaddr_in serverAddress;

    // Messages
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


    Packet packet;
     
    int replyMessage = 0;
    // Keep communicating with server until you drop the client
    while(1)
    {

        sleep(replyMessage);

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            packet.imageData[i] = i + clinetValue;
        }

        packet.clinetNumber = clinetValue;


        /// Get the image
        ///
        /// Send the image wait for the receiver to send back move on and the angle
         
        // Send the message
        errorCode = send(appSocket, &packet, sizeof(Packet), 0);

        if(errorCode < 0)
        {
            std::cout << "Sending the message failed" << std::endl;
            return 1;
        }
         
        // Receive a reply from the server
        errorCode = recv(appSocket , &replyMessage , sizeof(int), 0);

        if(errorCode < 0)
        {
            std::cout << "Receving the message from the server failed, Exiting" << std::endl;
            break;
        }
        else
        {
            std::cout << "Server reply : " << replyMessage << std::endl;

            sCount++;
        }
    }

    // If the loop terminates, then close the socket
    close(appSocket);
    return 0;
}

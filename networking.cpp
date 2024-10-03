#include <iostream>
#include <fcntl.h> //fcntl
#include <stdio.h>
#include <cstring>
#include <unistd.h>     // close()
#include <arpa/inet.h>  // for inet_pton and inet_ntop
#include <sys/socket.h> // socket functions
#include <vector>
#include <errno.h> // errno values
#include "networking.h"
#include "contactList.h"

#define TIMEOUT_SEC 10
// extern std::vector<PeerInfo> contactList;
// extern std::map<std::string, PeerInfo> contactList;

//******************************** */

bool pingPeer(const std::string &ip, int port, int &sock)
{
    // Simulate sending a ping to the peer's IP and port and waiting for a response
    // Return true if the peer responds, false otherwise

    // Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return false;
    }

    // Set up the peer address structure
    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr) <= 0)
    {
        perror("Invalid or unsupported IP address");
        close(sock);
        return false;
    }

    // Set the socket to non-blocking mode for timeout handling
    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("Failed to set non-blocking mode");
        close(sock);
        return false;
    }

    // Attempt to connect to the peer
    int connResult = connect(sock, (struct sockaddr *)&peerAddr, sizeof(peerAddr));
    if (connResult < 0 && errno != EINPROGRESS)
    { // If connect fails and isn't in progress
        perror("Connection failed");
        close(sock);
        return false;
    }

    // Set up the file descriptor set for select()
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    // Wait for the socket to become writable (meaning the connection is established)
    int selectResult = select(sock + 1, NULL, &writeSet, NULL, &timeout);
    if (selectResult <= 0)
    { // Timeout or error
        if (selectResult == 0)
        {
            std::cerr << "Ping timeout.\n";
        }
        else
        {
            perror("Select failed"); // what is this exactly?
        }
        close(sock);
        return false;
    }

    // Send the "Are you available to chat?" message
    std::string pingMessage = "Are you available to chat? Yes or No?\n";

    if (send(sock, pingMessage.c_str(), pingMessage.size(), 0) < 0)
    {
        perror("Failed to send ping message");
        close(sock);
        return false;
    }

    // Wait for a response from the peer (using the same timeout mechanism)
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);
    selectResult = select(sock + 1, &writeSet, NULL, NULL, &timeout);
    if (selectResult <= 0)
    { // Timeout or error
        if (selectResult == 0)
        {
            std::cerr << "No response from peer.Timing out.\n";
        }
        else
        {
            perror("Select failed while waiting for response");
        }
        close(sock);
        return false;
    }

    // Receive the response
    char buffer[1024] = {0};
    if (recv(sock, buffer, sizeof(buffer), 0) < 0)
    {
        perror("Failed to receive response");
        close(sock);
        return false;
    }

    std::string response(buffer); // response is initialised with the contents of the c-style buffer array

    // Check the response
    if (response == "Yes")
    {
        std::cout << "Peer is available for chat.\n";
        return true;
    }
    else
    {
        std::cout << "Peer is not available for chat.\n";
        close(sock);
        return false;
    }
}

void startListening(const std::string &ip, int port)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address); // in bytes

    // creat a socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creatioon failed");
        exit(EXIT_FAILURE);
    }

    // address structure to be passed to bind()
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // convert ip address from presentation(p) to binary(n)

    int binary_ip = inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
    if (binary_ip <= 0)
    {
        perror("invalid or unsupported ip address");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // bind socket to ip and port
    int bound_socket_fd = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if (bound_socket_fd < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        // 3 connections queued before blocking any further connections
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket setup complete. Listening on IP " << ip << " and port " << port << "...\n";

    // accept incoming connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    auto peerIp = inet_ntoa(address.sin_addr);
    auto peerPort = ntohs(address.sin_port);
    std::cout << "Peer connected from IP :" << peerIp << "and port" << peerPort << "\n";
    // let us find the name of the peer connecting
    std::string peerName = "Unknown";
    ; // default in case of non-match
    for (const auto &contact : contactList)
    {
        if (contact.second.ip == peerIp && contact.second.port == peerPort)
        {
            peerName = contact.first;
            break;
        }
    }

    handleMessaging(new_socket, peerName);
    // Handle incoming ping message
    // char buffer[1024]={0};
    // recv(new_socket,buffer,sizeof(buffer),0);
    // cout<<"Received message:"<< buffer<<"\n";
    // //closing sockets after connection
    // close(new_socket);
    close(server_fd);
}

void handleMessaging(int sock, const std::string &peerName)
{
    // function to handle messaging once the peer says yes
    cout << "You and" << peerName << "can start chatting now\n";
    cout << "Type 'exit' to end the chat";

    while (true)
    {
        // the loop goes on as long as exit is typed
        // or messages arent sent nor are they being received
        // user types a message

        std::string userMessage;
        std::cout << "You: ";
        std::getline(std::cin, userMessage);

        // Exit if and when the user types 'Exit'

        if (userMessage == "exit")
        {
            cout << "Ending chat with" << peerName << ".\n";
            break;
        }

        // send the message to the peer
        int sendFlag = send(sock, userMessage.c_str(), userMessage.size(), 0);
        if (sendFlag < 0)
        {
            perror("Failed to send message");
            break;
        }

        // Wait and receive a message from the peer

        char buffer[1024] = {0}; // message is assumed to fit 1kb, roughly 900 characters
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            if (bytesReceived == 0)
            {
                std::cout << "Peer " << peerName << "disconnected.\n";
            }
            else
            {
                perror("Failed to receive message from peer");
            }

            break;
        }
        // Display the peer's message
        std::string peerMessage(buffer, bytesReceived); // Dynamic message size
        cout << peerName << ": " << peerMessage << "\n";
    }
    // closing the socket to mark the end of the chat
    close(sock);
}
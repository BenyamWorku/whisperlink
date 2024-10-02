#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>
#include <errno.h>
#include "networking.h"
#include <contactList.h>

#define TIMEOUT_SEC 5
extern std::vector<PeerInfo> contactList;
void createTcpConnection(){
    int server_fd , new_socket;
    struct sockaddr_in address;
    int addrlen=sizeof(address);
    PeerInfo myInfo=contactList["me"];
    std::string ip =myInfo.ip;
    int port = myInfo.port;
    //creat a socket file descriptor
    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1){
        perror("Socket creatioon failed");
        exit(EXIT_FAILURE);
    }

    // address structure to be passed to bind()
    address.sin_family=AF_INET;
    address.sin_port=htons(port);

    // convert ip address from presentation(p) to binary(n)

   int binary_ip=inet_pton(AF_INET,ip.c_str(),&address.sin_addr);
   if(binary_ip <=0) {
    perror("invalid or unsupported ip address");
    exit(EXIT_FAILURE);

   }

   // bind socket to ip and port 
   int bound_socket_fd=bind(server_fd,(struct sockaddr*)&address,sizeof(address));
   if(bound_socket_fd < 0){
    perror("Bind failed");
    exit(EXIT_FAILURE);
   }

   // listen for incoming connections
    if(listen(server_fd,3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket setup complete. Listening on IP " << ip << " and port " << port << "...\n";

 // accept incoming connection
   if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Peer connected!\n";

    //closing sockets after connection
    close(new_socket);
    close(server_fd);
}

bool pingPeer(const std::string& ip, int port) {
    // Simulate sending a ping to the peer's IP and port and waiting for a response
    // Return true if the peer responds, false otherwise
    
    // Create a TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return false;
    }

    // Set up the peer address structure
    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr) <= 0) {
        perror("Invalid or unsupported IP address");
        close(sock);
        return false;
    }

    // Set the socket to non-blocking mode for timeout handling
    fcntl(sock, F_SETFL, O_NONBLOCK);

    // Attempt to connect to the peer
    int connResult = connect(sock, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
    if (connResult < 0 && errno != EINPROGRESS) {  // If connect fails and isn't in progress
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
    if (selectResult <= 0) {  // Timeout or error
        if (selectResult == 0) {
            std::cerr << "Ping timeout.\n";
        } else {
            perror("Select failed");
        }
        close(sock);
        return false;
    }

    // Send the "Are you available to chat?" message
    std::string pingMessage = "Are you available to chat?";
    if (send(sock, pingMessage.c_str(), pingMessage.size(), 0) < 0) {
        perror("Failed to send ping message");
        close(sock);
        return false;
    }

    // Wait for a response from the peer (using the same timeout mechanism)
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);
    selectResult = select(sock + 1, &writeSet, NULL, NULL, &timeout);
    if (selectResult <= 0) {  // Timeout or error
        if (selectResult == 0) {
            std::cerr << "No response from peer.\n";
        } else {
            perror("Select failed while waiting for response");
        }
        close(sock);
        return false;
    }

    // Receive the response
    char buffer[1024] = {0};
    if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
        perror("Failed to receive response");
        close(sock);
        return false;
    }

    std::string response(buffer);
    close(sock);

    // Check the response
    if (response == "Yes") {
        std::cout << "Peer is available for chat.\n";
        return true;
    } else {
        std::cout << "Peer is not available for chat.\n";
        return false;
    }
}

void switchtoListenerMode(){

}
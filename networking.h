#pragma once

bool pingPeer(const std::string& ip, int port,int& sock);

void startListening(const std::string& ip , int port);

void handleMessaging(int sock,const std::string& peerName);

void setReuse(int socket_fd);
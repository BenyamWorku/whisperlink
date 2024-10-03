#pragma once

void startListening(const std::string& ip , int port);
bool pingPeer(const std::string& ip, int port,int& sock);
void handleMessaging(int sock,const std::string& peerName);
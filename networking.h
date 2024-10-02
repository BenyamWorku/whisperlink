#pragma once

void createTcpConnection();
bool pingPeer(const std::string& ip, int port);
void switchtoListenerMode();
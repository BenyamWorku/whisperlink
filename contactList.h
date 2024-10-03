#pragma once
#include <string>
#include <map>
using namespace std;
struct PeerInfo
{
    string name;
    string ip;
    int port;
};

extern std::map<std::string, PeerInfo> contactList;
void displayContacts(const std::string &currentPeer);

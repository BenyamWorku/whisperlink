#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "contactList.h"
using namespace std;

// store peer info in a struct

map<string, PeerInfo> contactList = {
    {"alpha", {"alpha", "127.0.0.1", 10000}},
    {"beta", {"beta", "127.0.0.1", 20000}},
    {"gamma", {"gamma", "172.20.10.3", 30000}},// gamma is my vm
    {"alpha_2", {"alpha2", "127.0.0.1", 40000}} // handle name similarities later

};

void displayContacts(const std::string &currentPeer)
{
    cout << "\n--- Contact List ---\n";
    int index = 0;
    for (const auto &contact : contactList)
    {
        if (contact.first == currentPeer)
        {
            continue; // filtering out current user
        }
        cout << index + 1 << ":" << "Peer: " << contact.first << " - IP: " << contact.second.ip << ", Port: " << contact.second.port << endl;
        ++index;
    }
}


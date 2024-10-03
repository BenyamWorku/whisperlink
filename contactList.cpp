#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "contactList.h"
using namespace std;

// store peer info in a struct

// struct PeerInfo {
//     string name;
//     string ip;
//     int port;

// };

// create a unique string key from ip and port

// string createKey(const string &ip ,int port) {
//     stringstream ss;
//     ss<<ip<<":"<<port;
//     return ss.str();
// }

// ports will be assigned dynamically unless peer provides it at runtime
map<string, PeerInfo> contactList = {
    // {"me",{"me","127.0.0.1",7000}},
    {"alpha", {"alpha", "127.0.0.1", 0}},
    {"beta", {"beta", "127.0.0.1", 0}},
    {"gamma", {"gamma", "127.0.0.1", 0}},
    {"alpha_2", {"alpha2", "127.0.0.1", 0}} // handle name similarities later

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

// {createKey("127.0.0.1",2000),{"Alpha","127.0.0.1",2000}},
// {createKey("127.0.0.1",2001),{"Beta","127.0.0.1",2001}},
// {createKey("127.0.0.1",2002),{"Gamma","127.0.0.1",2002}},
// {createKey("127.0.0.1",2003),{"Alpha","127.0.0.1",2003}}
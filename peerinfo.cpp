#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


//store peer info in a struct

struct PeerInfo {
    string name;
    string ip;
    int port;
    
};

// create a unique string key from ip and port 

string createKey(const string &ip ,int port) {
    stringstream ss;
    ss<<ip<<":"<<port;
    return ss.str();
}

unordered_map<string,PeerInfo> contactList={
{createKey("192.168.1.101",2000),{"Alpha","192.168.1.101",2000}},
{createKey("192.168.1.102",2001),{"Beta","192.168.1.102",2001}},
{createKey("192.168.1.103",2002),{"Gamma","192.168.1.103",2002}},
{createKey("192.168.1.104",2003),{"Alpha","192.168.1.104",2003}}

};
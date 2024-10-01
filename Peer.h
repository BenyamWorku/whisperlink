#ifndef PEER_H
#define PEER_H
// or instead of the header guards use #Pragma once
#include <string>

class Peer {
    public:
    Peer(const std::string &ip); //constructor
    // member functions

    private:
    std::string ip_;
    //member variables
};

#endif 
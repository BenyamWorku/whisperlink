#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include<arpa/inet.h>
#include <sys/socket.h>
#include "Peer.h"
#include "Utils/Encryption.h" 
#include "peerinfo.h"

using namespace std;

int main() {
//    int socket_desc ,client_sock,client_size;
//    Peer peer_a("127.0.0.1");
//    Peer peer_b("172.20.10.5");
    extern unordered_map<string,PeerInfo> contactList;
    int menuOption=0;
    string selectedPeer;
    while (menuOption!=3) {
        cout<<"#n --- Main Menu ---\n";
        cout<<"1. Create Connection\n";
        cout<<"2.Show Contact List\n";
        cout<<"3.Exit\n";

        cout<<"Select an option 1 or 2 or 3 \n";

        switch(menuOption) {
            case 1:
                selectedPeer=selectPeerByName();
                pingPeer(selectedPeer);
                break;
            case 2:
                displayContacts();
                break;
            case 3:
                cout<<"Exiting ...\n";
                break;
            default:
                cerr<<"Invalid Option.Please select agin\n";

        }
        
    }
    return 0;
}

#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Peer.h"
#include "Utils/Encryption.h" 
#include "contactList.h"
#include "networking.h"

using namespace std;

// Function declarations
void checkForAvailablePeers();
string convert2Lower(std::string somestring);
bool pingPeer(const std::string& ip, int port, int& somesocket);
//void startListening(std::string &ip, int port);
extern map<string, PeerInfo> contactList;

//********************* *//
 int sock = -1 ;
int main() {
    // this sock will be passed to pingPeer which will modify it and pass it onto handleMessaging
   
    int menuOption = 0;
    string selectedPeer;

    while (menuOption != 3) {
        cout << "\n--- Main Menu ---\n";
        cout << "1. Check for available peers\n";
        cout << "2. Show Contact List\n"; // Menu to select a peer and start talking, or just exit
        cout << "3. Exit\n";
        cout << "Select an option (1, 2, or 3): ";
        cin >> menuOption;

        switch (menuOption) {
            case 1:
                checkForAvailablePeers(); // Checks the network for peers and notifies the user
                break;
            case 2:
                displayContacts();
                cout << "\nSelect a peer by name  (or 0 to exit): ";
                cin >> selectedPeer;
                selectedPeer=convert2Lower(selectedPeer);

                if (selectedPeer != "0" && contactList.find(selectedPeer) != contactList.end()) {
                    // Handle the chat initiation with the selected peer
                    
                    if (pingPeer(contactList[selectedPeer].ip, contactList[selectedPeer].port,sock)) {
                        cout << "Chat with " << selectedPeer << " has started...\n";
                        // Start the chat logic here
                        handleMessaging(sock,selectedPeer);
                    } else {
                        cout << "Peer " << selectedPeer << " is not available.\n";
                        //startListening(myip,myport)
                    }
                }
                break;
            case 3:
                cout << "Exiting ...\n";
                break;
            default:
                cerr << "Invalid Option. Please select again.\n";
                break;
        }
    }
    return 0;
}

// This function checks if any peers are available to chat
void checkForAvailablePeers() {
    bool peerAvailable = false;

    for (const auto& contact : contactList) {
        //pingPeer(contact.second.ip, contact.second.port)
        if (pingPeer(contact.second.ip, contact.second.port,sock)) {  // Simulate pinging a peer
            cout << "Peer " << contact.first << " is available for chat." << endl;
            peerAvailable = true;
        }
    }

    if (!peerAvailable) {
        cout << "No peers are available for chat. Starting to listen for incoming connections...\n";
        // Start socket using own ip and port  and listen for incoming connections
        PeerInfo myInfo=contactList["me"];
        std::string ip =myInfo.ip;
        int port = myInfo.port;
        startListening(ip,port);  
    }
}

 std::string convert2Lower(std::string givenString){
   
    for(char &c: givenString){
        c=tolower(c);
    }
   
    return givenString;
}


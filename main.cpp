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
#include <unistd.h>

using namespace std;

// Function declarations
void checkForAvailableListeners(const string &currentPeer);
string convert2Lower(std::string somestring);
bool pingPeer(const std::string &ip, int port, int &somesocket);
// void setDynamicPort(const std::string &peerName, int dynamicPort);
// int getDefaultPort(const string &peerName);
extern map<string, PeerInfo> contactList;

//********************* *//
int sock = -1; // this sock will be passed to pingPeer which will modify it and pass it onto handleMessaging

int main(int argc, char *argv[])
{
    // check if peerName is supplied
    if (argc < 2)
    {
        cerr << "Usage: whisperlink <name> [port]\n"; // name isnt optional
        return 1;
    }
    // getting peer name
    std::string peerName = argv[1];

    
    // Main menu
    int menuOption = 0;
    std::string selectedPeer;

    while (menuOption != 3)
    {
        cout << "\n--- Main Menu ---\n";
        cout << "1. Listen for Incoming Connections\n";
        cout << "2. Find and ping a Peer to Chat \n"; // Menu to select a peer and start talking, or just exit
        cout << "3. Exit\n";
        cout << "Select an option (1, 2, or 3): ";
        cin >> menuOption;

        switch (menuOption)
        {
        case 1:
            checkForAvailableListeners(peerName); // Checks the network for peers and notifies the user
            break;
        case 2:
            displayContacts(peerName);
            cout << "\nSelect a peer by name  (or 0 to exit):\n ";
            cin >> selectedPeer;
            selectedPeer = convert2Lower(selectedPeer);

            if (selectedPeer != "0" && contactList.find(selectedPeer) != contactList.end())
            {
                // Handle the chat initiation with the selected peer

                if (pingPeer(contactList[selectedPeer].ip, contactList[selectedPeer].port, sock))
                {
                    cout << "Chat with " << selectedPeer << " has started...\n";
                    // Start the chat logic here
                    handleMessaging(sock, selectedPeer);
                }
                else
                {
                    cout << "Peer " << selectedPeer << " is not available.\n";
                    // startListening(myip,myport)
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

void checkForAvailableListeners(const string &currentPeer)
{
    bool listenerFound = false;
    string availablePeer;
    // ping all peers except the current peer
    for (const auto &contact : contactList)
    {
        if (contact.first == currentPeer)
        {
            continue; // skip the current peer
        }
        // ping to check if any peer is listening
        if (pingPeer(contact.second.ip, contact.second.port, sock))
        {
            cout << "Peer " << contact.first << " is available and listening.\n";
            availablePeer = contact.first;
            listenerFound = true;
            break;
        }
    }

    if (listenerFound)
    {
        // prompt to ask user if they want a chit chat with the available peer
        string userResponse;
        cout << "Would you like to chat with " << availablePeer << "? (Yes/No): \n";
        cin >> userResponse;
        userResponse = convert2Lower(userResponse);
        if (userResponse == "yes")
        {
            // Start the chat with the available peer
            handleMessaging(sock, availablePeer);
        }
        else
        {
            cout << "You chose not to chat. Returning to the main menu.\n";
            return; // control returns to menu
        }
    }
    else
    {
        // No listener found, start listening for connections
        cout << "Sadly, no available peers.You are now waiting for incoming connections...\n";
        PeerInfo myInfo = contactList[currentPeer];
        startListening(myInfo.ip, myInfo.port);
    }
}

std::string convert2Lower(std::string givenString)
{

    for (char &c : givenString)
    {
        c = tolower(c);
    }

    return givenString;
}


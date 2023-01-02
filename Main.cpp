/*
 * Main.cpp
 * 
 * by Ethan Nanavati
 * last edited: 1/2/23
 *
 * interfaces with the user and implements  Diffie-Hellman encryption
 * 
 */


#include <iostream>
#include <string>
#include <cmath>
#include "SecuredInfo.h"


using namespace std;

int main() {
     
    SecuredInfo Alice;
    
    // this copies only the public values of Alice
    SecuredInfo Bob = Alice;
    string message;
    
    cout << "Please input your 5 or fewer a-z letter message message Bob: ";
    cin >> message; 
    Bob.SelectMessage(message); 
    
    cout << endl << "Transfering the message to Alice..." << endl << endl;
    Alice.mEncrypted = Bob.mEncrypted;
    Alice.Bpublic = Bob.Bpublic;
    Alice.Adecryption();
    
    cout << "Alice thinks the message is: ";
    cout << Alice.get_mDecrypted() << endl;  
    
    return 0;
}
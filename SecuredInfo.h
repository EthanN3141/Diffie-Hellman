/*
 * MetroSim.h
 * 
 *  by Ethan Nanavati 1/2/23
 * implements the MetroSim interface and is responsible for commanding the
 * program
 * 
 */

#ifndef _DIFFIEHELLMAN_H_
#define _DIFFIEHELLMAN_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

class SecuredInfo
{
public:
    SecuredInfo();
    
    // copies the public info from an instance to a new one.
    SecuredInfo(SecuredInfo& s1);
    
    // computations to encode and decode the message
    void Bcomputations();
    void Adecryption();

    // reads the message for Bob and calls Bcomputations
    void SelectMessage(std::string m);
    
    // returns mDecrypted
    std::string get_mDecrypted();
    
    // converts between text and numbers
    long Alphabetical_Decoding(std::string m);
    std::string Alphabetical_Encoding(long m);
        
    // functions that do modular arrethmatic procedures on longs
    long md(long arg, long mod);
    long powerMod(long a, long b, long m);
    long find_inverse(long c, long p);
    
    // functions that help find a prime number
    bool sieve_test(long p);
    bool Rabin_Miller_Test(long p, long iter);
    long find_prime(int digits);
    long find_root(long p);
    
    
    long Bpublic;
    long Apublic;
    long prime;
    long root;
    long mEncrypted;

        
private:
    // various helper functions
    bool Rabin_Miller_Helper(long p);
    long extended_euclidean(long a, long b, long& x, long& y);
    void Base_X_Encoding(std::vector<long>& encoding, long base, long m);
    std::string ArrayToString(std::vector<long>& encoding);
    
    
    long Asecret;
    long Bsecret;
    long cipher;
    long message;
    long decrypter;
    std::string mDecrypted;        
};

#endif

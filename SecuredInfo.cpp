/*
 * SecuredInfo.cpp
 * 
 * by Ethan Nanavati 
 * last edited: 1/2/23
 * Prepares all data necessary for encryption and performs DiffieHellman 
 * encryption
 * 
 */

 #include <vector>
 #include <iostream>
 #include <string>
 #include <cstdlib>
 #include <cmath>

#include "SecuredInfo.h"


using namespace std;

/* Name: constructor
 * purpose: initialize the information for Alice
 * parameters: none
 * returns: none
 * Note: THIS ONLY WORKS FOR DIGITS <= 8 because of the data type. Doing larger 
 * primes will result in undesired behavior. A potential fix is using a vector 
 * to store numbers to enable dynamic allocation for memory (which is the
 * current shortfalling) or using llong or similar types.
 * if there are issues, use digits <= 7 instead. The goal of these restrictions
 * is to make p^2 < LONGMAX.
 */
SecuredInfo::SecuredInfo() {
    prime = find_prime(8);
    root = find_root(prime);
    Asecret = md(rand(), prime);
    Apublic = powerMod(root, Asecret, prime);
}


/* Name: copy constructor
 * purpose: copies an instance of a SecuredInfo but just the public info
 * parameters: the SecuredInfo to be copied
 * returns: none
 * NOTE: THIS IS NOT A TYPICAL COPY CONSTRUCTOR. READ THE PURPOSE.
 */
SecuredInfo::SecuredInfo(SecuredInfo& s1) {
    Bpublic = s1.Bpublic;
    Apublic = s1.Apublic;
    prime = s1.prime;
    root = s1.root;
    mEncrypted = s1.mEncrypted;
}




/* Name: SelectMessage
 * purpose: reads the message for Bob and calls Bcomputations
 * parameters: the message Bob chose
 * returns: none
 */
void SecuredInfo::SelectMessage(string m) {
    message = Alphabetical_Decoding(m);
    
    Bcomputations();
}



/* Name: get_mDecrypted
 * purpose: returns mDecrypted
 * parameters: none
 * returns: mDecrypted
 */
string SecuredInfo::get_mDecrypted() {
    return mDecrypted;
}


/* Name: Alphabetical_Decoding
 * purpose: converts an alphabetical message into a base 10 representation.
 * parameters: the message to be decoded
 * returns: the base 10 representation fo the message
 * Notes: each character is represented by x*27^(distance from left of string)
 * where x is a number from 1 to 26 where a:1, b:2, ... z:26. The distance 
 * from the left of the string starts at 0.
 */
long SecuredInfo::Alphabetical_Decoding(string m) {
    // it goes left to right naturally. ie char 0 is leftmost char in string.
    long encoding = 0;
    long ascii_value = 0;
    long message_length = m.length();
    for (long i = 0; i < message_length; i++) {
        ascii_value = long(m[i]);
        if (ascii_value < 97){
            ascii_value+= 32;
        }
        if ((ascii_value < 97) or (ascii_value > 122)){
            throw invalid_argument("received non alphabetical text");
        }
        else {
            ascii_value = ascii_value - 96;
            encoding+= ascii_value * pow(27, i); 
        }
        
    } 
    return encoding;
}

/* Name: Alphabetical_Encoding
 * purpose: converts a base 10 number into a unique string.
 * parameters: the number to be converted
 * returns: a unique string corresponding to the number passed in
 */
string SecuredInfo::Alphabetical_Encoding(long m) {
    vector<long> encoding;
    
    Base_X_Encoding(encoding, 27, m);

    return ArrayToString(encoding);
}


/* Name: ArrayToString
 * purpose: converts an array of numbers from 1 to 27 into a string
 * parameters: the array of numbers
 * returns: a unique string corresponding to the array of numbers
 */
string SecuredInfo::ArrayToString(vector<long>& encoding) {
    string ogMessage= "";
    long message_length = encoding.size();
    for (long i = message_length - 1; i >= 0; i = i - 1) {
        ogMessage+= char(96 + encoding[i]);
    }
    return ogMessage;
}

/* Name: Base_X_Encoding
 * purpose: to write a base 10 number in the desired base
 * parameters: the array that the basis encoding will be stored (encoding),
 * the length of the array (digits), the base used in the encoding (base), the 
 * base 10 number to be encoded (m)
 * returns: none
 * this function will modify the encoding array that is passed in to contain the
 * desired base representation of the number m.
 */
void SecuredInfo::Base_X_Encoding(vector<long>& encoding, long base, long m) {
    // find the smallest power of the base that is greater than m.
    long digits = 0;
    while (pow(base, digits) <= m) {
        digits++;
    }
    digits = digits - 1;
    
    int i = digits;
    long j = 0;
    
    while ( i >= 0) {
        j = 0;
        
        while (m >= j * pow(base, i)) {
            j++;
        }
        
        encoding.push_back(j - 1);
        m = m - (j - 1) * pow(base, i);
        j = 0;
        i = i - 1;
    }
}



/* Name: Bcomputations
 * purpose: encrypts Bob's message
 * parameters: none
 * returns: none
 */
void SecuredInfo::Bcomputations() {
    Bsecret = md(rand(), prime);
    // to prevent it from being less than 2
    Bsecret = (Bsecret / 2) + 3;

    Bpublic = powerMod(root, Bsecret, prime);

    cipher = powerMod(Apublic, Bsecret, prime);

    mEncrypted = md(message * cipher, prime);
    
}

/* Name: Adecryption
 * purpose: decrypts Bob's message
 * parameters: none
 * returns: none
 */
void SecuredInfo::Adecryption() {
    long Cipher = powerMod(Bpublic, Asecret, prime);
    decrypter = md(find_inverse(Cipher, prime), prime);
    mDecrypted = Alphabetical_Encoding(md(decrypter * mEncrypted, prime));
}



/* Name: find_inverse
 * purpose: to produce the inverse of cmodp
 * parameters: the number to be inverted (c) and the modulus (p)
 * returns: the inverse of cmodp
 * Note: implemented using the euclidean algorithm
 * 
 */
long SecuredInfo::find_inverse(long c, long p) {
    long x, y;
    extended_euclidean(p, c, x, y);
    return y;
}

// code is adapted from codingninjas.com
// this is a helper function for find_inverse that uses the inverse 
// Euclidean Algorithm
long SecuredInfo::extended_euclidean(long a, long b, long& x, long& y) {
    if (b == 0) {
        // if b is 0, return
        x = 1, y = 1000;
        return a;
    }
    long x1, y1;
    long d = extended_euclidean(b, md(a,b), x1, y1);
    y = x1 - (a / b) * y1;
    x = y1;
    return d;
}




/* Name: find_prime
 * purpose: to produce a large prime with high likelyhood
 * parameters: none
 * returns: a number that is very likely prime
 * O(iter * log^4(p)) time (for now iter is O(1)) assuming the density of primes
 * is 1/(logp)
 * 
 */
long SecuredInfo::find_prime(int digits) {
    long min = 2 * pow(10, digits - 1);
    long mod = pow(10, digits) - 2 * pow(10, digits - 1);
    long testprime = 0;
    bool Prime = false;
    while (not Prime) {
        long arg = rand();
        testprime = min + md(arg, mod);
        if (sieve_test(testprime)) {
            // does 1500 iterations for high likelyhood of primality
            if(Rabin_Miller_Test(testprime, 1500)) {
                return testprime;
            }
        }
    } 
    return -1;
}


/* Name: find_root
 * purpose: to produce a primitive root of prime p
 * parameters: the prime p to find the primitive root of
 * returns: a primitive root of p
 * O(log^2(p)) time on average
 */
long SecuredInfo::find_root(long p) {
    long preMod;
    long testRoot;
    long tester = (p - 1) / 2;
    long criteria;
    bool found = false;
    
    // cout << "starts finding" << endl;
    while (not found) {
        // cout  << "starts a loop" << endl;
        preMod = rand();
        testRoot = md(preMod, p);
        // if g^(fi(p)/2)=-1modp, g is a primitive root of p
        criteria = powerMod(testRoot, tester, p);
        if (criteria == p - 1) {
            found = true;
        }
    }
    
    return testRoot;
}


/* Name: md
 * purpose: perform modular arrethmatic on long numbers
 * parameters: the argument arg and the modulus mod. Both of these must be 
 * positive numbers for program to run successfully.
 * returns: argmod(mod)
 * O(log^2(arg)) time 
 */
long SecuredInfo::md(long arg, long mod) {
    // potential improvement by multiplying by a smaller number based on the
    // number of digits in arg and mod. Prob not too much tho.
    if (arg < 0) {
        arg = arg + (mod * prime);
    }
    
    
    if (arg > 0 and arg < mod) {
        return arg;
    }
    
    // finds the minimum k ST, mod*(10^k) > arg
    long k = 0;
    while (mod*(pow(10,k)) <= arg) {
        k++;
    }
    
    k = k - 1;
    long i;
    // subtracts off the largest i ST (i - 1)*mod*(10^k) is less than arg.
    // by doing this reppeatedly, we find the least residue of arg.
    while (k >= 0) {
        i = 0;
        while ((i < 11) and (i * mod * pow(10, k) <= arg)) {
            i++;
        }
        if (i == 11) {
            throw std::runtime_error( "logical error in md function" );
        }
        arg = arg - (i - 1) * mod * pow(10, k);
        k = k - 1;
    }
    return arg;
    
}

/* Name: sieve_test
 * purpose: test to see if p is divisible by any of the primes less than 100
 * parameters: the candidate prime
 * returns: true if possibly prime, false if not
 * O(logp) time
 */
bool SecuredInfo::sieve_test(long p) {
    // initializes the sieve
    bool sieve[101];
    sieve[0] = false;
    sieve[1] = false;
    for (long i=2; i < 101; i++) {
        sieve[i] = true; 
    }
    
    // performs the sieve and if p is divisible by anything, return false
    for (long i = 2; i < 100; i++) {
        if (sieve[i] == true) {
            if (md(p, i) == 0) {
                return false;
            }
            for (long j = 2; (i * j) < 100; j++) {
                sieve[i] = false;
            }
        }
    }
    
    return true;
}

/* Name: Rabin_Miller_Test
 * purpose: performs iter Rabin Miller Tests to determine with high 
 * likelyhood ~ ( 1 - .75^iter ), whether or not p is prime.
 * parameters: the number to be tested for primality p, and the number of
 * tests iter.
 * returns: whether or not the number passed in was prime.
 * O(iter * log^3(p)) time
 */
bool SecuredInfo::Rabin_Miller_Test(long p, long iter) {
    bool is_likely_prime;
    for (int i = 0; i < iter; i++) {
        is_likely_prime = Rabin_Miller_Helper(p);
    }
    return is_likely_prime;
}

/* Name: Rabin_Miller_Helper
 * purpose: performs iter Rabin Miller Test
 * parameters: the number to be tested for primality p
 * returns: whether or not the number passed in passed the test.
 * O(log^3(p)) time
 */
bool SecuredInfo::Rabin_Miller_Helper(long p) {
    long x;
    
    // note that there can be reppeats of a but assuming large p, 
    // this should occur infrequently.
    long a;
    long newRand = rand();
    a = md(newRand, p-1);
    
    // finds r and odd d such that d*2^r = p - 1
    long r = 0;
    long d = p - 1;
    while (md(d, 2) == 0) {
        d /= 2;
        r++;
    }
    
    // performs the test
    x = powerMod(a, d, p);
    if (x == 1 or x == p-1) {
        return true;
    }
    else {
        // while loop inspired in part by Geek4Geeks
        while (d < p - 1) {
            x = md(x * x, p);
            d *= 2;
            if (x == 1) {
                return false;
            }
            else if (x == p - 1) {
                return true;
            }
        }    
    }   
    return false;
}

/* Name: powerMod
 * purpose: computes (a^b)modm
 * parameters: base (a), power (b), and modulus (m)
 * returns: (a^b)modm
 * O(log(ba)log(m)) time
 // without question, there is an error here. I just don't know what :(
 * apparently this can return a negative number?
 */
long SecuredInfo::powerMod(long a, long b, long m) {
    long partialProduct = 1;
    while (b > 0) {
        if (not (b & 1)) {
            b /= 2;
            a *= a;
            a = md(a, m);
        }
        // b is even so we are looking at a square that we can break down.
        // ie, instead of doing a product of everything, we just do the stuff
        // inside the square and then square it.
        else {
            partialProduct *= a;
            b = b - 1;
        }
        partialProduct = md(partialProduct, m);   
    }
    return partialProduct;
}
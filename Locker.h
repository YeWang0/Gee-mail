//
//  Locker.h
//  Gee-mail
//
//  Created by Yan Zheng on 11/6/16.
//  Copyright © 2016 Yan Zheng. All rights reserved.
//

#ifndef Locker_h
#define Locker_h

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include<random>
#include <gcrypt.h>

using namespace std;

class Locker{
    
public:
    
    const string salt = "mysalt";
    const int stretch = 100;
    string key;
    string password;
    
    Locker();
    Locker(string key);
    int* string2bytes(string s, int a[]);
    string bytes2string(int a[], int size);
    int* hexDigest2bytes(string h, int b[]);
    string bytes2hexDigest(int b[], int size);
    
    void hash();
    string firstHash(string toHash);
    string stretchHash(string toHash);
    
    
    static int GET_RANDOM_INT(int upper)
    {
        mt19937 rng;
        rng.seed(random_device()());
        uniform_int_distribution<std::mt19937::result_type> dist6(0,upper); 
        return dist6(rng); 	
    }
    
    string generateNonce();
    string salsa20(int s, string nonce);
    string ciphertext(string message, string salsaString, int n);
    
    string encrypt(string txt, string pw);
    string decrypt(string ciphertext, string pw);
    
};

#endif /* Locker_h */

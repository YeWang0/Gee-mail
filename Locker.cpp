//
//  Locker.cpp
//  Gee-mail
//
//  Created by Yan Zheng on 11/6/16.
//  Copyright © 2016 Yan Zheng. All rights reserved.
//

#include "Locker.h"

using namespace std;

Locker:: Locker()
{
    key = "";
    password = "";
}

Locker:: Locker(string pw)
{
    key = "";
    password = pw;
}
    
int* Locker:: string2bytes(string s, int a[])
{
    for(int i = 0; i<s.length(); i++){
        a[i] = s[i];
    }
    return a;
}

string Locker:: bytes2string(int a[], int size)
{
    string s = "";
    for (int i = 0; i<size; i++){
        char c = (char) a[i];
        s+= c;
    }
    return s;
}


int* Locker:: hexDigest2bytes(string h, int b[])
{
    for(int i = 0; i<h.length(); i+=2){
        string byte = h.substr(i,2);
        int k = (int)strtol(byte.c_str(), NULL, 16);
        b[i/2] = k;
    }
    return b;
    
}

string Locker:: bytes2hexDigest(int b[], int size)
{
    char hex[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    string s = "";
    for (int i = 0; i<size; i++){
        char c1 = hex[b[i]/16];
        s+=c1;
        char c2 = hex[b[i] % 16];
        s+=c2;
    }
    return s;
}

void Locker:: hash()
{
    string hashString = "0"+password+salt;
    string hash1 = firstHash(hashString);
    //cout << "first hash: "<< hash1<<endl;
    key = stretchHash(hash1);
}

string Locker:: firstHash(string toHash)
{
    size_t index;
    size_t txtLength = toHash.length();
    char * hashBuffer = (char*) malloc(33);
    char * textBuffer = (char*) malloc(txtLength+1);
    char hashResult[65] = "";
    memset(hashBuffer, 0, 33);

    strncpy(textBuffer, toHash.c_str(), txtLength);

    gcry_md_hash_buffer(
        GCRY_MD_SHA256, // gcry_cipher_hd_t
        hashBuffer,    // void *
        textBuffer,    // const void *
        txtLength);   // size_t

    for (index = 0; index<32; index++)
         sprintf(hashResult+index*2, "%02X", (unsigned char)hashBuffer[index]);

    return hashResult;
}

string Locker:: stretchHash(string h)
{
    string theHash = h;
    for(int i=1;i<stretch;i++){
        theHash = firstHash(theHash+password+salt);
    }
    return theHash;
}

string Locker:: generateNonce()
{
    int nonceInt[8] = {};
    for (int i=0;i<8;i++){
        nonceInt[i] = GET_RANDOM_INT(255);
    }
    string hexNonce = bytes2hexDigest(nonceInt,8);
    return hexNonce;
}

string Locker:: salsa20(int tLength, string nonce)
{
    gcry_error_t     gcryError;
    gcry_cipher_hd_t gcryCipherHd;
    size_t           index;
    char salsaResult[2*tLength+1];
    string errMessage = "Encryption Error!!\n";
    //char * salsaKey = "andy love simoneandy love simone"; // 32 bytes
    //char * iniVector = "saraandy"; // 8 bytes

    gcryError = gcry_cipher_open(
        &gcryCipherHd, // gcry_cipher_hd_t *
        GCRY_CIPHER_SALSA20,   // int
        GCRY_CIPHER_MODE_STREAM,   // int
        0);            // unsigned int
    if (gcryError)
    {
        printf("gcry_cipher_open failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return errMessage;
    }
    // printf("gcry_cipher_open worked\n");
    
    gcryError = gcry_cipher_setkey(gcryCipherHd, key.c_str(), 32);
    if (gcryError)
    {
        printf("gcry_cipher_setkey failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return errMessage;
    }
    // printf("gcry_cipher_setkey worked\n");
    
    gcryError = gcry_cipher_setiv(gcryCipherHd, nonce.c_str(), 8);
    if (gcryError)
    {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return errMessage;
    }
    // printf("gcry_cipher_setiv worked\n");
    
    size_t txtLength = tLength;
    char * encBuffer = (char*) malloc(txtLength);
    char * textBuffer = (char*) malloc(txtLength);
    memset(textBuffer, 0, tLength);

    gcryError = gcry_cipher_encrypt(
        gcryCipherHd, // gcry_cipher_hd_t
        encBuffer,    // void *
        txtLength,    // size_t
        textBuffer,    // const void *
        txtLength);   // size_t
    if (gcryError)
    {
        printf("gcry_cipher_decrypt failed:  %s/%s\n",
               gcry_strsource(gcryError),
               gcry_strerror(gcryError));
        return errMessage;
    }
    // printf("gcry_cipher_decrypt worked\n");
    
    for (index = 0; index<txtLength; index++)
        sprintf(salsaResult+index*2, "%02X", (unsigned char)encBuffer[index]);
        //printf("%02X", (unsigned char)encBuffer[index]);

    return salsaResult;
}

string Locker:: ciphertext(string message, string salsaString, int s)
{
   
    int messageBytes[s];
    int outBytes[s];
    int salsaBytes[s];
    
    
    string2bytes(message, messageBytes);
    // for (int i=0;i<s;i++){
    //     cout <<"messageBytes["<<i<<"]:"<<messageBytes[i]<<endl;
    // }
    // cout<<endl;
    
    hexDigest2bytes(salsaString, salsaBytes);
    // for (int i=0;i<s;i++){
    //     cout <<"salsaBytes["<<i<<"]:"<<salsaBytes[i]<<endl;
    // }
    // cout<<endl;
    
    for (int i=0;i<s;i++){
        outBytes[i] = salsaBytes[i]^messageBytes[i];
        //cout<<"outBytes["<<i<<"]:"<<outBytes[i]<<endl;
    }
    // cout<<endl;
    
    string cipherResult = bytes2hexDigest(outBytes, s);
    return cipherResult;
}

string Locker:: encrypt(string txt, string pw)
{
    password = pw;
    hash();
    string nonce = generateNonce();
    
    int s = txt.length();
    //cout<<"txt length: "<<s<<endl;
    string salsaString = salsa20(s, nonce);
    //cout<<"salsa20: "<<salsaString<<"   length:"<<salsaString.length()<< endl;
    string cipher = nonce + ciphertext(txt, salsaString, s);
    
    //cout << "nonce: "<< nonce<<"   length:"<<nonce.length()<<endl;
    //cout<<"key: "<<key<<"   length:"<<key.length()<<endl;
    
    return cipher;
}

string Locker:: decrypt(string ciphertext, string pw)
{
    password = pw;
    hash();
    string nonce = ciphertext.substr(0,16);
    // cout << "nonce: "<< nonce<<"   length:"<<nonce.length()<<endl;
    string outHex = ciphertext.substr(16);
    // cout << "outHex: "<< outHex<<"   length:"<<outHex.length()<<endl;
    
    int s = outHex.length()/2;
    
    string salsaHex = salsa20(s,nonce);
    // cout << "salsaHex: "<< salsaHex<<"   length:"<<salsaHex.length()<<endl;
    int messageBytes[s];
    int outBytes[s];
    int salsaBytes[s];
    
    hexDigest2bytes(outHex, outBytes);
    // for (int i=0;i<s;i++){
    //     cout <<"outBytes["<<i<<"]:"<<outBytes[i]<<endl;
    // }
    // cout<<endl;
    hexDigest2bytes(salsaHex, salsaBytes);
    // for (int i=0;i<s;i++){
    //     cout <<"salsaBytes["<<i<<"]:"<<salsaBytes[i]<<endl;
    // }
    // cout<<endl;
    
    for (int i=0;i<s;i++){
        messageBytes[i] = salsaBytes[i]^outBytes[i];
        //cout<<"messageBytes["<<i<<"]:"<<messageBytes[i]<<endl;
    }
    
    string message = bytes2string(messageBytes, s);
    return message;
    
}

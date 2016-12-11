#include <sqlite3.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <map>
#include <vector>

using namespace std;

class User {   
        // Message<list> inbox;
        // Message<list> outbox;

    public:
        string username;
        User();
        User(string u);
        void init(string u);
        // void getName();
        // void getPassword();
        int getMessageData();
        void sendMessage(string receiver, string message, string subject, string passphrase);
        int readInboxMessage(int id, string passphrase);
        int readOutboxMessage(int id, string passphrase);
        void getInboxList();
        void getOutboxList();
        // int getUsersList();
        
        
        ~User();
        
    };
    
    
#include "user.h"
#include "Locker.h"
#include "sql_operation.h"

using namespace std;

map<int, vector<string>> inboxTable;
map<int, vector<string>> outboxTable;

static int callback(void* sign, int argc, char **argv, char **azColName){
    int i;
    int index = stoi(argv[0]);
    int s = *(int*) sign;
    // cout << s<<endl;
    for(i=1; i<argc; i++){
        // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        string str = argv[i] ? argv[i] : "NULL";
        if(s == 1){
            inboxTable[index].push_back(str); 
            // cout<<str<<endl;
        }else if(s == 2){
            outboxTable[index].push_back(str);
            // cout<<str<<endl;
        }else{
            
        }
        
    }
    return 0;
}


User::User()
{
    username = "";
}

void User::init(string u){
    username = u;
    getMessageData();
}
int User::getMessageData()
{   
    // cout<<"username:"<<username<<endl;
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("message.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    };
    
    //inbox
    string inbox_sql_str="select * from message where Receiver = '" + username + "';";
    char *inbox_sql = new char[inbox_sql_str.length() + 1];
    strcpy(inbox_sql, inbox_sql_str.c_str());
    int inbox_sign = 1;
    rc = sqlite3_exec(db, inbox_sql, callback, &inbox_sign, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "Inbox SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
    //   fprintf(stdout, "The Inbox record fetched:\n");
    }
    
    
    //outbox
    string outbox_sql_str="select * from message where Sender = '" + username + "';";
    char *outbox_sql = new char[outbox_sql_str.length() + 1];
    strcpy(outbox_sql, outbox_sql_str.c_str());
    int outbox_sign = 2;
    rc = sqlite3_exec(db, outbox_sql, callback, &outbox_sign, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "Outbox SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
    //   fprintf(stdout, "The Outbox record fetched:\n");
    }
    
    sqlite3_close(db);
    return 0;
}



void User::sendMessage(string receiver, string subject, string message, string passphrase)
{
    Locker l1;
    string ciphermessage = l1.encrypt(message, passphrase);
    string ciphersubject = l1.encrypt(subject, passphrase);
    
    string sql_str="INSERT INTO Message (Sender, Receiver, Subject, Message, Status) "  \
          "VALUES ( '"+username+"', '"+receiver+"', '"+ciphersubject+"', '"+ciphermessage+"','0'); "; 
 
    char *sql = new char[sql_str.length() + 1];
    strcpy(sql, sql_str.c_str());
    cout<<sql<<endl;
    bool sendSignal = sql_insert((char*)"message.db",sql);
    if(sendSignal){
        cout<<"Message sent successfully!"<<endl;
    }else{
        cout<<"Message can not be sent"<<endl;
    }
};

int User::readInboxMessage(int id, string passphrase){
    bool hasID = false;
    for(auto e : inboxTable){//check if id exits in database
        if(e.first == id){
            hasID = true;
            break;
        }
    }
    if(hasID){
        Locker locker;
    
        string deSubject = locker.decrypt(inboxTable[id][3],passphrase);
        string deMessage = locker.decrypt(inboxTable[id][4],passphrase);
          
        cout<<"Message content: "<<endl;
        cout<<"Id: "<<id<<endl;
        cout<<"Sender: "<<inboxTable[id][0]<<endl;
        cout<<"Datetime: "<<inboxTable[id][2]<<endl;
        cout<<"Subject: "<<deSubject<<endl;
        cout<<"Message: "<<deMessage<<endl;
        
        //update status
        string id_str = to_string(id);
        string sql_str="UPDATE Message SET status = 1 where id = '" + id_str + "';"; 
     
        char *sql = new char[sql_str.length() + 1];
        strcpy(sql, sql_str.c_str());
        sql_update((char*)"message.db",sql);
    }else{
        return 1;
    }
    return 0;
};

int User::readOutboxMessage(int id, string passphrase){
    bool hasID = false;
    for(auto e : outboxTable){//check if id exits in database
        if(e.first == id){
            hasID = true;
            break;
        }
    }
    if(hasID){
        Locker locker;
    
        //decode;
        string deSubject = locker.decrypt(outboxTable[id][3],passphrase);
        string deMessage = locker.decrypt(outboxTable[id][4],passphrase);
        cout<<endl;
        cout<<"Message content: "<<endl;
        cout<<"Id: "<<id<<endl;
        cout<<"Receiver: "<<outboxTable[id][1]<<endl;
        cout<<"Datetime: "<<outboxTable[id][2]<<endl;
        cout<<"Subject: "<<deSubject<<endl;
        cout<<"Message: "<<deMessage<<endl;
    }else{
        return 1;
    }
    return 0;
};

void User::getInboxList(){
    Locker locker;
    
    cout<<"Inbox: "<<endl;
    
    cout<<left<<setw(5)<<"ID"<<setw(15)<<"Sender"<<setw(25)<<"Datetime"<<setw(20)<<"Status"<<endl;
    
    for (auto elem: inboxTable){
        // string deSubject = locker.decrypt(elem.second[3],passphrase);
        cout<<left<<setw(5)<<elem.first<<setw(15)<<elem.second[0]<<setw(25)<<elem.second[2]<<setw(20); 
        if(elem.second[5] == "0"){
            cout<<setw(10)<<"unread"<<endl;
        }else{
            cout<<setw(10)<<"read"<<endl;
        }
        
    }
}


void User::getOutboxList(){
    Locker locker;
    
    cout<<"Outbox: "<<endl;
    
    cout<<left<<setw(5)<<"ID"<<setw(15)<<"Receiver"<<setw(25)<<"Datetime"<<endl;
    
    for (auto elem: outboxTable){
        // string deSubject = locker.decrypt(elem.second[3],passphrase);
        cout<<left<<setw(5)<<elem.first<<setw(15)<<elem.second[1]<<setw(25)<<elem.second[2]<<endl; 
    }
};

User::~User()
{
    cout<<"Destruct current user~"<<endl;
    username = "";
    inboxTable.clear();
    outboxTable.clear();
}
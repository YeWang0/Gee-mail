#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#include "sql_operation.h"
#include "user.h"

using namespace std;
class Client{
    public:
        User CurrentUser;
        bool login=false;
        bool registed=false;
        string mysalt="mysalt";
        
    public:
        bool login_status(){
            return login;
        }
        bool Login(string username,string password){
            if(login_status()){
                cout<<"Already login!"<<endl;
                return true;
            }
            
            string str=password+mysalt;
            char *cstr = new char[str.length() + 1];
            strcpy(cstr, str.c_str());
            unsigned int hashed_password=SDBMHash(cstr);
            
            string sql_str="select * from USER where name='"+username+"' and password='"+to_string(hashed_password)+"' limit 1";
            // cout<<sql_str<<endl;
            char* sql=new char[sql_str.length() + 1];
            strcpy(sql, sql_str.c_str());
            sql_select(1,this,sql);
            
            if(login){
                CurrentUser.init(username);
                cout<<"login successfully!"<<endl;
                return true;
            }else{
                return false;
            }
        }
        int login_callback(int argc, char **argv, char **azColName){

            if(argc>0){
                login=true;
            }
            return(0);
        }

        bool Username_registed(string username){
            registed=false;
            string sql_str="SELECT * FROM USER "  \
              "WHERE NAME='"+username+"'"; 
            // cout<<sql_str<<endl;
            char *sql = new char[sql_str.length() + 1];
            strcpy(sql, sql_str.c_str());
            // cout<<sql<<endl;
            sql_select(2,this,sql);
            return registed;
        }
        int registed_callback(int argc, char **argv, char **azColName){
            // cout<<"registed_callback ->"<<argc<<endl;          
            if(argc>0){
                registed=true;
            }
            return(0);
        }
        bool Register(string username,string password){
            string str=password+mysalt;
            char *cstr = new char[str.length() + 1];
            strcpy(cstr, str.c_str());
            unsigned int hashed_password=SDBMHash(cstr);
            // delete(cstr);
            // hash_password
            string sql_str="INSERT INTO USER (NAME,PASSWORD) "  \
              "VALUES ( '"+username+"', '"+to_string(hashed_password)+"'); "; 
            // cout<<str2<<endl;
            char *sql = new char[sql_str.length() + 1];
            strcpy(sql, sql_str.c_str());
            // cout<<sql<<endl;
            return sql_insert((char*)"user.db",sql);
        }
        
        bool Signout(){
            login=false;
            CurrentUser=User();
            return true;
        }
        
        void getInboxList(){
            if(!this->login_status()){
                cout<<"Can not get InboxList"<<endl;
                cout<<"Please login!"<<endl;
                return;
            }
            cout<<"***get Inbox List***"<<endl;
            CurrentUser.getInboxList();
        }
        void getOutboxList(){
            if(!this->login_status()){
                cout<<"Can not get OutboxList"<<endl;
                cout<<"Please login!"<<endl;
                return; 
            }
            cout<<"***get Outbox List***"<<endl;
            CurrentUser.getOutboxList();
        }
        void sendMessage(string receiver,string title,string content,string passphrase){
            if(!this->login_status()){
                cout<<"Can not send Message"<<endl;
                cout<<"Please login!"<<endl;
                return; 
            }
            cout<<"***send Message***"<<endl;
            CurrentUser.sendMessage(receiver,title,content,passphrase);
            // CurrentUser.sendMessage("123","Title of E-mail", "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.", "123");
        }
        
        void readInboxMessage(int MessageId,string passphrase){
            if(!this->login_status()){
                cout<<"Can not read Message"<<endl;
                cout<<"Please login!"<<endl;
                return; 
            }
            cout<<"***read Inbox Message***"<<endl;
            if(CurrentUser.readInboxMessage(MessageId,passphrase)!=0){
                cout<<"Can't find the message"<<endl;
            }
        }
        void readOutboxMessage(int MessageId,string passphrase){
            if(!this->login_status()){
                cout<<"Can not read Message"<<endl;
                cout<<"Please login!"<<endl;
                return; 
            }
            cout<<"***read Outbox Message***"<<endl;
            if(CurrentUser.readOutboxMessage(MessageId,passphrase)!=0){
                cout<<"Can't find the message"<<endl;
            }
        }
        void getUsersList(){
            if(!this->login_status()){
                cout<<"Can not get UsersList"<<endl;
                cout<<"Please login!"<<endl;
                return; 
            }
            
            string sql_str="SELECT NAME FROM USER";
            //   "WHERE NAME!='"+username+"'"; 
            char *sql = new char[sql_str.length() + 1];
            strcpy(sql, sql_str.c_str());
            cout<<"***get User List***"<<endl;
            cout<<"User list:"<<endl;
            sql_select(3,this,sql);
            
            // cout<<"***getUsersList***"<<endl;
        }
        // getUsersList_callback
        int getUsersList_callback(int argc, char **argv, char **azColName){     
            int i;
            
            for(i=0; i<argc; i++){
                printf("%s\n", argv[i] ? argv[i] : "NULL");
            }
            return(0);
        }
        
        static int sql_select_callback1(void* param, int argc, char **argv, char **azColName){
            // printf("Callback1 function called\n");
            Client* myclient = reinterpret_cast<Client*>(param);
            myclient->login_callback(argc, argv, azColName);
            return(0);
        }
        static int sql_select_callback2(void* param, int argc, char **argv, char **azColName){
            // printf("Callback2 function called\n");
            Client* myclient = reinterpret_cast<Client*>(param);
            myclient->registed_callback(argc, argv, azColName);
            return(0);
        }
        static int sql_select_callback3(void* param, int argc, char **argv, char **azColName){
            // printf("Callback2 function called\n");
            Client* myclient = reinterpret_cast<Client*>(param);
            myclient->getUsersList_callback(argc, argv, azColName);
            return(0);
        }
        
        bool sql_select(int op, void* param, char* sql)
        {
          sqlite3 *db;
          char *zErrMsg = 0;
          int rc;
        //   cout<<"op:"<<op<<endl;
          /* Open database */
          rc = sqlite3_open("user.db", &db);
          if( rc ){
              fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
              return false;
          }else{
            //   fprintf(stderr, "Opened database successfully\n");
          }
          
          if(op==1){
            rc = sqlite3_exec(db, sql, sql_select_callback1, (void*)param, &zErrMsg);
          }
          else if(op==2){
            rc = sqlite3_exec(db, sql, sql_select_callback2, (void*)param, &zErrMsg);
          }
          else if(op==3){
            rc = sqlite3_exec(db, sql, sql_select_callback3, (void*)param, &zErrMsg);
          }
          
          if( rc != SQLITE_OK ){
              fprintf(stderr, "SQL error: %s\n", zErrMsg);
              sqlite3_free(zErrMsg);
              return false;
          }else{
            //   fprintf(stdout, "Operation done successfully\n");
          }
          sqlite3_close(db);
          return true;
        }
        
        unsigned int SDBMHash(char *str)
        {
            unsigned int hash = 0;
        
            while (*str)
            {
                // equivalent to: hash = 65599*hash + (*str++);
                hash = (*str++) + (hash << 6) + (hash << 16) - hash;
            }
        
            return (hash & 0x7FFFFFFF);
        }
};



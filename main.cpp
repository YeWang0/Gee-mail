// #include <sqlite3.h>
#include <iostream>
#include <stdlib.h>
// #include "sql_insert.cpp"
// #include "sql_select.cpp"
#include "client.h"
using namespace std;
int main(){
    Client myclient;
    // myclient.Register("wang1","passwd");
    // myclient.Login("wang1","passwd");
    // myclient.Signout();
    // myclient.getInboxList();
    // myclient.getOutboxList();
    // myclient.sendMessage();
    // myclient.readMessage();
    // myclient.getUsersList();
    // myclient.Username_registed("wang1");
    system("clear");
    while(1){
        cin.clear();
        system("clear");
        // cout<<myclient.CurrentUser.username<<endl;
        
        cout<<"Welcome to the Gee mail system!"<<endl;
        if(!myclient.login_status()){
            cout<<"*******************************"<<endl;
            cout<<"*** 1. Login"<<endl;
            cout<<"*** 2. Sign up"<<endl;
            cout<<"*** 3. End"<<endl;
            cout<<"*******************************"<<endl;
            cout<<"Choose an operation:"<<endl;
            string op="";
            getline(cin,op);
            if(op.compare("1")==0){
                string username;
                string password;
                cout<<"Please input your user name:"<<endl;
                getline (cin,username);
                cout<<"Your input is : "<<username<<endl;
                cout<<"Please input your password:"<<endl;
                getline (cin,password);
                cout<<"Your input is : "<<password<<endl;
                if(!myclient.Login(username,password)){
                    cout<<"Failed to login!"<<endl;
                }
            }else if(op.compare("2")==0){
                string username;
                string password="";
                cout<<"Please input your user name:"<<endl;
                getline (cin,username);
                cout<<"Your input is : "<<username<<endl;
                while(myclient.Username_registed(username)){
                    cout<<"The user name has been registed!"<<endl;
                    cout<<"Please input your user name:"<<endl;
                    getline (cin,username);
                    cout<<"Your input is : "<<username<<endl;
                }
                cout<<"Please input your password:"<<endl;
                getline (cin,password);
                cout<<"Your input is : "<<password<<endl;
                myclient.Register(username,password);
                myclient.Login(username,password);
                cout<<"Logged in"<<endl;
            }else if(op.compare("3")==0){
                cout<<"*******************************"<<endl;
                cout<<"Bye!"<<endl;
                cout<<"*******************************"<<endl;
                break;   
            }else{
                cout<<"Invalid operation!"<<endl;
            }
        }else{
            // cout<<"Logged in"<<endl;
            cout<<"*******************************"<<endl;
            cout<<"*** 1. Print Inbox List"<<endl;
            cout<<"*** 2. Print Outbox List"<<endl;
            cout<<"*** 3. Read Inbox Message"<<endl;
            cout<<"*** 4. Read Outbox Message"<<endl;
            cout<<"*** 5. Print Users List"<<endl;
            cout<<"*** 6. Send Message"<<endl;
            cout<<"*** 7. Sign out"<<endl;
            cout<<"*** 8. End"<<endl;
            cout<<"*******************************"<<endl;
            cout<<"Choose an operation:"<<endl;
            string op="";
            getline(cin,op);
            if(op.compare("1")==0){
                myclient.getInboxList();
            }else if(op.compare("2")==0){
                myclient.getOutboxList();
            }else if(op.compare("3")==0){
                cout<<"Input message id:"<<endl;
                string Id;
                int MessageId=-1;
                getline (cin,Id);
                try{
                    cout<<"Message id: "<<Id<<"->"<<stoi(Id)<<endl;
                    MessageId=stoi(Id);
                }catch(...){
                    cout<<"Invalid message id!"<<endl;
                    cin.clear();
                    cout<<"\nPress ENTER to continue...\n";
                    getchar();
                    continue;
                }
                string passphrase;
                cout<<"Inpt passphrase for the message:"<<endl;
                getline (cin,passphrase);
                myclient.readInboxMessage(MessageId,passphrase);
            }else if(op.compare("4")==0){
                cout<<"Input message id:"<<endl;
                string Id;
                int MessageId=-1;
                getline (cin,Id);
                try{
                    cout<<"Message id: "<<Id<<"->"<<stoi(Id)<<endl;
                    MessageId=stoi(Id);
                }catch(...){
                    cout<<"Invalid message id!"<<endl;
                    cin.clear();
                    cout<<"\nPress ENTER to continue...\n";
                    getchar();
                    continue;
                }
                string passphrase;
                cout<<"Input passphrase for the message:"<<endl;
                getline (cin,passphrase);
                myclient.readOutboxMessage(MessageId,passphrase);
            }else if(op.compare("5")==0){
                myclient.getUsersList();
            }else if(op.compare("6")==0){
                string receiver="";
                string title;
                string content;
                string passphrase;
                cout<<"Please input the receiver name:"<<endl;
                getline (cin,receiver);
                while(!myclient.Username_registed(receiver)){
                    cout<<"The receiver name does not exist!"<<endl;
                    cout<<"Please input the receiver name:"<<endl;
                    getline (cin,receiver);
                    cout<<"Your input is : "<<receiver<<endl;
                }
                cout<<"Please input the title:"<<endl;
                getline (cin,title);
                cout<<"Please input the content:"<<endl;
                getline (cin,content);
                cout<<"Please input the passphrase:"<<endl;
                getline (cin,passphrase);
                myclient.sendMessage(receiver,title,content,passphrase);
                
            }else if(op.compare("7")==0){
                myclient.Signout();
            }else if(op.compare("8")==0){
                cout<<"*******************************"<<endl;
                cout<<"Bye!"<<endl;
                cout<<"*******************************"<<endl;
                break;   
            }else{
                cout<<"Invalid operation!"<<endl;
            }
            
        }
        // cin.clear();
        cin.clear();
        cout<<"\nPress ENTER to continue...\n";
        getchar();
        
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <string>
using namespace std;
static int sql_callback(void *NotUsed, int argc, char **argv, char **azColName);

bool sql_insert(char* db_name,char const* sql);

bool sql_update(char* db_name,char const* sql);

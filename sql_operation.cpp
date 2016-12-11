#include "sql_operation.h"

using namespace std;
static int sql_callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

bool sql_insert(char* db_name,char const* sql){
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   // char *sql;

   /* Open database */
   rc = sqlite3_open(db_name, &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return false;
   }else{
      // fprintf(stderr, "Opened database successfully\n");
   }
   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, sql_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return false;
   }else{
      // fprintf(stdout, "Records created successfully\n");
   }
   sqlite3_close(db);
   return true;
}

bool sql_update(char* db_name,char const* sql){
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   // char *sql;

   /* Open database */
   rc = sqlite3_open(db_name, &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return false;
   }else{
      // fprintf(stderr, "Opened database successfully\n");
   }

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, sql_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return false;
   }else{
      // fprintf(stdout, "Records updated successfully\n");
   }
   sqlite3_close(db);
   return true;
}
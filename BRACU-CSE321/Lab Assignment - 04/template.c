#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 3
#define MAX_RESOURCES 3
#define MAX_NAME_LEN 20

typedef enum{ 
    //to do
}Permission;

//User and Resource Definitions
typedef struct{
    //to do
}User;

typedef struct{
    //to do
}Resource;

//ACL Entry
typedef struct{
    //to do
}ACLEntry;

typedef struct{
    //to do
}ACLControlledResource;

//Capability Entry
typedef struct{
    //to do
}Capability;

typedef struct{
    //to do
}CapabilityUser;

//Utility Functions
void printPermissions(int perm){
    //to do
}

int hasPermission(int userPerm, int requiredPerm){
    //to do
}

//ACL System
void checkACLAccess(ACLControlledResource *res, const char *userName, int perm){
    //to do
}

//Capability System
void checkCapabilityAccess(CapabilityUser *user, const char *resourceName, int perm){
    //to do
}


int main(){
    //Sample users and resources
    User users[MAX_USERS] = {{"Alice"}, {"Bob"}, {"Charlie"}};
    Resource resources[MAX_RESOURCES] = {{"File1"}, {"File2"}, {"File3"}};

    
    //ACL Setup
    //to do

    //Capability Setup
    //to do

    //Test ACL
    //to do

    //Test Capability
    //to do

    return 0;
}
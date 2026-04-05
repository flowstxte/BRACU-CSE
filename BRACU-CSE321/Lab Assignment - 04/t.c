#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 5
#define MAX_RESOURCES 5
#define MAX_NAME_LEN 20

typedef enum{
    READ = 1,
    WRITE = 2,
    EXECUTE = 4
} Permission;

//User and Resource Definitions
typedef struct{
    char name[MAX_NAME_LEN];
} User;

typedef struct{
    char name[MAX_NAME_LEN];
} Resource;

//ACL Entry
typedef struct{
    char username[MAX_NAME_LEN];
    int permissions;
} ACLEntry;

typedef struct{
    Resource resource;
    ACLEntry entries[MAX_USERS];
    int entryCount;
} ACLControlledResource;

//Capability Entry
typedef struct{
    char resourceName[MAX_NAME_LEN];
    int permissions;
} Capability;

typedef struct{
    User user;
    Capability capabilities[MAX_RESOURCES];
    int capabilityCount;
} CapabilityUser;

//Utility function
void printPermissions(int perm){
    if (perm & READ)
        printf("READ ");
    if (perm & WRITE)
        printf("WRITE ");
    if (perm & EXECUTE)
        printf("EXECUTE ");
}

int hasPermission(int userPerm, int requiredPerm){
    return (userPerm & requiredPerm) == requiredPerm;
}

//ACL System
void checkACLAccess(ACLControlledResource *res, const char *userName, int perm){
    for (int i = 0; i < res->entryCount; i++){
        if (strcmp(res->entries[i].username, userName) == 0){
            if (hasPermission(res->entries[i].permissions, perm)){
                printf("ACL Check: User %s requests ", userName);
                printPermissions(perm);
                printf("on %s: Access GRANTED\n", res->resource.name);
                return;
            }
            else{
                printf("ACL Check: User %s requests ", userName);
                printPermissions(perm);
                printf("on %s: Access DENIED\n", res->resource.name);
                return;
            }
        }
    }
    printf("ACL Check: User %s has NO entry for resource %s: Access DENIED\n", userName, res->resource.name);
}

//Capability System
void checkCapabilityAccess(CapabilityUser *user, const char *resourceName, int perm){
    for (int i = 0; i < user->capabilityCount; i++){
        if (strcmp(user->capabilities[i].resourceName, resourceName) == 0){
            if (hasPermission(user->capabilities[i].permissions, perm)){
                printf("Capability Check: User %s requests ", user->user.name);
                printPermissions(perm);
                printf("on %s: Access GRANTED\n", resourceName);
                return;
            }
            else{
                printf("Capability Check: User %s requests ", user->user.name);
                printPermissions(perm);
                printf("on %s: Access DENIED\n", resourceName);
                return;
            }
        }
    }
    printf("Capability Check: User %s has NO capability for %s: Access DENIED\n", user->user.name, resourceName);
}

void addACLEntry(ACLControlledResource *res, const char *username, int permissions){
    if (res->entryCount >= MAX_USERS){
        printf("ACL entry limit reached for resource %s\n", res->resource.name);
    }
    else{
        strcpy(res->entries[res->entryCount].username, username);
        res->entries[res->entryCount].permissions = permissions;
        res->entryCount++;
    }
}

void addCapability(CapabilityUser *user, const char *resourceName, int permissions){
    if (user->capabilityCount >= MAX_RESOURCES){
        printf("Capability entry limit reached for user %s\n", user->user.name);
    }
    else{
        strcpy(user->capabilities[user->capabilityCount].resourceName, resourceName);
        user->capabilities[user->capabilityCount].permissions = permissions;
        user->capabilityCount++;
    }
}

int main(){
    //Sample users and resources
    User users[MAX_USERS] = {{"Alice"}, {"Bob"}, {"Charlie"}, {"Zihan"}, {"Rafi"}};
    Resource resources[MAX_RESOURCES] = {{"File1"}, {"File2"}, {"File3"}, {"File4"}, {"File5"}};

    //ACL Setup
    ACLControlledResource aclResources[MAX_RESOURCES];
    for (int i = 0; i < MAX_RESOURCES; i++){
        aclResources[i].resource = resources[i];
        aclResources[i].entryCount = 0;
    }
    addACLEntry(&aclResources[0], "Alice", READ | WRITE);
    addACLEntry(&aclResources[0], "Bob", READ);
    addACLEntry(&aclResources[1], "Charlie", READ | EXECUTE);
    addACLEntry(&aclResources[1], "Zihan", WRITE);
    addACLEntry(&aclResources[2], "Rafi", READ | WRITE | EXECUTE);
    addACLEntry(&aclResources[3], "Alice", EXECUTE);
    addACLEntry(&aclResources[3], "Zihan", READ);
    addACLEntry(&aclResources[4], "Bob", WRITE | EXECUTE);
    addACLEntry(&aclResources[4], "Charlie", READ);

    //Capability Setup
    CapabilityUser capUsers[MAX_USERS];
    for (int i = 0; i < MAX_USERS; i++){
        capUsers[i].user = users[i];
        capUsers[i].capabilityCount = 0;
    }
    addCapability(&capUsers[0], "File1", READ | WRITE);
    addCapability(&capUsers[0], "File4", EXECUTE);
    addCapability(&capUsers[1], "File1", READ);
    addCapability(&capUsers[1], "File5", WRITE | EXECUTE);
    addCapability(&capUsers[2], "File2", READ | EXECUTE);
    addCapability(&capUsers[2], "File5", READ);
    addCapability(&capUsers[3], "File2", WRITE);
    addCapability(&capUsers[3], "File4", READ);
    addCapability(&capUsers[4], "File3", READ | WRITE | EXECUTE);

    //Test ACL
    checkACLAccess(&aclResources[0], "Alice", READ);
    checkACLAccess(&aclResources[0], "Bob", WRITE);
    checkACLAccess(&aclResources[1], "Charlie", EXECUTE);
    checkACLAccess(&aclResources[2], "Rafi", WRITE);
    checkACLAccess(&aclResources[3], "Zihan", READ);
    checkACLAccess(&aclResources[4], "Charlie", WRITE);

    //Test Capability
    checkCapabilityAccess(&capUsers[0], "File1", READ);
    checkCapabilityAccess(&capUsers[1], "File5", EXECUTE);
    checkCapabilityAccess(&capUsers[2], "File2", WRITE);
    checkCapabilityAccess(&capUsers[3], "File4", READ);
    checkCapabilityAccess(&capUsers[4], "File3", EXECUTE);
    checkCapabilityAccess(&capUsers[0], "File2", READ);

    return 0;
}

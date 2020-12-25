#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#define SHMSZ 17
#define SHM_KEY 123456

int *ptr;

int main () {
    int id = 0;
    //key_t SHM_KEY = 123456;
    //int i = 0;
    //char *shm = NULL;
    /************************* TODO 1 *************************/
    // Create a shared memory section
    if( ( id = shmget( SHM_KEY, SHMSZ, IPC_CREAT |IPC_EXCL | 0666 ) ) < 0 )
    {
        perror( "shmget" );
        exit(1);
    }
    
    /************************* TODO 1 *************************/

    /************************* TODO 2 *************************/
    // Attach the memory section
    // the return value is a pointer to the shared memory section
    if( ( ptr = shmat( id, NULL, 0 ) ) == (void *)-1 )
    {
        perror( "shmat" );
        exit(1);
    }
    
    //printf("%d", id);
    //ptr = shmat(id, NULL, 0);
    
   /* if(ptr == (void *)-1)
    {
        printf("fault 2");
    }*/
    //printf("%d", *ptr);
    //printf("size of pointer %ls", ptr);
    ptr[0] = 0;
    printf("\033[1;32m[server] The value is %d\033[0m\n", ptr[0]);

    while(1) {
        int cmd;

        printf("\n");
        printf("1: Show the value\n");
        printf("2: Modify the value\n");
        printf("3: Exit\n");
        printf("Enter commands: ");
        scanf("%d", &cmd);

        if (cmd == 1)
            printf("\033[1;32m[server] The value is %d\033[0m\n", ptr[0]);
        else if (cmd == 2) {
            printf("Input new value: ");
            scanf("%d", &ptr[0]);
        }
        else
            break;        
    } 
}

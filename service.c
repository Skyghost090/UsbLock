#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include "main.h"
#include <pthread.h>
static pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

void open_crypt_device(){

}

int main(int argc, char *argv[]){
    uid = geteuid();
    if(uid != 0){
        printf("Please use root to run this program\n");
        return 2;
    }
    if(argv[1] && argv[2]){
        while(1){
            pthread_mutex_init(&my_mutex, NULL);
            pthread_mutex_lock(&my_mutex);
            if (access(argv[1], F_OK) == 0) {
                char command[150];
                snprintf(command, sizeof(command), "echo $(cat private_key.txt) | cryptsetup luksOpen %s private", argv[1]);
                system(command);
            } else {
                system("cryptsetup close private");
            }
            pthread_mutex_unlock(&my_mutex);
            pthread_mutex_destroy(&my_mutex);
            sleep(1);
        }
    } else {
        printf("Please type a mountpoint\n");
    }
}

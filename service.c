#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include "main.h"
#include <pthread.h>

static pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

static void check_crypt_device(char *param, char *command_buffer, unsigned long command_buffer_size){
    if (access(param, F_OK) == 0) {
        snprintf(command_buffer, command_buffer_size, "echo $(cat private_key.txt) | cryptsetup luksOpen %s private", param);
        system(command_buffer);
    } else {
        system("cryptsetup close private");
    }
}

static void service_loop(char *param){
    char command[150];
    while(1000){
        check_crypt_device(param, command, sizeof(command));
    }
}

int main(int argc, char *argv[]){
    pthread_mutex_init(&my_mutex, NULL);
    pthread_mutex_lock(&my_mutex);
    uid = geteuid();
    if(uid != 0){
        printf("Please use root to run this program\n");
        return 2;
    }
    if(argv[1]){
        service_loop(argv[1]);
    } else {
        printf("Please type a mountpoint\n");
    }
    pthread_mutex_unlock(&my_mutex);
    pthread_mutex_destroy(&my_mutex);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"

int main(int argc, char *argv[]){
    uid = geteuid();
    if(uid != 0){
        printf("Please use root to run this program\n");
        return 2;
    }

    if(argv[1] && argv[2]){
        sprintf(mountCommand, "cryptsetup open --type plain %s private --key-file %s &> /dev/null", argv[1], argv[2]);
        sprintf(umountCommand, "cd /media/%s && sudo umount `ls`", getlogin());
        while(1){
            if (access(argv[1], F_OK) == 0) {
                system(mountCommand);
            } else {
                system(umountCommand);
                system("cryptsetup close private");
            }
            sleep(1);
        }
    } else {
        printf("Please type a mountpoint\n");
    }
}

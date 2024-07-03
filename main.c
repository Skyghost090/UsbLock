#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/mount.h>
#include "main.h"

int errorMsg(){
    printf("Type a valid param, --help for more information\n");
    return 1;
}

int lockUnit(char unitStorage[]){
    gnutls_global_init();
    gnutls_x509_privkey_t key;
    gnutls_x509_privkey_init(&key);
    gnutls_x509_privkey_generate(key, GNUTLS_PK_RSA, 256, 0);
    gnutls_x509_privkey_export(key, GNUTLS_X509_FMT_PEM, output_buffer, &output_size);

    FILE *key_file = fopen("private_key.txt", "w+");
    fwrite(output_buffer, 1, output_size, key_file);

    fclose(key_file);
    chmod("private_key.txt", 600);

    gnutls_x509_privkey_deinit(key);
    gnutls_global_deinit();
    sprintf(lockCommand, "%s %s %s", lockCommand, unitStorage, lockCommand2);
    createId = rand();
    sprintf(serviceName, "usblock%i.service", createId);
    sprintf(installService, "systemctl enable %s", serviceName);
    //printf("%s", lockCommand); //to print a full command
    if(system(lockCommand) || system("mkfs.ext4 /dev/mapper/private") != 0){
        printf("Failed to create ecryption unit\n");
        return 2;
    }

    chdir("/etc/systemd/system");
    fileService = fopen(serviceName, "w+");
    fprintf(fileService,"[Unit]\nDescription=Encryption service for usbLock\nAfter=network.target\nStartLimitIntervalSec=0\n[Service]\nType=simple\nRestart=always\nRestartSec=1\nUser=root\nExecStart=%s/service %s %s/private_key.txt\n[Install]\nWantedBy=multi-user.target\n", cwd, unitStorage, cwd);
    fclose(fileService);
    if(system(installService) != 0){
        printf("Failed to create a service for encryption\n");
        return 3;
    }
    return 0;
}

int serviceRemove(){
    if(system("rm /etc/systemd/system/usblock*") != 0){
        printf("No have a service\n");
        return 4;
    }
    printf("Please format your unit\n");
    return 0;
}

int main(int argc, char *argv[]){
    getcwd(cwd, sizeof(cwd));
    uid = geteuid();
    if(uid != 0){
        printf("Please use root to run this program\n");
        return 2;
    }
    if(argv[1] && strcmp("--help", argv[1]) == 0){
        printf("%s", helpString);
        return 0;
    } else if(argv[1] && argv[2] && strcmp("-l", argv[1]) == 0){
        exit(lockUnit(argv[2]));
    } else if(argv[1] && strcmp("-r", argv[1]) == 0){
        exit(serviceRemove());
    } else {
        exit(errorMsg());
    }
}

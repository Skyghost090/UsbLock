#include <stdio.h>
#include <unistd.h>
#include "main.h"

static int build_service(char *devunit){
    FILE *file_service;
    getcwd(cwd, sizeof(cwd));
    chdir("/etc/systemd/system");
    file_service = fopen("usblock.service", "w+");
    char service_buffer[4000];
    snprintf(service_buffer, sizeof(service_buffer),"[Unit]\nDescription=Encryption service for usbLock\nAfter=network.target\nStartLimitIntervalSec=0\n[Service]\nType=simple\nRestart=always\nRestartSec=1\nUser=root\nExecStart=%s/service %s %s/private_key.txt\n[Install]\nWantedBy=multi-user.target\n", cwd, devunit, cwd);
    fputs(service_buffer, file_service);
    fclose(file_service);
    char *const params[] = {"enable", "usblock.service"};
    execv("/usr/bin/systemctl", params);
    printf("Please reboot system\n");
    return 0;
}

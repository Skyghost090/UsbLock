#include <stdbool.h>
char helpString[] = "usblock [OPTIONS] [UNIT]\n\n[RULES]:\n* Please before eject your unit umount dm mountpoint\n* Please configure a 1 device\n\n[OPTIONS]:\n-l: for lock a unit\n-r: for remove device\n";
char lockCommand[] = "cryptsetup open --type plain";
char lockCommand2[] = "private --key-file private_key.txt";
unsigned char output_buffer[4096];
size_t output_size = sizeof(output_buffer);
uid_t uid;
char umountCommand[300];
char mountCommand[300];
int outStatus;
bool conditionalUmount = false;
char userName;
FILE *fileService;
char serviceName[300];
char installService[400];
int createId;
char cwd[1024];

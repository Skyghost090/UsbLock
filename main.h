#include <sys/types.h>
char help_string[214] = "usage: usblock <path> <disk-id-path>\n<path>: refers to either a regular file or a block device.\n       WARNING: the file or device will be wiped.\n<disk-id-path>: path to disk id of /dev/disk/by-id/(diskid)\n";
char cwd[1024];
uid_t uid;

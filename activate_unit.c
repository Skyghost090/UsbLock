#include <stdio.h>
#include <libcryptsetup.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "check_init_status.c"

static void get_device_parameters(const char *device_name, struct crypt_active_device active_device){
    printf("Active device parameters for %s:\n"
    "\tDevice offset (in sectors): %" PRIu64 "\n"
    "\tIV offset (in sectors)    : %" PRIu64 "\n"
    "\tdevice size (in sectors)  : %" PRIu64 "\n"
    "\tread-only flag            : %s\n",
    device_name, active_device.offset, active_device.iv_offset, active_device.size,
    active_device.flags & CRYPT_ACTIVATE_READONLY ? "1" : "0");
}

static void get_luks_info(struct crypt_device *context, const char *device_name){
    printf("%s device %s/%s is active.\n", crypt_get_type(context), crypt_get_dir(), device_name);
    printf("\tcipher used: %s\n", crypt_get_cipher(context));
    printf("\tcipher mode: %s\n", crypt_get_cipher_mode(context));
    printf("\tdevice UUID: %s\n", crypt_get_uuid(context));
}

static int activate_and_check_status(const char *path, const char *device_name, const char *key, const int key_size){
    static struct crypt_device *context;
    static struct crypt_active_device active_device;
    static int init;

    init = crypt_init(&context, path);
    check_init_status(init, context, "crypt_init() failed");
    init = crypt_load(context, CRYPT_LUKS, NULL);
    check_init_status(init, context,"crypt_load() failed on device");
    init = crypt_activate_by_passphrase(context, device_name, CRYPT_ANY_SLOT, key, key_size, CRYPT_ACTIVATE_IGNORE_CORRUPTION);

    if(system("mkfs.ext4 /dev/mapper/private") != 0) return 10;

    //Get info about the active device.
    init = crypt_get_active_device(context, device_name, &active_device);
    if(check_init_status(init, context, "Get info about active device failed") != 0){
        crypt_deactivate(context, device_name);
        exit(20);
    }

    crypt_free(context);
    return 0;
}

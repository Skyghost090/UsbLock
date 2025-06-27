#include <stdio.h>
#include <libcryptsetup.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

static int activate_and_check_status(const char *path, const char *device_name, const char *key, const int key_size)
{
    static struct crypt_device *cd;
    static struct crypt_active_device cad;
    static int r;

    /*
     * LUKS device activation example.
     */
    r = crypt_init(&cd, path);
    if (r < 0) {
        printf("crypt_init() failed for %s.\n", path);
        return r;
    }

    /*
     * crypt_load() is used to load existing LUKS header from a block device
     */
    r = crypt_load(cd,              /* crypt context */
                   CRYPT_LUKS,      /* requested type - here LUKS of any type */
                   NULL);           /* additional parameters (not used) */

    if (r < 0) {
        printf("crypt_load() failed on device %s.\n", crypt_get_device_name(cd));
        crypt_free(cd);
        return r;
    }

    /*
     * Device activation creates a device-mapper device with the specified name.
     */
    r = crypt_activate_by_passphrase(cd,            /* crypt context */
                                     device_name,   /* device name to activate */
                                     CRYPT_ANY_SLOT,/* the keyslot use (try all here) */
                                     key, key_size,      /* passphrase */
                                     CRYPT_ACTIVATE_IGNORE_CORRUPTION); /* flags */

    if(system("mkfs.ext4 /dev/mapper/private") != 0){
        return 10;
    }

    if (r < 0) {
        printf("Device %s activation failed.\n", device_name);
        crypt_free(cd);
        return r;
    }

    printf("%s device %s/%s is active.\n", crypt_get_type(cd), crypt_get_dir(), device_name);
    printf("\tcipher used: %s\n", crypt_get_cipher(cd));
    printf("\tcipher mode: %s\n", crypt_get_cipher_mode(cd));
    printf("\tdevice UUID: %s\n", crypt_get_uuid(cd));

    /*
     * Get info about the active device.
     */
    r = crypt_get_active_device(cd, device_name, &cad);
    if (r < 0) {
        printf("Get info about active device %s failed.\n", device_name);
        crypt_deactivate(cd, device_name);
        crypt_free(cd);
        return r;
    }

    printf("Active device parameters for %s:\n"
    "\tDevice offset (in sectors): %" PRIu64 "\n"
    "\tIV offset (in sectors)    : %" PRIu64 "\n"
    "\tdevice size (in sectors)  : %" PRIu64 "\n"
    "\tread-only flag            : %s\n",
    device_name, cad.offset, cad.iv_offset, cad.size,
    cad.flags & CRYPT_ACTIVATE_READONLY ? "1" : "0");

    crypt_free(cd);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <libcryptsetup.h>
#include <pthread.h>
#include <sys/stat.h>
#include "create_key.c"
#include "activate_unit.c"
#include "build_service.c"

static pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

static int format_and_add_keyslots(const char *path)
{
    struct crypt_device *cd;
    static int init;

    /*
     * The crypt_init() call is used  to initialize crypt_device context,
     * The path parameter specifies a device path.
     *
     * For path, you can use either link to a file or block device.
     * The loopback device will be detached automatically.
     */

    init = crypt_init(&cd, path);
    if (init < 0) {
        printf("crypt_init() failed for %s.\n", path);
        return init;
    }

    printf("Context is attached to block device %s.\n", crypt_get_device_name(cd));

    /*
     * So far, no data were written to the device.
     */
    printf("Device %s will be formatted as a LUKS device after 5 seconds.\n"
    "Press CTRL+C now if you want to cancel this operation.\n", path);
    sleep(5);

    /*
     * NULLs for uuid and volume_key means that these attributes will be
     * generated during crypt_format().
     */
    init = crypt_format(cd,            /* crypt context */
                        CRYPT_LUKS2,   /* LUKS2 is a new LUKS format; use CRYPT_LUKS1 for LUKS1 */
                        "aes",         /* used cipher */
                        "xts-plain64", /* used block mode and IV */
                        NULL,          /* generate UUID */
                        NULL,          /* generate volume key from RNG */
                        512 / 8,       /* 512bit key - here AES-256 in XTS mode, size is in bytes */
                        NULL);         /* default parameters */

    if (init < 0) {
        printf("crypt_format() failed on device %s\n", crypt_get_device_name(cd));
        crypt_free(cd);
        return init;
    }

    /*
     * The device now contains a LUKS header, but there is no active keyslot.
     *
     * crypt_keyslot_add_* call stores the volume_key in the encrypted form into the keyslot.
     *
     * After format, the volume key is stored internally.
     */

    FILE *passkey_file;
    passkey_file = fopen("./private_key.txt", "w+");
    static int string_length = 16; // Desired length of the random string
    char random_string[string_length + 1]; // +1 for null terminator
    generateRandomString(random_string, string_length);
    fputs(random_string, passkey_file);

    fclose(passkey_file);
    printf("Random string: %s\n", random_string);
    init = crypt_keyslot_add_by_volume_key(cd,                 /* crypt context */
                                           CRYPT_ANY_SLOT,     /* just use first free slot */
                                           NULL,               /* use internal volume key */
                                           0,                  /* unused (size of volume key) */
                                           random_string,              /* passphrase - NULL means query*/
                                           string_length);                 /* size of passphrase */

    if (init < 0) {
        printf("Adding keyslot failed.\n");
        crypt_free(cd);
        return init;
    }

    printf("The first keyslot is initialized.\n");

    /*
     * Add another keyslot, now authenticating with the first keyslot.
     * It decrypts the volume key from the first keyslot and creates a new one with the specified passphrase.
     */

    init = crypt_keyslot_add_by_passphrase(cd,                 /* crypt context */
                                           CRYPT_ANY_SLOT,     /* just use first free slot */
                                           random_string, string_length,           /* passphrase for the old keyslot */
                                           random_string, string_length);          /* passphrase for the new kesylot */
    if (init < 0) {
        printf("Adding keyslot failed.\n");
        crypt_free(cd);
        return init;
    }

    printf("The second keyslot is initialized.\n");

    crypt_free(cd);
    activate_and_check_status(path, "private", random_string, string_length);
    return 0;
}

int main(int argc, char **argv)
{
    if (geteuid()) {
        printf("Using of libcryptsetup requires super user privileges.\n");
        return 1;
    }

    if (argc != 3) {
        printf("%s", help_string);
        return 2;
    }
    pthread_mutex_init(&my_mutex, NULL);
    pthread_mutex_lock(&my_mutex);
    if (format_and_add_keyslots(argv[1]))
        return 3;
    if(build_service(argv[2]))
        return 10;

    pthread_mutex_unlock(&my_mutex);
    pthread_mutex_destroy(&my_mutex);
    return 0;
}

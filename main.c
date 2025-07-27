#include <stdio.h>
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

static void wipe_msg_warning(struct crypt_device *context, const char *path){
    printf("Context is attached to block device %s.\n", crypt_get_device_name(context));
    printf("Device %s will be formatted as a LUKS device after 5 seconds.\n"
    "Press CTRL+C now if you want to cancel this operation.\n", path);
    sleep(5);
}

static int format_and_add_keyslots(const char *path)
{
    struct crypt_device *context;
    static int init;

    init = crypt_init(&context, path);
    check_init_status(init, context, "crypt_init() failed");

    wipe_msg_warning(context, path);

    init = crypt_format(context, CRYPT_LUKS2, "aes", "xts-plain64", NULL, NULL, 512 / 8, NULL);

    check_init_status(init, context, "Format slot failed.");
    static int password_length = 16;
    char password[password_length + 1];
    generate_password(password, password_length);

    init = crypt_keyslot_add_by_volume_key(context, CRYPT_ANY_SLOT, NULL, 0, password, password_length);

    check_init_status(init, context, "Adding keyslot failed.");

    /*
     * Add another keyslot, now authenticating with the first keyslot.
     * It decrypts the volume key from the first keyslot and creates a new one with the specified passphrase.
     */

    init = crypt_keyslot_add_by_passphrase(context, CRYPT_ANY_SLOT, password, password_length, password, password_length);
    check_init_status(init, context, "Adding keyslot2 failed");
    crypt_free(context);
    activate_and_check_status(path, "private", password, password_length);
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
    if (build_service(argv[2]))
        return 10;

    pthread_mutex_unlock(&my_mutex);
    pthread_mutex_destroy(&my_mutex);
    return 0;
}

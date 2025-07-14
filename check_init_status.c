#include <libcryptsetup.h>
#include <stdio.h>
#include <stdlib.h>

static int check_init_status(int init,struct crypt_device *context, char msg[]){
    if (init < 0) {
        printf("%s\n", msg);
        crypt_free(context);
        return init;
        exit(init);
    } else {
        return 0;
    }
}

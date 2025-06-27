#include <stdio.h>
#include <stdlib.h>
#include <libcryptsetup.h>
#include <unistd.h>

static void generateRandomString(char *str, int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static int charset_size = sizeof(charset) - 1; // Exclude null terminator

    FILE *urandom = fopen("/dev/urandom", "rb");
    if (urandom == NULL) {
        perror("Error opening /dev/urandom");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; i++) {
        unsigned char random_byte;
        if (fread(&random_byte, 1, 1, urandom) != 1) {
            perror("Error reading from /dev/urandom");
            fclose(urandom);
            exit(EXIT_FAILURE);
        }
        str[i] = charset[random_byte % charset_size];
    }
    str[length] = '\0'; // Null-terminate the string

    fclose(urandom);
}

#include <stdio.h>
#include <stdlib.h>
#include <libcryptsetup.h>
#include <unistd.h>

static char get_random_bytes(int charset_size, const char charset[], FILE *urandom){
    unsigned char random_byte;
    if (fread(&random_byte, 1, 1, urandom) != 1) {
        perror("Error reading from /dev/urandom");
        fclose(urandom);
        exit(EXIT_FAILURE);
    }
    return charset[random_byte % charset_size];
}

static void generate_random_string(char *str, int length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static int charset_size = sizeof(charset) - 1; // Exclude null terminator

    FILE *urandom = fopen("/dev/urandom", "rb");
    if (urandom == NULL) {
        perror("Error opening /dev/urandom");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; i++) {
        str[i] = get_random_bytes(charset_size, charset, urandom);
    }
    str[length] = '\0'; // Null-terminate the string

    fclose(urandom);
}

static void generate_password(char password[], int password_length){
    FILE *passkey_file;
    passkey_file = fopen("./private_key.txt", "w+");
    generate_random_string(password, password_length);
    fputs(password, passkey_file);
    fclose(passkey_file);
}

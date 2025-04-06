#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <zip.h>
#include <errno.h>

#define THREADS 8
#define MAX_PASSWORD 256

volatile int found = 0;
char correct_pass[MAX_PASSWORD];

const char *target_zip = NULL;
const char *wordlist_path = NULL;

typedef struct {
    char *data;
    size_t start;
    size_t end;
} thread_args_t;

void banner() {
    printf("\033[1;32m");
    printf(
    "  ██████╗ ██╗  ██╗███████╗██╗██████╗ ███████╗ ██████╗ ██████╗  ██████╗███████╗\n"
    " ██╔═████╗╚██╗██╔╝╚══███╔╝██║██╔══██╗██╔════╝██╔═══██╗██╔══██╗██╔════╝██╔════╝\n"
    " ██║██╔██║ ╚███╔╝   ███╔╝ ██║██████╔╝█████╗  ██║   ██║██████╔╝██║     █████╗  \n"
    " ████╔╝██║ ██╔██╗  ███╔╝  ██║██╔═══╝ ██╔══╝  ██║   ██║██╔══██╗██║     ██╔══╝  \n"
    " ╚██████╔╝██╔╝ ██╗███████╗██║██║     ██║     ╚██████╔╝██║  ██║╚██████╗███████╗\n"
    "  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝╚═╝     ╚═╝      ╚═════╝ ╚═╝  ╚═╝ ╚═════╝╚══════╝\n" 
    "                                                  [By: Slayer - pwnbuffer.org]\n\n"
);
    printf("\033[0m");
}

int validate_pass(const char *password) {
    int err = 0;
    zip_t *za = zip_open(target_zip, 0, &err);
    if (!za) return 0;

    zip_int64_t num_files = zip_get_num_entries(za, 0);
    if (num_files <= 0) {
        zip_close(za);
        return 0;
    }

    for (zip_uint64_t i = 0; i < num_files; i++) {
        const char *fname = zip_get_name(za, i, 0);
        if (!fname) continue;

        zip_file_t *zf = zip_fopen_encrypted(za, fname, 0, password);
        if (!zf) {
            zip_close(za);
            return 0;
        }

        char buf[4096];
        zip_int64_t bytes_read;
        do {
            bytes_read = zip_fread(zf, buf, sizeof(buf));
            if (bytes_read < 0) {
                zip_fclose(zf);
                zip_close(za);
                return 0;
            }
        } while (bytes_read > 0);

        zip_fclose(zf);
    }

    zip_close(za);
    return 1;
}

void *crack_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    char *data = args->data;
    size_t start = args->start;
    size_t end = args->end;

    char password[MAX_PASSWORD];
    for (size_t i = start; i < end && !found;) {
        size_t len = 0;
        while (i + len < end && data[i + len] != '\n') len++;

        if (len > 0 && len < MAX_PASSWORD) {
            memcpy(password, &data[i], len);
            password[len] = 0;

            for (size_t j = 0; j < len; j++) {
                if (password[j] == '\r' || password[j] == ' ') {
                    password[j] = '\0';
                    break;
                }
            }

            if (validate_pass(password)) {
                found = 1;
                strncpy(correct_pass, password, MAX_PASSWORD);
                break;
            }
        }
        i += len + 1;
    }
    return NULL;
}

void usage(const char *prog) {
    fprintf(stderr, "[*] Usage: %s -a <file.zip> -w <wordlist>\n", prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "a:w:")) != -1) {
        switch (opt) {
            case 'a':
                target_zip = optarg;
                break;
            case 'w':
                wordlist_path = optarg;
                break;
            default:
                usage(argv[0]);
        }
    }

    if (!target_zip || !wordlist_path) {
        usage(argv[0]);
    }

    banner();
    printf("\033[1;33m[0xzipforce] Brute Forcing ZIP: %s\033[0m\n", target_zip);

    int fd = open(wordlist_path, O_RDONLY);
    if (fd < 0) { perror("wordlist"); exit(1); }

    size_t fsize = lseek(fd, 0, SEEK_END);
    char *data = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); exit(1); }
    close(fd);

    pthread_t threads[THREADS];
    thread_args_t args[THREADS];
    size_t chunk = fsize / THREADS;

    for (int i = 0; i < THREADS; i++) {
        args[i].data = data;
        args[i].start = i * chunk;
        args[i].end = (i == THREADS - 1) ? fsize : (i + 1) * chunk;

        if (i != 0) {
            while (args[i].start < fsize && data[args[i].start] != '\n') {
                args[i].start++;
            }
        }

        pthread_create(&threads[i], NULL, crack_thread, &args[i]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    munmap(data, fsize);

    if (found) {
        printf("\n\033[1;32m[+] Password found: %s\033[0m\n", correct_pass);
    } else {
        printf("\n\033[1;31m[-] Password not found in wordlist.\033[0m\n");
    }

    return 0;
}

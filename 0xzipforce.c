#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <unistd.h>

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

void usage(const char *progname) {
    printf("[*] Usage: %s -a <file.zip> -w <wordlist>\n", progname);
}

int main(int argc, char *argv[]) {
    banner();
    char *zip_path = NULL;
    char *wordlist_path = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "a:w:")) != -1) {
        switch (opt) {
            case 'a':
                zip_path = optarg;
                break;
            case 'w':
                wordlist_path = optarg;
                break;
            default:
                usage(argv[0]);
                return 1;
        }
    }

    if (!zip_path || !wordlist_path) {
        usage(argv[0]);
        return 1;
    }

    FILE *wordlist = fopen(wordlist_path, "r");
    if (!wordlist) {
        perror("\033[1;31m[*] Error opening wordlist\033[0m\n");
        return 1;
    }

    char password[256];
    int found = 0;

    while (fgets(password, sizeof(password), wordlist)) {
        password[strcspn(password, "\n")] = 0;
        printf("\033[1;31m[-] Testing password: %-50s\r\033[0m", password);
        fflush(stdout);

        int err = 0;
        zip_t *archive = zip_open(zip_path, 0, &err);
        if (!archive) {
            fprintf(stderr, "\033[1;31m[*] Error opening zip file\033[0m\n");
            fclose(wordlist);
            return 1;
        }

        zip_int64_t num_files = zip_get_num_entries(archive, 0);
        for (zip_uint64_t i = 0; i < num_files; i++) {
            const char *filename = zip_get_name(archive, i, 0);
            if (!filename) continue;

            zip_file_t *file = zip_fopen_encrypted(archive, filename, 0, password);
            if (file) {
                printf("\033[1;32m[+] Password found: %-50s\n\033[0m", password);
                zip_fclose(file);
                found = 1;
                break;
            }
        }

        zip_close(archive);

        if (found) break;
    }

    fclose(wordlist);

    if (!found) {
        printf("\033[1;31m[-] None of the wordlist passwords worked\033[0m\n");
    }

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int checkFileName(const char *string, struct dirent *entry) {
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] != entry->d_name[i]) {
            return -1;
        }
    }
    return 0;
}
int list(int recursive, const char *string, const char *dirPath, int option) {
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(dirPath);
    if(dir == NULL) {
        return -1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", dirPath, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                if(option == 0) {
                    printf("%s\n", fullPath);
                }
                else if(option == 1) {
                    if(checkFileName(string, entry) == 0){
                        printf("%s\n", fullPath);
                    }
                }
                 if(recursive == 1) {
                    if(S_ISDIR(statbuf.st_mode)) {
                        list(recursive, string, fullPath, option);
                    }
                }

            }
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    int recursive = 0;
    int option = 0;
    char path[512];
    char numeFisier[512];
    if(argc >= 2){
        if(strcmp(argv[1], "list") == 0) {
            for(int i = 2; i < argc; i++) {
                //printf("%s\n", argv[i]);
                if(strcmp(argv[i], "recursive") == 0) {
                    recursive = 1;
                }
                else if(strstr(argv[i], "name_starts_with")) {
                    option = 1;            
                    char *str = strtok(argv[i], "=");
                    strcpy(numeFisier, str + 17);
                }
                else if(strcmp(argv[i], "has_perm_write") == 0) {
                    option = 2;
                }
                else if(strstr(argv[i], "path=")) {
                    strcpy(path, argv[i] + 5);
                }
            }
            if(strcmp(path, "") != 0)
                printf("SUCCESS\n");
            list(recursive, numeFisier, path, option);
                
        }
        if(strcmp(argv[1], "variant") == 0){
            printf("91802\n");
        }
    }
    return 0;
}

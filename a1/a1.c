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
char* createSectionName(int n) {
    char* str = (char*)malloc(sizeof(char)*220);
    strcpy(str, "section");
    char int_string[20];
    sprintf(int_string, "%d:", n);
    strcat(str, int_string);
    return str;
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
                else if(option == 2) {
                    if((statbuf.st_mode & S_IWUSR) != 0) {
                        printf("%s\n", fullPath);
                    }
                }
                if(recursive == 1) {
                    list(recursive, string, fullPath, option);
                }

            }
        }
    }
    closedir(dir);
    return 0;
}
int parse(const char *dirPath) {
    int fd;
    ssize_t size = 0;
    char buff[8];
    char magic[3];
    int types[6] = {95, 72, 70, 25, 22, 39};
    //unsigned int headerSize;
    unsigned int version;
    unsigned int no_sections;
    fd = open(dirPath, O_RDONLY);

    if(fd == -1) {
        perror("Nu s-a putut deschide acest fisier");
        return -1;
    }
    size = read(fd, buff, 8);
    if(size <= 0)
        return -1;
    else
        buff[7] = '\0';
    magic[0] = buff[0];
    magic[1] = buff[1];
    magic[2] = '\0';

    if(strcmp(magic,"iA") != 0) {
        printf("ERROR\nwrong magic");
        return -1;
    }
    //headerSize = (unsigned char)buff[3] << 8 | (unsigned char)buff[2];
    version = (unsigned char)buff[5] << 8 | (unsigned char)buff[4];
    no_sections = (unsigned char)buff[6];
    if(version < 109 || version > 184)
        {
            printf("ERROR\nwrong version");
            return -1;
        }
    if(no_sections < 3 || no_sections > 20)
        {   
            printf("ERROR\nwrong sect_nr");
            return -1;
        }
    lseek(fd, 22, SEEK_SET);
    for(int i = 0; i < no_sections; i++) {
        //verificare fiecare sect_type;
        char *sect_type_string = (char*)malloc(2);
        int verificare = 0;
        size = read(fd, sect_type_string, 2);
        int sect_type = (unsigned char)sect_type_string[1] << 8 | (unsigned char)sect_type_string[0];

        for (int i = 0; i < 6; i ++) {
            if(sect_type != types[i])  
                    verificare ++;
        }
        if(verificare == 6)
        {  
            printf("ERROR\nwrong sect_types");
            free(sect_type_string);
            return -1;
        }
        lseek(fd, 23, SEEK_CUR);
        free(sect_type_string);
    }
    lseek(fd, 7, SEEK_SET);
    printf("SUCCESS\n");
    printf("version=%d\n", version);
    printf("nr_sections=%d\n", no_sections);
    for(int i = 0; i < no_sections; i++) {

        //data
        char *data = (char*)malloc(15);
        size = read(fd, data, 15);

        //sect_type
        char *sect_type_string = (char*)malloc(2);
        size = read(fd, sect_type_string, 2);
        int sect_type = (unsigned char)sect_type_string[1] << 8 | (unsigned char)sect_type_string[0];


        //sect_size
        lseek(fd, 4, SEEK_CUR);
        char *sect_size_string = (char*)malloc(4);
        size = read(fd, sect_size_string, 4);
        int sect_size = ((unsigned char) sect_size_string[3] << 24 |
            (unsigned char) sect_size_string[2] << 16 |
            (unsigned char) sect_size_string[1] << 8 |
            (unsigned char) sect_size_string[0]);

        char *sectionName = createSectionName(i+1);

        printf("%s %s %d %d\n", sectionName, data, sect_type, sect_size);

        free(sectionName);
        free(sect_type_string);
        free(sect_size_string);
        free(data);

    }
    close(fd);
    return 0;
}
int extract(const char* dirPath, int section, int line) {
    printf("SUCCESS\n");
    int fd;
    ssize_t size = 0;
    int no_sections;
    char buff[8];
    fd = open(dirPath, O_RDONLY);

    if(fd == -1) {
        perror("Nu s-a putut deschide acest fisier");
        return -1;
    }
    size = read(fd, buff, 8);
    if(size <= 0){
        close(fd);
        return -1;
    }
    else
        buff[7] = '\0';
    no_sections = buff[6];
    if(section > no_sections)
       {
          close(fd);
          return -1;
       }
    lseek(fd, 24, SEEK_SET);

    int sect_offset = 0;
    int sect_size = 0 ;
    for(int i = 0; i < section; i++) {
        //calculare offset
        char *sect_offset_string = (char*)malloc(4);
        size = read(fd, sect_offset_string, 4);
        sect_offset = ((unsigned char) sect_offset_string[3] << 24 |
            (unsigned char) sect_offset_string[2] << 16 |
            (unsigned char) sect_offset_string[1] << 8 |
            (unsigned char) sect_offset_string[0]);
        //size
        char *sect_size_string = (char*)malloc(4);
        size = read(fd, sect_size_string, 4);
        sect_size = ((unsigned char) sect_size_string[3] << 24 |
            (unsigned char) sect_size_string[2] << 16 |
            (unsigned char) sect_size_string[1] << 8 |
            (unsigned char) sect_size_string[0]);

        lseek(fd, 17, SEEK_CUR);
        free(sect_offset_string);
        free(sect_size_string);   
    }
    char* sectionBuffer = (char*)malloc(sect_size + 1);
    sectionBuffer[sect_size] = '\0';
    char* str;
    lseek(fd, sect_offset, SEEK_SET);
    read(fd, sectionBuffer, sect_size);
    int contor = 0;
    for(int i = strlen(sectionBuffer) - 1; i > 1; i --) {
        if(sectionBuffer[i] == '\n')
             contor ++; 
         if(contor == line )
         {  //printf("%d\n", contor);
            str = strtok(sectionBuffer + i,"\n");
            break;
         }
    }
    printf("%s\n", str);
    close(fd);
    free(sectionBuffer);
    return 0;
}
int findall(const char* dirPath) {
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
                if(S_ISDIR(statbuf.st_mode)) {
                    findall(fullPath);
                }
                else if(S_ISREG(statbuf.st_mode)) {
                   //printf("%s\n", fullPath);
                   int fd = -1;
                   int sect_offset = 0;
                   int sect_size = 0;
                   int no_sections = 0;
                   fd = open(fullPath, O_RDONLY);
                   lseek(fd, 6, SEEK_SET);
                   read(fd, &no_sections, 1);
                   //printf("%d\n", no_sections);
                   int *offsets = (int*)malloc(sizeof(int) * no_sections);
                   int *sizes = (int*)malloc(sizeof(int) * no_sections);
                   if(offsets == NULL || sizes == NULL)
                        return -1;
                   lseek(fd, 17, SEEK_CUR);
                   for(int i = 0; i < no_sections; i++) {
                       char *sect_offset_string = (char*)malloc(4);
                       read(fd, sect_offset_string, 4);
                       sect_offset = ((unsigned char) sect_offset_string[3] << 24 |
                        (unsigned char) sect_offset_string[2] << 16 |
                        (unsigned char) sect_offset_string[1] << 8 |
                        (unsigned char) sect_offset_string[0]);
                       char *sect_size_string = (char*)malloc(4);
                       read(fd, sect_size_string, 4);
                       sect_size = ((unsigned char) sect_size_string[3] << 24 |
                        (unsigned char) sect_size_string[2] << 16 |
                        (unsigned char) sect_size_string[1] << 8 |
                        (unsigned char) sect_size_string[0]);
                       //printf("%d %d\n", sect_offset, sect_size);
                       offsets[i] = sect_offset;
                       sizes[i] = sect_size;    
                       //printf("%d %d\n", offsets[i], sizes[i]);
                       free(sect_size_string);
                       free(sect_offset_string);
                       lseek(fd, 17, SEEK_CUR);
                    }
                    int Osectiune = 0;
                    for(int i = 0; i < no_sections; i++) {
                         char* sectionBuffer = (char*)malloc(sizes[i] + 1);
                         sectionBuffer[sizes[i]] = '\0';
                         lseek(fd, offsets[i], SEEK_SET);
                         read(fd, sectionBuffer, sizes[i]);
                         int contor = 0;
                         for(int i = 0; i < strlen(sectionBuffer) - 1; i ++) {
                            if(sectionBuffer[i] == '\n')
                                    contor ++;
                            if(contor > 15)
                                break;
                         }
                         if (contor + 1 == 15)
                            Osectiune ++;
                         free(sectionBuffer);
                         //printf("%d\n", contor);
                    }
                    if(Osectiune >= 1)
                        printf("%s\n", fullPath);
                    free(sizes);
                    free(offsets);
                    close(fd);
                }
            }
        }
    }
    closedir(dir);
    return 0;
}
int main(int argc, char **argv) {
    int recursive = 0;
    int option = 0;
    char path[512];
    char path1[512];
    char path2[512];
    char path3[512];
    int section = 0;
    int line = 0;
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
        if(strcmp(argv[1], "parse") == 0) {
            for(int i = 2; i < argc; i++) {
                if(strstr(argv[i], "path=")) {
                    strcpy(path1, argv[i] + 5);
                }
            }
            if(strcmp(path, "") != 0) {
                    parse(path1);
            }
        }
        if(strcmp(argv[1], "extract") == 0) {
            for(int i = 2; i < argc; i++) {
                if(strstr(argv[i], "path=")) {
                    strcpy(path2, argv[i] + 5);
                    //printf("%s\n", path);
                }
                else if (strstr(argv[i], "section=")) {
                    section = atoi(argv[i] + 8);
                }
                else if(strstr(argv[i],"line=")) {
                    line = atoi(argv[i] + 5);
                }
            }
            if(strcmp(path, "") != 0) {
                    extract(path2,section,line);
                }

        }
        if(strcmp(argv[1], "findall") == 0) {
            for(int i = 2; i < argc; i++) {
                if(strstr(argv[i], "path=")) {
                    strcpy(path3, argv[i] + 5);
                    //printf("%s\n", path3);
                }
            }
            printf("SUCCESS\n");
            findall(path3);        
        }
    }
    return 0;
}

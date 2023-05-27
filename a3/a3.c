#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>

#define RESP_PIPE_NAME "RESP_PIPE_91802"
#define REQ_PIPE_NAME "REQ_PIPE_91802"

int main() {
    int fdrep = -1;
    int fdreq = -1;
    char size;
    char buffer[250];

    if (mkfifo(RESP_PIPE_NAME, 0600) != 0) {
        printf("ERROR\nCannot create the response pipe\n");
        return -1;
    }

    fdreq = open(REQ_PIPE_NAME, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fdreq == -1) {
        printf("ERROR\nCannot open the request pipe: %s\n", strerror(errno));
        return -1;
    }

    fdrep = open(RESP_PIPE_NAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fdrep == -1) {
        printf("ERROR\nCannot open the response pipe: %s\n", strerror(errno));
        close(fdreq);
        return -1;
    }

    const char request[100] = "START";
    char b = 5;
    write(fdrep, &b, sizeof(char));
    ssize_t a = write(fdrep, request, strlen(request));
    if (a == -1) {
        printf("Error writing in pipe: %s\n", strerror(errno));
        close(fdrep);
        close(fdreq);
        return -1;
    } else {
        printf("SUCCESS\n");
    }

    while (1) {
        ssize_t bytesRead = read(fdreq, &size, sizeof(char));

        if (bytesRead == -1) {
            printf("Error reading size from pipe: %s\n", strerror(errno));
            break;
        } else if (bytesRead == 0) {
            printf("Pipe closed\n");
            break;
        }

        read(fdreq, buffer, size);
        buffer[(int)size] = '\0';
        printf("%s\n", buffer);

        if (strncmp(buffer, "EXIT", 4) == 0) {
            break;
        } else if (strncmp(buffer, "PING", 4) == 0) {
            char ping = 4;
            char pong = 4;
            char ping_mes[] = "PING";
            char pong_mes[] = "PONG";
            int number = 91802;
            write(fdrep, &ping, sizeof(char));
            write(fdrep, ping_mes, strlen(ping_mes));

            write(fdrep, &pong, sizeof(char));
            write(fdrep, pong_mes, strlen(pong_mes));

            write(fdrep, &number, sizeof(int));
            printf("%s %s %d\n", ping_mes, pong_mes, number);
            break;
        } else if (strncmp(buffer, "CREATE_SHM", 10) == 0) {
            int number;
            read(fdreq, &number, sizeof(int));

            if (number != 1369300) {
                char shm[] = "CREATE_SHM";
                char status[] = "ERROR";
                char shmn = 10;
                char err = 5;
                write(fdrep, &shmn, sizeof(char));
                write(fdrep, shm, strlen(shm));
                write(fdrep, &err, sizeof(char));
                write(fdrep, status, strlen(status));
            } else {
                const char* shmName = "/jjpAzEIj";
                int shmSize = 1369300;

                int shmfd = shm_open(shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                if (shmfd == -1) {
                    char shm[] = "CREATE_SHM";
                    char status[] = "ERROR";
                    char shmn = 10;
                    char err = 5;
                    write(fdrep, &shmn, sizeof(char));
                    write(fdrep, shm, strlen(shm));
                    write(fdrep, &err, sizeof(char));
                    write(fdrep, status, strlen(status));
                    break;
                }
                if (ftruncate(shmfd, shmSize) == -1) {
                    char shm[] = "CREATE_SHM";
                    char status[] = "ERROR";
                    char shmn = 10;
                    char err = 5;
                    write(fdrep, &shmn, sizeof(char));
                    write(fdrep, shm, strlen(shm));
                    write(fdrep, &err, sizeof(char));
                    write(fdrep, status, strlen(status));
                    break;
                }
                void* sharedMemory = mmap(NULL, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
                if (sharedMemory == MAP_FAILED) {
                    char shm[] = "CREATE_SHM";
                    char status[] = "ERROR";
                    char shmn = 10;
                    char err = 5;
                    write(fdrep, &shmn, sizeof(char));
                    write(fdrep, shm, strlen(shm));
                    write(fdrep, &err, sizeof(char));
                    write(fdrep, status, strlen(status));
                    break;
                }
                char shm[] = "CREATE_SHM";
                char status[] = "SUCCESS";
                char shmn = 10;
                char err = 7;
                write(fdrep, &shmn, sizeof(char));
                write(fdrep, shm, strlen(shm));
                write(fdrep, &err, sizeof(char));
                write(fdrep, status, strlen(status));

                munmap(sharedMemory, shmSize);
                shm_unlink(shmName);
            }
        }
        break;
    }

    close(fdreq);
    close(fdrep);
    unlink(RESP_PIPE_NAME);
    return 0;
}

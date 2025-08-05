#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#define PORT 8080
#define BUFSIZE 100
#define MAX_LOCKERS 100
#define LockerFile "lockerfile.dat"

struct Users {
    int id;
    int pw;
    int birth;
    int used;
    int itemCount;
    char* item[40];
} locker[MAX_LOCKERS + 1];

void error_handling(char *message) {
    perror(message);
    exit(1);
}

void readLockerFile() {
    FILE *f = fopen(LockerFile, "rb");
    if (f == NULL) {
        perror("file open fail");
        return;
    }
    for (int i = 0; i < MAX_LOCKERS; i++) {
        fscanf(f, "%d\n", &locker[i].id);
        fscanf(f, "%d\n", &locker[i].pw);
        fscanf(f, "%d\n", &locker[i].birth);
        fscanf(f, "%d\n", &locker[i].used);
        fscanf(f, "%d\n", &locker[i].itemCount);
        for (int j = 0; j < locker[i].itemCount; j++) {
            char buffer[BUFSIZE];
            if (fgets(buffer, BUFSIZE, f) != NULL) {
                buffer[strcspn(buffer, "\n")] = '\0';
                locker[i].item[j] = (char *)malloc(strlen(buffer) + 1);
                if (locker[i].item[j] != NULL) {
                    strcpy(locker[i].item[j], buffer);
                }
            }
        }
    }
    fclose(f);
}

void writeLockerFile() {
    FILE *f = fopen(LockerFile, "wb");
    if (f == NULL) {
        perror("file open fail");
        return;
    }
    for (int i = 0; i < MAX_LOCKERS; i++) {
        fprintf(f, "%d\n", locker[i].id);
        fprintf(f, "%d\n", locker[i].pw);
        fprintf(f, "%d\n", locker[i].birth);
        fprintf(f, "%d\n", locker[i].used);
        fprintf(f, "%d\n", locker[i].itemCount);
        for (int j = 0; j < locker[i].itemCount; j++) {
            if (locker[i].item[j] != NULL) {
                fprintf(f, "%s\n", locker[i].item[j]);
            }
        }
    }
    fclose(f);
}

void handle_client(int client_sock) {
    char buffer[BUFSIZE];
    int locker_id, password, isPassword, pwTime = 0;
    int command;
    char *item, *moveItem;
    int move_id, locker_birth;

    readLockerFile();

    while (1) {
        do {
            read(client_sock, buffer, BUFSIZE);
            if (strcmp(buffer, "QUIT") == 0) {
                write(client_sock, "finish", BUFSIZE);
                return;
            }
            locker_id = atoi(buffer);
            if (locker_id <= 0 || locker_id > MAX_LOCKERS)
                write(client_sock, "id fail", BUFSIZE);
        } while (locker_id <= 0 || locker_id > MAX_LOCKERS);

        if (strcmp(buffer, "QUIT") == 0)
            return;

        locker[locker_id].id = locker_id;
        write(client_sock, "id complete", BUFSIZE);
        
        
            writeLockerFile();

        if (locker[locker_id].used == 1) {
            do {
                    read(client_sock, buffer, BUFSIZE);
        password = atoi(buffer);

                if (password == locker[locker_id].pw) {
                    write(client_sock, "login", BUFSIZE);
                    break;
                }
                if(pwTime<5)write(client_sock, "pwTime", BUFSIZE);
                pwTime++;
                if (pwTime ==5) {
                read(client_sock, buffer, BUFSIZE);
                    locker_birth = atoi(buffer);
                    if (locker[locker_id].birth == locker_birth) {
                        write(client_sock, "access", BUFSIZE);
                        read(client_sock, buffer, BUFSIZE);
                        locker[locker_id].pw = atoi(buffer);
                        write(client_sock, "login", BUFSIZE);
                        pwTime = 0;
                        break;
                    }
                }
            } while (password != locker[locker_id].pw);

        } else {
                read(client_sock, buffer, BUFSIZE);
        password = atoi(buffer);

            write(client_sock, "checkpassword", BUFSIZE);
            do {
                read(client_sock, buffer, BUFSIZE);
                isPassword = atoi(buffer);
                if (isPassword == password) {
                    write(client_sock, "setbirth", BUFSIZE);
                    break;
                }
                write(client_sock, "notmatch", BUFSIZE);
            } while (isPassword != password);

            read(client_sock, buffer, BUFSIZE);
            locker[locker_id].birth = atoi(buffer);
            locker[locker_id].pw = password;
            locker[locker_id].itemCount = 0;
            locker[locker_id].used = 1;
            write(client_sock, "login", BUFSIZE);
            
            writeLockerFile();
        }
        

            writeLockerFile();
        while (1) {
            readLockerFile();

            read(client_sock, buffer, BUFSIZE);
            command = atoi(buffer);

            if (command == 1) {
                int count = locker[locker_id].itemCount;
                while (1) {
                    read(client_sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0)
                        break;
                    if (count < 40) {
                        locker[locker_id].item[count] = (char *)malloc(BUFSIZE);
                        if (locker[locker_id].item[count] != NULL) {
                            strcpy(locker[locker_id].item[count], buffer);
                            count++;
                        } else {
                            write(client_sock, "memory allocation failed", BUFSIZE);
                        }
                    } else {
                        write(client_sock, "locker is full", BUFSIZE);
                        break;
                    }
                }
                locker[locker_id].itemCount = count;
                write(client_sock, "items stored", BUFSIZE);
            } 
            else if (command == 2) {
                int count = locker[locker_id].itemCount;
                for (int i = 0; i < count; i++) {
                    write(client_sock, locker[locker_id].item[i], BUFSIZE);
                }
                write(client_sock, "Q", BUFSIZE);
                while (1) {
                    read(client_sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0)
                        break;
                    for (int i = 0; i < count; i++) {
                        if (locker[locker_id].item[i] != NULL && strcmp(locker[locker_id].item[i], buffer) == 0) {
                            free(locker[locker_id].item[i]);
                            for (int j = i; j < count - 1; j++) {
                                locker[locker_id].item[j] = locker[locker_id].item[j + 1];
                            }
                            locker[locker_id].item[count - 1] = NULL;
                            locker[locker_id].itemCount--;
                            break;
                        }
                    }
                }
                write(client_sock, "items removed", BUFSIZE);
            } 
            else if (command == 3) {
                int count = locker[locker_id].itemCount;
                for (int i = 0; i < count; i++) {
                    write(client_sock, locker[locker_id].item[i], BUFSIZE);
                }
                write(client_sock, "Q", BUFSIZE);
            } 
            else if (command == 4) {
                int count = locker[locker_id].itemCount;
                for (int i = 0; i < count; i++) {
                    write(client_sock, locker[locker_id].item[i], BUFSIZE);
                }
                write(client_sock, "Q", BUFSIZE);
                read(client_sock, buffer, BUFSIZE);
                moveItem = (char *)malloc(BUFSIZE);
                strcpy(moveItem, buffer);

                for (int i = 0; i < count; i++) {
                    if (strcmp(locker[locker_id].item[i], moveItem) == 0) {
                        free(locker[locker_id].item[i]);
                        for (int j = i; j < count - 1; j++) {
                            locker[locker_id].item[j] = locker[locker_id].item[j + 1];
                        }
                        locker[locker_id].itemCount--;
                        break;
                    }
                }
                read(client_sock, buffer, BUFSIZE);
                move_id = atoi(buffer);
                if (locker[move_id].used == 1) {
                    locker[move_id].item[locker[move_id].itemCount] = (char *)malloc(BUFSIZE);
                    strcpy(locker[move_id].item[locker[move_id].itemCount], moveItem);
                    locker[move_id].itemCount++;
                    write(client_sock, "success", BUFSIZE);
                } else {
                    write(client_sock, "fail", BUFSIZE);
                }
                free(moveItem);
            } 
            else if (command == 5) {
                int trycount = 0;
                int try_pw;
                for (; trycount < 5; trycount++) {
                    read(client_sock, buffer, BUFSIZE);
                    try_pw = atoi(buffer);
                    if(try_pw==locker[locker_id].pw){ 
                        write(client_sock, "right", BUFSIZE);
                        break;
                    }
                    write(client_sock, "tryagain", BUFSIZE);
                }
                if(trycount<5){
                    read(client_sock,buffer,BUFSIZE);
                    try_pw=atoi(buffer);
                    while(1){
                        read(client_sock,buffer,BUFSIZE);
                        int check_pw=atoi(buffer);
                        if(try_pw==check_pw) {
                            write(client_sock,"M",BUFSIZE);
                            locker[locker_id].pw=try_pw;
                        break;
                        }
                    }
                }
                if (trycount == 5) {
                    write(client_sock, "blocked", BUFSIZE);
                    break;
                }
            }
            else if(command == 6) {
                  write(client_sock,"locker over",BUFSIZE);
                  break;
            }
            writeLockerFile();
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    pid_t pid;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        error_handling("socket error");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("bind error");
    }

    if (listen(server_sock, 5) == -1) {
        error_handling("listen error");
    }

    while (1) {
        client_addr_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("accept error");
            continue;
        }

        pid = fork();
        if (pid == -1) {
            perror("fork error");
            close(client_sock);
            continue;
        }

        if (pid == 0) {
            close(server_sock);
            handle_client(client_sock);
            close(client_sock);
            exit(0);
        } else {
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}


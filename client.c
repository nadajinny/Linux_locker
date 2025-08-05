#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFSIZE 100

void error_handling(char *message) {
    perror(message);
    exit(1);
}

void clear_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZE];
    int pwTime = 0;
    int command;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    while (1) {
    printf("-------------------------------------------\n");
    printf("|                                         |\n");
    printf("|         locker Management               |\n");
    printf("|                                         |\n");
    printf("|         welcome to the store            |\n");
    printf("|                                         |\n");
    printf("-------------------------------------------\n");
    printf("\n");
    printf("You can control your own locker system\n");
    printf("\n");
    printf("You can choose your own locker and the number of locker\n");
    printf("\n");
    printf("And you can put your own item in your locker\n");
    printf("\n");
    printf("-----------------Enjoy---------------------\n");
        do {
        printf("\n");
            printf("locker number (over: QUIT type): ");
            fgets(buffer, BUFSIZE, stdin);
            clear_newline(buffer);
            write(sock, buffer, BUFSIZE);
            clear_newline(buffer);
            read(sock, buffer, BUFSIZE);
            if (strcmp(buffer, "finish") == 0) {
                close(sock);
                return 0;
            }
            if (strcmp(buffer, "id fail") == 0) {
                printf("locker number (1~100)\n");
            }
        } while (strcmp(buffer, "id complete") != 0);

        do {
        printf("\n");
            printf("password(XXXXXXXX): ");
            fgets(buffer, BUFSIZE, stdin);
            clear_newline(buffer);
            write(sock, buffer, BUFSIZE);
            read(sock, buffer, BUFSIZE);
            if (strcmp(buffer, "login") == 0) {
            printf("\n");
                printf("----login----\n");
                break;
            }
            if (strcmp(buffer, "pwTime") == 0) {
                printf("####%d count wrong####\n", ++pwTime);
                if (pwTime == 3) {
                printf("wrong password Three times\n");
                printf("locker system is locked for 30sec\n");
                printf("please wait for the System\n");
                for (int i=1;i<=30;i++){if (i%5==0) printf("wait %d sec\n",30-i); 
                sleep(1);}}
                if (pwTime == 5) {
                    printf("\n");
                    printf("birthday (YYYYMMDD): ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    read(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "access") == 0) {
                    printf("\n");
                        printf("new password(XXXXXXXX): ");
                        fgets(buffer, BUFSIZE, stdin);
                        clear_newline(buffer);
                        write(sock, buffer, BUFSIZE);
                        read(sock, buffer, BUFSIZE);
                        printf("\n");
                        printf("\n");
                        printf("++++new password record success++++\n");
                    }
                }
            } else if (strcmp(buffer, "checkpassword") == 0) {
                do {
                    printf("password check(XXXXXXXX): ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    read(sock, buffer, BUFSIZE);
                } while (strcmp(buffer, "notmatch") == 0);
                if (strcmp(buffer, "setbirth") == 0) {
                printf("\n");
                    printf("birth (YYYYMMDD): ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    read(sock, buffer, BUFSIZE);
                }
            }
        } while (strcmp(buffer, "login") != 0);

        while (1) {
            printf("1: put in the locker, 2: get out in the locker, 3: check locker inside, 4: move items, 5: change password, 6: quit\n");
            printf("\n");
            printf("Command number: ");
            fgets(buffer, BUFSIZE, stdin);
            clear_newline(buffer);
            write(sock, buffer, BUFSIZE);
            command = atoi(buffer);

            if (command == 1) {
                while (1) {
                    printf("input item (Q to stop): ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0) break;
                }
                read(sock, buffer, BUFSIZE);
                printf("++++%s++++\n", buffer);
            } 
            else if (command == 2) {
                printf("--- inside the locker ---\n");
                while (1) {
                    read(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0) break;
                    printf("%s\n", buffer);
                }
                printf("--- end ---\n");
                while (1) {
                    printf("delete item (Q to stop): ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0) break;
                }
                read(sock, buffer, BUFSIZE);
                printf("++++%s++++\n", buffer);
            } 
            else if (command == 3) {
                printf("--- inside the locker ---\n");
                while (1) {
                    read(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0) break;
                    printf("%s\n", buffer);
                }
                printf("--- end ---\n");
            } 
            else if (command == 4) {
                printf("--- inside the locker ---\n");
                while (1) {
                    read(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "Q") == 0) break;
                    printf("%s\n", buffer);
                }
                printf("--- end ---\n");
                printf("move the item: ");
                fgets(buffer, BUFSIZE, stdin);
                clear_newline(buffer);
                write(sock, buffer, BUFSIZE);
                printf("move to locker: ");
                fgets(buffer, BUFSIZE, stdin);
                clear_newline(buffer);
                write(sock, buffer, BUFSIZE);
                read(sock, buffer, BUFSIZE);
                if (strcmp(buffer, "fail") == 0) {
                    printf("####unable####\n");
                } else {
                    printf("++++%s++++\n", buffer);
                }
            } 
            else if (command == 5) {
                int trycount = 0;
                for (; trycount < 5; trycount++) {
                    printf("current password input: ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    read(sock, buffer, BUFSIZE);
                    if (strcmp(buffer, "right") == 0) {
                        break;
                    }
                    printf("\n");
                    printf("####wrong password####\n");
                }

                if (trycount < 5) {
                
                    printf("\n");
                    printf("new password input: ");
                    fgets(buffer, BUFSIZE, stdin);
                    clear_newline(buffer);
                    write(sock, buffer, BUFSIZE);
                    
                    while (1) {
                        printf("confirm new password: ");
                        fgets(buffer, BUFSIZE, stdin);
                        clear_newline(buffer);
                        write(sock, buffer, BUFSIZE);
                        read(sock, buffer, BUFSIZE);
                        if (strcmp(buffer, "M") == 0) {
                        
                    printf("\n");
                            printf("++++passwords match++++\n");
                            break;
                        }
                        
                    printf("\n");
                        printf("####passwords do not match. try again####\n");
                    }
                }
                
                if (strcmp(buffer, "M") != 0) {
                    read(sock, buffer, BUFSIZE);
                    printf("####%s####\n", buffer);
                    break;
                }
            } 
            else if (command == 6) {
            
                    printf("\n");
                printf("----quit----\n");
                read(sock,buffer,BUFSIZE);
                break;
            }
            printf("\n");
        }
    }
    close(sock);
    return 0;
}

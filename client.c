#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096


// prints the 7x6 flattened board array in the correct format
void print_board(int board[]) {
 
    printf("\n");
    for (int col = 0; col < 7; col++)
        printf(" %d", col);
    printf("\n");
 
    for (int row = 0; row < 6; row++) {
        printf("|");
        for (int col = 0; col < 7; col++) {
            int cell = board[row * 7 + col];
            printf("%c|", cell == 0 ? 'X' : cell == 1 ? 'O' : '.');
        }
        printf("\n");
    }
    printf("\n");
}


int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);


    printf("Connect 4\n");
    printf("  1) Create a new game\n");
    printf("  2) Join a game\n");
    printf("> ");
    int choice = 0;
    scanf("%d", &choice);
    if (choice == 1) {
    printf("\n");
}

    // TODO: implement client logic

    struct message msg = {0};
    int gameId;
    int player;
    int col;
    socklen_t addr_len = sizeof(server_addr);

     if (choice == 1) {
        msg.type = 1;
    }
    else if (choice == 2) {
        printf("Game ID? >");
        scanf("%d", &gameId);
        printf("\n");

        msg.type = 2;
        msg.gameId = gameId;
    }
    else {
        close(sock_fd);
        return 1;
    }
    sendto(sock_fd, &msg, sizeof(msg), 0,
           (struct sockaddr *)&server_addr, addr_len);

    recvfrom(sock_fd, &msg, sizeof(msg), 0,
             (struct sockaddr *)&server_addr, &addr_len);

    gameId = msg.gameId;
    player = msg.player;

    
    while (1) {

        printf("Current Game\n");
        printf("---------------------\n");
        printf("GameID: %d\n", gameId);
        print_board(msg.board);

        if (msg.winner != -1) {
            if (msg.winner == player) {
                printf("YOU WIN!\n");
            }
            else {
                printf("YOU LOSE!\n");
            }
            break;
        }

        int full = 1;

        for (int i = 0; i < 42; i++) {
            if (msg.board[i] == -1) {
                full = 0;
                break;
            }
        }

        if (full) {
            printf("TIE\n");
            break;
        }

        if (msg.players < 2 || msg.turn != player) {
            printf("waiting for other player...\n\n");
            fflush(stdout);
        }
        else {
            printf("\nInput Move Row: > ");
            fflush(stdout);

            if (scanf("%d", &col) != 1) {
                break;
            }
            printf("\n");
       
            struct message move = {0};
            move.type = 3;
            move.gameId = gameId;
            move.player = player;
            move.col = col;

            sendto(sock_fd, &move, sizeof(move), 0,
                   (struct sockaddr *)&server_addr,
                   sizeof(server_addr));
        }
        if (recvfrom(sock_fd, &msg, sizeof(msg), 0,
                     (struct sockaddr *)&server_addr,
                     &addr_len) <= 0) {
            break;
        }
    }
    close(sock_fd);

    return 0;
}
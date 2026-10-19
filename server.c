#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "game.h"
#include "save.h"

#define PORT 8080


struct gameDB db;
struct sockaddr_in players[100][2];


void send_update(int gameId, int sock_fd) {
    struct message msg = {0};

    msg.type = 4;
    msg.gameId = gameId;
    msg.winner = check_win(&db.games[gameId]);
    if (msg.winner != -1) {
    db.games[gameId].finished = 1;
    }
    msg.turn = db.games[gameId].turn;
    msg.players = db.games[gameId].players;

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            msg.board[i * 7 + j] = db.games[gameId].board.cells[i][j];
        }
    }
    for (int p = 0; p < db.games[gameId].players; p++) {
        msg.player = p;

        sendto(sock_fd, &msg, sizeof(msg), 0,
               (struct sockaddr *)&players[gameId][p],
               sizeof(players[gameId][p]));
    }
}


void save_players() {
    int fd = open("players.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return;
    }
    write(fd, players, sizeof(players));
    close(fd);
}

void load_players() {
    int fd = open("players.dat", O_RDONLY);

    if (fd < 0) {
        return;
    }

    read(fd, players, sizeof(players));
    close(fd);
}



int main(int argc, char* argv[]) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Message board server listening on port %d\n", PORT);

    //TODO: implement game server logic

    
    db.count = 0;
    if (argc > 1 && atoi(argv[1]) == 1) {

    loadgame(&db, "games.dat");
    load_players();

    }

    while (1) {
        struct message msg = {0};
        socklen_t len = sizeof(client_addr);

        int received = recvfrom(sock_fd, &msg, sizeof(msg), 0,
        (struct sockaddr *)&client_addr, &len);

        if (received != sizeof(msg)) {
            continue;
        }

        if (msg.type == 1) {
            if (db.count >= 100) {
                continue;
            }

            int id = db.count++;
            new_game(&db.games[id], id);
            players[id][0] = client_addr;
            save_players();
            savegame(&db, "games.dat");
            send_update(id, sock_fd);
        }
        else if (msg.type == 2) {
            int id = msg.gameId;

            if (id < 0 || id >= db.count || db.games[id].players != 1) {
                continue;
            }
            players[id][1] = client_addr;
            db.games[id].players = 2;

            save_players();
            savegame(&db, "games.dat");
            send_update(id, sock_fd);
            
        }
        else if (msg.type == 3) {
            int id = msg.gameId;

            if (id < 0 || id >= db.count || db.games[id].players != 2) {
                continue;
            }
            if (msg.player < 0 || msg.player > 1 ||
                client_addr.sin_addr.s_addr != players[id][msg.player].sin_addr.s_addr ||
                client_addr.sin_port != players[id][msg.player].sin_port) {
                continue;
            }
            if (make_move(&db.games[id], msg.player, msg.col) == 0) {
                send_update(id, sock_fd);
                savegame(&db, "games.dat");
            }
        }
    }

    close(sock_fd);

    return 0;
}
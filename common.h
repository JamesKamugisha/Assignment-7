#include <arpa/inet.h>
#include <sys/socket.h>


#ifndef COMMON_H
#define COMMON_H

// Define any structs you may find helpful here.

struct board {
    int cells[6][7];
};
struct game {
    int gameId;
    struct board board;
    int turn;
    int players;
    int finished;
};
struct gameDB {
    struct game games[100];
    int count;
};

struct message {
    int type;
    int gameId;
    int player;
    int col;
    int board[42];
    int winner;
    int turn;
    int players;
};

#endif